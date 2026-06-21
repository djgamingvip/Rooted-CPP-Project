#pragma once
#include <jni.h>
#include <string>
#include <vector>
#include <cstring>
#include <cmath>

// Vector2 is defined in Socket.h - don't redefine it
// Just include Socket.h or forward declare
#ifndef VECTOR2_DEFINED
struct Vector2;
#endif

struct Color {
    int r, g, b, a;
    Color(int R, int G, int B, int A) : r(R), g(G), b(B), a(A) {}
    Color(int R, int G, int B) : r(R), g(G), b(B), a(255) {}
    static Color White() { return Color(255, 255, 255, 255); }
    static Color Black() { return Color(0, 0, 0, 255); }
    static Color Red() { return Color(255, 0, 0, 255); }
    static Color Green() { return Color(0, 255, 0, 255); }
    static Color Blue() { return Color(0, 0, 255, 255); }
    static Color Yellow() { return Color(255, 255, 0, 255); }
};

class PowerDrawing {
private:
    JNIEnv* _env;
    jobject espview;
    jobject canvas;

    std::string sanitizeUTF8(const char* text) {
        if (!text || text[0] == '\0') return "";
        std::string result;
        const unsigned char* p = (const unsigned char*)text;
        while (*p) {
            if (*p <= 0x7F) {
                result += (char)*p++;
            } else if ((*p & 0xE0) == 0xC0 && (p[1] & 0xC0) == 0x80) {
                result += (char)*p++;
                result += (char)*p++;
            } else if ((*p & 0xF0) == 0xE0 && (p[1] & 0xC0) == 0x80 && (p[2] & 0xC0) == 0x80) {
                result += (char)*p++;
                result += (char)*p++;
                result += (char)*p++;
            } else if ((*p & 0xF8) == 0xF0 && (p[1] & 0xC0) == 0x80 && (p[2] & 0xC0) == 0x80 &&
                    (p[3] & 0xC0) == 0x80) {
                result += (char)*p++;
                result += (char)*p++;
                result += (char)*p++;
                result += (char)*p++;
            } else {
                p++;
            }
        }
        return result;
    }

public:
    PowerDrawing() : _env(nullptr), espview(nullptr), canvas(nullptr) {}

    PowerDrawing(JNIEnv* env, jobject esp, jobject cvs)
            : _env(env), espview(esp), canvas(cvs) {}

    bool isValid() const {
        return _env && espview && canvas;
    }

    int getWidth() {
        if (isValid()) {
            jclass cls = _env->GetObjectClass(canvas);
            jmethodID mid = _env->GetMethodID(cls, "getWidth", "()I");
            return _env->CallIntMethod(canvas, mid);
        }
        return 0;
    }

    int getHeight() {
        if (isValid()) {
            jclass cls = _env->GetObjectClass(canvas);
            jmethodID mid = _env->GetMethodID(cls, "getHeight", "()I");
            return _env->CallIntMethod(canvas, mid);
        }
        return 0;
    }

    void DrawLine(Color color, float thickness, Vector2 from, Vector2 to) {
        if (isValid()) {
            jclass cls = _env->GetObjectClass(espview);
            jmethodID mid = _env->GetMethodID(cls, "DrawLine",
                    "(Landroid/graphics/Canvas;IIIIFFFFF)V");
            _env->CallVoidMethod(espview, mid, canvas, color.a, color.r, color.g, color.b,
                    thickness, from.x, from.y, to.x, to.y);
        }
    }

    void DrawBox(Color color, float thickness, Vector2 topLeft, Vector2 bottomRight) {
        if (isValid()) {
            jclass cls = _env->GetObjectClass(espview);
            jmethodID mid = _env->GetMethodID(cls, "DrawBox",
                    "(Landroid/graphics/Canvas;IIIIFFFFF)V");
            _env->CallVoidMethod(espview, mid, canvas, color.a, color.r, color.g, color.b,
                    thickness, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
        }
    }

    void DrawFilledBox(Color color, Vector2 topLeft, Vector2 bottomRight) {
        if (isValid()) {
            jclass cls = _env->GetObjectClass(espview);
            jmethodID mid = _env->GetMethodID(cls, "DrawFilledBox",
                    "(Landroid/graphics/Canvas;IIIIFFFF)V");
            if (mid) {
                _env->CallVoidMethod(espview, mid, canvas, color.a, color.r, color.g, color.b,
                        topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
            } else {
                __android_log_print(ANDROID_LOG_ERROR, "PowerDrawing", "DrawFilledBox method not found!");
            }
        }
    }

    void DrawCircle(Color color, float thickness, Vector2 center, float radius) {
        if (isValid()) {
            jclass cls = _env->GetObjectClass(espview);
            jmethodID mid = _env->GetMethodID(cls, "DrawCircle",
                    "(Landroid/graphics/Canvas;IIIIFFFF)V");
            _env->CallVoidMethod(espview, mid, canvas, color.a, color.r, color.g, color.b,
                    thickness, center.x, center.y, radius);
        }
    }

    void DrawCircleFilled(Color color, Vector2 center, float radius) {
        if (isValid()) {
            jclass cls = _env->GetObjectClass(espview);
            jmethodID mid = _env->GetMethodID(cls, "DrawCircleFilled",
                    "(Landroid/graphics/Canvas;IIIIFFF)V");
            _env->CallVoidMethod(espview, mid, canvas, color.a, color.r, color.g, color.b,
                    center.x, center.y, radius);
        }
    }

    void DrawFilledTriangle(Color color, Vector2 p1, Vector2 p2, Vector2 p3) {
        if (isValid()) {
            jclass cls = _env->GetObjectClass(espview);
            jmethodID mid = _env->GetMethodID(cls, "DrawFilledTriangle",
                    "(Landroid/graphics/Canvas;IIIIFFFFFF)V");
            _env->CallVoidMethod(espview, mid, canvas, color.a, color.r, color.g, color.b,
                    p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
        }
    }

    void DrawText(Color color, float size, Vector2 pos, const char* text) {
        if (!isValid() || !text) return;

        std::string safe = sanitizeUTF8(text);
        if (safe.empty()) return;

        jclass cls = _env->GetObjectClass(espview);
        jmethodID mid = _env->GetMethodID(cls, "DrawText",
                "(Landroid/graphics/Canvas;IIIIFFFLjava/lang/String;)V");

        jstring jtext = _env->NewStringUTF(safe.c_str());
        _env->CallVoidMethod(espview, mid, canvas, color.a, color.r, color.g, color.b,
                size, pos.x, pos.y, jtext);
        _env->DeleteLocalRef(jtext);
    }

    float MeasureText(const char* text, float size) {
        if (!isValid() || !text) return 0;

        std::string safe = sanitizeUTF8(text);
        if (safe.empty()) return 0;

        jclass cls = _env->GetObjectClass(espview);
        jmethodID mid = _env->GetMethodID(cls, "MeasureText",
                "(Ljava/lang/String;F)F");

        jstring jtext = _env->NewStringUTF(safe.c_str());
        float width = _env->CallFloatMethod(espview, mid, jtext, size);
        _env->DeleteLocalRef(jtext);

        return width;
    }
};