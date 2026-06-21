// DcXMoveWithFlyMap.h
// Header file with declarations only
// All implementations are in djgaming.cpp

#pragma once

// Structures
struct Vector3;
struct Vvector3;

// Global variables
extern bool IsPlayerFlying;
extern bool FlyMap;
extern bool FootballHack;
extern bool BallUnlimitedMarcosMods;  // NEW
extern Vector3 SavedGroundPos;
extern Vector3 SetMap;
extern int HighVp;
extern const int MaxFlyHeight;

// Function declarations
extern void DcMove();
extern void* GarenaMSDKMgr(void *cmd, void* a);
extern int hook_BEV_Jump_onExecute(int Player, int a);
extern int hook_isground(void *Player, void* a);
extern float hook_Football_get_Duration(void* thiz);  // Changed parameter name
extern void SetupHooks();