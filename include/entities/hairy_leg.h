#ifndef HAIRY_LEG_H
#define HAIRY_LEG_H

#include "raylib.h"
#include "graphics/sprites.h"

typedef struct Player Player;

typedef enum {
    HL_IDLE,
    HL_JUMPING_UP,
    HL_HANGING,
    HL_FALLING,
    HL_VULNERABLE,
    HL_SWEEPING,
    HL_SWEEP_RECOVERING,
    HL_KICKING,
    HL_JUMPING_BACK,
    HL_DEAD
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
bool IsHairyLegKickColliding(const HairyLeg *leg, Rectangle playerHitbox);
void DamageHairyLeg(HairyLeg *leg, int damage);
bool ShouldHairyLegJumpBackFromCorner(const HairyLeg *leg, Rectangle playerRect, float screenWidth);
bool TryDamageHairyLegFromPlayerAttack(HairyLeg *leg, Player *player, float playerScale);
void DrawHairyLeg(HairyLeg *leg, float scale);
void UnloadHairyLeg(HairyLeg *leg);

#endif
