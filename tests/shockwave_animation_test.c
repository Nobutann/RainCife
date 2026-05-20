#include <assert.h>
#include <math.h>
#include <stdbool.h>

#include "entities/hairy_leg.h"
#include "entities/player.h"

extern Rectangle GetHairyLegKickFrameHitbox(const HairyLeg *leg, float scale);
extern Rectangle GetHairyLegBodyFrameHitbox(const HairyLeg *leg, float scale);
extern Rectangle GetHairyLegSweepFrameHitbox(const HairyLeg *leg, float scale);

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

static HairyLeg MakeKickingLeg(int direction, int frame)
{
    HairyLeg leg = {0};

    leg.rect = (Rectangle){100.0f, 200.0f, 40.0f, 206.64f};
    leg.state = HL_KICKING;
    leg.direction = direction;
    leg.sprites.kick.frameCount = HAIRY_LEG_KICK_FRAME_COUNT;
    leg.sprites.kick.frameWidth = 362;
    leg.sprites.kick.currentFrame = frame;
    leg.sprites.kick.sheet.width = 2896;
    leg.sprites.kick.sheet.height = 252;
    leg.currentAnim = &leg.sprites.kick;

    return leg;
}

static HairyLeg MakeSweepingLeg(int direction, int frame)
{
    HairyLeg leg = {0};

    leg.rect = (Rectangle){100.0f, 419.84f, 40.0f, 60.0f};
    leg.bodyHitbox = leg.rect;
    leg.state = HL_SWEEPING;
    leg.direction = direction;
    leg.groundY = 500.0f;
    leg.sprites.rasteira.frameCount = HAIRY_LEG_SWEEP_FRAME_COUNT;
    leg.sprites.rasteira.frameWidth = 252;
    leg.sprites.rasteira.currentFrame = frame;
    leg.sprites.rasteira.sheet.width = 2016;
    leg.sprites.rasteira.sheet.height = 252;
    leg.currentAnim = &leg.sprites.rasteira;

    return leg;
}

static HairyLeg MakeVulnerableLeg(int direction)
{
    HairyLeg leg = {0};

    leg.rect = (Rectangle){100.0f, 200.0f, 40.0f, 206.64f};
    leg.bodyHitbox = leg.rect;
    leg.state = HL_VULNERABLE;
    leg.direction = direction;
    leg.groundY = 500.0f;
    leg.sprites.idle.frameCount = 7;
    leg.sprites.idle.frameTime = 0.01f;
    leg.sprites.idle.sheet.width = 700;
    leg.sprites.idle.sheet.height = 252;
    leg.currentAnim = &leg.sprites.idle;

    return leg;
}

static HairyLeg MakeIdleLeg(int direction, float timer)
{
    HairyLeg leg = MakeVulnerableLeg(direction);

    leg.state = HL_IDLE;
    leg.timer = timer;

    return leg;
}

static float CenterX(Rectangle rect)
{
    return rect.x + rect.width / 2.0f;
}

