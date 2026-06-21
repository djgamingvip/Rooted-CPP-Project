#pragma once
#include <cmath>

struct Matrix4x4 {
    float M11, M12, M13, M14;
    float M21, M22, M23, M24;
    float M31, M32, M33, M34;
    float M41, M42, M43, M44;
};

struct Vector2 {
    float x, y;
    Vector2() : x(0), y(0) {}
    Vector2(float X, float Y) : x(X), y(Y) {}
};

struct Vector3 {
    float x, y, z;
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}

    Vector3 operator+(const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
    Vector3 operator-(const Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
    Vector3 operator*(float s) const { return Vector3(x * s, y * s, z * s); }

    static float Dot(const Vector3& a, const Vector3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    static float Distance(const Vector3& a, const Vector3& b) {
        float dx = a.x - b.x;
        float dy = a.y - b.y;
        float dz = a.z - b.z;
        return sqrtf(dx * dx + dy * dy + dz * dz);
    }

    static Vector3 Normalize(const Vector3& v) {
        float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
        if (len > 0.0001f) return Vector3(v.x / len, v.y / len, v.z / len);
        return Vector3(0, 0, 0);
    }
};

struct Vector4 {
    float x, y, z, w;
};

struct Quaternion {
    float x, y, z, w;
    Quaternion() : x(0), y(0), z(0), w(1) {}
    Quaternion(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}
};

struct TMatrix {
    Vector4 position{};
    Quaternion rotation{};
    Vector4 scale{};
};