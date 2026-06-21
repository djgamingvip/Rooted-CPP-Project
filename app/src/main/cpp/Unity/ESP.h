#pragma once
#include "Unity.h"
#include "../Socket.h"

// ── ESP rendering helpers ─────────────────────────────────────────────────────

// Computes a 2-D bounding box for a player given head + root world positions.
// Returns false if either point is behind the camera.
struct ESPBox2D {
    float left, top, right, bottom;
    float width()  const { return right  - left; }
    float height() const { return bottom - top;  }
};

inline bool GetPlayerBox(const ESPPlayer& p,
                         const Matrix4x4& vp,
                         int screenW, int screenH,
                         ESPBox2D& outBox) {
    Vector2 head, root;
    if (!WorldToScreen(p.HeadPos, vp, screenW, screenH, head)) return false;
    if (!WorldToScreen(p.RootPos, vp, screenW, screenH, root)) return false;

    float boxH  = fabsf(root.y - head.y);
    float boxW  = boxH * 0.4f; // typical aspect ratio for a standing player

    outBox.left   = head.x - boxW * 0.5f;
    outBox.top    = head.y;
    outBox.right  = head.x + boxW * 0.5f;
    outBox.bottom = root.y;
    return true;
}

// Distance-to-screen size hint (pixels per metre at 1080p reference)
inline float DistanceToBoxHeight(float distanceMetres, int screenH) {
    if (distanceMetres < 1.0f) distanceMetres = 1.0f;
    return (screenH * 1.8f) / distanceMetres; // 1.8 m ≈ player height
}
