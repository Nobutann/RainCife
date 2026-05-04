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
} Enemy;

void InitEnemy(Enemy *enemy, EnemyType type, int screenWidth, int screenHeight, int baseSpeed);
void UpdateEnemy(Enemy *enemy, int screenWidth, int screenHeight, int baseSpeed, Rectangle playerHitbox);
void DrawEnemy(Enemy *enemy, Texture2D *textures, Texture2D extraTex1, Texture2D extraTex2);

#endif