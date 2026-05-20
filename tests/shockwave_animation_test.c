#include <assert.h>
#include <math.h>
#include <stdbool.h>

#include "entities/hairy_leg.h"
#include "entities/player.h"

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

static HairyLeg MakeLandingLegWithShockwaveSheet(void)
{
    HairyLeg leg = {0};

    leg.rect = (Rectangle){100.0f, 260.0f, 40.0f, 200.0f};
    leg.state = HL_FALLING;
    leg.direction = 1;
    leg.groundY = 500.0f;
    leg.sprites.fall.frameCount = 2;
    leg.sprites.fall.frameTime = 0.01f;
    leg.sprites.fall.sheet.width = 504;
    leg.sprites.fall.sheet.height = 294;
    leg.sprites.shockwave.width = 1260;
    leg.sprites.shockwave.height = 252;
    leg.currentAnim = &leg.sprites.fall;

    return leg;
}

int main(void)
{
    Rectangle unusedPlayer = {0};
    HairyLeg leg = MakeLandingLegWithShockwaveSheet();

    assert(HAIRY_LEG_KICK_FRAME_COUNT == 8);

    UpdateHairyLeg(&leg, unusedPlayer, 0.01f, leg.groundY, 1.0f);

    assert(leg.waveLeft.active);
    assert(leg.waveRight.active);
    float legHitboxTipY = leg.rect.y + leg.rect.height;
    assert(fabsf((leg.waveLeft.rect.y + leg.waveLeft.rect.height) - (legHitboxTipY + 14.0f)) < 0.001f);
    assert(fabsf((leg.waveRight.rect.y + leg.waveRight.rect.height) - (legHitboxTipY + 14.0f)) < 0.001f);
    assert(leg.waveLeft.speed.x == 720.0f);
    assert(leg.waveRight.speed.x == 720.0f);
    assert(fabsf(leg.waveLeft.rect.width - 113.4f) < 0.001f);
    assert(fabsf(leg.waveRight.rect.width - 113.4f) < 0.001f);
    assert(fabsf(leg.waveLeft.rect.height - 113.4f) < 0.001f);
    assert(fabsf(leg.waveRight.rect.height - 113.4f) < 0.001f);
    assert(fabsf(leg.waveLeft.hitbox.width - 62.37f) < 0.001f);
    assert(fabsf(leg.waveRight.hitbox.width - 62.37f) < 0.001f);
    assert(fabsf(leg.waveLeft.hitbox.height - 39.69f) < 0.001f);
    assert(fabsf(leg.waveRight.hitbox.height - 39.69f) < 0.001f);
    assert(fabsf((leg.waveLeft.hitbox.y + leg.waveLeft.hitbox.height) - (leg.waveLeft.rect.y + leg.waveLeft.rect.height - 6.804f)) < 0.001f);

    assert(leg.waveLeft.currentFrame == 0);
    UpdateHairyLeg(&leg, unusedPlayer, 0.08f, leg.groundY, 1.0f);
    assert(leg.waveLeft.currentFrame == 1);
    assert(leg.waveRight.currentFrame == 1);

    for (int i = 0; i < 4; i++) {
        UpdateHairyLeg(&leg, unusedPlayer, 0.08f, leg.groundY, 1.0f);
    }

    assert(leg.waveLeft.currentFrame == 0);
    assert(leg.waveRight.currentFrame == 0);

    return 0;
}
