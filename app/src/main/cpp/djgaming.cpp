#include "dobby.h"
#include <jni.h>
#include <string>
#include <android/log.h>
#include <dlfcn.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <cstring>
#include <algorithm>

// Include headers
#include "Socket.h"
#include "PowerTypes.h"
#include "DrawEsp.h"
#include "MemoryReader.cpp"  // Memory reading functions

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "DJGaming", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "DJGaming", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "DJGaming", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, "DJGaming", __VA_ARGS__)

// ============================================
// GLOBAL VARIABLES - ESP (Add these)
// ============================================
bool EnableEsp = false;
bool EspLine = false;
bool EspBox = false;
bool EspSkeleton = false;
bool EspTarget = false;
bool EspDistance = false;
bool EspName = false;
bool EnemyCounter = false;
bool EspHealth = false;
bool ShowFov = false;
bool EspDirection = false;
float FovDirection = 130.0f;
float ESP_OFFSET_X = 0.0f;

// ============================================
// GLOBAL VARIABLES - AIMBOT (Add these)
// ============================================
int AimFov = 120;
int HeadshotRate = 50;
float AIM_SMOOTH = 0.2f;
bool AimSilent = false;
bool AimBotLite = false;
bool AimMagnet = false;
int TargetIn = 1;
int WhenAim = 1;

// ============================================
// GLOBAL VARIABLES - FLY FEATURES
// ============================================
bool IsPlayerFlying = false;
bool FlyMap = false;
bool FootballHack = false;
bool BallUnlimitedMarcosMods = false;
Vector3 SavedGroundPos;
Vector3 SetMap;
int HighVp = 0;
const int MaxFlyHeight = 6;

// ============================================
// SOCKET CONTROL UPDATE FUNCTION
// ============================================
void UpdateControlPacket() {
    std::lock_guard<std::mutex> lock(g_mutex);

    g_control.EnableEsp = EnableEsp;
    g_control.EspLine = EspLine;
    g_control.EspBox = EspBox;
    g_control.EspSkeleton = EspSkeleton;
    g_control.EspTarget = EspTarget;
    g_control.EspDistance = EspDistance;
    g_control.EspName = EspName;
    g_control.EnemyCounter = EnemyCounter;
    g_control.EspHealth = EspHealth;
    g_control.ShowFov = ShowFov;
    g_control.EspDirection = EspDirection;
    g_control.FovDirection = FovDirection;
    g_control.ESP_OFFSET_X = ESP_OFFSET_X;
    g_control.AimFov = AimFov;
    g_control.HeadshotRate = HeadshotRate;
    g_control.AIM_SMOOTH = AIM_SMOOTH;
    g_control.AimSilent = AimSilent;
    g_control.AimBotLite = AimBotLite;
    g_control.AimMagnet = AimMagnet;
    g_control.TargetIn = TargetIn;
    g_control.WhenAim = WhenAim;
    g_control.FlyUp = IsPlayerFlying;
    g_control.FlyMap = FlyMap;
    g_control.FootballHack = FootballHack;

    sendControlPacket();
}

