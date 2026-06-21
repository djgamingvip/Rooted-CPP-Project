#pragma once
#include <stdint.h>

// Free Fire Max Offsets (Update these for current game version)
namespace Offsets {
    // libil2cpp.so offsets
    constexpr uint64_t GAME_FACADE_CURRENT_MATCH = 0x3266cc0;
    constexpr uint64_t PLAYER_TRANSFORM           = 0x5c0;
    constexpr uint64_t PLAYER_HEALTH              = 0x2e3e1ac;
    constexpr uint64_t PLAYER_NAME                = 0x2dcfddc;
    constexpr uint64_t PLAYER_LIST                = 0xC0;
    constexpr uint64_t LOCAL_PLAYER               = 0xB0;

    // Transform offsets
    constexpr uint64_t TRANSFORM_POSITION = 0x40;
}
