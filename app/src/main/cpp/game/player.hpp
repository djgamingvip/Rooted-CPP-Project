#pragma once
#include <cstdint>
#include "../Socket.h"

struct Player {
    uint64_t address   = 0;
    uint64_t transform = 0;
    Vector3  headPos;
    Vector3  rootPos;
    int      health    = 0;
    int      maxHealth = 200;
    float    distance  = 0.0f;
    char     name[32]  = {};
    bool     isLocal   = false;
    bool     isValid   = false;
};
