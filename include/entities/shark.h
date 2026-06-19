#ifndef SHARK_H
#define SHARK_H

#include "raylib.h"
#include "gameplay/difficulty.h"

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
    bool isWaterBubble;
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
    bool dying;
    int health;
    float hitFlashTimer;
    float deathTimer;
    float deathStartY;
    WaterBall balls[MAX_WATER_BALLS];

    Texture2D texShoot;      // tubarao_shooting-Sheet.png — spritesheet 9 frames
    Texture2D texIdle;       // tubarao_idle-Sheet.png — spritesheet 9 frames
    Texture2D texDashLeft;   // Shark_dash-Sheet.png — spritesheet 6 frames (prep e retorno esquerda)
    Texture2D texDashRight;  // Shark_angry-Sheet.png — spritesheet 6 frames (dash direita)
    Texture2D texJump;       // tubarao_flying-Sheet.png — spritesheet 12 frames
    Texture2D texBubble;     // bubble.png — projétil
    Texture2D texWaterBubble;// water_bubble.png — projétil menor da arc attack

    Texture2D texDeath;

    float animTimer;
    int animFrame;
    float dashStartX;
    int dashSoundCount;
} Shark;

void InitShark(Shark *shark, int screenWidth, int screenHeight);
void ResetShark(Shark *shark, int screenWidth, int screenHeight);
void UpdateShark(Shark *shark, Rectangle playerRect, float deltaTime, int screenWidth, int screenHeight, GameplayDifficulty difficulty);
void DrawShark(Shark *shark);
void UnloadShark(Shark *shark);
Rectangle GetSharkHitbox(Shark *shark);
void DamageShark(Shark *shark, int damage);
bool TryDamageSharkFromPlayerAttack(Shark *shark, struct Player *player, float playerScale);

#endif
