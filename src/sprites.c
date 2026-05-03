#include <stdlib.h>
#include "sprites.h"

#define FRAME_TIME 0.08f

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
    for (int i = 0; i < layeredAnimation->layerCount; i++)
    {
        DrawAnimationFrame(&layeredAnimation->layers[i], position, scale, flipX, tint);
    }
}

void UnloadLayeredAnimation(LayeredAnimation *layeredAnimation)
{
    for (int i = 0; i < layeredAnimation->layerCount; i++)
    {
        UnloadAnimation(&layeredAnimation->layers[i]);
    }
}

void LoadPlayerSprites(PlayerSprites *playerSprites)
{
    playerSprites->walkFront.layerCount = 3;
    playerSprites->walkFront.layers[0] = LoadAnimation("assets/sprites/Player/walkFront/Running_legs_forward-Sheet.png", 8, FRAME_TIME);
    playerSprites->walkFront.layers[1] = LoadAnimation("assets/sprites/Player/walkFront/Running_body_foward-Sheet.png", 8, FRAME_TIME);
    playerSprites->walkFront.layers[2] = LoadAnimation("assets/sprites/Player/Head_running-Sheet.png", 8, FRAME_TIME);

    playerSprites->walkBackwards.layerCount = 3;
    playerSprites->walkBackwards.layers[0] = LoadAnimation("assets/sprites/Player/walkBack/Running_legs_backwards-Sheet.png", 8, FRAME_TIME);
    playerSprites->walkBackwards.layers[1] = LoadAnimation("assets/sprites/Player/walkBack/Running_body_backward.png", 1, FRAME_TIME);
    playerSprites->walkBackwards.layers[2] = LoadAnimation("assets/sprites/Player/Head_running-Sheet.png", 8, FRAME_TIME);

    playerSprites->jumpUp.layerCount = 1;
    playerSprites->jumpUp.layers[0] = LoadAnimation("assets/sprites/Player/jump/Jump_up.png", 1, FRAME_TIME);

    playerSprites->jumpDown.layerCount = 1;
    playerSprites->jumpDown.layers[0] = LoadAnimation("assets/sprites/Player/jump/Jump_down.png", 1, FRAME_TIME);

    playerSprites->idle.layerCount = 1;
    playerSprites->idle.layers[0] = LoadAnimation("assets/sprites/Player/idle/Idle_complete-Sheet.png", 6, FRAME_TIME);
}

float LoadAttackAnimation(PlayerSprites *playerSprites, const char *path, int frameCount, float frameTime)
{
    playerSprites->attack.layerCount = 3;
    playerSprites->attack.layers[0] = LoadAnimation("assets/sprites/Player/walkFront/Running_legs_forward-Sheet.png", 8, frameTime);
    playerSprites->attack.layers[1] = LoadAnimation(path, frameCount, frameTime);
    playerSprites->attack.layers[2] = LoadAnimation("assets/sprites/Player/Head_running-Sheet.png", 8, frameTime);
    return frameCount * frameTime;
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

void DrawAnimationFrame(Animation *animation, Vector2 position, float scale, bool flipX, Color tint)
{
    if (animation->sheet.id > 0 && animation->frameCount > 0)
    {
        float fw = animation->frameWidth;

        Rectangle source = 
        {
            animation->currentFrame * fw + (flipX ? fw : 0),
            0,
            flipX ? -fw : fw,
            animation->sheet.height
        };

        Rectangle dest = 
        {
            position.x,
            position.y,
            animation->frameWidth * scale,
            animation->sheet.height * scale
        };

        DrawTexturePro(animation->sheet, source, dest, (Vector2){0, 0}, 0.0f, tint);
    }
}