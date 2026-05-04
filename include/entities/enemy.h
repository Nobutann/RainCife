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
    ENEMY_CABOCLO,
    ENEMY_FISH,
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
} Enemy;

void InitEnemy(Enemy *enemy, EnemyType type, int screenWidth, int screenHeight, int baseSpeed);
void UpdateEnemy(Enemy *enemy, int screenWidth, int screenHeight, int baseSpeed);
void DrawEnemy(Enemy *enemy, Texture2D texture);

#endif