#pragma once
#include <cmath>

// Extended Vector3 with operator overloads and common utilities.
// The plain Vector3 POD in Socket.h is kept for packet compatibility;
// this version is used in game-logic code.
struct Vec3 {
    float x, y, z;

    Vec3()                        : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vec3  operator+(const Vec3& o) const { return {x+o.x, y+o.y, z+o.z}; }
    Vec3  operator-(const Vec3& o) const { return {x-o.x, y-o.y, z-o.z}; }
    Vec3  operator*(float s)       const { return {x*s,   y*s,   z*s};   }
    Vec3& operator+=(const Vec3& o)      { x+=o.x; y+=o.y; z+=o.z; return *this; }
    Vec3& operator-=(const Vec3& o)      { x-=o.x; y-=o.y; z-=o.z; return *this; }

    float Length()    const { return sqrtf(x*x + y*y + z*z); }
    float LengthSq()  const { return x*x + y*y + z*z; }
    Vec3  Normalized() const {
        float l = Length();
        return (l > 1e-6f) ? Vec3(x/l, y/l, z/l) : Vec3();
    }

    static float Dot(const Vec3& a, const Vec3& b) {
        return a.x*b.x + a.y*b.y + a.z*b.z;
    }
    static Vec3 Cross(const Vec3& a, const Vec3& b) {
        return { a.y*b.z - a.z*b.y,
                 a.z*b.x - a.x*b.z,
                 a.x*b.y - a.y*b.x };
    }
    static float Distance(const Vec3& a, const Vec3& b) {
        return (a - b).Length();
    }
};
