#ifndef SHARK_H
#define SHARK_H

#include "raylib.h"

struct Player;
typedef struct Player Player;

typedef enum {
    SHARK_IDLE,
    SHARK_PREP_LEFT,
    SHARK_DASH_WAIT,
    SHARK_DASH_RIGHT,
    SHARK_DASH_LEFT,
    SHARK_SHOOTING,
    SHARK_ARC_ATTACK,
    SHARK_WAIT_RETURN
} SharkState;

typedef struct {
    Rectangle rect;
    Rectangle hitbox;
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

    Texture2D texShoot;   // Shark_attack_bubble.png — spritesheet 3 frames
    Texture2D texDash;      // Shark_dash.png       — prep e retorno (esquerda)
    Texture2D texDashRight; // Shark_vermelho.png   — dash da esquerda para direita
    Texture2D texJump;    // tubarao_flying-Sheet.png — spritesheet 12 frames
    Texture2D texBubble;  // bubble.png — projétil

    float animTimer;
    int animFrame;
} Shark;

void InitShark(Shark *shark, int screenWidth, int screenHeight);
void UpdateShark(Shark *shark, Rectangle playerRect, float deltaTime, int screenWidth, int screenHeight);
void DrawShark(Shark *shark);
void UnloadShark(Shark *shark);
Rectangle GetSharkHitbox(Shark *shark);
void DamageShark(Shark *shark, int damage);
bool TryDamageSharkFromPlayerAttack(Shark *shark, struct Player *player, float playerScale);

#endif
