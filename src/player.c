#include <raylib.h>
#include "player.h"

void InitPlayer(Player *player, Vector2 initialPos, float speed)
{
    LoadPlayerSprites(&player->sprites);
    player->position = initialPos;
    player->velocity = (Vector2) {0, 0};
    player->speed = speed;
    player->onGround = true;
    player->currentAnim = &player->sprites.idle;
}

void UpdatePlayer(Player *player, float dt)
{
    if (IsKeyDown(KEY_D))
    {
        player->velocity.x = player->speed;
        player->facingRight = false;
        if (player->onGround)
        {
            player->currentAnim = &player->sprites.walkRight;
        }
    }
    else if (IsKeyDown(KEY_A))
    {
        player->velocity.x = -player->speed;
        player->facingRight = true;
        if (player->onGround)
        {
            player->currentAnim = &player->sprites.walkRight;
        }
    }
    else
    {
        player->velocity.x = 0;
        if (player->onGround)
        {
            player->currentAnim = &player->sprites.idle;
        }
    }

    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W)) && player->onGround)
    {
        player->velocity.y = JUMP_FORCE;
        player->onGround = false;
        player->currentAnim = &player->sprites.jump;
    }

    if (!player->onGround)
    {
        player->velocity.y += GRAVITY * dt;
    }

    player->position.x += player->velocity.x * dt;
    player->position.y += player->velocity.y * dt;

    if (player->position.y >= GROUND)
    {
        player->position.y = GROUND;
        player->velocity.y = 0;
        player->onGround = true;
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