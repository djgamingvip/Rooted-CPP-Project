#pragma once

#include <mutex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <cstring>
#include <android/log.h>
#include <cmath>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Socket", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "Socket", __VA_ARGS__)

// Vector2 structure
struct Vector2 {
    float x, y;
    Vector2() : x(0), y(0) {}
    Vector2(float X, float Y) : x(X), y(Y) {}
};

// Vector3 structure
struct Vector3 {
    float x, y, z;
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
};

// Matrix4x4 structure
struct Matrix4x4 {
    float M11, M12, M13, M14;
    float M21, M22, M23, M24;
    float M31, M32, M33, M34;
    float M41, M42, M43, M44;
};

// ESP Player data
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

// ESP Packet sent from server to app
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

// Control Packet sent from app to server
struct ControlPacket {
    bool EnableEsp{false};
    bool EspLine{false};
    bool EspBox{false};
    bool EspSkeleton{false};
    bool EspTarget{false};
    bool EspDistance{false};
    bool EspName{false};
    bool EnemyCounter{false};
    bool EspHealth{false};
    bool ShowFov{false};
    bool EspDirection{false};
    float FovDirection{130.0f};
    float ESP_OFFSET_X{0.0f};

    int AimFov{120};
    int HeadshotRate{50};
    float AIM_SMOOTH{0.2f};
    bool AimSilent{false};
    bool AimBotLite{false};
    bool AimMagnet{false};
    int TargetIn{1};
    int WhenAim{1};
    bool AimLegit{false};

    // Fly features
    bool FlyUp{false};
    bool FlyMap{false};
    bool FootballHack{false};
};

extern ESPPacket g_packet;
extern std::mutex g_mutex;
extern int g_client;
extern ControlPacket g_control;

void startSocketServer();
void sendControlPacket();