#include <raylib.h>
#include "entities/player.h"
#include "gameplay/weapon.h"
#include "core/screens.h"

#define BOSS_INTRO_PLAYER_GAP 220.0f

void InitPlayer(Player *player, Vector2 initialPos, float speed)
{
    LoadPlayerSprites(&player->sprites, GetSelectedClothingId());
    player->position = initialPos;
    player->velocity = (Vector2) {0, 0};
    player->speed = speed;
    player->onGround = true;
    player->currentAnim = &player->sprites.walkFront;
    player->isBossFighting = true;
    player->facingRight = false;
    player->sprites.attack = (LayeredAnimation){0};
    EquipWeapon(player, WEAPON_HAMMER);
    player->weapon.cooldownTimer = player->weapon.cooldown;
    player->isJumping = false;
}

static bool IsHammerAirAttack(const Player *player)
{
    return !player->onGround && player->weapon.attacking && player->weapon.type == WEAPON_HAMMER;
}

static void DrawLayeredAnimationLayer(LayeredAnimation *layeredAnimation, int layerIndex, Vector2 position, float scale, bool flipX, Color tint)
{
    float refWidth = layeredAnimation->layers[0].frameWidth * scale;
    float fw = layeredAnimation->layers[layerIndex].frameWidth * scale;
    float offsetX = flipX ? 0.0f : (refWidth - fw);
    float manualOffsetX = layeredAnimation->layers[layerIndex].offsetX;
    Vector2 layerPos =
    {
        position.x + offsetX + (flipX ? manualOffsetX : -manualOffsetX) * scale,
        position.y + layeredAnimation->layers[layerIndex].offsetY * scale
    };

    DrawAnimationFrame(&layeredAnimation->layers[layerIndex], layerPos, scale, flipX, tint);
}

void UpdatePlayer(Player *player, float dt, float groundY, float scale, const Config *config)
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

    if (IsKeyDown(config->teclaFrente))
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
    else if (IsKeyDown(config->teclaTras))
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

    if (IsKeyPressed(config->teclaPular) && player->onGround)
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

    if (player->isJumping && IsKeyDown(config->teclaPular))
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

    if (IsKeyReleased(config->teclaPular))
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
        float legsHeight = (float)player->sprites.attack.layers[0].sheet.height;
        float bodyHeight = (float)player->sprites.attack.layers[1].sheet.height;
        if (player->weapon.type == WEAPON_HAMMER)
        {
            player->sprites.attack.layers[2] = player->sprites.idleHead.layers[0];
        }

        float headWidth = (float)player->sprites.attack.layers[2].frameWidth;
        float headHeight = (float)player->sprites.attack.layers[2].sheet.height;
        player->sprites.attack.layers[1].offsetX = (legsWidth - bodyWidth) * 0.5f;
        player->sprites.attack.layers[2].offsetX = (legsWidth - headWidth) * 0.5f;
        player->sprites.attack.layers[1].offsetY = legsHeight - bodyHeight;
        player->sprites.attack.layers[2].offsetY = legsHeight - headHeight;

        if (player->weapon.type == WEAPON_HAMMER)
        {
            player->sprites.attack.layers[1].offsetX += 14.0f;
            player->sprites.attack.layers[2].offsetX += 3.0f;
            player->sprites.attack.layers[2].offsetY += 0.0f;
        }
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
        
        if (IsKeyDown(config->teclaAgachar))
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
                LayeredAnimation *runningAnim = (!player->isBossFighting && player->velocity.x < 0) ? &player->sprites.walkBackwards : &player->sprites.walkFront;
                player->sprites.attack.layers[0].sheet = runningAnim->layers[0].sheet;
                player->sprites.attack.layers[0].frameWidth = runningAnim->layers[0].frameWidth;
                player->sprites.attack.layers[0].frameCount = runningAnim->layers[0].frameCount;
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

void PlacePlayerForBossIntro(Player *player, Rectangle bossHitbox, float groundY, float scale)
{
    player->velocity = (Vector2){0.0f, 0.0f};
    player->onGround = true;
    player->isBossFighting = true;
    player->isJumping = false;
    player->jumpHoldTimer = 0.0f;
    player->weapon.attacking = false;
    player->weapon.attackTimer = 0.0f;
    player->weapon.hitConnected = false;

    if (player->sprites.idle.layerCount > 0)
    {
        player->currentAnim = &player->sprites.idle;
    }
    else if (!player->currentAnim && player->sprites.walkFront.layerCount > 0)
    {
        player->currentAnim = &player->sprites.walkFront;
    }

    float spriteH = 0.0f;
    if (player->sprites.walkFront.layerCount > 0)
    {
        spriteH = player->sprites.walkFront.layers[0].sheet.height * scale;
    }
    else if (player->currentAnim && player->currentAnim->layerCount > 0)
    {
        spriteH = player->currentAnim->layers[0].sheet.height * scale;
    }

    if (spriteH > 0.0f)
    {
        player->position.y = groundY - spriteH * 1.1f;
    }

    Rectangle hitbox = GetPlayerHitbox(player, scale);
    float targetHitboxRight = bossHitbox.x - BOSS_INTRO_PLAYER_GAP * scale;
    player->position.x += targetHitboxRight - (hitbox.x + hitbox.width);

    hitbox = GetPlayerHitbox(player, scale);
    if (hitbox.x < 0.0f)
    {
        player->position.x -= hitbox.x;
    }
}

void DrawPlayer(Player *player, float scale)
{   
    if (IsHammerAirAttack(player))
    {
        bool flipX = !player->facingRight;
        DrawLayeredAnimationLayer(player->currentAnim, 1, player->position, scale, flipX, WHITE);
        DrawLayeredAnimationLayer(player->currentAnim, 0, player->position, scale, flipX, WHITE);
        DrawLayeredAnimationLayer(player->currentAnim, 2, player->position, scale, flipX, WHITE);
    }
    else
    {
        DrawLayeredAnimation(player->currentAnim, player->position, scale, !player->facingRight, WHITE);
    }

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
