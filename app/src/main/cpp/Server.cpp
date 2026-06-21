// Server.cpp - This needs to run as a separate process
#include "Socket.h"
#include <dlfcn.h>
#include <unistd.h>
#include <thread>

uintptr_t GetModuleBase(const char* module) {
    FILE* fp = fopen("/proc/self/maps", "r");
    if (!fp) return 0;
    char line[512];
    uintptr_t base = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, module)) {
            sscanf(line, "%lx-%*lx", &base);
            break;
        }
    }
    fclose(fp);
    return base;
}

// This would read game memory and send ESPPacket
void GameMemoryReader() {
    // Wait for game to load
    uintptr_t libBase = 0;
    while (libBase == 0) {
        libBase = GetModuleBase("libil2cpp.so");
        sleep(1);
    }

    // Use your offsets to read game data
    uintptr_t gameFacade = libBase + 0x7644900; // Your offset

    ESPPacket packet;

    while (true) {
        // Read player data from game memory
        // ...

        // Send to socket
        if (g_client > 0) {
            send(g_client, &packet, sizeof(packet), MSG_NOSIGNAL);
        }
        usleep(16000); // ~60 FPS
    }
}