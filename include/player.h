#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include "sprites.h"
#include "weapon.h"

#define GRAVITY 800.0f
#define JUMP_FORCE -400.0f
#define SPEED 500.0f
#define GROUND 500.0f

typedef struct Player
{
    Vector2 position;
    Vector2 velocity;
    float speed;
    bool onGround;
    PlayerSprites sprites;
    LayeredAnimation *currentAnim;
    bool facingRight;
    bool isBossFighting;
    Weapon weapon;
} Player;

void InitPlayer(Player *player, Vector2 initialPos, float speed);
void UpdatePlayer(Player *player, float dt);
void DrawPlayer(Player *player);
void UnloadPlayer(Player *player);

#endif
