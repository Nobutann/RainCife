#include <stdlib.h>
#include "graphics/sprites.h"
#include <string.h>

#define FRAME_TIME 0.08f
#define HAIRY_LEG_DESIGN_FRAME_TIME 0.10f

static const PlayerSpriteSet PLAYER_SPRITE_SETS[] =
{
    {
        "Rato",
        "assets/sprites/Player/Spr_Mouse/idle/Mouse_Idle_complete-Sheet.png",
        "assets/sprites/Player/Spr_Mouse/idle/Mouse_Idle_complete_cesar-Sheet.png",
        "assets/sprites/Player/Spr_Mouse/walkFront/Mouse_Running_legs_forward-Sheet.png",
        "assets/sprites/Player/Spr_Mouse/walkFront/Mouse_Running_body_foward-Sheet.png",
        "assets/sprites/Player/Spr_Mouse/Spr_rat/Mouse_Running_body_foward_cesar-Sheet.png",
        "assets/sprites/Player/Spr_Mouse/Mouse_Head_running-Sheet.png",
        "assets/sprites/Player/Spr_Mouse/walkBack/Mouse_Running_legs_backwards-Sheet.png",
        "assets/sprites/Player/Spr_Mouse/walkBack/Mouse_Running_body_backward.png",
        "assets/sprites/Player/Spr_Mouse/Spr_rat/Mouse_Running_body_backward_cesar.png",
        "assets/sprites/Player/Spr_Mouse/jump/Mouse_Jump_up.png",
        "assets/sprites/Player/Spr_Mouse/Spr_rat/Mouse_Jump_up_cesar.png",
        "assets/sprites/Player/Spr_Mouse/jump/Mouse_Jump_down.png",
        "assets/sprites/Player/Spr_Mouse/Spr_rat/Mouse_Jump_down_cesar.png",
        "assets/sprites/Player/Spr_Mouse/idle/Mouse_Idle_legs_attack.png",
        "assets/sprites/Player/Spr_Mouse/idle/Mouse_Idle_head_attack.png",
        "assets/sprites/Player/Spr_Mouse/attack/Mouse_Legs_jump_up.png",
        "assets/sprites/Player/Spr_Mouse/attack/Mouse_Legs_jump_down.png",
        "assets/sprites/Player/Spr_Mouse/attack/melee/Mouse_Attack_sword-Sheet.png",
        "assets/sprites/Player/Spr_Mouse/Spr_rat/Mouse_Attack_sword_cesar-Sheet.png",
        "assets/sprites/Player/Spr_Mouse/attack/melee/Mouse_Attack_hammer-Sheet.png",
        "assets/sprites/Player/Spr_Mouse/Spr_rat/Mouse_Attack_hammer_cesar-Sheet.png",
        "assets/sprites/Player/Spr_Mouse/Spr_rat/Mouse_Arm_gun.png",
        "assets/sprites/Player/Spr_Mouse/Spr_rat/Mouse_Running_gun_body.png",
        "assets/sprites/Player/Spr_Mouse/Spr_rat/Mouse_Running_gun_body.png",
        "assets/sprites/Player/Spr_Mouse/Spr_rat/Mouse_Running_body_gun_backward.png",
        "assets/sprites/Player/Spr_Mouse/Spr_rat/Mouse_Running_body_gun_backward.png",
        "assets/sprites/Player/Spr_Mouse/Spr_rat/Mouse_Jump_up_gun.png",
        "assets/sprites/Player/Spr_Mouse/Spr_rat/Mouse_Jump_up_gun_cesar.png",
        "assets/sprites/Player/Spr_Mouse/Spr_rat/Mouse_Jump_down_gun.png",
        "assets/sprites/Player/Spr_Mouse/Spr_rat/Mouse_Jump_down_gun_cesar.png",
        "assets/sprites/Player/Spr_Mouse/Spr_rat/Mouse_Idle_body_gun.png"
    },
    {
        "Capivara",
        "assets/sprites/Player/Spr_Capibara/Capibara_Idle_complete-Sheet.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Idle_complete_cesar-Sheet.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Running_legs_forward-Sheet.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Running_body_foward-Sheet.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Running_body_foward_cesar-Sheet.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Head_running-Sheet.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Running_legs_backwards-Sheet.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Running_body_backward.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Running_body_backward_cesar.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Jump_up.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Jump_up_cesar.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Jump_down.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Jump_down_cesar.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Idle_legs_attack.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Idle_head_attack.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Leg_jump_up.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Leg_jump_down.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Attack_sword-Sheet.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Attack_sword_cesar-Sheet.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Attack_hammer-Sheet.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Attack_hammer_cesar-Sheet.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Arm_gun.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Running_gun_body.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Running_gun_body_cesar-Sheet.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Running_body_gun_backward.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Running_body_gun_backward_cesar.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Jump_up_gun.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Jump_up_gun_cesar.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Jump_down_gun.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Jump_down_gun_cesar.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Idle_body_gun.png"
    },
    {
        "Guaiamum",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Idle_complete-Sheet.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Idle_complete_cesar-Sheet.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Running_legs_forward-Sheet.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Running_body_foward-Sheet.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Running_body_foward_cesar-Sheet.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Head_running-Sheet.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Running_legs_backwards-Sheet.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Running_body_backward.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Running_body_backward_cesar.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Jump_up.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Jump_up_cesar.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Jump_down.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Jump_down_cesar.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Idle_legs_attack.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Idle_head_attack.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Leg_jump_up.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Leg_jump_down.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Attack_sword-Sheet.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Attack_sword_cesar-Sheet.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Attack_hammer-Sheet.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Attack_hammer_cesar-Sheet.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Arm_gun.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Running_gun_body-Sheet.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Running_gun_body_cesar-Sheet.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Running_body_gun_backward.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Running_body_gun_backward_cesar.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Jump_up_gun.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Jump_up_gun_cesar.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Jump_down_gun.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Jump_down_gun_cesar.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Idle_body_gun.png"
    }
};

