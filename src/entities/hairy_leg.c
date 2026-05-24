#include "entities/hairy_leg.h"
#include "entities/player.h"
#include <stdbool.h>
#include "core/sounds.h"

#define HAIRY_LEG_JUMP_STARTUP_FRAMES 3
#define HAIRY_LEG_JUMP_LOCKED_FRAME 3
#define HAIRY_LEG_JUMP_FALLBACK_FRAME_TIME 0.08f
#define HAIRY_LEG_SWEEP_HEIGHT 60.0f
#define HAIRY_LEG_SWEEP_TIME 1.2f
#define HAIRY_LEG_SWEEP_RECOVER_TIME 1.0f
#define HAIRY_LEG_LEFT_CORNER_PLAYER_MARGIN 300.0f
#define HAIRY_LEG_RIGHT_CORNER_PLAYER_MARGIN 420.0f
#define HAIRY_LEG_CORNER_LEG_MARGIN 500.0f
#define HAIRY_LEG_HANGING_WARNING_TIME 2.0f
#define HAIRY_LEG_SHADOW_GROUND_OFFSET 42.0f
#define HAIRY_LEG_SHOCKWAVE_FALLBACK_WIDTH 88.0f
#define HAIRY_LEG_SHOCKWAVE_FALLBACK_HEIGHT 95.0f
#define HAIRY_LEG_SHOCKWAVE_DRAW_SCALE 0.45f
#define HAIRY_LEG_SHOCKWAVE_SPEED 720.0f
#define HAIRY_LEG_SHOCKWAVE_FRAME_TIME 0.08f
#define HAIRY_LEG_SHOCKWAVE_HITBOX_WIDTH_RATIO 0.55f
#define HAIRY_LEG_SHOCKWAVE_HITBOX_HEIGHT_RATIO 0.35f
#define HAIRY_LEG_SHOCKWAVE_HITBOX_BOTTOM_MARGIN_RATIO 0.06f
#define HAIRY_LEG_SHOCKWAVE_VERTICAL_OFFSET 14.0f
#define HAIRY_LEG_SWEEP_RIGHT_HITBOX_VERTICAL_OFFSET 12.0f
#define HAIRY_LEG_DEATH_SPRITE_BOTTOM_TRANSPARENT_PIXELS 31.0f
#define HAIRY_LEG_HIT_FLASH_DURATION 0.12f

static const Rectangle HAIRY_LEG_KICK_FRAME_HITBOXES[HAIRY_LEG_KICK_FRAME_COUNT] = {
    {174.0f, 214.0f, 105.0f, 27.0f},
    {174.0f, 210.0f, 105.0f, 31.0f},
    {174.0f, 204.0f, 106.0f, 37.0f},
    { 60.0f, 180.0f,  68.0f, 40.0f},
    { 45.0f, 180.0f,  63.0f, 37.0f},
    { 48.0f, 180.0f,  71.0f, 42.0f},
    {103.0f, 180.0f,  98.0f, 54.0f},
    {174.0f, 210.0f, 105.0f, 31.0f}
};

static const Rectangle HAIRY_LEG_KICK_BODY_HITBOXES[HAIRY_LEG_KICK_FRAME_COUNT] = {
    {205.0f,  8.0f, 80.0f, 220.0f},
    {180.0f, 30.0f, 80.0f, 210.0f},
    {174.0f, 36.0f, 80.0f, 200.0f},
    {150.0f, 38.0f, 80.0f, 182.0f},
    {125.0f, 36.0f, 80.0f, 184.0f},
    {135.0f, 34.0f, 80.0f, 188.0f},
    {145.0f, 24.0f, 80.0f, 208.0f},
    {180.0f, 30.0f, 80.0f, 210.0f}
};

static const Rectangle HAIRY_LEG_SWEEP_FRAME_HITBOXES[HAIRY_LEG_SWEEP_FRAME_COUNT] = {
    { 65.0f, 216.0f,  98.0f, 26.0f},
    { 64.0f, 216.0f,  92.0f, 26.0f},
    { 75.0f, 215.0f,  98.0f, 27.0f},
    { 65.0f, 216.0f,  95.0f, 25.0f},
    {  6.0f, 160.0f, 126.0f, 55.0f},
    {  2.0f, 168.0f, 146.0f, 55.0f},
    {  0.0f, 165.0f, 138.0f, 50.0f},
    {  6.0f, 170.0f, 104.0f, 55.0f}
};

