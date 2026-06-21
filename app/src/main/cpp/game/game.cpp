#include "game.hpp"
#include "offsets.h"
#include <android/log.h>
#include <dirent.h>
#include <fcntl.h>
#include <cstring>
#include <cstdio>
#include <csignal>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Game", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "Game", __VA_ARGS__)

// Static member definitions
int      Game::gamePid = 0;
uint64_t Game::libBase = 0;

int Game::GetPid() {
    if (gamePid != 0 && kill(gamePid, 0) == 0) return gamePid;

    DIR* dir = opendir("/proc");
    if (!dir) return 0;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        int pid = atoi(entry->d_name);
        if (pid <= 0) continue;

        char path[256];
        snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
        int fd = open(path, O_RDONLY);
        if (fd < 0) continue;

        char cmdline[256] = {};
        ssize_t len = read(fd, cmdline, sizeof(cmdline) - 1);
        close(fd);

        if (len > 0 && strcmp(cmdline, "com.dts.freefiremax") == 0) {
            closedir(dir);
            gamePid = pid;
            LOGI("Found game PID: %d", pid);
            return pid;
        }
    }
    closedir(dir);
    return 0;
}

uint64_t Game::GetLibBase() {
    if (libBase != 0) return libBase;

    int pid = GetPid();
    if (pid == 0) return 0;

    char mapsPath[256];
    snprintf(mapsPath, sizeof(mapsPath), "/proc/%d/maps", pid);
    FILE* fp = fopen(mapsPath, "r");
    if (!fp) return 0;

    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "libil2cpp.so") && (strstr(line, "r--p") || strstr(line, "r-xp"))) {
            libBase = strtoull(line, nullptr, 16);
            fclose(fp);
            LOGI("libil2cpp.so base: 0x%llx", (unsigned long long)libBase);
            return libBase;
        }
    }
    fclose(fp);
    return 0;
}

bool Game::IsValid() {
    return GetPid() != 0 && GetLibBase() != 0;
}

void Game::Update() {
    // Called periodically to refresh PID / base if game restarted
    if (gamePid != 0 && kill(gamePid, 0) == -1) {
        LOGI("Game process gone — resetting");
        gamePid = 0;
        libBase = 0;
    }
    GetPid();
    GetLibBase();
}
