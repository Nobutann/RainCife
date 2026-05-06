#ifndef SHARK_H
#define SHARK_H

#include "raylib.h"

typedef enum {
    SHARK_IDLE,
    SHARK_PREP_LEFT,
    SHARK_DASH_RIGHT,
    SHARK_DASH_LEFT,
    SHARK_SHOOTING,
    SHARK_ARC_ATTACK,
    SHARK_WAIT_RETURN
} SharkState;

typedef struct {
    Rectangle rect;
    Vector2 direction;
    float speed;
    bool active;
} WaterBall;

#define MAX_WATER_BALLS 30

typedef struct {
    Rectangle rect;
    Vector2 startPos;
    Vector2 velocity;
    SharkState state;
    float timer;
    float shootTimer;
    int shootCount;
    int targetShootCount;
    int arcDrops;
    bool active;
    int health;
    WaterBall balls[MAX_WATER_BALLS];
} Shark;

void InitShark(Shark *shark, int screenWidth, int screenHeight);
void UpdateShark(Shark *shark, Rectangle playerRect, float deltaTime, int screenWidth, int screenHeight);
void DrawShark(Shark *shark);
void UnloadShark(Shark *shark);

#endif
