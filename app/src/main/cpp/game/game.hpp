#pragma once
#include <cstdint>

class Game {
public:
    static uint64_t GetLibBase();
    static int      GetPid();
    static bool     IsValid();
    static void     Update();

private:
    static int      gamePid;
    static uint64_t libBase;
};
