#include <raylib.h>
#include "player.h"
#include "weapon.h"

void InitPlayer(Player *player, Vector2 initialPos, float speed)
{
    LoadPlayerSprites(&player->sprites);
    player->position = initialPos;
    player->velocity = (Vector2) {0, 0};
    player->speed = speed;
    player->onGround = true;
    player->currentAnim = &player->sprites.walkFront;
    player->isBossFighting = false;
    player->facingRight = false;
    player->sprites.attack = (LayeredAnimation){0};
    EquipWeapon(player, WEAPON_HAMMER);
    player->weapon.cooldownTimer = player->weapon.cooldown;
}

void UpdatePlayer(Player *player, float dt)
{
    if (IsKeyDown(KEY_D))
    {
        player->velocity.x = player->speed;
        player->facingRight = false;
        if (player->onGround && !player->weapon.attacking)
        {
            player->currentAnim = &player->sprites.walkFront;
        }
    }
    else if (IsKeyDown(KEY_A))
    {
        player->velocity.x = -player->speed;
        if (player->isBossFighting)
        {
            player->facingRight = true;
            if (player->onGround && !player->weapon.attacking)
            {
                player->currentAnim = &player->sprites.walkFront;
            }
        }
        else
        {
            if (player->onGround && !player->weapon.attacking)
            {
                player->currentAnim = &player->sprites.walkBackwards;
            }
        }
    }
    else
    {
        player->velocity.x = 0;
        if (player->isBossFighting)
        {
            if (player->onGround && !player->weapon.attacking)
            {
                player->currentAnim = &player->sprites.idle;
            }
        }
        else
        {
            if (player->onGround && !player->weapon.attacking)
            {
                player->currentAnim = &player->sprites.walkFront;
            }
        }
    }

    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W)) && player->onGround)
    {
        player->velocity.y = JUMP_FORCE;
        player->onGround = false;
        if (!player->weapon.attacking)
        {
            player->currentAnim = &player->sprites.jumpUp;
        }
    }

    if (!player->onGround)
    {
        player->velocity.y += GRAVITY * dt;
        if (player->velocity.y > 0 && !player->weapon.attacking)
        {
            player->currentAnim = &player->sprites.jumpDown;
        }
    }

    if (player->weapon.attacking)
    {
        player->weapon.attackTimer -= dt;
        if (player->weapon.attackTimer <= 0)
        {
            player->weapon.attacking = false;
        }
    }

    player->position.x += player->velocity.x * dt;
    player->position.y += player->velocity.y * dt;

    if (player->position.y >= GROUND)
    {
        player->position.y = GROUND;
        player->velocity.y = 0;
        player->onGround = true;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        UseWeapon(player);
    }

    if (player->weapon.cooldownTimer > 0)
    {
        player->weapon.cooldownTimer -= dt;
    }

    UpdateLayeredAnimation(player->currentAnim, dt);
}

void DrawPlayer(Player *player)
{
    DrawLayeredAnimation(player->currentAnim, player->position, 2.0f, !player->facingRight, WHITE);
}

void UnloadPlayer(Player *player)
{
    UnloadPlayerSprites(&player->sprites);
}