// ============================================
// JNI EXPORTS
// ============================================
extern "C" {

JNIEXPORT jstring JNICALL
Java_com_vip_djgaming_MainActivity_getMessage(JNIEnv* env, jclass /* clazz */) {
    return env->NewStringUTF("DJ GAMING VIP - Fully Loaded");
}

// FLY FEATURES
JNIEXPORT void JNICALL
Java_com_vip_djgaming_MainActivity_setFly(JNIEnv *env, jclass clazz, jboolean fly) {
    IsPlayerFlying = fly;
    LOGI("✈️ Fly mode: %s", fly ? "ON" : "OFF");
    UpdateControlPacket();
}

JNIEXPORT void JNICALL
Java_com_vip_djgaming_MainActivity_setFlyMap(JNIEnv *env, jclass clazz, jboolean flyMap) {
    FlyMap = flyMap;
    LOGI("🗺️ Fly Map: %s", flyMap ? "ON" : "OFF");
    UpdateControlPacket();
}

JNIEXPORT void JNICALL
Java_com_vip_djgaming_MainActivity_setFootballHack(JNIEnv *env, jclass clazz, jboolean active) {
    FootballHack = active;
    BallUnlimitedMarcosMods = active;
    LOGI("⚽ Football Hack: %s", active ? "ON" : "OFF");
    UpdateControlPacket();
}

// ESP FEATURES
JNIEXPORT void JNICALL
Java_com_vip_djgaming_MainActivity_setEsp(JNIEnv *env, jclass clazz, jboolean enable) {
    EnableEsp = enable;
    LOGI("👁️ ESP: %s", enable ? "ON" : "OFF");
    UpdateControlPacket();
}

JNIEXPORT void JNICALL
Java_com_vip_djgaming_MainActivity_setEspLine(JNIEnv *env, jclass clazz, jboolean enable) {
    EspLine = enable;
    LOGI("📏 ESP Line: %s", enable ? "ON" : "OFF");
    UpdateControlPacket();
}

JNIEXPORT void JNICALL
Java_com_vip_djgaming_MainActivity_setEspBox(JNIEnv *env, jclass clazz, jboolean enable) {
    EspBox = enable;
    LOGI("📦 ESP Box: %s", enable ? "ON" : "OFF");
    UpdateControlPacket();
}

JNIEXPORT void JNICALL
Java_com_vip_djgaming_MainActivity_setEspName(JNIEnv *env, jclass clazz, jboolean enable) {
    EspName = enable;
    LOGI("📝 ESP Name: %s", enable ? "ON" : "OFF");
    UpdateControlPacket();
}

JNIEXPORT void JNICALL
Java_com_vip_djgaming_MainActivity_setEspDistance(JNIEnv *env, jclass clazz, jboolean enable) {
    EspDistance = enable;
    LOGI("📏 ESP Distance: %s", enable ? "ON" : "OFF");
    UpdateControlPacket();
}

JNIEXPORT void JNICALL
Java_com_vip_djgaming_MainActivity_setEspHealth(JNIEnv *env, jclass clazz, jboolean enable) {
    EspHealth = enable;
    LOGI("❤️ ESP Health: %s", enable ? "ON" : "OFF");
    UpdateControlPacket();
}

JNIEXPORT void JNICALL
Java_com_vip_djgaming_MainActivity_setEnemyCounter(JNIEnv *env, jclass clazz, jboolean enable) {
    EnemyCounter = enable;
    LOGI("👥 Enemy Counter: %s", enable ? "ON" : "OFF");
    UpdateControlPacket();
}

JNIEXPORT void JNICALL
Java_com_vip_djgaming_MainActivity_setShowFov(JNIEnv *env, jclass clazz, jboolean enable) {
    ShowFov = enable;
    LOGI("🎯 Show FOV: %s", enable ? "ON" : "OFF");
    UpdateControlPacket();
}

JNIEXPORT void JNICALL
Java_com_vip_djgaming_MainActivity_setEspOffsetX(JNIEnv *env, jclass clazz, jfloat offset) {
    ESP_OFFSET_X = offset;
    LOGI("📐 ESP Offset X: %.1f", offset);
    UpdateControlPacket();
}

// AIMBOT FEATURES
JNIEXPORT void JNICALL
Java_com_vip_djgaming_MainActivity_setAimbot(JNIEnv *env, jclass clazz, jboolean active) {
    AimBotLite = active;
    LOGI("🎯 Aimbot: %s", active ? "ON" : "OFF");
    UpdateControlPacket();
}

JNIEXPORT void JNICALL
Java_com_vip_djgaming_MainActivity_setAimSilent(JNIEnv *env, jclass clazz, jboolean active) {
    AimSilent = active;
    LOGI("🔫 Silent Aim: %s", active ? "ON" : "OFF");
    UpdateControlPacket();
}

JNIEXPORT void JNICALL
Java_com_vip_djgaming_MainActivity_setAimFov(JNIEnv *env, jclass clazz, jint fov) {
    AimFov = fov;
    LOGI("🎯 Aim FOV: %d", fov);
    UpdateControlPacket();
}

// DRAWING
JNIEXPORT void JNICALL
Java_com_vip_djgaming_MainActivity_drawESP(JNIEnv* env, jclass clazz, jobject espView, jobject canvas) {
    DrawESP(env, espView, canvas);
}

// INITIALIZATION
JNIEXPORT void JNICALL
Java_com_vip_djgaming_MainActivity_initNative(JNIEnv *env, jclass clazz) {
    LOGI("========================================");
    LOGI("🎮 DJ Gaming VIP - Initialized");
    LOGI("========================================");

    // Start socket server
    std::thread([]() {
        startSocketServer();
    }).detach();

    // Start memory reader
    StartMemoryReader();

    LOGI("✅ Socket server started on port 7743");
    LOGI("✅ Memory reader started");
}

JNIEXPORT jstring JNICALL
Java_com_vip_djgaming_MainActivity_testNative(JNIEnv* env, jclass clazz) {
    LOGI("✅ Native bridge working!");
    return env->NewStringUTF("✅ DJ Gaming VIP - Ready! ESP + Aimbot Active");
}

} // extern "C"