static Animation *GetHairyLegAnimationForState(HairyLeg *leg) {
    switch (leg->state) {
        case HL_IDLE:
        case HL_VULNERABLE:
        case HL_HANGING:
            return &leg->sprites.idle;
        case HL_JUMPING_UP:
        case HL_JUMPING_BACK:
            return &leg->sprites.jump;
        case HL_FALLING:
            return &leg->sprites.fall;
        case HL_KICKING:
            return &leg->sprites.kick;
        case HL_SWEEPING:
            return &leg->sprites.rasteira;
        case HL_SWEEP_RECOVERING:
            return &leg->sprites.recovery;
        case HL_DEAD:
            return &leg->sprites.death;
    }

    return &leg->sprites.idle;
}

static void SyncHairyLegAnimation(HairyLeg *leg) {
    leg->currentAnim = GetHairyLegAnimationForState(leg);
}

static int GetHairyLegDirectionTowardPlayer(const HairyLeg *leg, Rectangle playerRect) {
    float centroRato = playerRect.x + (playerRect.width / 2.0f);
    float centroPerna = leg->rect.x + (leg->rect.width / 2.0f);

    return centroRato > centroPerna ? 1 : -1;
}

static int GetHairyLegFrameForProgress(float progress, int frameCount) {
    if (frameCount <= 0) {
        return 0;
    }

    if (progress < 0.0f) {
        progress = 0.0f;
    }
    if (progress >= 1.0f) {
        progress = 1.0f;
    }

    int frame = (int)(progress * frameCount);
    if (frame >= frameCount) {
        frame = frameCount - 1;
    }

    return frame;
}

static int GetHairyLegShockwaveFrameCount(const HairyLeg *leg) {
    int width = leg->sprites.shockwave.width;
    int height = leg->sprites.shockwave.height;

    if (height > 0 && width >= height * 2 && width % height == 0) {
        return width / height;
    }

    return 1;
}

static float GetHairyLegShockwaveFrameWidth(const HairyLeg *leg) {
    float width = leg->sprites.shockwave.width > 0
        ? (float)leg->sprites.shockwave.width
        : HAIRY_LEG_SHOCKWAVE_FALLBACK_WIDTH;

    int frameCount = GetHairyLegShockwaveFrameCount(leg);
    if (frameCount > 1) {
        width /= (float)frameCount;
    }

    return width;
}

static float GetHairyLegShockwaveWidth(const HairyLeg *leg, float scale) {
    float width = GetHairyLegShockwaveFrameWidth(leg);
    return width * scale * HAIRY_LEG_SHOCKWAVE_DRAW_SCALE;
}

static float GetHairyLegShockwaveHeight(const HairyLeg *leg, float scale) {
    float height = leg->sprites.shockwave.height > 0
        ? (float)leg->sprites.shockwave.height
        : HAIRY_LEG_SHOCKWAVE_FALLBACK_HEIGHT;
    return height * scale * HAIRY_LEG_SHOCKWAVE_DRAW_SCALE;
}

static Rectangle GetHairyLegShockwaveHitbox(Rectangle visualRect) {
    float hitboxW = visualRect.width * HAIRY_LEG_SHOCKWAVE_HITBOX_WIDTH_RATIO;
    float hitboxH = visualRect.height * HAIRY_LEG_SHOCKWAVE_HITBOX_HEIGHT_RATIO;
    float bottomMargin = visualRect.height * HAIRY_LEG_SHOCKWAVE_HITBOX_BOTTOM_MARGIN_RATIO;

    return (Rectangle){
        visualRect.x + (visualRect.width - hitboxW) * 0.5f,
        visualRect.y + visualRect.height - hitboxH - bottomMargin,
        hitboxW,
        hitboxH
    };
}

static Shockwave MakeHairyLegShockwave(Rectangle visualRect, Vector2 speed) {
    Shockwave wave = {0};
    wave.rect = visualRect;
    wave.hitbox = GetHairyLegShockwaveHitbox(visualRect);
    wave.speed = speed;
    wave.active = true;
    return wave;
}

static void SyncHairyLegShockwaveHitbox(Shockwave *wave) {
    wave->hitbox = GetHairyLegShockwaveHitbox(wave->rect);
}

