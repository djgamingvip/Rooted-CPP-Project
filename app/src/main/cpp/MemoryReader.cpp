#include "Socket.h"
#include <android/log.h>
#include <unistd.h>
#include <thread>
#include <atomic>
#include <cstring>
#include <cmath>
#include <sys/uio.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/syscall.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "MemoryReader", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "MemoryReader", __VA_ARGS__)

std::atomic<bool> memoryReaderRunning{true};
int gamePid = 0;
uint64_t libBase = 0;

// Working offsets (Using 64-bit logic)
#define OFFSET_GAME_FACADE_CURRENT_MATCH    0x3266cc0
#define OFFSET_PLAYER_HEAD_TF              0x5c0
#define OFFSET_PLAYER_IS_CLIENT_BOT        0x3d8
#define OFFSET_PLAYER_GET_CUR_HP           0x2e3e1ac
#define OFFSET_PLAYER_GET_NICKNAME         0x2dcfddc
#define OFFSET_TRANSFORM                   0x10
#define OFFSET_TRANSFORM_POSITION          0x40

// More reliable PID finding by iterating /proc manually
int FindGamePid() {
    DIR* dir = opendir("/proc");
    if (!dir) return 0;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        int pid = atoi(entry->d_name);
        if (pid <= 0) continue;
        char cmdPath[256];
        snprintf(cmdPath, sizeof(cmdPath), "/proc/%d/cmdline", pid);
        int fd = open(cmdPath, O_RDONLY);
        if (fd >= 0) {
            char cmdline[256];
            ssize_t len = read(fd, cmdline, sizeof(cmdline) - 1);
            close(fd);
            if (len > 0) {
                cmdline[len] = '\0';
                // Check for Free Fire Max package name
                if (strcmp(cmdline, "com.dts.freefiremax") == 0) {
                    closedir(dir);
                    return pid;
                }
            }
        }
    }
    closedir(dir);
    return 0;
}

// Improved Module Base finding supporting 64-bit addresses
uint64_t GetModuleBase(int pid, const char* modName) {
    char mapsPath[256];
    snprintf(mapsPath, sizeof(mapsPath), "/proc/%d/maps", pid);
    FILE* fp = fopen(mapsPath, "r");
    if (!fp) return 0;

    char line[1024];
    uint64_t base = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, modName)) {
            base = strtoull(line, NULL, 16);
            if (strstr(line, "r--p") || strstr(line, "r-xp")) {
                break; // Found the primary segment
            }
        }
    }
    fclose(fp);
    return base;
}

// Memory read using process_vm_readv
// Uses uint64_t for address to prevent truncation on 64-bit targets
template<typename T>
T ReadProcessMemory(uint64_t address) {
    T result = T();
    if (address == 0 || gamePid == 0) return result;

    struct iovec local = {&result, sizeof(T)};
    struct iovec remote = {(void*)(uintptr_t)address, sizeof(T)};

    // process_vm_readv is the most efficient way to read another process's memory
    // requires same UID or root/CAP_SYS_PTRACE
    ssize_t bytes = process_vm_readv(gamePid, &local, 1, &remote, 1, 0);
    if (bytes < 0) {
        // Log error only occasionally to avoid spamming
        static int errorCount = 0;
        if (errorCount++ % 1000 == 0) {
            LOGE("process_vm_readv failed at 0x%llx: %s", address, strerror(errno));
        }
    }
    return result;
}

// Helper to read 64-bit pointers explicitly
uint64_t ReadPtr(uint64_t address) {
    return ReadProcessMemory<uint64_t>(address);
}

Vector3 GetPosition(uint64_t transform) {
    Vector3 pos;
    if (transform == 0) return pos;
    // Position is typically at 0x30 or 0x40 in IL2CPP Transform components
    pos.x = ReadProcessMemory<float>(transform + OFFSET_TRANSFORM_POSITION + 0);
    pos.y = ReadProcessMemory<float>(transform + OFFSET_TRANSFORM_POSITION + 4);
    pos.z = ReadProcessMemory<float>(transform + OFFSET_TRANSFORM_POSITION + 8);
    return pos;
}

