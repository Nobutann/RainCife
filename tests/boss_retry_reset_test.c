#include <assert.h>
#include <math.h>

#include "entities/hairy_leg.h"
#include "entities/player.h"
#include "entities/shark.h"

bool IsPlayerAttackHitboxActive(const Player *player)
{
    (void)player;
    return false;
}

Rectangle GetPlayerAttackHitbox(Player *player, float scale)
{
    (void)player;
    (void)scale;
    return (Rectangle){0};
}

static void AssertHairyLegTextureIdsPreserved(const HairyLeg *leg)
{
    assert(leg->sprites.idle.sheet.id == 101);
    assert(leg->sprites.jump.sheet.id == 102);
    assert(leg->sprites.fall.sheet.id == 103);
    assert(leg->sprites.kick.sheet.id == 104);
    assert(leg->sprites.rasteira.sheet.id == 105);
    assert(leg->sprites.recovery.sheet.id == 106);
    assert(leg->sprites.death.sheet.id == 107);
    assert(leg->sprites.shadow.sheet.id == 108);
    assert(leg->sprites.shockwave.id == 109);
}

static void AssertSharkTextureIdsPreserved(const Shark *shark)
{
    assert(shark->texShoot.id == 201);
    assert(shark->texDashLeft.id == 202);
    assert(shark->texDashRight.id == 203);
    assert(shark->texJump.id == 204);
    assert(shark->texBubble.id == 205);
    assert(shark->texWaterBubble.id == 206);
}

int main(void)
{
    HairyLeg leg = {0};
    leg.sprites.idle.sheet.id = 101;
    leg.sprites.idle.sheet.height = 252;
    leg.sprites.jump.sheet.id = 102;
    leg.sprites.fall.sheet.id = 103;
    leg.sprites.kick.sheet.id = 104;
    leg.sprites.rasteira.sheet.id = 105;
    leg.sprites.recovery.sheet.id = 106;
    leg.sprites.death.sheet.id = 107;
    leg.sprites.shadow.sheet.id = 108;
    leg.sprites.shockwave.id = 109;
    leg.state = HL_DEAD;
    leg.health = 0;
    leg.waveLeft.active = true;
    leg.waveRight.active = true;
    leg.isKickActive = true;

    ResetHairyLeg(&leg, (Vector2){600.0f, 500.0f}, 500.0f, 2.0f);

    AssertHairyLegTextureIdsPreserved(&leg);
    assert(leg.state == HL_IDLE);
    assert(leg.health == 100);
    assert(!leg.waveLeft.active);
    assert(!leg.waveRight.active);
    assert(!leg.isKickActive);
    assert(leg.currentAnim == &leg.sprites.idle);

    Shark shark = {0};
    shark.texShoot.id = 201;
    shark.texDashLeft.id = 202;
    shark.texDashRight.id = 203;
    shark.texJump.id = 204;
    shark.texBubble.id = 205;
    shark.texWaterBubble.id = 206;
    shark.active = false;
    shark.health = 0;
    shark.balls[0].active = true;

    ResetShark(&shark, 1280, 720);

    AssertSharkTextureIdsPreserved(&shark);
    assert(shark.active);
    assert(shark.health == 100);
    assert(!shark.balls[0].active);

    HairyLeg falling = {0};
    falling.rect = (Rectangle){100.0f, 260.0f, 40.0f, 200.0f};
    falling.state = HL_FALLING;
    falling.direction = 1;
    falling.groundY = 500.0f;
    falling.sprites.fall.frameCount = 2;
    falling.sprites.fall.frameTime = 0.01f;
    falling.sprites.fall.sheet = (Texture2D){0};
    falling.sprites.fall.sheet.width = 504;
    falling.sprites.fall.sheet.height = 294;
    falling.currentAnim = &falling.sprites.fall;

    Rectangle playerHitbox = {100.0f, 240.0f, 50.0f, 80.0f};
    UpdateHairyLeg(&falling, playerHitbox, 0.01f, falling.groundY, 1.0f);

    float legHitboxTipY = falling.rect.y + falling.rect.height;
    assert(fabsf((falling.waveLeft.rect.y + falling.waveLeft.rect.height) - (legHitboxTipY + 14.0f)) < 0.001f);
    assert(fabsf((falling.waveRight.rect.y + falling.waveRight.rect.height) - (legHitboxTipY + 14.0f)) < 0.001f);

    return 0;
}