static void UpdateHairyLegShockwaveAnimation(Shockwave *wave, const HairyLeg *leg, float deltaTime) {
    if (!wave->active) {
        return;
    }

    int frameCount = GetHairyLegShockwaveFrameCount(leg);
    if (frameCount <= 1) {
        wave->currentFrame = 0;
        wave->frameTimer = 0.0f;
        return;
    }

    wave->frameTimer += deltaTime;
    while (wave->frameTimer >= HAIRY_LEG_SHOCKWAVE_FRAME_TIME) {
        wave->frameTimer -= HAIRY_LEG_SHOCKWAVE_FRAME_TIME;
        wave->currentFrame++;
        if (wave->currentFrame >= frameCount) {
            wave->currentFrame = 0;
        }
    }
}

static void ResetHairyLegShadowWarning(HairyLeg *leg) {
    leg->sprites.shadow.currentFrame = 0;
    leg->sprites.shadow.timer = 0.0f;
}

static void UpdateHairyLegShadowWarning(HairyLeg *leg) {
    Animation *shadow = &leg->sprites.shadow;
    if (shadow->frameCount <= 0) {
        return;
    }

    float progress = leg->timer / HAIRY_LEG_HANGING_WARNING_TIME;
    if (progress < 0.0f) {
        progress = 0.0f;
    }
    if (progress >= 1.0f) {
        progress = 1.0f;
    }

    int frame = (int)(progress * shadow->frameCount);
    if (frame >= shadow->frameCount) {
        frame = shadow->frameCount - 1;
    }

    shadow->currentFrame = frame;
    shadow->timer = 0.0f;
}

float GetHairyLegSpriteOffsetX(const HairyLeg *leg, float scale) {
    bool flipX = (leg->direction == 1);

    if (leg->state == HL_SWEEPING) {
        return flipX ? -(scale * 10.0f) : -(scale * 160.0f);
    }

    if (leg->state == HL_KICKING) {
        return flipX ? -(scale * 200.0f) : -(scale * 115.0f);
    }

    return flipX ? -(scale * 90.0f) : -(scale * 115.0f);
}

float GetHairyLegSpriteOffsetY(const HairyLeg *leg, float scale) {
    float currentSpriteH = (float)leg->currentAnim->sheet.height * scale;
    float emptyBottom = currentSpriteH * 0.08f;
    float offsetY = -(currentSpriteH - emptyBottom - leg->rect.height);

    if (leg->state == HL_DEAD) {
        offsetY += HAIRY_LEG_DEATH_SPRITE_BOTTOM_TRANSPARENT_PIXELS * scale;
    }

    return offsetY;
}

static Rectangle GetHairyLegFrameHitbox(const HairyLeg *leg, float scale, const Animation *animation, const Rectangle *localHitboxes, int frameCount) {
    if (frameCount <= 0) {
        return leg->rect;
    }

    int frame = animation->currentFrame;

    if (frame < 0) {
        frame = 0;
    }
    if (frame >= frameCount) {
        frame = frameCount - 1;
    }

    Rectangle localHitbox = localHitboxes[frame];
    float frameWidth = animation->frameWidth > 0
        ? (float)animation->frameWidth
        : (float)animation->sheet.width / (float)frameCount;
    bool flipX = (leg->direction == 1);

    if (flipX) {
        localHitbox.x = frameWidth - localHitbox.x - localHitbox.width;
    }

    float currentSpriteH = (float)animation->sheet.height * scale;
    float emptyBottom = currentSpriteH * 0.08f;
    float offsetX = GetHairyLegSpriteOffsetX(leg, scale);
    float offsetY = -(currentSpriteH - emptyBottom - leg->rect.height);

    return (Rectangle){
        leg->rect.x + offsetX + localHitbox.x * scale,
        leg->rect.y + offsetY + localHitbox.y * scale,
        localHitbox.width * scale,
        localHitbox.height * scale
    };
}

Rectangle GetHairyLegBodyFrameHitbox(const HairyLeg *leg, float scale) {
    return GetHairyLegFrameHitbox(leg, scale, &leg->sprites.kick, HAIRY_LEG_KICK_BODY_HITBOXES, HAIRY_LEG_KICK_FRAME_COUNT);
}

