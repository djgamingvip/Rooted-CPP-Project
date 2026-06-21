#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <cmath>
#include <thread>
#include <chrono>

// ============================================
// STRUCTURES
// ============================================

struct Vector3 {
    float x, y, z;
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
};

struct Matrix4x4 {
    float M11, M12, M13, M14;
    float M21, M22, M23, M24;
    float M31, M32, M33, M34;
    float M41, M42, M43, M44;
};

struct ESPPlayer {
    uint8_t FixEsp{};
    uint8_t IsDieing{};
    Vector3 HeadPos;
    Vector3 RootPos;
    int MaxHp{};
    int Hp{};
    float distance{};
    char name[32]{};
};

struct ESPPacket {
    int widthGame{};
    int heightGame{};
    Matrix4x4 ViewMatrix{};
    Vector3 MainCameraTransformPos{};
    Vector3 Taget;
    int count{};
    int EnemyCount{};
    ESPPlayer players[64];
};

// ============================================
// WORKING OFFSETS (from your offsets.h)
// ============================================
#define OFFSET_CURRENT_MATCH        0x6EE0BB8
#define OFFSET_HEAD_TF              0x648
#define OFFSET_IS_CLIENT_BOT        0x450
#define OFFSET_IS_DIEING            0x683A614
#define OFFSET_GET_CUR_HP           0x68AD85C
#define OFFSET_TRANSFORM            0x10

// ============================================
// GLOBAL VARIABLES
// ============================================
int gamePid = 0;
uintptr_t libBase = 0;
int sock = -1;
bool running = true;

// ============================================
// UTILITY FUNCTIONS
// ============================================

void log_msg(const char* tag, const char* msg) {
    FILE* log = fopen("/data/local/tmp/reader.log", "a");
    if (log) {
        fprintf(log, "[%s] %s\n", tag, msg);
        fclose(log);
    }
}

int FindGamePid() {
    FILE* fp = popen("pidof com.dts.freefiremax 2>/dev/null", "r");
    if (!fp) return 0;
    int pid = 0;
    fscanf(fp, "%d", &pid);
    pclose(fp);
    return pid;
}

uintptr_t GetModuleBase(int pid, const char* module) {
    char mapsPath[256];
    snprintf(mapsPath, sizeof(mapsPath), "/proc/%d/maps", pid);
    FILE* fp = fopen(mapsPath, "r");
    if (!fp) return 0;

    char line[512];
    uintptr_t base = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, module) && strstr(line, "r--p")) {
            sscanf(line, "%lx-", &base);
            break;
        }
    }
    fclose(fp);
    return base;
}

template<typename T>
T ReadMemory(uintptr_t address) {
    T result = T();
    if (address == 0 || gamePid == 0) return result;

    char memPath[256];
    snprintf(memPath, sizeof(memPath), "/proc/%d/mem", gamePid);
    FILE* fp = fopen(memPath, "rb");
    if (!fp) return result;

    fseek(fp, address, SEEK_SET);
    fread(&result, sizeof(T), 1, fp);
    fclose(fp);
    return result;
}

Vector3 GetPosition(uintptr_t transform) {
    Vector3 pos;
    pos.x = ReadMemory<float>(transform + OFFSET_HEAD_TF);
    pos.y = ReadMemory<float>(transform + OFFSET_HEAD_TF + 4);
    pos.z = ReadMemory<float>(transform + OFFSET_HEAD_TF + 8);
    return pos;
}

// ============================================
// SOCKET FUNCTIONS
// ============================================

void ConnectToApp() {
    while (running && sock < 0) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            sleep(1);
            continue;
        }

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(7743);
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            close(sock);
            sock = -1;
            sleep(1);
        } else {
            log_msg("SOCKET", "Connected to DJ Gaming App");
        }
    }
}

// ============================================
// MAIN MEMORY READING LOOP
// ============================================

