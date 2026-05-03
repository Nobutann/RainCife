#ifndef HAIRY_LEG_H
#define HAIRY_LEG_H

#include "raylib.h"
#include "graphics/sprites.h"

typedef enum {
    HL_IDLE,
    HL_JUMPING_UP,
    HL_FALLING,
    HL_VULNERABLE,
    HL_SWEEPING,
    HL_KICKING
} HairyLegState;

typedef struct {
    Rectangle rect;
    Vector2 speed;
    bool active;
} Shockwave;

typedef struct {
    Rectangle rect;
    HairyLegState state;
    float timer;
    int health;
    Shockwave waveLeft;
    Shockwave waveRight;
    Rectangle kickHitbox;
    bool isKickActive;
    int direction;
    HairyLegSprites sprites;
    Animation *currentAnim;
    float groundY;
} HairyLeg;

void InitHairyLeg(HairyLeg *leg, Vector2 startPosition, float groundY, float scale);
void UpdateHairyLeg(HairyLeg *leg, Rectangle playerRect, float deltaTime, float groundY, float scale);
void DrawHairyLeg(HairyLeg *leg, float scale);
void UnloadHairyLeg(HairyLeg *leg);

#endif