Rectangle GetHairyLegKickFrameHitbox(const HairyLeg *leg, float scale) {
    return GetHairyLegFrameHitbox(leg, scale, &leg->sprites.kick, HAIRY_LEG_KICK_FRAME_HITBOXES, HAIRY_LEG_KICK_FRAME_COUNT);
}

Rectangle GetHairyLegSweepFrameHitbox(const HairyLeg *leg, float scale) {
    Rectangle hitbox = GetHairyLegFrameHitbox(leg, scale, &leg->sprites.rasteira, HAIRY_LEG_SWEEP_FRAME_HITBOXES, HAIRY_LEG_SWEEP_FRAME_COUNT);

    if (leg->direction == 1) {
        hitbox.y += HAIRY_LEG_SWEEP_RIGHT_HITBOX_VERTICAL_OFFSET * scale;
    }

    return hitbox;
}

static float GetRectangleCenterX(Rectangle rect) {
    return rect.x + rect.width / 2.0f;
}

static float GetHairyLegCurrentBodyCenterX(const HairyLeg *leg, float scale) {
    if (leg->state == HL_KICKING) {
        return GetRectangleCenterX(GetHairyLegBodyFrameHitbox(leg, scale));
    }

    return GetRectangleCenterX(leg->rect);
}

static void SetHairyLegCurrentBodyCenterX(HairyLeg *leg, float bodyCenterX, float scale) {
    float currentBodyCenterX = GetHairyLegCurrentBodyCenterX(leg, scale);
    leg->rect.x += bodyCenterX - currentBodyCenterX;
}

static void SetHairyLegStatePreservingBodyCenterX(HairyLeg *leg, HairyLegState state, float scale) {
    float bodyCenterX = GetHairyLegCurrentBodyCenterX(leg, scale);

    leg->state = state;
    SetHairyLegCurrentBodyCenterX(leg, bodyCenterX, scale);
}

void ResetHairyLeg(HairyLeg *leg, Vector2 startPosition, float groundY, float scale) {
    leg->currentAnim = &leg->sprites.idle;
    leg->groundY = groundY;
    float spriteH = (float)leg->currentAnim->sheet.height * scale;
    float emptyTop = spriteH * 0.10f;
    float emptyBottom = spriteH * 0.08f;
    float hitboxH = spriteH - emptyTop - emptyBottom;
    leg->rect = (Rectangle){ startPosition.x, groundY - emptyBottom - hitboxH, 40.0f * scale, hitboxH };
    leg->bodyHitbox = leg->rect;
    leg->state = HL_IDLE;
    leg->timer = 0.0f;
    leg->health = 100;
    leg->hitFlashTimer = 0.0f;
    leg->waveLeft = (Shockwave){0};
    leg->waveRight = (Shockwave){0};
    leg->isKickActive = false;
    leg->direction = -1;
    leg->sprites.idle.currentFrame = 0;
    leg->sprites.idle.timer = 0.0f;
    leg->sprites.jump.currentFrame = 0;
    leg->sprites.jump.timer = 0.0f;
    leg->sprites.fall.currentFrame = 0;
    leg->sprites.fall.timer = 0.0f;
    leg->sprites.kick.currentFrame = 0;
    leg->sprites.kick.timer = 0.0f;
    leg->sprites.rasteira.currentFrame = 0;
    leg->sprites.rasteira.timer = 0.0f;
    leg->sprites.recovery.currentFrame = 0;
    leg->sprites.recovery.timer = 0.0f;
    leg->sprites.death.currentFrame = 0;
    leg->sprites.death.timer = 0.0f;
    ResetHairyLegShadowWarning(leg);
    StopLegSweepSound();
}

void InitHairyLeg(HairyLeg *leg, Vector2 startPosition, float groundY, float scale) {
    LoadHairyLegSprites(&leg->sprites);
    ResetHairyLeg(leg, startPosition, groundY, scale);
}

bool IsHairyLegKickColliding(const HairyLeg *leg, Rectangle playerHitbox) {
    return leg->isKickActive && CheckCollisionRecs(playerHitbox, leg->kickHitbox);
}

