#pragma once
#include <cstdint>
#include "../Socket.h"

// ── IL2CPP type helpers ───────────────────────────────────────────────────────

// Generic IL2CPP object header
struct Il2CppObject {
    uint64_t klass;  // pointer to Il2CppClass
    uint64_t monitor;
};

// IL2CPP managed string layout
struct Il2CppString {
    Il2CppObject object;
    int32_t      length;
    uint16_t     chars[1]; // UTF-16 chars follow
};

// Generic IL2CPP array header
template<typename T>
struct Il2CppArray {
    Il2CppObject object;
    uint64_t     bounds;
    uint32_t     max_length;
    T            vector[1];
};

// ── Unity math types ─────────────────────────────────────────────────────────
// (Vector2 / Vector3 / Matrix4x4 are already in Socket.h — no re-definition)

struct Quaternion {
    float x, y, z, w;
    Quaternion() : x(0), y(0), z(0), w(1) {}
    Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
};

// ── Camera projection helper ──────────────────────────────────────────────────
// Returns false when the point is behind the camera.
inline bool WorldToScreen(const Vector3& worldPos,
                          const Matrix4x4& vp,
                          int screenW, int screenH,
                          Vector2& outScreen) {
    float clipX = vp.M11 * worldPos.x + vp.M21 * worldPos.y + vp.M31 * worldPos.z + vp.M41;
    float clipY = vp.M12 * worldPos.x + vp.M22 * worldPos.y + vp.M32 * worldPos.z + vp.M42;
    float clipW = vp.M14 * worldPos.x + vp.M24 * worldPos.y + vp.M34 * worldPos.z + vp.M44;

    if (clipW < 0.01f) return false;

    float ndcX = clipX / clipW;
    float ndcY = clipY / clipW;

    outScreen.x = (ndcX + 1.0f) * 0.5f * static_cast<float>(screenW);
    outScreen.y = (1.0f - ndcY) * 0.5f * static_cast<float>(screenH);
    return true;
}