int main(void)
{
    Rectangle unusedPlayer = {0};
    HairyLeg leg = MakeLandingLegWithShockwaveSheet();
    HairyLeg vulnerableLeg = MakeVulnerableLeg(-1);
    HairyLeg idleAfterRecoveryLeg = MakeIdleLeg(-1, 0.1f);
    HairyLeg rightKickFinishingLeg = MakeKickingLeg(1, HAIRY_LEG_KICK_FRAME_COUNT - 1);
    HairyLeg leftKickFinishingLeg = MakeKickingLeg(-1, HAIRY_LEG_KICK_FRAME_COUNT - 1);
    HairyLeg leftIdleChoosingNextActionLeg = MakeIdleLeg(-1, 1.01f);
    Rectangle playerOnRight = {400.0f, 300.0f, 40.0f, 80.0f};
    HairyLeg leftKickFrame5 = MakeKickingLeg(-1, 5);
    HairyLeg leftKickFrame6 = MakeKickingLeg(-1, 6);
    HairyLeg rightKickFrame5 = MakeKickingLeg(1, 5);
    HairyLeg leftSweepFrame0 = MakeSweepingLeg(-1, 0);
    HairyLeg leftSweepFrame4 = MakeSweepingLeg(-1, 4);
    HairyLeg leftSweepFrame5 = MakeSweepingLeg(-1, 5);
    HairyLeg rightSweepFrame4 = MakeSweepingLeg(1, 4);
    HairyLeg rightSweepFrame5 = MakeSweepingLeg(1, 5);
    HairyLeg sweepStartLeg = MakeSweepingLeg(-1, 0);
    Rectangle leftFrame5Hitbox = GetHairyLegKickFrameHitbox(&leftKickFrame5, 1.0f);
    Rectangle leftFrame6Hitbox = GetHairyLegKickFrameHitbox(&leftKickFrame6, 1.0f);
    Rectangle rightFrame5Hitbox = GetHairyLegKickFrameHitbox(&rightKickFrame5, 1.0f);
    Rectangle leftFrame5BodyHitbox = GetHairyLegBodyFrameHitbox(&leftKickFrame5, 1.0f);
    Rectangle leftFrame6BodyHitbox = GetHairyLegBodyFrameHitbox(&leftKickFrame6, 1.0f);
    Rectangle rightFrame5BodyHitbox = GetHairyLegBodyFrameHitbox(&rightKickFrame5, 1.0f);
    Rectangle leftSweepFrame0Hitbox = GetHairyLegSweepFrameHitbox(&leftSweepFrame0, 1.0f);
    Rectangle leftSweepFrame4Hitbox = GetHairyLegSweepFrameHitbox(&leftSweepFrame4, 1.0f);
    Rectangle leftSweepFrame5Hitbox = GetHairyLegSweepFrameHitbox(&leftSweepFrame5, 1.0f);
    Rectangle rightSweepFrame4Hitbox = GetHairyLegSweepFrameHitbox(&rightSweepFrame4, 1.0f);
    Rectangle rightSweepFrame5Hitbox = GetHairyLegSweepFrameHitbox(&rightSweepFrame5, 1.0f);

    assert(HAIRY_LEG_KICK_FRAME_COUNT == 8);
    assert(HAIRY_LEG_SWEEP_FRAME_COUNT == 8);
    assert(fabsf(leftFrame5Hitbox.x - 33.0f) < 0.001f);
    assert(fabsf(leftFrame5Hitbox.y - 354.8f) < 0.001f);
    assert(fabsf(leftFrame5Hitbox.width - 71.0f) < 0.001f);
    assert(fabsf(leftFrame5Hitbox.height - 42.0f) < 0.001f);
    assert(fabsf(leftFrame6Hitbox.x - 88.0f) < 0.001f);
    assert(fabsf(leftFrame6Hitbox.width - 98.0f) < 0.001f);
    assert(fabsf(rightFrame5Hitbox.x - 143.0f) < 0.001f);
    assert(fabsf(rightFrame5Hitbox.width - 71.0f) < 0.001f);
    assert(fabsf(leftFrame5BodyHitbox.x - 120.0f) < 0.001f);
    assert(fabsf(leftFrame5BodyHitbox.y - 208.8f) < 0.001f);
    assert(fabsf(leftFrame5BodyHitbox.width - 80.0f) < 0.001f);
    assert(fabsf(leftFrame5BodyHitbox.height - 188.0f) < 0.001f);
    assert(fabsf(leftFrame6BodyHitbox.x - 130.0f) < 0.001f);
    assert(fabsf(leftFrame6BodyHitbox.width - 80.0f) < 0.001f);
    assert(fabsf(rightFrame5BodyHitbox.x - 47.0f) < 0.001f);
    assert(fabsf(rightFrame5BodyHitbox.width - 80.0f) < 0.001f);
    assert(fabsf(leftSweepFrame0Hitbox.x - 5.0f) < 0.001f);
    assert(fabsf(leftSweepFrame0Hitbox.y - 464.0f) < 0.001f);
    assert(fabsf(leftSweepFrame0Hitbox.width - 98.0f) < 0.001f);
    assert(fabsf(leftSweepFrame0Hitbox.height - 26.0f) < 0.001f);
    assert(fabsf(leftSweepFrame4Hitbox.x + 54.0f) < 0.001f);
    assert(fabsf(leftSweepFrame4Hitbox.y - 408.0f) < 0.001f);
    assert(fabsf(leftSweepFrame4Hitbox.width - 126.0f) < 0.001f);
    assert(fabsf(leftSweepFrame4Hitbox.height - 55.0f) < 0.001f);
    assert(fabsf(leftSweepFrame5Hitbox.x + 58.0f) < 0.001f);
    assert(fabsf(leftSweepFrame5Hitbox.y - 416.0f) < 0.001f);
    assert(fabsf(leftSweepFrame5Hitbox.width - 146.0f) < 0.001f);
    assert(fabsf(leftSweepFrame5Hitbox.height - 55.0f) < 0.001f);
    assert(fabsf(rightSweepFrame4Hitbox.x - 210.0f) < 0.001f);
    assert(fabsf(rightSweepFrame4Hitbox.y - 420.0f) < 0.001f);
    assert(fabsf(rightSweepFrame4Hitbox.width - 126.0f) < 0.001f);
    assert(fabsf(rightSweepFrame4Hitbox.height - 55.0f) < 0.001f);
    assert(fabsf(rightSweepFrame5Hitbox.x - 194.0f) < 0.001f);
    assert(fabsf(rightSweepFrame5Hitbox.y - 428.0f) < 0.001f);
    assert(fabsf(rightSweepFrame5Hitbox.width - 146.0f) < 0.001f);
    assert(fabsf(rightSweepFrame5Hitbox.height - 55.0f) < 0.001f);

    UpdateHairyLeg(&vulnerableLeg, playerOnRight, 0.1f, vulnerableLeg.groundY, 1.0f);
    assert(vulnerableLeg.state == HL_VULNERABLE);
    assert(vulnerableLeg.direction == -1);

    UpdateHairyLeg(&idleAfterRecoveryLeg, playerOnRight, 0.1f, idleAfterRecoveryLeg.groundY, 1.0f);
    assert(idleAfterRecoveryLeg.state == HL_IDLE);
    assert(idleAfterRecoveryLeg.direction == -1);

    rightKickFinishingLeg.groundY = 500.0f;
    rightKickFinishingLeg.rect.x = -300.0f;
    rightKickFinishingLeg.timer = 1.19f;
    rightKickFinishingLeg.sprites.kick.frameTime = 0.01f;
    rightKickFinishingLeg.sprites.idle.frameCount = 7;
    rightKickFinishingLeg.sprites.idle.frameTime = 0.01f;
    rightKickFinishingLeg.sprites.idle.sheet.width = 700;
    rightKickFinishingLeg.sprites.idle.sheet.height = 252;
    float bodyCenterBeforeKickEnds = CenterX(GetHairyLegBodyFrameHitbox(&rightKickFinishingLeg, 1.0f));
    UpdateHairyLeg(&rightKickFinishingLeg, playerOnRight, 0.02f, rightKickFinishingLeg.groundY, 1.0f);
    float bodyCenterAfterKickEnds = CenterX(rightKickFinishingLeg.bodyHitbox);
    assert(rightKickFinishingLeg.state == HL_VULNERABLE);
    assert(rightKickFinishingLeg.direction == 1);
    assert(fabsf(bodyCenterAfterKickEnds - bodyCenterBeforeKickEnds) < 0.001f);

    leftKickFinishingLeg.groundY = 500.0f;
    leftKickFinishingLeg.timer = 1.19f;
    leftKickFinishingLeg.sprites.kick.frameTime = 0.01f;
    leftKickFinishingLeg.sprites.idle.frameCount = 7;
    leftKickFinishingLeg.sprites.idle.frameTime = 0.01f;
    leftKickFinishingLeg.sprites.idle.sheet.width = 700;
    leftKickFinishingLeg.sprites.idle.sheet.height = 252;
    float leftBodyCenterBeforeKickEnds = CenterX(GetHairyLegBodyFrameHitbox(&leftKickFinishingLeg, 1.0f));
    UpdateHairyLeg(&leftKickFinishingLeg, playerOnRight, 0.02f, leftKickFinishingLeg.groundY, 1.0f);
    float leftBodyCenterAfterKickEnds = CenterX(leftKickFinishingLeg.bodyHitbox);
    assert(leftKickFinishingLeg.state == HL_VULNERABLE);
    assert(leftKickFinishingLeg.direction == -1);
    assert(fabsf(leftBodyCenterAfterKickEnds - leftBodyCenterBeforeKickEnds) < 0.001f);

    leftIdleChoosingNextActionLeg.health = 100;
    float bodyCenterBeforeDirectionFlip = CenterX(leftIdleChoosingNextActionLeg.bodyHitbox);
    UpdateHairyLeg(&leftIdleChoosingNextActionLeg, playerOnRight, 0.01f, leftIdleChoosingNextActionLeg.groundY, 1.0f);
    float bodyCenterAfterDirectionFlip = CenterX(leftIdleChoosingNextActionLeg.bodyHitbox);
    assert(leftIdleChoosingNextActionLeg.direction == 1);
    assert(fabsf(bodyCenterAfterDirectionFlip - bodyCenterBeforeDirectionFlip) < 0.001f);

    UpdateHairyLeg(&sweepStartLeg, unusedPlayer, 0.01f, sweepStartLeg.groundY, 1.0f);
    assert(sweepStartLeg.state == HL_SWEEPING);
    assert(sweepStartLeg.isKickActive);
    assert(sweepStartLeg.sprites.rasteira.currentFrame == 0);
    assert(fabsf(sweepStartLeg.kickHitbox.x - 6.0f) < 0.001f);
    assert(fabsf(sweepStartLeg.kickHitbox.y - 464.0f) < 0.001f);
    assert(fabsf(sweepStartLeg.kickHitbox.width - 98.0f) < 0.001f);
    assert(fabsf(sweepStartLeg.kickHitbox.height - 26.0f) < 0.001f);

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