void DamageHairyLeg(HairyLeg *leg, int damage) {
    if (damage <= 0) {
        return;
    }

    leg->health -= damage;
    leg->hitFlashTimer = HAIRY_LEG_HIT_FLASH_DURATION;
    if (leg->health < 0) {
        leg->health = 0;
    }

    if (leg->health == 0 && leg->state != HL_DEAD) {
        leg->state = HL_DEAD;
        leg->timer = 0.0f;
        leg->isKickActive = false;
        leg->waveLeft.active = false;
        leg->waveRight.active = false;
        leg->sprites.death.currentFrame = 0;
        leg->sprites.death.timer = 0.0f;
        leg->currentAnim = &leg->sprites.death;
        StopLegSweepSound();
    }
}

bool ShouldHairyLegJumpBackFromCorner(const HairyLeg *leg, Rectangle playerRect, float screenWidth) {
    float playerRight = playerRect.x + playerRect.width;

    bool encurraladoEsquerda = (
        playerRect.x < HAIRY_LEG_LEFT_CORNER_PLAYER_MARGIN &&
        leg->direction == -1 &&
        leg->rect.x < HAIRY_LEG_CORNER_LEG_MARGIN
    );
    bool encurraladoDireita = (
        playerRight > screenWidth - HAIRY_LEG_RIGHT_CORNER_PLAYER_MARGIN &&
        leg->direction == 1 &&
        leg->rect.x > screenWidth - HAIRY_LEG_CORNER_LEG_MARGIN
    );

    return encurraladoEsquerda || encurraladoDireita;
}

bool TryDamageHairyLegFromPlayerAttack(HairyLeg *leg, Player *player, float playerScale) {
    if (leg->state == HL_DEAD) {
        return false;
    }

    if (!IsPlayerAttackHitboxActive(player) || player->weapon.hitConnected) {
        return false;
    }

    Rectangle attackHitbox = GetPlayerAttackHitbox(player, playerScale);
    Rectangle bodyHitbox = (leg->bodyHitbox.width > 0.0f && leg->bodyHitbox.height > 0.0f)
        ? leg->bodyHitbox
        : leg->rect;
    if (!CheckCollisionRecs(attackHitbox, bodyHitbox)) {
        return false;
    }

    DamageHairyLeg(leg, (int)player->weapon.damage);
    player->weapon.hitConnected = true;
    return true;
}

