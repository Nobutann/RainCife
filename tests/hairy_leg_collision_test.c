#include <assert.h>
#include <math.h>
#include <stdbool.h>

#include "entities/hairy_leg.h"
#include "entities/player.h"

extern bool IsHairyLegKickColliding(const HairyLeg *leg, Rectangle playerHitbox);
extern Rectangle GetAnimationFrameSource(const Animation *animation, bool flipX);
extern float GetHairyLegSpriteOffsetX(const HairyLeg *leg, float scale);
extern bool ShouldHairyLegJumpBackFromCorner(const HairyLeg *leg, Rectangle playerRect, float screenWidth);
extern Rectangle GetPlayerAttackHitbox(Player *player, float scale);
extern void PlacePlayerForBossIntro(Player *player, Rectangle bossHitbox, float groundY, float scale);
extern bool TryDamageHairyLegFromPlayerAttack(HairyLeg *leg, Player *player, float playerScale);

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

static Player MakeAttackingPlayer(bool facingRight)
{
    Player player = {0};

    player.position = (Vector2){100.0f, 50.0f};
    player.facingRight = facingRight;
    player.weapon.attacking = true;
    player.weapon.hitConnected = false;
    player.weapon.damage = 2.0f;
    player.sprites.walkFront.layerCount = 1;
    player.sprites.walkFront.layers[0].frameWidth = 100;
    player.sprites.walkFront.layers[0].sheet.height = 200;
    player.currentAnim = &player.sprites.walkFront;

    return player;
}