const PlayerSpriteSet *GetPlayerSpriteSet(int characterId)
{
    int index = characterId - 1;
    int count = (int)(sizeof(PLAYER_SPRITE_SETS) / sizeof(PLAYER_SPRITE_SETS[0]));
    if (index < 0 || index >= count)
    {
        index = 0;
    }
    return &PLAYER_SPRITE_SETS[index];
}

const char *GetPlayerCharacterName(int characterId)
{
    return GetPlayerSpriteSet(characterId)->name;
}

Animation LoadAnimation(const char* path, int frameCount, float frameTime)
{
    Animation animation = {0};
    animation.sheet = LoadTexture(path);
    animation.frameCount = frameCount;
    animation.frameWidth = animation.sheet.width / frameCount;
    animation.frameTime = frameTime;

    return animation;
}

void UnloadAnimation(Animation *animation)
{
    UnloadTexture(animation->sheet);
}

void UpdateLayeredAnimation(LayeredAnimation *layredAnimation, float dt)
{
    for (int i = 0; i < layredAnimation->layerCount; i++)
    {
        UpdateAnimation(&layredAnimation->layers[i], dt);
    }
}

void DrawLayeredAnimation(LayeredAnimation *layeredAnimation, Vector2 position, float scale, bool flipX, Color tint)
{
    float refFrameWidth = layeredAnimation->referenceFrameWidth > 0.0f
        ? layeredAnimation->referenceFrameWidth
        : (float)layeredAnimation->layers[0].frameWidth;
    float refWidth = refFrameWidth * scale;

    for (int i = 0; i < layeredAnimation->layerCount; i++)
    {
        float fw = layeredAnimation->layers[i].frameWidth * scale;
        float offsetX = flipX ? 0 : (refWidth - fw);
        float manualOffsetX = layeredAnimation->layers[i].offsetX;
        Vector2 layerPos = {
            position.x + offsetX + (flipX ? manualOffsetX : -manualOffsetX) * scale,
            position.y + layeredAnimation->layers[i].offsetY * scale
        };
        DrawAnimationFrame(&layeredAnimation->layers[i], layerPos, scale, flipX, tint);
    }
}

void UnloadLayeredAnimation(LayeredAnimation *layeredAnimation)
{
    for (int i = 0; i < layeredAnimation->layerCount; i++)
    {
        UnloadAnimation(&layeredAnimation->layers[i]);
    }
}

