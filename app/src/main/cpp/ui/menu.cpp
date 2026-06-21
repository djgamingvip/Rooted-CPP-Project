#include "menu.hpp"
#include "../Socket.h"
#include <android/log.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Menu", __VA_ARGS__)

namespace Menu {

void SyncToControlPacket(ControlPacket& pkt) {
    Config& cfg = Config::Get();

    // ESP
    pkt.EnableEsp    = cfg.esp.enabled;
    pkt.EspLine      = cfg.esp.line;
    pkt.EspBox       = cfg.esp.box;
    pkt.EspSkeleton  = cfg.esp.skeleton;
    pkt.EspName      = cfg.esp.name;
    pkt.EspHealth    = cfg.esp.health;
    pkt.EspDistance  = cfg.esp.distance;
    pkt.EspTarget    = cfg.esp.target;
    pkt.ShowFov      = cfg.esp.showFov;
    pkt.EspDirection = cfg.esp.direction;
    pkt.FovDirection = cfg.esp.fovRadius;
    pkt.ESP_OFFSET_X = cfg.esp.offsetX;

    // Aimbot
    pkt.AimBotLite   = cfg.aim.enabled;
    pkt.AimSilent    = cfg.aim.silent;
    pkt.AimMagnet    = cfg.aim.magnet;
    pkt.AimLegit     = cfg.aim.legit;
    pkt.AimFov       = cfg.aim.fov;
    pkt.HeadshotRate = cfg.aim.headshotRate;
    pkt.AIM_SMOOTH   = cfg.aim.smooth;
    pkt.TargetIn     = cfg.aim.targetBone;
    pkt.WhenAim      = cfg.aim.triggerMode;

    // Movement
    pkt.FlyUp        = cfg.mov.fly;
    pkt.FlyMap       = cfg.mov.flyMap;
    pkt.FootballHack = cfg.mov.football;
}

void ApplyControlPacket(const ControlPacket& pkt) {
    Config& cfg = Config::Get();

    cfg.esp.enabled   = pkt.EnableEsp;
    cfg.esp.line      = pkt.EspLine;
    cfg.esp.box       = pkt.EspBox;
    cfg.esp.skeleton  = pkt.EspSkeleton;
    cfg.esp.name      = pkt.EspName;
    cfg.esp.health    = pkt.EspHealth;
    cfg.esp.distance  = pkt.EspDistance;
    cfg.esp.target    = pkt.EspTarget;
    cfg.esp.showFov   = pkt.ShowFov;
    cfg.esp.direction = pkt.EspDirection;
    cfg.esp.fovRadius = pkt.FovDirection;
    cfg.esp.offsetX   = pkt.ESP_OFFSET_X;

    cfg.aim.enabled      = pkt.AimBotLite;
    cfg.aim.silent       = pkt.AimSilent;
    cfg.aim.magnet       = pkt.AimMagnet;
    cfg.aim.legit        = pkt.AimLegit;
    cfg.aim.fov          = pkt.AimFov;
    cfg.aim.headshotRate = pkt.HeadshotRate;
    cfg.aim.smooth       = pkt.AIM_SMOOTH;
    cfg.aim.targetBone   = pkt.TargetIn;
    cfg.aim.triggerMode  = pkt.WhenAim;

    cfg.mov.fly      = pkt.FlyUp;
    cfg.mov.flyMap   = pkt.FlyMap;
    cfg.mov.football = pkt.FootballHack;
}

void Render(const ESPPacket& /*data*/) {
    // Placeholder — wire up ImGui or custom draw calls here
    LOGI("Menu::Render called (stub)");
}

} // namespace Menu