void UpdateHairyLeg(HairyLeg *leg, Rectangle playerRect, float deltaTime, float groundY, float scale) {
    leg->groundY = groundY;
    if (leg->hitFlashTimer > 0.0f) {
        leg->hitFlashTimer -= deltaTime;
        if (leg->hitFlashTimer < 0.0f) {
            leg->hitFlashTimer = 0.0f;
        }
    }
    SyncHairyLegAnimation(leg);

    float currentSpriteH = (float)leg->currentAnim->sheet.height * scale;
    float emptyTop = currentSpriteH * 0.10f;
    float emptyBottom = currentSpriteH * 0.08f;
    float defaultHitboxH = currentSpriteH - emptyTop - emptyBottom;
    leg->rect.height = defaultHitboxH;
    leg->isKickActive = false;

    if (leg->state != HL_JUMPING_UP && leg->state != HL_FALLING && leg->state != HL_HANGING && leg->state != HL_SWEEPING && leg->state != HL_SWEEP_RECOVERING) {
        leg->rect.y = leg->groundY - emptyBottom - leg->rect.height;
    }

    if (leg->waveLeft.active) {
        leg->waveLeft.rect.x -= leg->waveLeft.speed.x * deltaTime;
        SyncHairyLegShockwaveHitbox(&leg->waveLeft);
        UpdateHairyLegShockwaveAnimation(&leg->waveLeft, leg, deltaTime);
    }
    if (leg->waveRight.active) {
        leg->waveRight.rect.x += leg->waveRight.speed.x * deltaTime;
        SyncHairyLegShockwaveHitbox(&leg->waveRight);
        UpdateHairyLegShockwaveAnimation(&leg->waveRight, leg, deltaTime);
    }

    switch (leg->state) {
        case HL_IDLE:
            leg->timer += deltaTime;
            if (leg->timer > 1.0f) {
                int screenW = GetScreenWidth();
                float bodyCenterX = GetHairyLegCurrentBodyCenterX(leg, scale);
                leg->direction = GetHairyLegDirectionTowardPlayer(leg, playerRect);

                if (ShouldHairyLegJumpBackFromCorner(leg, playerRect, (float)screenW)) {
                    leg->state = HL_JUMPING_BACK;
                    leg->sprites.jump.currentFrame = 0;
                    leg->timer = 0.0f;
                }
                else {
                int chance = GetRandomValue(1, 100);

                if (leg->health > 70) {
                    if (chance <= 60) { leg->state = HL_KICKING; leg->sprites.kick.currentFrame = 0; leg->sprites.kick.timer = 0.0f; PlayLegStompSound(); }
                    else if (chance <= 80) { leg->state = HL_SWEEPING; leg->sprites.rasteira.currentFrame = 0; leg->sprites.rasteira.timer = 0.0f; }
                    else { leg->state = HL_JUMPING_UP; leg->sprites.jump.currentFrame = 0; leg->sprites.jump.timer = 0.0f; PlayLegJumpSound(); }
                }
                else if (leg->health > 40) {
                    if (chance <= 33) { leg->state = HL_KICKING; leg->sprites.kick.currentFrame = 0; leg->sprites.kick.timer = 0.0f; PlayLegStompSound(); }
                    else if (chance <= 66) { leg->state = HL_SWEEPING; leg->sprites.rasteira.currentFrame = 0; leg->sprites.rasteira.timer = 0.0f; }
                    else { leg->state = HL_JUMPING_UP; leg->sprites.jump.currentFrame = 0; leg->sprites.jump.timer = 0.0f; PlayLegJumpSound(); }
                }
                else {
                    if (chance <= 40) { leg->state = HL_JUMPING_UP; leg->sprites.jump.currentFrame = 0; leg->sprites.jump.timer = 0.0f; PlayLegJumpSound(); }
                    else if (chance <= 80) { leg->state = HL_SWEEPING; leg->sprites.rasteira.currentFrame = 0; leg->sprites.rasteira.timer = 0.0f; }
                    else { leg->state = HL_KICKING; leg->sprites.kick.currentFrame = 0; leg->sprites.kick.timer = 0.0f; PlayLegStompSound(); }
                }
                }
                leg->timer = 0.0f;
                SetHairyLegCurrentBodyCenterX(leg, bodyCenterX, scale);
            }
            break;

        case HL_JUMPING_UP:
            leg->timer += deltaTime;
            float jumpFrameTime = leg->sprites.jump.frameTime;
            if (jumpFrameTime <= 0.0f) {
                jumpFrameTime = HAIRY_LEG_JUMP_FALLBACK_FRAME_TIME;
            }

            if (leg->timer < jumpFrameTime) {
                leg->sprites.jump.currentFrame = 0;
            }
            else if (leg->timer < jumpFrameTime * 2.0f) {
                leg->sprites.jump.currentFrame = 1;
            }
            else if (leg->timer < jumpFrameTime * HAIRY_LEG_JUMP_STARTUP_FRAMES) {
                leg->sprites.jump.currentFrame = 2;
            }
            else {
                leg->sprites.jump.currentFrame = HAIRY_LEG_JUMP_LOCKED_FRAME;
                leg->rect.y -= 3000 * deltaTime;
            }

            if (leg->rect.y < -leg->rect.height) {
                leg->state = HL_HANGING;
                leg->rect.x = playerRect.x;
                leg->timer = 0.0f;
                ResetHairyLegShadowWarning(leg);
            }
            break;

            case HL_JUMPING_BACK:
                leg->timer += deltaTime;
                leg->rect.y -= 1200 * deltaTime;
                leg->rect.x -= 2000 * leg->direction * deltaTime;

                if (leg->timer > 0.4f) {
                    leg->state = HL_FALLING;
                    leg->sprites.fall.currentFrame = 0;
                    leg->timer = 0.0f;
                }
                break;

        case HL_HANGING:
            leg->timer += deltaTime;
            leg->rect.x = playerRect.x;

            if(leg->timer > HAIRY_LEG_HANGING_WARNING_TIME){
                leg->state = HL_FALLING;
                leg->sprites.fall.currentFrame = 0;
                leg->sprites.fall.timer = 0.0f;
                leg->timer = 0.0f;
                ResetHairyLegShadowWarning(leg);
            } else {
                UpdateHairyLegShadowWarning(leg);
            }

            break;


            case HL_FALLING:
                if (leg->sprites.fall.currentFrame == 0) {
                    leg->rect.y += 1700 * deltaTime;

                    if (leg->rect.y >= leg->groundY - emptyBottom - defaultHitboxH) {
                        float waveW = GetHairyLegShockwaveWidth(leg, scale);
                        float waveH = GetHairyLegShockwaveHeight(leg, scale);

                        leg->rect.y = leg->groundY - emptyBottom - defaultHitboxH;
                        float legHitboxTipY = leg->rect.y + leg->rect.height;
                        float waveY = legHitboxTipY - waveH + (HAIRY_LEG_SHOCKWAVE_VERTICAL_OFFSET * scale);

                        leg->waveLeft = MakeHairyLegShockwave((Rectangle){leg->rect.x - waveW, waveY, waveW, waveH}, (Vector2){HAIRY_LEG_SHOCKWAVE_SPEED, 0});
                        leg->waveRight = MakeHairyLegShockwave((Rectangle){leg->rect.x + leg->rect.width, waveY, waveW, waveH}, (Vector2){HAIRY_LEG_SHOCKWAVE_SPEED, 0});
                        PlayLegShockwaveSound();

                        leg->sprites.fall.currentFrame = 1;
                        leg->timer = 0.0f;
                    }
                } else {
                    leg->timer += deltaTime;
                    if (leg->timer > 0.5f) {
                        leg->state = HL_IDLE;
                        leg->sprites.idle.currentFrame = 0;
                        leg->timer = 0.0f;
                    }
                }
                break;

        case HL_VULNERABLE:
            leg->timer += deltaTime;
            if (leg->timer > 0.5f) {
                leg->state = HL_IDLE;
                leg->sprites.idle.currentFrame = 0;
                leg->timer = 0.0f;
            }

            break;

        case HL_SWEEPING:
            leg->timer += deltaTime;
            leg->rect.height = HAIRY_LEG_SWEEP_HEIGHT * scale;
            leg->rect.y = leg->groundY - emptyBottom - leg->rect.height;
            leg->sprites.rasteira.currentFrame = GetHairyLegFrameForProgress(
                leg->timer / HAIRY_LEG_SWEEP_TIME,
                leg->sprites.rasteira.frameCount
            );
            leg->isKickActive = true;

            if (leg->timer < 0.5f) {
                leg->rect.x -= 100 * leg->direction * deltaTime;
            }
            else if (leg->timer < HAIRY_LEG_SWEEP_TIME) {
                leg->rect.x += 1600 * leg->direction * deltaTime;
            }
            else {
                leg->state = HL_SWEEP_RECOVERING;
                leg->sprites.recovery.currentFrame = 0;
                leg->sprites.recovery.timer = 0.0f;
                leg->isKickActive = false;
                leg->timer = 0.0f;
            }

            break;

        case HL_SWEEP_RECOVERING:
            leg->timer += deltaTime;
            float sweepRecoverProgress = leg->timer / HAIRY_LEG_SWEEP_RECOVER_TIME;
            if (sweepRecoverProgress > 1.0f) {
                sweepRecoverProgress = 1.0f;
            }

            float sweepStartHeight = HAIRY_LEG_SWEEP_HEIGHT * scale;
            leg->rect.height = sweepStartHeight + (defaultHitboxH - sweepStartHeight) * sweepRecoverProgress;
            leg->rect.y = leg->groundY - emptyBottom - leg->rect.height;
            leg->isKickActive = false;
            leg->sprites.recovery.currentFrame = GetHairyLegFrameForProgress(
                sweepRecoverProgress,
                leg->sprites.recovery.frameCount
            );

            if (sweepRecoverProgress >= 1.0f) {
                leg->state = HL_VULNERABLE;
                leg->sprites.idle.currentFrame = 0;
                leg->timer = 0.0f;
            }
            break;

        case HL_KICKING:
            leg->timer += deltaTime;
            if (leg->timer < 0.4f) {
                leg->rect.x += 500 * leg->direction * deltaTime;
                leg->isKickActive = false;
            }
            else if (leg->timer < 0.7f) {
                leg->isKickActive = true;
            }
            else if (leg->timer < 1.2f) {
                leg->isKickActive = false;
                leg->rect.x -= 20 * leg->direction * deltaTime;
            }
            else {
                SetHairyLegStatePreservingBodyCenterX(leg, HL_VULNERABLE, scale);
                leg->sprites.idle.currentFrame = 0;
                leg->timer = 0.0f;
            }
            break;

        case HL_DEAD:
            leg->timer += deltaTime;
            leg->isKickActive = false;
            break;
    }
    int screenWidth = GetScreenWidth();
    if (screenWidth > 0) {
        if (leg->rect.x < 0) {
            leg->rect.x = 0;
        }
        else if (leg->rect.x + leg->rect.width > screenWidth) {
            leg->rect.x = screenWidth - leg->rect.width;
        }
    }

    SyncHairyLegAnimation(leg);

    bool isAttack = (leg->state == HL_KICKING);
    bool isLastFrame = (leg->currentAnim->currentFrame >= leg->currentAnim->frameCount - 1);

    if (leg->state != HL_SWEEPING && leg->state != HL_SWEEP_RECOVERING && leg->state != HL_FALLING && leg->state != HL_JUMPING_UP) {
        if (!(isAttack && isLastFrame)) {
            UpdateAnimation(leg->currentAnim, deltaTime);
        }
        if (isAttack && leg->currentAnim->currentFrame >= leg->currentAnim->frameCount) {
            leg->currentAnim->currentFrame = leg->currentAnim->frameCount - 1;
        }
    }

    if (leg->state == HL_KICKING) {
        leg->bodyHitbox = GetHairyLegBodyFrameHitbox(leg, scale);
        if (leg->isKickActive) {
            leg->kickHitbox = GetHairyLegKickFrameHitbox(leg, scale);
        }
    } else if (leg->state == HL_SWEEPING) {
        leg->bodyHitbox = leg->rect;
        if (leg->isKickActive) {
            leg->kickHitbox = GetHairyLegSweepFrameHitbox(leg, scale);
        }
    } else {
        leg->bodyHitbox = leg->rect;
    }

    // Sweep sound management
    if (leg->state == HL_SWEEPING) {
        if (leg->timer >= 0.5f && (leg->timer - deltaTime) < 0.5f) {
            PlayLegSweepSound();
        }
    }
}