void LoadPlayerSprites(PlayerSprites *playerSprites, int characterId, int clothingId)
{
    memset(playerSprites, 0, sizeof(PlayerSprites));
    bool clothed = clothingId == 2;
    const PlayerSpriteSet *sprites = GetPlayerSpriteSet(characterId);

    playerSprites->walkFront.layerCount = 3;
    playerSprites->walkFront.layers[0] = LoadAnimation(sprites->walkFrontLegs, 8, FRAME_TIME);
    playerSprites->walkFront.layers[1] = LoadAnimation(
        clothed
            ? sprites->walkFrontBodyClothed
            : sprites->walkFrontBody,
        8,
        FRAME_TIME
    );
    playerSprites->walkFront.layers[2] = LoadAnimation(sprites->headRunning, 8, FRAME_TIME);

    playerSprites->walkBackwards.layerCount = 3;
    playerSprites->walkBackwards.layers[0] = LoadAnimation(sprites->walkBackLegs, 8, FRAME_TIME);
    playerSprites->walkBackwards.layers[1] = LoadAnimation(
        clothed
            ? sprites->walkBackBodyClothed
            : sprites->walkBackBody,
        1,
        FRAME_TIME
    );
    playerSprites->walkBackwards.layers[2] = LoadAnimation(sprites->headRunning, 8, FRAME_TIME);

    playerSprites->jumpUp.layerCount = 1;
    playerSprites->jumpUp.layers[0] = LoadAnimation(
        clothed
            ? sprites->jumpUpClothed
            : sprites->jumpUp,
        1,
        FRAME_TIME
    );

    playerSprites->jumpDown.layerCount = 1;
    playerSprites->jumpDown.layers[0] = LoadAnimation(
        clothed
            ? sprites->jumpDownClothed
            : sprites->jumpDown,
        1,
        FRAME_TIME
    );

    playerSprites->idle.layerCount = 1;
    playerSprites->idle.layers[0] = LoadAnimation(
        clothed
            ? sprites->idleClothed
            : sprites->idle,
        6,
        FRAME_TIME
    );

    playerSprites->idleLegs.layerCount = 1;
    playerSprites->idleLegs.layers[0] = LoadAnimation(sprites->idleLegsAttack, 1, FRAME_TIME);

    playerSprites->idleHead.layerCount = 1;
    playerSprites->idleHead.layers[0] = LoadAnimation(sprites->idleHeadAttack, 1, FRAME_TIME);

    playerSprites->jumpUpLegs.layerCount = 1;
    playerSprites->jumpUpLegs.layers[0] = LoadAnimation(sprites->jumpUpLegs, 1, FRAME_TIME);

    playerSprites->jumpDownLegs.layerCount = 1;
    playerSprites->jumpDownLegs.layers[0] = LoadAnimation(sprites->jumpDownLegs, 1, FRAME_TIME);

    playerSprites->armGun = LoadTexture(sprites->armGun);

    playerSprites->walkBackwardsGun.layerCount = 3;
    playerSprites->walkBackwardsGun.layers[0] = LoadAnimation(sprites->walkBackLegs, 8, FRAME_TIME);
    playerSprites->walkBackwardsGun.layers[1] = LoadAnimation(clothed ? sprites->walkBackBodyGunClothed : sprites->walkBackBodyGun, 1, FRAME_TIME);
    playerSprites->walkBackwardsGun.layers[2] = LoadAnimation(sprites->headRunning, 8, FRAME_TIME);

    playerSprites->jumpUpGun.layerCount = 1;
    playerSprites->jumpUpGun.layers[0] = LoadAnimation(clothed ? sprites->jumpUpGunClothed : sprites->jumpUpGun, 1, FRAME_TIME);

    playerSprites->jumpDownGun.layerCount = 1;
    playerSprites->jumpDownGun.layers[0] = LoadAnimation(clothed ? sprites->jumpDownGunClothed : sprites->jumpDownGun, 1, FRAME_TIME);

    playerSprites->idleGun.layerCount = 3;
    playerSprites->idleGun.layers[0] = LoadAnimation(sprites->idleLegsAttack, 1, FRAME_TIME);
    playerSprites->idleGun.layers[1] = LoadAnimation(sprites->idleBodyGun, 1, FRAME_TIME);
    playerSprites->idleGun.layers[2] = LoadAnimation(sprites->idleHeadAttack, 1, FRAME_TIME);
    
}

float LoadAttackAnimation(PlayerSprites *playerSprites, int characterId, const char *path, int frameCount, float frameTime)
{
    const PlayerSpriteSet *sprites = GetPlayerSpriteSet(characterId);

    playerSprites->attack.layerCount = 3;
    playerSprites->attack.layers[0] = LoadAnimation(sprites->walkFrontLegs, 8, frameTime);
    playerSprites->attack.layers[1] = LoadAnimation(path, frameCount, frameTime);
    playerSprites->attack.layers[2] = LoadAnimation(sprites->headRunning, 8, frameTime);
    return frameCount * frameTime;
}

void LoadPistolAnimation(PlayerSprites *playerSprites, int characterId, int clothingId)
{
    bool clothed = clothingId == 2;
    const PlayerSpriteSet *sprites = GetPlayerSpriteSet(characterId);
    UnloadLayeredAnimation(&playerSprites->attack);
    playerSprites->attack.layerCount = 3;
    playerSprites->attack.layers[0] = LoadAnimation(sprites->walkFrontLegs, 8, FRAME_TIME);
    playerSprites->attack.layers[1] = LoadAnimation(clothed ? sprites->walkFrontBodyGunClothed : sprites->walkFrontBodyGun, 8, FRAME_TIME);
    playerSprites->attack.layers[2] = LoadAnimation(sprites->headRunning, 8, FRAME_TIME);
}

