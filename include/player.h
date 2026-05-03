#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include "sprites.h"
#include "weapon.h"

#define GRAVITY 800.0f
#define JUMP_FORCE -400.0f
#define SPEED 500.0f
#define GROUND_RATIO 0.82f
#define SIDEWALK_THICKNESS_RATIO 0.08f

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
void UpdatePlayer(Player *player, float dt, float groundY, float scale);
void DrawPlayer(Player *player, float scale);
void UnloadPlayer(Player *player);
Rectangle GetPlayerHitbox(Player *player, float scale);

#endif
