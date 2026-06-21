#include "Socket.h"
#include <thread>
#include <atomic>

ESPPacket g_packet;
std::mutex g_mutex;
int g_client = -1;
ControlPacket g_control;
std::atomic<bool> serverRunning{true};

void startSocketServer() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        LOGE("Failed to create socket");
        return;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(7743);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        LOGE("Bind failed");
        close(server_fd);
        return;
    }

    if (listen(server_fd, 1) < 0) {
        LOGE("Listen failed");
        close(server_fd);
        return;
    }

    LOGI("Socket server listening on port 7743");

    g_client = accept(server_fd, nullptr, nullptr);
    if (g_client < 0) {
        LOGE("Accept failed");
        close(server_fd);
        return;
    }

    int flag = 1;
    setsockopt(g_client, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
    LOGI("Client connected!");

    // Receive control packets in background
    std::thread receiver([]() {
        while (serverRunning && g_client > 0) {
            ControlPacket packet;
            int received = recv(g_client, &packet, sizeof(packet), 0);
            if (received == sizeof(packet)) {
                std::lock_guard<std::mutex> lock(g_mutex);
                g_control = packet;
                LOGI("Received control packet - FlyUp: %d, FlyMap: %d, Football: %d",
                        packet.FlyUp, packet.FlyMap, packet.FootballHack);
            } else if (received <= 0) {
                break;
            }
        }
    });
    receiver.detach();
}

void sendControlPacket() {
    if (g_client > 0) {
        send(g_client, &g_control, sizeof(g_control), MSG_NOSIGNAL);
    }
}