void ReadGameMemory() {
    log_msg("READER", "Memory reader started");

    while (running) {
        // Find game process
        if (gamePid == 0) {
            gamePid = FindGamePid();
            if (gamePid == 0) {
                sleep(1);
                continue;
            }
            char msg[64];
            snprintf(msg, sizeof(msg), "Found Free Fire PID: %d", gamePid);
            log_msg("GAME", msg);
        }

        // Find libil2cpp base
        if (libBase == 0) {
            libBase = GetModuleBase(gamePid, "libil2cpp.so");
            if (libBase == 0) {
                sleep(1);
                continue;
            }
            char msg[64];
            snprintf(msg, sizeof(msg), "libil2cpp base: 0x%lx", libBase);
            log_msg("GAME", msg);
        }

        // Ensure socket connection
        if (sock < 0) {
            ConnectToApp();
            sleep(1);
            continue;
        }

        // Get CurrentMatch
        uintptr_t currentMatch = ReadMemory<uintptr_t>(libBase + OFFSET_CURRENT_MATCH);
        if (currentMatch == 0) {
            usleep(50000);
            continue;
        }

        // Get LocalPlayer
        uintptr_t localPlayer = ReadMemory<uintptr_t>(currentMatch + 0xB0);
        if (localPlayer == 0) {
            usleep(50000);
            continue;
        }

        // Get local player position
        uintptr_t localTransform = ReadMemory<uintptr_t>(localPlayer + OFFSET_TRANSFORM);
        Vector3 localPos = GetPosition(localTransform);

        // Get dictionary and player list
        uintptr_t dictionary = ReadMemory<uintptr_t>(currentMatch + 0xC0);
        if (dictionary == 0) {
            usleep(50000);
            continue;
        }

        uintptr_t values = ReadMemory<uintptr_t>(dictionary + 0x18);
        int numValues = ReadMemory<int>(dictionary + 0x10);

        if (numValues > 100) numValues = 100;

        // Prepare packet
        ESPPacket packet;
        packet.count = 0;
        packet.widthGame = 1080;
        packet.heightGame = 2400;

        // Simple identity matrix for world-to-screen
        memset(&packet.ViewMatrix, 0, sizeof(packet.ViewMatrix));
        packet.ViewMatrix.M11 = 1;
        packet.ViewMatrix.M22 = 1;
        packet.ViewMatrix.M33 = 1;
        packet.ViewMatrix.M44 = 1;

        // Loop through players
        for (int i = 0; i < numValues && packet.count < 64; i++) {
            uintptr_t enemy = ReadMemory<uintptr_t>(values + (i * 8));
            if (enemy == 0 || enemy == localPlayer) continue;

            // Check if dead
            bool isDead = ReadMemory<bool>(enemy + OFFSET_IS_DIEING);
            if (isDead) continue;

            // Get enemy position
            uintptr_t enemyTransform = ReadMemory<uintptr_t>(enemy + OFFSET_TRANSFORM);
            if (enemyTransform == 0) continue;

            Vector3 enemyPos = GetPosition(enemyTransform);

            // Calculate distance
            float dx = localPos.x - enemyPos.x;
            float dy = localPos.y - enemyPos.y;
            float dz = localPos.z - enemyPos.z;
            float distance = sqrtf(dx*dx + dy*dy + dz*dz);

            if (distance > 200.0f) continue;

            // Get health
            int health = ReadMemory<int>(OFFSET_GET_CUR_HP);
            if (health <= 0) health = 100;

            ESPPlayer& player = packet.players[packet.count];
            player.HeadPos = enemyPos;
            player.RootPos = enemyPos;
            player.RootPos.y -= 1.5f;
            player.Hp = health;
            player.MaxHp = 200;
            player.distance = distance;
            snprintf(player.name, sizeof(player.name), "Enemy");

            packet.count++;
        }

        // Send packet if players found
        if (packet.count > 0) {
            send(sock, &packet, sizeof(packet), MSG_NOSIGNAL);
            char msg[64];
            snprintf(msg, sizeof(msg), "Sent %d players", packet.count);
            log_msg("ESP", msg);
        }

        usleep(50000); // 20 FPS
    }
}

// ============================================
// MAIN FUNCTION
// ============================================

int main() {
    log_msg("MAIN", "Standalone Memory Reader Starting...");

    // Daemonize (run in background)
    if (fork() > 0) {
        return 0; // Parent exits
    }

    setsid(); // Create new session

    // Redirect stdout/stderr to /dev/null
    freopen("/dev/null", "r", stdin);
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);

    // Run memory reader
    ReadGameMemory();

    return 0;
}