#ifndef ENEMY_H
#define ENEMY_H

#include <raylib.h>
#include "graphics/sprites.h"

typedef enum
{
    ENEMY_BIRD1,
    ENEMY_BIRD2,
    ENEMY_BIKE,
    ENEMY_WOOD,
    ENEMY_POSTE,
    ENEMY_FISH,
    ENEMY_SAFE_POSTE,
    ENEMY_COUNT
} EnemyType;

typedef struct
{
    EnemyType type;
    Vector2 position;
    Vector2 size;
    Vector2 velocity;
    bool active;
    int state;
    double stateTimer;
    Vector2 basePosition;
    bool headDetached;
    float animationTimer;
    int currentFrame;
    Vector2 hitboxOffset;
    Vector2 hitboxSize;
} Enemy;

typedef struct {
    Texture2D textures[ENEMY_COUNT];
    Texture2D posteSemCabeca;
    Texture2D posteCabecas;
    Texture2D fishWaterJump;
    Animation fishAnticipation;
    Animation birdAnimation;
    Texture2D bikeSkin2;
    Texture2D bikeSkinItau;
} EnemyAssets;

void InitEnemy(Enemy *enemy, EnemyType type, int screenWidth, int screenHeight, int baseSpeed);
void UpdateEnemy(Enemy *enemy, int screenWidth, int screenHeight, int baseSpeed, Rectangle playerHitbox);
void DrawEnemy(Enemy *enemy, EnemyAssets *assets);
Rectangle GetEnemyHitbox(Enemy *enemy);

#endif