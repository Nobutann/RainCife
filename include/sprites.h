#ifndef SPRITES_H
#define SPRITES_H

#include <raylib.h>

#define MAX_LAYERS 4

typedef struct
{
    Texture2D sheet;
    int frameCount;
    int frameWidth;
    int currentFrame;
    float frameTime;
    float timer;
} Animation;

typedef struct
{
    Animation layers[MAX_LAYERS];
    int layerCount;
} LayeredAnimation;

typedef struct
{
    LayeredAnimation idle;
    LayeredAnimation walkFront;
    LayeredAnimation walkBackwards;
    LayeredAnimation jumpUp;
    LayeredAnimation jumpDown;
} PlayerSprites;

typedef struct {
    Animation idle;
    Animation jump;
    Animation fall;
    Animation kick;
    Animation rasteira;
    Animation death;
} HairyLegSprites;

Animation LoadAnimation(const char* path, int frameCount, float frameTime);
void UnloadAnimation(Animation *animation);
void UpdateLayeredAnimation(LayeredAnimation *layredAnimation, float dt);
void DrawLayeredAnimation(LayeredAnimation *layeredAnimation, Vector2 position, float scale, bool flipX, Color tint);
void UnloadLayeredAnimation(LayeredAnimation *layeredAnimation);
void LoadPlayerSprites(PlayerSprites *playerSprites);
void UnloadPlayerSprites(PlayerSprites *playerSprites);
void UpdateAnimation(Animation *animation, float dt);
void DrawAnimationFrame(Animation *animation, Vector2 position, float scale, bool flipX, Color tint);
void LoadHairyLegSprites(HairyLegSprites *sprites);
void UnloadHairyLegSprites(HairyLegSprites *sprites);

#endif
