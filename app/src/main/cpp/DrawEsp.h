#pragma once
#include "Socket.h"
#include "PowerTypes.h"
#include <android/log.h>

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "DrawESP", __VA_ARGS__)

extern bool EnableEsp, EspLine, EspBox, EspSkeleton, EspTarget, EspDistance;
extern bool EspName, EnemyCounter, EspHealth, ShowFov, EspDirection;
extern float FovDirection, ESP_OFFSET_X;
extern int AimFov;

Vector2 WorldToScreen(Matrix4x4 viewMatrix, Vector3 pos, int width, int height, float offsetX = 0) {
    Vector2 screenPos = {-1, -1};

    // For test data, directly use coordinates
    screenPos.x = pos.x + offsetX;
    screenPos.y = pos.y;

    // Clamp to screen bounds
    if (screenPos.x < 0) screenPos.x = 0;
    if (screenPos.x > width) screenPos.x = width;
    if (screenPos.y < 0) screenPos.y = 0;
    if (screenPos.y > height) screenPos.y = height;

    return screenPos;
}

void DrawESP(JNIEnv* env, jobject espView, jobject canvas) {
    PowerDrawing draw(env, espView, canvas);
    if (!draw.isValid()) return;

    int screenWidth = draw.getWidth();
    int screenHeight = draw.getHeight();

    if (screenWidth <= 0 || screenHeight <= 0) return;

    ESPPacket packet;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        packet = g_packet;
    }

    static int frame = 0;
    frame++;

    if (!EnableEsp) {
        if (frame % 120 == 0) LOGD("ESP disabled");
        return;
    }

    if (frame % 60 == 0) {
        LOGD("Drawing ESP - Count: %d, Width: %d, Height: %d", packet.count, screenWidth, screenHeight);
    }

    // Force test data if no real data
    if (packet.count == 0) {
        // Create test box in center
        float centerX = screenWidth / 2;
        float centerY = screenHeight / 2;

        // Draw a test rectangle to confirm drawing works
        draw.DrawFilledBox(Color(255, 0, 0, 100), Vector2(centerX - 50, centerY - 50), Vector2(centerX + 50, centerY + 50));
        draw.DrawBox(Color(255, 255, 255, 255), 2.0f, Vector2(centerX - 50, centerY - 50), Vector2(centerX + 50, centerY + 50));

        if (frame % 60 == 0) {
            LOGD("Drawing test box at center - EnableEsp: %d", EnableEsp);
        }
        return;
    }

    int enemyCount = 0;

    for (int i = 0; i < packet.count && i < 64; i++) {
        ESPPlayer& player = packet.players[i];

        Vector2 headScreen = WorldToScreen(packet.ViewMatrix, player.HeadPos, screenWidth, screenHeight, ESP_OFFSET_X);
        Vector2 rootScreen = WorldToScreen(packet.ViewMatrix, player.RootPos, screenWidth, screenHeight, ESP_OFFSET_X);

        if (headScreen.x < 0 || headScreen.y < 0 || rootScreen.x < 0 || rootScreen.y < 0) continue;

        enemyCount++;

        Color espColor = player.IsDieing ? Color(255, 0, 0, 255) : Color(0, 255, 0, 255);

        if (EspLine) {
            Vector2 center = {(float)screenWidth / 2 + ESP_OFFSET_X, (float)screenHeight / 2};
            draw.DrawLine(espColor, 2.0f, center, headScreen);
        }

        if (EspBox) {
            float heightBox = rootScreen.y - headScreen.y;
            float widthBox = heightBox * 0.6f;
            Vector2 topLeft = {headScreen.x - widthBox / 2, headScreen.y};
            Vector2 bottomRight = {headScreen.x + widthBox / 2, rootScreen.y};
            draw.DrawBox(espColor, 2.0f, topLeft, bottomRight);
        }

        if (EspName && player.name[0] != '\0') {
            float textSize = 22.0f;
            float textWidth = draw.MeasureText(player.name, textSize);
            Vector2 textPos = {headScreen.x - textWidth / 2, headScreen.y - 25};
            draw.DrawText(Color(0, 0, 0, 255), textSize, {textPos.x - 1, textPos.y}, player.name);
            draw.DrawText(Color(0, 0, 0, 255), textSize, {textPos.x + 1, textPos.y}, player.name);
            draw.DrawText(Color(0, 0, 0, 255), textSize, {textPos.x, textPos.y - 1}, player.name);
            draw.DrawText(Color(0, 0, 0, 255), textSize, {textPos.x, textPos.y + 1}, player.name);
            draw.DrawText(espColor, textSize, textPos, player.name);
        }

        if (EspDistance && !player.IsDieing) {
            char distText[16];
            snprintf(distText, sizeof(distText), "%.0fm", player.distance);
            float textSize = 18.0f;
            Vector2 textPos = {headScreen.x - 15, rootScreen.y + 15};
            draw.DrawText(Color(255, 255, 255, 255), textSize, textPos, distText);
        }

        if (EspHealth && player.MaxHp > 0) {
            float hpPercent = (float)player.Hp / (float)player.MaxHp;
            float boxHeight = rootScreen.y - headScreen.y;
            float barWidth = 4.0f;
            Vector2 barPos = {headScreen.x - (boxHeight * 0.6f) / 2 - 8, headScreen.y};

            draw.DrawFilledBox(Color(0, 0, 0, 180), barPos, {barPos.x + barWidth, barPos.y + boxHeight});

            float filledHeight = boxHeight * hpPercent;
            Color hpColor = hpPercent > 0.6f ? Color(0, 255, 0, 255) :
                    (hpPercent > 0.3f ? Color(255, 255, 0, 255) : Color(255, 0, 0, 255));
            draw.DrawFilledBox(hpColor,
                    {barPos.x, barPos.y + boxHeight - filledHeight},
                    {barPos.x + barWidth, barPos.y + boxHeight});
        }
    }

    if (EnemyCounter) {
        Vector2 centerPos = {(float)screenWidth / 2 + ESP_OFFSET_X, 50.0f};
        float radius = 23.0f;

        Color bgColor = enemyCount >= 4 ? Color(255, 0, 0, 200) :
                (enemyCount >= 2 ? Color(255, 180, 0, 200) : Color(0, 200, 0, 200));

        draw.DrawCircleFilled(bgColor, centerPos, radius);
        draw.DrawCircle(Color(255, 255, 255, 255), 1.5f, centerPos, radius);

        char countText[8];
        snprintf(countText, sizeof(countText), "%d", enemyCount);
        float textSize = 26.0f;
        float textWidth = draw.MeasureText(countText, textSize);
        Vector2 textPos = {centerPos.x - textWidth / 2, centerPos.y + textSize * 0.35f};
        draw.DrawText(Color(255, 255, 255, 255), textSize, textPos, countText);
    }
}