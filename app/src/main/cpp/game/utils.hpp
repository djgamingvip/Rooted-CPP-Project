#pragma once
#include <cstdint>
#include <cmath>
#include "../Socket.h"

// ── Memory ──────────────────────────────────────────────────────────────────

template<typename T>
inline T ReadMem(int pid, uint64_t address) {
    T result = T{};
    if (!address || !pid) return result;
    struct iovec local  = { &result, sizeof(T) };
    struct iovec remote = { reinterpret_cast<void*>(static_cast<uintptr_t>(address)), sizeof(T) };
    process_vm_readv(pid, &local, 1, &remote, 1, 0);
    return result;
}

inline uint64_t ReadPtr(int pid, uint64_t address) {
    return ReadMem<uint64_t>(pid, address);
}

// ── Math helpers ─────────────────────────────────────────────────────────────

inline float Distance3D(const Vector3& a, const Vector3& b) {
    float dx = a.x - b.x, dy = a.y - b.y, dz = a.z - b.z;
    return sqrtf(dx*dx + dy*dy + dz*dz);
}

inline float Distance2D(float x1, float y1, float x2, float y2) {
    float dx = x1 - x2, dy = y1 - y2;
    return sqrtf(dx*dx + dy*dy);
}