void DrawHairyLegShadowWarning(HairyLeg *leg, float scale) {
    if (leg->state == HL_HANGING) {
        Animation *shadow = &leg->sprites.shadow;
        float shadowW = (float)shadow->frameWidth * scale;
        float shadowH = (float)shadow->sheet.height * scale;
        Vector2 warningPosition = {
            leg->rect.x + (leg->rect.width * 0.5f) - (shadowW * 0.5f),
            leg->groundY - shadowH - (HAIRY_LEG_SHADOW_GROUND_OFFSET * scale)
        };
        DrawAnimationFrame(shadow, warningPosition, scale, false, WHITE);
    }
}

static void DrawHairyLegShockwave(const HairyLeg *leg, const Shockwave *wave, bool flipX) {
    Texture2D shockwave = leg->sprites.shockwave;
    if (shockwave.id <= 0) {
        return;
    }

    int frameCount = GetHairyLegShockwaveFrameCount(leg);
    int currentFrame = wave->currentFrame;
    if (currentFrame < 0 || currentFrame >= frameCount) {
        currentFrame = 0;
    }

    float frameWidth = GetHairyLegShockwaveFrameWidth(leg);
    Rectangle source = {
        currentFrame * frameWidth,
        0.0f,
        flipX ? -frameWidth : frameWidth,
        (float)shockwave.height
    };
    DrawTexturePro(shockwave, source, wave->rect, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
}

void DrawHairyLeg(HairyLeg *leg, float scale) {
    bool flipX = (leg->direction == 1);
    float offsetX = GetHairyLegSpriteOffsetX(leg, scale);
    float offsetY = GetHairyLegSpriteOffsetY(leg, scale);

    Vector2 posicaoBoss = { leg->rect.x + offsetX, leg->rect.y + offsetY };
    DrawAnimationFrame(leg->currentAnim, posicaoBoss, scale, flipX, leg->hitFlashTimer > 0.0f ? RED : WHITE);

    if (leg->waveLeft.active) {
        DrawHairyLegShockwave(leg, &leg->waveLeft, true);
    }
    if (leg->waveRight.active) {
        DrawHairyLegShockwave(leg, &leg->waveRight, false);
    }
}

void UnloadHairyLeg(HairyLeg *leg) {
    UnloadHairyLegSprites(&leg->sprites);
}