static Player MakeBossIntroPlayer(void)
{
    Player player = {0};

    player.position = (Vector2){900.0f, 10.0f};
    player.velocity = (Vector2){250.0f, -300.0f};
    player.onGround = false;
    player.isBossFighting = false;
    player.isJumping = true;
    player.jumpHoldTimer = 0.2f;
    player.weapon.attacking = true;
    player.weapon.attackTimer = 0.3f;
    player.weapon.hitConnected = true;
    player.sprites.idle.layerCount = 1;
    player.sprites.idle.layers[0].frameWidth = 100;
    player.sprites.idle.layers[0].sheet.height = 200;
    player.sprites.walkFront.layerCount = 1;
    player.sprites.walkFront.layers[0].frameWidth = 100;
    player.sprites.walkFront.layers[0].sheet.height = 200;
    player.currentAnim = &player.sprites.walkFront;

    return player;
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

    HairyLeg rightCornerPressure = {0};
    rightCornerPressure.rect = (Rectangle){800.0f, 300.0f, 40.0f, 200.0f};
    rightCornerPressure.direction = 1;
    Rectangle rightPlayerWithRoom = {840.0f, 300.0f, 40.0f, 80.0f};
    assert(ShouldHairyLegJumpBackFromCorner(&rightCornerPressure, rightPlayerWithRoom, 1280.0f));

    Player introPlayer = MakeBossIntroPlayer();
    Rectangle introBossHitbox = {500.0f, 300.0f, 40.0f, 200.0f};
    PlacePlayerForBossIntro(&introPlayer, introBossHitbox, 520.0f, 1.0f);
    Rectangle introPlayerHitbox = GetPlayerHitbox(&introPlayer, 1.0f);
    assert(fabsf((introPlayerHitbox.x + introPlayerHitbox.width) - 280.0f) < 0.001f);
    assert(fabsf(introPlayer.position.y - 300.0f) < 0.001f);
    assert(introPlayer.velocity.x == 0.0f);
    assert(introPlayer.velocity.y == 0.0f);
    assert(introPlayer.onGround);
    assert(introPlayer.isBossFighting);
    assert(!introPlayer.isJumping);
    assert(!introPlayer.weapon.attacking);
    assert(!introPlayer.weapon.hitConnected);
    assert(introPlayer.currentAnim == &introPlayer.sprites.idle);

    Player rightAttackPlayer = MakeAttackingPlayer(false);
    Rectangle rightAttackHitbox = GetPlayerAttackHitbox(&rightAttackPlayer, 1.0f);
    Rectangle rightPlayerBody = GetPlayerHitbox(&rightAttackPlayer, 1.0f);
    assert(rightAttackHitbox.x == rightPlayerBody.x + rightPlayerBody.width);

    Player leftAttackPlayer = MakeAttackingPlayer(true);
    Rectangle leftAttackHitbox = GetPlayerAttackHitbox(&leftAttackPlayer, 1.0f);
    Rectangle leftPlayerBody = GetPlayerHitbox(&leftAttackPlayer, 1.0f);
    assert(leftAttackHitbox.x + leftAttackHitbox.width == leftPlayerBody.x);

    HairyLeg damagedLeg = {0};
    damagedLeg.health = 100;
    damagedLeg.rect = rightAttackHitbox;
    assert(TryDamageHairyLegFromPlayerAttack(&damagedLeg, &rightAttackPlayer, 1.0f));
    assert(damagedLeg.health == 98);
    assert(rightAttackPlayer.weapon.hitConnected);

    assert(!TryDamageHairyLegFromPlayerAttack(&damagedLeg, &rightAttackPlayer, 1.0f));
    assert(damagedLeg.health == 98);

    DamageHairyLeg(&damagedLeg, 200);
    assert(damagedLeg.health == 0);
    assert(damagedLeg.state == HL_DEAD);
    assert(damagedLeg.currentAnim == &damagedLeg.sprites.death);

    HairyLeg deathLeg = {0};
    deathLeg.health = 2;
    deathLeg.state = HL_KICKING;
    deathLeg.isKickActive = true;
    deathLeg.rect = (Rectangle){120.0f, 300.0f, 40.0f, 200.0f};
    deathLeg.groundY = 500.0f;
    deathLeg.sprites.death.frameCount = 1;
    deathLeg.sprites.death.frameTime = 0.01f;
    deathLeg.sprites.death.sheet = (Texture2D){0};
    deathLeg.sprites.death.sheet.width = 100;
    deathLeg.sprites.death.sheet.height = 252;
    DamageHairyLeg(&deathLeg, 2);
    assert(deathLeg.health == 0);
    assert(deathLeg.state == HL_DEAD);
    assert(deathLeg.currentAnim == &deathLeg.sprites.death);
    assert(deathLeg.sprites.death.currentFrame == 0);
    assert(!deathLeg.isKickActive);

    UpdateHairyLeg(&deathLeg, unusedPlayer, 0.5f, deathLeg.groundY, 1.0f);
    assert(deathLeg.state == HL_DEAD);
    assert(deathLeg.currentAnim == &deathLeg.sprites.death);
    assert(deathLeg.timer >= 0.5f);

    HairyLeg sweepRecovery = MakeSweepingLeg(1, 1.2f);
    UpdateHairyLeg(&sweepRecovery, unusedPlayer, 0.01f, sweepRecovery.groundY, 1.0f);
    assert(sweepRecovery.state == HL_SWEEP_RECOVERING);
    assert(sweepRecovery.rect.height == 60.0f);
    assert(!sweepRecovery.isKickActive);
    assert(GetHairyLegSpriteOffsetX(&sweepRecovery, 1.0f) == -90.0f);

    UpdateHairyLeg(&sweepRecovery, unusedPlayer, 0.3f, sweepRecovery.groundY, 1.0f);
    assert(sweepRecovery.state == HL_SWEEP_RECOVERING);
    assert(sweepRecovery.rect.height > 60.0f);
    assert(sweepRecovery.rect.height < 120.0f);
    assert(GetHairyLegSpriteOffsetX(&sweepRecovery, 1.0f) == -90.0f);

    UpdateHairyLeg(&sweepRecovery, unusedPlayer, 0.3f, sweepRecovery.groundY, 1.0f);
    assert(sweepRecovery.state == HL_SWEEP_RECOVERING);
    assert(sweepRecovery.rect.height < 206.64f);

    UpdateHairyLeg(&sweepRecovery, unusedPlayer, 0.4f, sweepRecovery.groundY, 1.0f);
    assert(sweepRecovery.state == HL_VULNERABLE);
    assert(sweepRecovery.rect.height == 206.64f);

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
