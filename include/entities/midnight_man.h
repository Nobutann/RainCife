#ifndef MIDNIGHT_MAN_H
#define MIDNIGHT_MAN_H

#include "raylib.h"

#define MM_HAND_COUNT 3

typedef enum {
    MM_IDLE,
    MM_TELEGRAPH,
    MM_RISING,
    MM_RETREATING
} MidnightManState;

typedef struct {
    bool active;
    int health;
    MidnightManState state;
    float timer;

    Texture2D texHandOpen;

    float handXPositions[MM_HAND_COUNT];
    float handsY;
    float telegraphY;
    float riseStopY;
    float handDrawWidth;
    float handDrawHeight;
} MidnightMan;

void InitMidnightMan(MidnightMan *mm, int screenWidth, int screenHeight, float groundY);
void UpdateMidnightMan(MidnightMan *mm, Rectangle playerRect, float deltaTime, int screenWidth, int screenHeight, float groundY);
void DrawMidnightMan(const MidnightMan *mm);
void UnloadMidnightMan(MidnightMan *mm);

#endif
