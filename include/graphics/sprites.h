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
    float offsetX;
    float offsetY;
} Animation;

typedef struct
{
    Animation layers[MAX_LAYERS];
    int layerCount;
} LayeredAnimation;

typedef struct
{
    LayeredAnimation idle;
    LayeredAnimation idleLegs;
    LayeredAnimation idleHead;
    LayeredAnimation walkFront;
    LayeredAnimation walkBackwards;
    LayeredAnimation jumpUp;
    LayeredAnimation jumpDown;
    LayeredAnimation jumpUpLegs;
    LayeredAnimation jumpDownLegs;
    LayeredAnimation attack;
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
void LoadPlayerSprites(PlayerSprites *playerSprites, int clothingId);
void UnloadPlayerSprites(PlayerSprites *playerSprites);
float LoadAttackAnimation(PlayerSprites *playerSprites, const char *path, int frameCount, float frameTime);
void UnloadAttackAnimation(PlayerSprites *playerSprites);
void UpdateAnimation(Animation *animation, float dt);
Rectangle GetAnimationFrameSource(const Animation *animation, bool flipX);
void DrawAnimationFrame(Animation *animation, Vector2 position, float scale, bool flipX, Color tint);
void LoadHairyLegSprites(HairyLegSprites *sprites);
void UnloadHairyLegSprites(HairyLegSprites *sprites);

#endif
