#ifndef SPRITES_H
#define SPRITES_H

#include <raylib.h>

#define MAX_LAYERS 4
#define HAIRY_LEG_KICK_FRAME_COUNT 8
#define HAIRY_LEG_SWEEP_FRAME_COUNT 8

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
    float referenceFrameWidth;
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
    Texture2D armGun;
    LayeredAnimation walkBackwardsGun;
    LayeredAnimation jumpUpGun;
    LayeredAnimation jumpDownGun;
    LayeredAnimation idleGun;
} PlayerSprites;

typedef struct
{
    const char *name;
    const char *idle;
    const char *idleClothed;
    const char *walkFrontLegs;
    const char *walkFrontBody;
    const char *walkFrontBodyClothed;
    const char *headRunning;
    const char *walkBackLegs;
    const char *walkBackBody;
    const char *walkBackBodyClothed;
    const char *jumpUp;
    const char *jumpUpClothed;
    const char *jumpDown;
    const char *jumpDownClothed;
    const char *idleLegsAttack;
    const char *idleHeadAttack;
    const char *jumpUpLegs;
    const char *jumpDownLegs;
    const char *attackSword;
    const char *attackSwordClothed;
    const char *attackHammer;
    const char *attackHammerClothed;
    const char *armGun;
    const char *walkFrontBodyGun;
    const char *walkFrontBodyGunClothed;
    const char *walkBackBodyGun;
    const char *walkBackBodyGunClothed;
    const char *jumpUpGun;
    const char *jumpUpGunClothed;
    const char *jumpDownGun;
    const char *jumpDownGunClothed;
    const char *idleBodyGun;
} PlayerSpriteSet;

typedef struct {
    Animation idle;
    Animation jump;
    Animation fall;
    Animation kick;
    Animation rasteira;
    Animation recovery;
    Animation death;
    Animation shadow;
    Texture2D shockwave;
} HairyLegSprites;

Animation LoadAnimation(const char* path, int frameCount, float frameTime);
void UnloadAnimation(Animation *animation);
void UpdateLayeredAnimation(LayeredAnimation *layredAnimation, float dt);
void DrawLayeredAnimation(LayeredAnimation *layeredAnimation, Vector2 position, float scale, bool flipX, Color tint);
void UnloadLayeredAnimation(LayeredAnimation *layeredAnimation);
const PlayerSpriteSet *GetPlayerSpriteSet(int characterId);
const char *GetPlayerCharacterName(int characterId);
void LoadPlayerSprites(PlayerSprites *playerSprites, int characterId, int clothingId);
void UnloadPlayerSprites(PlayerSprites *playerSprites);
void LoadPistolAnimation(PlayerSprites *playerSprites, int characterId, int clothingId);
float LoadAttackAnimation(PlayerSprites *playerSprites, int characterId, const char *path, int frameCount, float frameTime);
void UnloadAttackAnimation(PlayerSprites *playerSprites);
void UpdateAnimation(Animation *animation, float dt);
Rectangle GetAnimationFrameSource(const Animation *animation, bool flipX);
void DrawAnimationFrame(Animation *animation, Vector2 position, float scale, bool flipX, Color tint);
void LoadHairyLegSprites(HairyLegSprites *sprites);
void UnloadHairyLegSprites(HairyLegSprites *sprites);

#endif
