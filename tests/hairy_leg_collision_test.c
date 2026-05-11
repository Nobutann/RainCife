#include <assert.h>
#include <stdbool.h>

#include "entities/hairy_leg.h"

extern bool IsHairyLegKickColliding(const HairyLeg *leg, Rectangle playerHitbox);
extern Rectangle GetAnimationFrameSource(const Animation *animation, bool flipX);
extern float GetHairyLegSpriteOffsetX(const HairyLeg *leg, float scale);

static HairyLeg MakeJumpingLeg(void)
{
    HairyLeg leg = {0};

    leg.rect = (Rectangle){100.0f, 500.0f, 40.0f, 200.0f};
    leg.state = HL_JUMPING_UP;
    leg.groundY = 700.0f;
    leg.sprites.idle.frameCount = 7;
    leg.sprites.idle.frameTime = 0.01f;
    leg.sprites.idle.sheet = (Texture2D){0};
    leg.sprites.idle.sheet.width = 700;
    leg.sprites.idle.sheet.height = 252;
    leg.sprites.jump.frameCount = 4;
    leg.sprites.jump.frameTime = 0.01f;
    leg.sprites.jump.sheet = (Texture2D){0};
    leg.sprites.jump.sheet.width = 400;
    leg.sprites.jump.sheet.height = 315;
    leg.currentAnim = &leg.sprites.jump;

    return leg;
}

static HairyLeg MakeFallingLeg(void)
{
    HairyLeg leg = {0};

    leg.rect = (Rectangle){100.0f, 250.0f, 40.0f, 200.0f};
    leg.state = HL_HANGING;
    leg.timer = 2.1f;
    leg.direction = 1;
    leg.groundY = 500.0f;
    leg.sprites.idle.frameCount = 7;
    leg.sprites.idle.frameTime = 0.01f;
    leg.sprites.idle.sheet = (Texture2D){0};
    leg.sprites.idle.sheet.width = 700;
    leg.sprites.idle.sheet.height = 252;
    leg.sprites.fall.frameCount = 2;
    leg.sprites.fall.frameTime = 0.01f;
    leg.sprites.fall.sheet = (Texture2D){0};
    leg.sprites.fall.sheet.width = 504;
    leg.sprites.fall.sheet.height = 294;
    leg.currentAnim = &leg.sprites.idle;

    return leg;
}

static HairyLeg MakeSweepingLeg(int direction, float timer)
{
    HairyLeg leg = {0};

    leg.rect = (Rectangle){100.0f, 400.0f, 40.0f, 200.0f};
    leg.state = HL_SWEEPING;
    leg.timer = timer;
    leg.direction = direction;
    leg.groundY = 500.0f;
    leg.sprites.rasteira.frameCount = 4;
    leg.sprites.rasteira.frameTime = 0.01f;
    leg.sprites.rasteira.sheet = (Texture2D){0};
    leg.sprites.rasteira.sheet.width = 1008;
    leg.sprites.rasteira.sheet.height = 252;
    leg.currentAnim = &leg.sprites.rasteira;

    return leg;
}

