#include <raylib.h>
#include "entities/player.h"
#include "gameplay/weapon.h"

void InitPlayer(Player *player, Vector2 initialPos, float speed)
{
    LoadPlayerSprites(&player->sprites);
    player->position = initialPos;
    player->velocity = (Vector2) {0, 0};
    player->speed = speed;
    player->onGround = true;
    player->currentAnim = &player->sprites.walkFront;
    player->isBossFighting = true;
    player->facingRight = false;
    player->sprites.attack = (LayeredAnimation){0};
    EquipWeapon(player, WEAPON_BAT);
    player->weapon.cooldownTimer = player->weapon.cooldown;
    player->isJumping = false;
}

void UpdatePlayer(Player *player, float dt, float groundY, float scale)
{
    float spriteH = player->sprites.walkFront.layers[0].sheet.height * scale;
    float feetOffset = spriteH * 1.1f;
    int screenWidth = GetScreenWidth();

    Rectangle hitbox = GetPlayerHitbox(player, scale);

    if (hitbox.x < 0)
    {
        player->position.x -= hitbox.x;
    }

    if (hitbox.x + hitbox.width > screenWidth)
    {
        player->position.x -= (hitbox.x + hitbox.width - screenWidth);
    }

    if (IsKeyDown(KEY_D))
    {
        player->velocity.x = player->speed;
        player->facingRight = false;
        if (player->onGround && !player->weapon.attacking)
        {
            player->currentAnim = &player->sprites.walkFront;
        }
        else if (player->weapon.attacking)
        {
            player->sprites.attack.layers[0].sheet = player->sprites.walkFront.layers[0].sheet;
            player->sprites.attack.layers[0].frameWidth = player->sprites.walkFront.layers[0].frameWidth;
            player->sprites.attack.layers[0].frameCount = player->sprites.walkFront.layers[0].frameCount;
            player->sprites.attack.layers[2].sheet = player->sprites.walkFront.layers[2].sheet;
            player->sprites.attack.layers[2].frameWidth = player->sprites.walkFront.layers[2].frameWidth;
            player->sprites.attack.layers[2].frameCount = player->sprites.walkFront.layers[2].frameCount;
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
            else if (player->weapon.attacking)
            {
                player->sprites.attack.layers[0].sheet = player->sprites.walkFront.layers[0].sheet;
                player->sprites.attack.layers[0].frameWidth = player->sprites.walkFront.layers[0].frameWidth;
                player->sprites.attack.layers[0].frameCount = player->sprites.walkFront.layers[0].frameCount;
                player->sprites.attack.layers[2].sheet = player->sprites.walkFront.layers[2].sheet;
                player->sprites.attack.layers[2].frameWidth = player->sprites.walkFront.layers[2].frameWidth;
                player->sprites.attack.layers[2].frameCount = player->sprites.walkFront.layers[2].frameCount;
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
        player->velocity.y = JUMP_FORCE_MIN;
        player->isJumping = true;
        player->onGround = false;
        player->jumpHoldTimer = 0.0f;
        if (!player->weapon.attacking)
        {
            player->currentAnim = &player->sprites.jumpUp;
        }
    }

    if (player->isJumping && (IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_W)))
    {
        if (player->jumpHoldTimer < JUMP_HOLD_MAX)
        {
            player->jumpHoldTimer += dt;
            float extraForce = (JUMP_FORCE_MAX - JUMP_FORCE_MIN) * (dt /JUMP_HOLD_MAX);
            player->velocity.y += extraForce;
            if (player->velocity.y < JUMP_FORCE_MAX)
            {
                player->velocity.y = JUMP_FORCE_MAX;
            }
        }
        else
        {
            player->isJumping = false;
        }
    }

    if (IsKeyReleased(KEY_SPACE) || IsKeyReleased(KEY_W))
    {
        player->isJumping = false;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        UseWeapon(player);
    }

    if (!player->onGround && player->weapon.attacking)
    {
        if (player->velocity.y <= 0)
        {
            player->sprites.attack.layers[0].sheet = player->sprites.jumpUpLegs.layers[0].sheet;
            player->sprites.attack.layers[0].frameWidth = player->sprites.jumpUpLegs.layers[0].frameWidth;
            player->sprites.attack.layers[0].frameCount = player->sprites.jumpUpLegs.layers[0].frameCount;
            player->sprites.attack.layers[0].offsetX = 0.0f;
            player->sprites.attack.layers[0].offsetY = 0.0f;
        }
        else
        {
            player->sprites.attack.layers[0].sheet = player->sprites.jumpDownLegs.layers[0].sheet;
            player->sprites.attack.layers[0].frameWidth = player->sprites.jumpDownLegs.layers[0].frameWidth;
            player->sprites.attack.layers[0].frameCount = player->sprites.jumpDownLegs.layers[0].frameCount;
            player->sprites.attack.layers[0].offsetX = 0.0f;
            player->sprites.attack.layers[0].offsetY = 0.0f;
        }

        float legsWidth = (float)player->sprites.attack.layers[0].frameWidth;
        float bodyWidth = (float)player->sprites.attack.layers[1].frameWidth;
        float headWidth = (float)player->sprites.attack.layers[2].frameWidth;
        float legsHeight = (float)player->sprites.attack.layers[0].sheet.height;
        float bodyHeight = (float)player->sprites.attack.layers[1].sheet.height;
        float headHeight = (float)player->sprites.attack.layers[2].sheet.height;
        player->sprites.attack.layers[1].offsetX = (legsWidth - bodyWidth) * 0.5f;
        player->sprites.attack.layers[2].offsetX = (legsWidth - headWidth) * 0.5f;
        player->sprites.attack.layers[1].offsetY = legsHeight - bodyHeight;
        player->sprites.attack.layers[2].offsetY = legsHeight - headHeight;
    }
    else if (player->weapon.attacking)
    {
        player->sprites.attack.layers[1].offsetX = 0.0f;
        player->sprites.attack.layers[2].offsetX = 0.0f;
        player->sprites.attack.layers[1].offsetY = 0.0f;
        player->sprites.attack.layers[2].offsetY = 0.0f;
    }

    if (!player->onGround)
    {
        player->velocity.y += GRAVITY * dt;
        
        if (IsKeyDown(KEY_S))
        {
            player->velocity.y += GRAVITY * 2.0f * dt;
        }
        
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
            if (player->onGround)
            {
                if (player->velocity.x != 0)
                {
                    player->currentAnim = &player->sprites.idle;
                }
                else
                {
                    player->currentAnim = &player->sprites.walkFront;
                }
            }
            else
            {
                player->currentAnim = &player->sprites.jumpDown;
            }
        }
    }

    player->position.x += player->velocity.x * dt;
    player->position.y += player->velocity.y * dt;

    if (player->position.y + feetOffset >= groundY)
    {
        player->position.y = groundY - feetOffset;
        player->velocity.y = 0;
        player->onGround = true;

        if (player->weapon.attacking)
        {
            if (player->velocity.x != 0)
            {
                player->sprites.attack.layers[0].sheet = player->sprites.walkFront.layers[0].sheet;
                player->sprites.attack.layers[0].frameWidth = player->sprites.walkFront.layers[0].frameWidth;
                player->sprites.attack.layers[0].frameCount = player->sprites.walkFront.layers[0].frameCount;
                player->sprites.attack.layers[0].offsetX = 0.0f;
                player->sprites.attack.layers[0].offsetY = 0.0f;
            }
            else
            {
                player->sprites.attack.layers[0].sheet = player->sprites.idleLegs.layers[0].sheet;
                player->sprites.attack.layers[0].frameWidth = player->sprites.idleLegs.layers[0].frameWidth;
                player->sprites.attack.layers[0].frameCount = player->sprites.idleLegs.layers[0].frameCount;
                player->sprites.attack.layers[0].offsetX = 0.0f;
                player->sprites.attack.layers[0].offsetY = 0.0f;
            }
        }
    }

    if (player->weapon.cooldownTimer > 0)
    {
        player->weapon.cooldownTimer -= dt;
    }

    UpdateLayeredAnimation(player->currentAnim, dt);
}

