#include <assert.h>
#include <math.h>

#include "entities/shark.h"

struct Player;

int GetRandomValue(int min, int max)
{
    (void)min;
    return max;
}

Texture2D LoadTexture(const char *fileName)
{
    (void)fileName;
    return (Texture2D){0};
}

void UnloadTexture(Texture2D texture)
{
    (void)texture;
}

void DrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint)
{
    (void)texture;
    (void)source;
    (void)dest;
    (void)origin;
    (void)rotation;
    (void)tint;
}

bool CheckCollisionRecs(Rectangle rec1, Rectangle rec2)
{
    (void)rec1;
    (void)rec2;
    return false;
}

Vector2 Vector2Subtract(Vector2 v1, Vector2 v2)
{
    return (Vector2){v1.x - v2.x, v1.y - v2.y};
}

Vector2 Vector2Normalize(Vector2 v)
{
    float length = sqrtf(v.x * v.x + v.y * v.y);
    if (length <= 0.0f)
    {
        return (Vector2){0};
    }

    return (Vector2){v.x / length, v.y / length};
}

bool IsPlayerAttackHitboxActive(const struct Player *player)
{
    (void)player;
    return false;
}

Rectangle GetPlayerAttackHitbox(struct Player *player, float scale)
{
    (void)player;
    (void)scale;
    return (Rectangle){0};
}

void PlaySharkShootSound(void) {}
void PlaySharkBubbleSound(void) {}
void PlaySharkJumpSound(void) {}
void PlaySharkSwimmingSound(void) {}
void StopSharkSwimmingSound(void) {}

static Shark MakeArcReadyShark(void)
{
    Shark shark = {0};
    ResetShark(&shark, 1280, 720);
    shark.timer = 10.0f;
    return shark;
}

static float StartArcAttackVelocityY(GameplayDifficulty difficulty)
{
    Shark shark = MakeArcReadyShark();
    Rectangle player = {0};

    UpdateShark(&shark, player, 0.0f, 1280, 720, difficulty);

    assert(shark.state == SHARK_ARC_ATTACK);
    return shark.velocity.y;
}

int main(void)
{
    float hardVelocityY = StartArcAttackVelocityY(GAMEPLAY_DIFFICULTY_HARD);

    assert(fabsf(hardVelocityY - (-1472.0f * 1.08f)) < 0.001f);
    assert(fabsf(StartArcAttackVelocityY(GAMEPLAY_DIFFICULTY_MEDIUM) - hardVelocityY) < 0.001f);
    assert(fabsf(StartArcAttackVelocityY(GAMEPLAY_DIFFICULTY_EASY) - hardVelocityY) < 0.001f);
    assert(fabsf(StartArcAttackVelocityY(GAMEPLAY_DIFFICULTY_HELENA) - hardVelocityY) < 0.001f);

    return 0;
}
