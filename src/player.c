#include <raylib.h>
#include "player.h"

void InitPlayer(Player *player, Vector2 initialPos, float speed)
{
    LoadPlayerSprites(&player->sprites);
    player->position    = initialPos;
    player->velocity    = (Vector2) {0, 0};
    player->speed       = speed;
    player->onGround    = true;
    player->currentAnim = &player->sprites.walkFront;
    player->isBossFighting = false;
    player->facingRight = false;
}

void UpdatePlayer(Player *player, float dt, float groundY, float scale)
{
    float spriteH  = player->sprites.walkFront.layers[0].sheet.height * scale;
    float feetOffset = spriteH * 1.0f;

    if (IsKeyDown(KEY_D))
    {
        player->velocity.x = player->speed;
        player->facingRight = false;
        if (player->onGround)
            player->currentAnim = &player->sprites.walkFront;
    }
    else if (IsKeyDown(KEY_A))
    {
        player->velocity.x = -player->speed;
        if (player->isBossFighting)
        {
            player->facingRight = true;
            if (player->onGround)
                player->currentAnim = &player->sprites.walkFront;
        }
        else
        {
            if (player->onGround)
                player->currentAnim = &player->sprites.walkBackwards;
        }
    }
    else
    {
        player->velocity.x = 0;
        if (player->isBossFighting)
        {
            if (player->onGround)
                player->currentAnim = &player->sprites.idle;
        }
        else
        {
            if (player->onGround)
                player->currentAnim = &player->sprites.walkFront;
        }
    }

    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W)) && player->onGround)
    {
        player->velocity.y = JUMP_FORCE;
        player->onGround   = false;
        player->currentAnim = &player->sprites.jumpUp;
    }

    if (!player->onGround)
    {
        player->velocity.y += GRAVITY * dt;
        if (player->velocity.y > 0)
            player->currentAnim = &player->sprites.jumpDown;
    }

    player->position.x += player->velocity.x * dt;
    player->position.y += player->velocity.y * dt;

    if (player->position.y + feetOffset >= groundY)
    {
        player->position.y = groundY - feetOffset;
        player->velocity.y = 0;
        player->onGround   = true;
    }

    UpdateLayeredAnimation(player->currentAnim, dt);
}

Rectangle GetPlayerHitbox(Player *player, float scale)
{
    if (player->currentAnim && player->currentAnim->layerCount > 0)
    {
        Animation *baseLayer = &player->currentAnim->layers[0];
        float frameRenderWidth  = baseLayer->frameWidth * scale;
        float frameRenderHeight = baseLayer->sheet.height * scale;

        float offsetX    = frameRenderWidth  * 0.35f;
        float offsetY    = frameRenderHeight * 0.30f;
        float hitboxW    = frameRenderWidth  * 0.35f;
        float hitboxH    = frameRenderHeight * 0.50f;

        return (Rectangle){
            player->position.x + offsetX,
            player->position.y + offsetY,
            hitboxW,
            hitboxH
        };
    }
    return (Rectangle){ player->position.x, player->position.y, 50, 50 };
}

void DrawPlayer(Player *player, float scale)
{
    DrawLayeredAnimation(player->currentAnim, player->position, scale, !player->facingRight, WHITE);
    Rectangle hitbox = GetPlayerHitbox(player, scale);
    DrawRectangleLines((int)hitbox.x, (int)hitbox.y, (int)hitbox.width, (int)hitbox.height, GREEN);
}

void UnloadPlayer(Player *player)
{
    UnloadPlayerSprites(&player->sprites);
}