Rectangle GetPlayerHitbox(Player *player, float scale)
{
    if (player->currentAnim && player->currentAnim->layerCount > 0)
    {
        Animation *baseLayer = &player->currentAnim->layers[0];
        float frameRenderWidth = baseLayer->frameWidth * scale;
        float frameRenderHeight = baseLayer->sheet.height * scale;

        float offsetX = frameRenderWidth * 0.45f;
        float offsetY = frameRenderHeight * 0.40f;
        float hitboxW = frameRenderWidth * 0.15f;
        float hitboxH = frameRenderHeight * 0.40f;

        return (Rectangle){
            player->position.x + offsetX,
            player->position.y + offsetY,
            hitboxW,
            hitboxH
        };
    }
    return (Rectangle){ player->position.x, player->position.y, 50, 50 };
}

bool IsPlayerAttackHitboxActive(const Player *player)
{
    return player->weapon.attacking;
}

Rectangle GetPlayerAttackHitbox(Player *player, float scale)
{
    if (!IsPlayerAttackHitboxActive(player))
    {
        return (Rectangle){0};
    }

    Rectangle bodyHitbox = GetPlayerHitbox(player, scale);
    float attackWidth = 90.0f * scale;
    float attackHeight = bodyHitbox.height * 0.85f;
    float attackY = bodyHitbox.y + (bodyHitbox.height - attackHeight) * 0.5f;
    bool attacksRight = !player->facingRight;
    float attackX = attacksRight ? bodyHitbox.x + bodyHitbox.width : bodyHitbox.x - attackWidth;

    return (Rectangle){ attackX, attackY, attackWidth, attackHeight };
}

void DrawPlayer(Player *player, float scale)
{   
    DrawLayeredAnimation(player->currentAnim, player->position, scale, !player->facingRight, WHITE);
    Rectangle hitbox = GetPlayerHitbox(player, scale);
    DrawRectangleLines((int)hitbox.x, (int)hitbox.y, (int)hitbox.width, (int)hitbox.height, GREEN);

    if (IsPlayerAttackHitboxActive(player))
    {
        Rectangle attackHitbox = GetPlayerAttackHitbox(player, scale);
        DrawRectangleLinesEx(attackHitbox, 2.0f, YELLOW);
    }
}

void UnloadPlayer(Player *player)
{
    UnloadPlayerSprites(&player->sprites);
}
