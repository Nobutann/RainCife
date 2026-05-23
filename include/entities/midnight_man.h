#ifndef MIDNIGHT_MAN_H
#define MIDNIGHT_MAN_H

#include "raylib.h"
#include "graphics/sprites.h"

#define MM_HAND_COUNT 3
#define MM_MAX_UMBRELLAS 16

typedef enum {
    MM_IDLE,
    MM_GROUND_TELEGRAPH,
    MM_GROUND_RISE,
    MM_GROUND_RETREAT,
    MM_CEILING_TELEGRAPH,
    MM_CEILING_SLAM,
    MM_CEILING_RETREAT,
    MM_SWEEP_TELEGRAPH,
    MM_SWEEP_MOVE,
    MM_SWEEP_RETREAT,
    MM_UMBRELLA_STORM,
    MM_DEAD
} MidnightManState;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float scale;
    bool active;
    bool isBig;
    float animTimer;
    int animFrame;
} MMUmbrella;

struct Player;
typedef struct Player Player;

typedef struct {
    bool active;
    int health;
    MidnightManState state;
    float timer;

    Texture2D texHandOpen;
    Texture2D texFist;          // Punch.png
    Texture2D texUmbrella;      // sombrinha-Sheet.png
    Texture2D texArm;           // Arms_idle.png

    float handXPositions[MM_HAND_COUNT];
    float handsY;
    float telegraphY;
    float riseStopY;
    float handDrawWidth;
    float handDrawHeight;

    // Hitboxes for player collision and player attacks
    Rectangle handHitboxes[MM_HAND_COUNT];
    bool handActive[MM_HAND_COUNT];

    // Sweep attack details
    float sweepX;
    float sweepY;
    float sweepWidth;
    float sweepHeight;
    int sweepDirection; // 1 = Left to Right, -1 = Right to Left
    Rectangle sweepHitbox;

    // Umbrella storm details
    MMUmbrella umbrellas[MM_MAX_UMBRELLAS];
    float umbrellaSpawnTimer;
    int attackCycle; // to cycle or track attacks

    Animation animShadow; // Hairy Leg style warning shadow alert
} MidnightMan;

void InitMidnightMan(MidnightMan *mm, int screenWidth, int screenHeight, float groundY);
void UpdateMidnightMan(MidnightMan *mm, Rectangle playerRect, float deltaTime, int screenWidth, int screenHeight, float groundY);
void DrawMidnightMan(const MidnightMan *mm);
void UnloadMidnightMan(MidnightMan *mm);

bool TryDamageMidnightManFromPlayerAttack(MidnightMan *mm, Player *player, float playerScale);
bool IsMidnightManColliding(const MidnightMan *mm, Rectangle playerHitbox);

#endif
