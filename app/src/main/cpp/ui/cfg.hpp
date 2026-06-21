#pragma once

// ── ESP ───────────────────────────────────────────────────────────────────────
struct EspConfig {
    bool enabled    = false;
    bool line       = false;
    bool box        = false;
    bool skeleton   = false;
    bool name       = false;
    bool health     = false;
    bool distance   = false;
    bool target     = false;
    bool showFov    = false;
    bool direction  = false;
    float fovRadius = 130.0f;
    float offsetX   = 0.0f;
};

// ── Aimbot ────────────────────────────────────────────────────────────────────
struct AimConfig {
    bool  enabled      = false;
    bool  silent       = false;
    bool  magnet       = false;
    bool  legit        = false;
    int   fov          = 120;
    int   headshotRate = 50;
    float smooth       = 0.2f;
    int   targetBone   = 1;   // 1 = head
    int   triggerMode  = 1;   // 1 = always, 2 = ads only
};

// ── Movement ──────────────────────────────────────────────────────────────────
struct MovConfig {
    bool fly      = false;
    bool flyMap   = false;
    bool football = false;
};

// ── Global config singleton ────────────────────────────────────────────────────
struct Config {
    EspConfig esp;
    AimConfig aim;
    MovConfig mov;

    static Config& Get() {
        static Config instance;
        return instance;
    }

private:
    Config() = default;
};