void UnloadAttackAnimation(PlayerSprites *playerSprites)
{
    UnloadLayeredAnimation(&playerSprites->attack);
}

void UnloadPlayerSprites(PlayerSprites *playerSprites)
{
    UnloadLayeredAnimation(&playerSprites->walkFront);
    UnloadLayeredAnimation(&playerSprites->idle);
    UnloadLayeredAnimation(&playerSprites->jumpUp);
    UnloadLayeredAnimation(&playerSprites->jumpDown);
    UnloadLayeredAnimation(&playerSprites->walkBackwards);
    UnloadLayeredAnimation(&playerSprites->attack);
    UnloadLayeredAnimation(&playerSprites->idleLegs);
    UnloadLayeredAnimation(&playerSprites->idleHead);
    UnloadLayeredAnimation(&playerSprites->jumpUpLegs);
    UnloadLayeredAnimation(&playerSprites->jumpDownLegs);
    UnloadLayeredAnimation(&playerSprites->walkBackwardsGun);
    UnloadLayeredAnimation(&playerSprites->jumpUpGun);
    UnloadLayeredAnimation(&playerSprites->jumpDownGun);
    UnloadTexture(playerSprites->armGun);
}

void UpdateAnimation(Animation *animation, float dt)
{
    animation->timer += dt;

    if (animation->timer >= animation->frameTime)
    {
        animation->timer = 0.0f;
        animation->currentFrame++;

        if (animation->currentFrame >= animation->frameCount)
        {
            animation->currentFrame = 0;
        }
    }
}

Rectangle GetAnimationFrameSource(const Animation *animation, bool flipX)
{
    float fw = (float)animation->frameWidth;
    float fh = (float)animation->sheet.height;

    return (Rectangle){
        animation->currentFrame * fw,
        0.0f,
        flipX ? -fw : fw,
        fh
    };
}

void DrawAnimationFrame(Animation *animation, Vector2 position, float scale, bool flipX, Color tint)
{
    if (animation->sheet.id > 0 && animation->frameCount > 0)
    {
        float fw = animation->frameWidth;
        float fh = animation->sheet.height;
        Rectangle source = GetAnimationFrameSource(animation, flipX);

        Rectangle dest =
        {
            position.x,
            position.y,
            fw * scale,
            fh * scale
        };

        DrawTexturePro(animation->sheet, source, dest, (Vector2){0, 0}, 0.0f, tint);
    }
}
void LoadHairyLegSprites(HairyLegSprites *sprites)
{
    sprites->idle = LoadAnimation("assets/sprites/Boss/Perna_idle-Sheet.png", 7, FRAME_TIME);
    sprites->jump = LoadAnimation("assets/sprites/Boss/Perna_jump-Sheet.png", 4, FRAME_TIME);
    sprites->fall = LoadAnimation("assets/sprites/Boss/Perna_fall-Sheet.png", 2, FRAME_TIME);
    sprites->kick = LoadAnimation("assets/sprites/Boss/Perna_kick-Sheet.png", HAIRY_LEG_KICK_FRAME_COUNT, FRAME_TIME);
    sprites->rasteira = LoadAnimation("assets/sprites/Boss/Perna_rasteira-Sheet.png", HAIRY_LEG_SWEEP_FRAME_COUNT, HAIRY_LEG_DESIGN_FRAME_TIME);
    sprites->recovery = LoadAnimation("assets/sprites/Boss/Perna_rperna_subindo-sheet.png", 7, HAIRY_LEG_DESIGN_FRAME_TIME);
    sprites->death = LoadAnimation("assets/sprites/Boss/Perna_death.png", 1, FRAME_TIME);
    sprites->shadow = LoadAnimation("assets/sprites/Boss/Shadow-Sheet.png", 4, FRAME_TIME);
    sprites->shockwave = LoadTexture("assets/sprites/Boss/shockwave.png");
}

void UnloadHairyLegSprites(HairyLegSprites *sprites)
{
    UnloadAnimation(&sprites->idle);
    UnloadAnimation(&sprites->jump);
    UnloadAnimation(&sprites->fall);
    UnloadAnimation(&sprites->kick);
    UnloadAnimation(&sprites->rasteira);
    UnloadAnimation(&sprites->recovery);
    UnloadAnimation(&sprites->death);
    UnloadAnimation(&sprites->shadow);
    UnloadTexture(sprites->shockwave);
}

void ResetAnimation(Animation *animation) {
    animation->currentFrame = 0;
    animation->timer = 0.0f;
}