void MemoryReaderThread() {
    LOGI("DJ Gaming Memory Reader Thread Started");

    while (memoryReaderRunning) {
        // 1. Ensure we have the Game PID
        if (gamePid == 0 || kill(gamePid, 0) == -1) {
            gamePid = FindGamePid();
            if (gamePid == 0) {
                usleep(2000000); // Wait 2s
                continue;
            }
            libBase = 0; // Reset base for new PID
            LOGI("✅ Found Game PID: %d", gamePid);
        }

        // 2. Ensure we have libil2cpp base address
        if (libBase == 0) {
            libBase = GetModuleBase(gamePid, "libil2cpp.so");
            if (libBase == 0) {
                usleep(1000000);
                continue;
            }
            LOGI("✅ libil2cpp.so Base: 0x%llx", libBase);
        }

        // 3. Get CurrentMatch pointer (Entry point for ESP)
        uint64_t currentMatch = ReadPtr(libBase + OFFSET_GAME_FACADE_CURRENT_MATCH);
        if (currentMatch == 0) {
            usleep(200000);
            continue;
        }

        // 4. Get LocalPlayer
        uint64_t localPlayer = ReadPtr(currentMatch + 0xB0);
        if (localPlayer == 0) {
            usleep(200000);
            continue;
        }

        // 5. Get Local Position (for distance calculation)
        uint64_t localTransform = ReadPtr(localPlayer + OFFSET_TRANSFORM);
        Vector3 localPos = GetPosition(localTransform);

        // 6. Get Player Dictionary/List
        uint64_t dictionary = ReadPtr(currentMatch + 0xC0);
        if (dictionary == 0) {
            usleep(200000);
            continue;
        }

        // Dictionary values array is usually at 0x18, count at 0x10
        uint64_t values = ReadPtr(dictionary + 0x18);
        int numValues = ReadProcessMemory<int>(dictionary + 0x10);

        if (numValues <= 0 || numValues > 1000) {
            usleep(200000);
            continue;
        }

        ESPPacket packet;
        packet.count = 0;
        packet.widthGame = 1080;
        packet.heightGame = 2400;

        // Identity ViewMatrix - App UI should handle real world-to-screen if possible
        memset(&packet.ViewMatrix, 0, sizeof(packet.ViewMatrix));
        packet.ViewMatrix.M11 = 1.0f; packet.ViewMatrix.M22 = 1.0f;
        packet.ViewMatrix.M33 = 1.0f; packet.ViewMatrix.M44 = 1.0f;

        // 7. Iterate through players
        for (int i = 0; i < numValues && packet.count < 64; i++) {
            // IL2CPP Array elements start at 0x20 offset and are 8 bytes apart in 64-bit
            uint64_t enemy = ReadPtr(values + 0x20 + (i * 8));
            if (enemy == 0 || enemy == localPlayer) continue;

            uint64_t enemyTransform = ReadPtr(enemy + OFFSET_TRANSFORM);
            if (enemyTransform == 0) continue;

            Vector3 enemyPos = GetPosition(enemyTransform);

            // Calculate distance
            float dx = localPos.x - enemyPos.x;
            float dy = localPos.y - enemyPos.y;
            float dz = localPos.z - enemyPos.z;
            float distance = sqrtf(dx*dx + dy*dy + dz*dz);

            // Filter by distance (200m usually enough for ESP)
            if (distance > 300.0f || distance < 0.5f) continue;

            // Get Health (Checking if it's a field offset)
            int health = 100;
            if (OFFSET_PLAYER_GET_CUR_HP < 0x1000) {
                health = ReadProcessMemory<int>(enemy + OFFSET_PLAYER_GET_CUR_HP);
            }

            ESPPlayer& p = packet.players[packet.count];
            p.HeadPos = enemyPos;
            p.RootPos = enemyPos;
            p.RootPos.y -= 1.6f; // Simple offset for feet
            p.Hp = (health > 0 && health <= 1000) ? health : 100;
            p.MaxHp = 200;
            p.distance = distance;

            // Read Player Name (IL2CPP String)
            uint64_t namePtr = ReadPtr(enemy + OFFSET_PLAYER_GET_NICKNAME);
            if (namePtr > 0x1000000) {
                int len = ReadProcessMemory<int>(namePtr + 0x10);
                if (len > 0 && len < 32) {
                    for (int j = 0; j < len; j++) {
                        // IL2CPP strings are UTF-16
                        p.name[j] = (char)ReadProcessMemory<uint16_t>(namePtr + 0x14 + (j * 2));
                    }
                    p.name[len] = '\0';
                } else {
                    strcpy(p.name, "Enemy");
                }
            } else {
                strcpy(p.name, "Enemy");
            }

            packet.count++;
        }

        // 8. Update global packet for the app to consume
        if (g_client > 0) {
            std::lock_guard<std::mutex> lock(g_mutex);
            g_packet = packet;
            if (packet.count > 0) {
                LOGI("📡 Found %d players in memory", packet.count);
            }
        }

        usleep(33000); // Update at ~30 FPS
    }
}

void StartMemoryReader() {
    std::thread reader(MemoryReaderThread);
    reader.detach();
    LOGI("✅ Memory Reader thread detached");
}
