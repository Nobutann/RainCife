#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include "graphics/sprites.h"
#include "gameplay/weapon.h"

#define GRAVITY 800.0f
#define JUMP_FORCE -600.0f
#define SPEED 600.0f
#define ENTITY_GROUND_RATIO 0.87f
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