int main(void)
{
    HairyLeg leg = {0};
    Rectangle playerHitbox = {20.0f, 30.0f, 10.0f, 10.0f};

    leg.isKickActive = true;
    leg.kickHitbox = (Rectangle){10.0f, 20.0f, 40.0f, 30.0f};
    leg.waveLeft.active = true;
    leg.waveLeft.rect = (Rectangle){200.0f, 200.0f, 10.0f, 10.0f};

    assert(IsHairyLegKickColliding(&leg, playerHitbox));

    leg.isKickActive = false;
    assert(!IsHairyLegKickColliding(&leg, playerHitbox));

    leg.isKickActive = true;
    playerHitbox = (Rectangle){200.0f, 200.0f, 5.0f, 5.0f};
    assert(!IsHairyLegKickColliding(&leg, playerHitbox));

    HairyLeg jumpingLeg = MakeJumpingLeg();
    Rectangle unusedPlayer = {0};
    float startY = jumpingLeg.rect.y;

    UpdateHairyLeg(&jumpingLeg, unusedPlayer, 0.005f, jumpingLeg.groundY, 1.0f);
    assert(jumpingLeg.sprites.jump.currentFrame == 0);
    assert(jumpingLeg.rect.y == startY);

    UpdateHairyLeg(&jumpingLeg, unusedPlayer, 0.005f, jumpingLeg.groundY, 1.0f);
    assert(jumpingLeg.sprites.jump.currentFrame == 1);
    assert(jumpingLeg.rect.y == startY);

    UpdateHairyLeg(&jumpingLeg, unusedPlayer, 0.01f, jumpingLeg.groundY, 1.0f);
    assert(jumpingLeg.sprites.jump.currentFrame == 2);
    assert(jumpingLeg.rect.y == startY);

    UpdateHairyLeg(&jumpingLeg, unusedPlayer, 0.01f, jumpingLeg.groundY, 1.0f);
    assert(jumpingLeg.sprites.jump.currentFrame == 3);
    assert(jumpingLeg.rect.y < startY);

    float airborneY = jumpingLeg.rect.y;
    UpdateHairyLeg(&jumpingLeg, unusedPlayer, 0.02f, jumpingLeg.groundY, 1.0f);
    assert(jumpingLeg.sprites.jump.currentFrame == 3);
    assert(jumpingLeg.rect.y < airborneY);

    HairyLeg rightFacingJump = MakeJumpingLeg();
    rightFacingJump.direction = 1;
    rightFacingJump.currentAnim = &rightFacingJump.sprites.idle;

    UpdateHairyLeg(&rightFacingJump, unusedPlayer, 0.03f, rightFacingJump.groundY, 1.0f);
    assert(rightFacingJump.currentAnim == &rightFacingJump.sprites.jump);
    assert(rightFacingJump.sprites.jump.currentFrame == 3);
    assert(rightFacingJump.rect.height == 258.3f);

    Animation jumpFrame = {0};
    jumpFrame.currentFrame = 3;
    jumpFrame.frameWidth = 252;
    jumpFrame.sheet.width = 1008;
    jumpFrame.sheet.height = 315;

    Rectangle flippedSource = GetAnimationFrameSource(&jumpFrame, true);
    assert(flippedSource.x == 756.0f);
    assert(flippedSource.width == -252.0f);

    HairyLeg rightFacingFall = MakeFallingLeg();
    UpdateHairyLeg(&rightFacingFall, unusedPlayer, 0.01f, rightFacingFall.groundY, 1.0f);
    assert(rightFacingFall.state == HL_FALLING);
    assert(rightFacingFall.sprites.fall.currentFrame == 0);

    rightFacingFall.rect.y = 260.0f;
    UpdateHairyLeg(&rightFacingFall, unusedPlayer, 0.01f, rightFacingFall.groundY, 1.0f);
    assert(rightFacingFall.sprites.fall.currentFrame == 1);

    HairyLeg rightSweepStartup = MakeSweepingLeg(1, 0.1f);
    UpdateHairyLeg(&rightSweepStartup, unusedPlayer, 0.01f, rightSweepStartup.groundY, 1.0f);
    assert(rightSweepStartup.sprites.rasteira.currentFrame == 1);

    HairyLeg rightSweepActive = MakeSweepingLeg(1, 0.6f);
    UpdateHairyLeg(&rightSweepActive, unusedPlayer, 0.01f, rightSweepActive.groundY, 1.0f);
    assert(rightSweepActive.isKickActive);
    assert(rightSweepActive.sprites.rasteira.currentFrame == 3);

    HairyLeg leftKick = {0};
    leftKick.state = HL_KICKING;
    leftKick.direction = -1;
    assert(GetHairyLegSpriteOffsetX(&leftKick, 1.0f) == -115.0f);

    HairyLeg rightKick = {0};
    rightKick.state = HL_KICKING;
    rightKick.direction = 1;
    assert(GetHairyLegSpriteOffsetX(&rightKick, 1.0f) == -200.0f);

    return 0;
}
