#include <stdlib.h>
#include "sprites.h"

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
    UpdateAnimation(&layredAnimation->layers[0], dt);
    for (int i = 1; i < layredAnimation->layerCount; i++)
    {
        layredAnimation->layers[i].currentFrame = layredAnimation->layers[0].currentFrame;
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
    playerSprites->walkRight.layerCount = 3;
    playerSprites->walkRight.layers[0] = LoadAnimation("assets/sprites/Player/Pernas_raton-Sheet.png", 8, 0.15f);
    playerSprites->walkRight.layers[1] = LoadAnimation("assets/sprites/Player/walkFront/Body_running_raton-Sheet.png", 8, 0.15f);
    playerSprites->walkRight.layers[2] = LoadAnimation("assets/sprites/Player/Head_raton-Sheet.png", 8, 0.15f);

    playerSprites->jump.layerCount = 1;
    playerSprites->jump.layers[0] = LoadAnimation("assets/sprites/Player/jump/Jump_raton.png", 1, 0.15f);

    playerSprites->idle.layerCount = 1;
    playerSprites->idle.layers[0] = LoadAnimation("assets/sprites/Player/idle/Raton_idle-Sheet.png", 6, 0.15f);
}

void UnloadPlayerSprites(PlayerSprites *playerSprites)
{
    UnloadLayeredAnimation(&playerSprites->walkRight);
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