#include "entities/midnight_man.h"
#include "core/sounds.h"
#include "entities/player.h"
#include <math.h>

#define MM_ATTACK_PHASE_DURATION 0.72f
#define MM_IDLE_DURATION 1.1f
#define MM_TELEGRAPH_DURATION 1.2f
#define MM_HAND_DRAW_HEIGHT_RATIO 0.38f

#define MM_PHASE2_TELEGRAPH_DURATION 0.6f
#define MM_PHASE2_RISE_DURATION 0.25f
#define MM_PHASE2_PAUSE_DURATION 0.2f
#define MM_PHASE2_RETREAT_DURATION 0.25f
#define MM_PHASE2_FIST_SCALE 0.6f

#define MM_FOLLOW_SLAM_DURATION 3.5f
#define MM_FOLLOW_SPEED 730.0f
#define MM_FOLLOW_FIST_SCALE 0.5f
#define MM_FOLLOW_TURN_SPEED 1.45f

#define MM_CEILING_TELEGRAPH_DURATION 1.2f
#define MM_CEILING_SLAM_DURATION 0.3f
#define MM_CEILING_PAUSE_DURATION 0.5f
#define MM_CEILING_RETREAT_DURATION 0.7f

#define MM_UMBRELLA_STORM_DURATION 5.5f
#define MM_UMBRELLA_SPAWN_INTERVAL 0.45f
#define MM_HIT_FLASH_DURATION 0.12f

#define MM_SHOCKWAVE_FALLBACK_WIDTH 88.0f
#define MM_SHOCKWAVE_FALLBACK_HEIGHT 95.0f
#define MM_SHOCKWAVE_DRAW_SCALE 0.45f
#define MM_SHOCKWAVE_SPEED 720.0f
#define MM_SHOCKWAVE_FRAME_TIME 0.08f
#define MM_SHOCKWAVE_HITBOX_WIDTH_RATIO 0.55f
#define MM_SHOCKWAVE_HITBOX_HEIGHT_RATIO 0.35f
#define MM_SHOCKWAVE_HITBOX_BOTTOM_MARGIN_RATIO 0.06f
#define MM_SHOCKWAVE_VERTICAL_OFFSET -20.0f

#define MM_ARM_STORM_ENTER_DURATION 1.4f
#define MM_ARM_STORM_ACTIVE_DURATION 4.0f
#define MM_ARM_STORM_RETREAT_DURATION 1.0f
#define MM_ARM_STORM_Y_RATIO 0.08f
#define MM_ARM_STORM_UMBRELLA_INTERVAL 0.35f

static float GetMMShockwaveFrameWidth(const MidnightMan *mm)
{
    float width = mm->texShockwave.width > 0 ? (float)mm->texShockwave.width : MM_SHOCKWAVE_FALLBACK_WIDTH;
    int height = mm->texShockwave.height;
    int frameCount = 1;

    if (height > 0 && width >= height * 2 && (int)width % height == 0)
    {
        frameCount = (int)width / height;
    }

    if (frameCount > 1)
    {
        width /= (float)frameCount;
    }

    return width;
}

static float GetMMShockwaveWidth(const MidnightMan *mm, float scale)
{
    return GetMMShockwaveFrameWidth(mm) * scale * MM_SHOCKWAVE_DRAW_SCALE;
}

static float GetMMShockwaveHeight(const MidnightMan *mm, float scale)
{
    float height = mm->texShockwave.height > 0 ? (float)mm->texShockwave.height : MM_SHOCKWAVE_FALLBACK_HEIGHT;
    return height * scale * MM_SHOCKWAVE_DRAW_SCALE;
}

static Rectangle GetMMShockwaveHitbox(Rectangle visualRect)
{
    float hitboxW = visualRect.width * MM_SHOCKWAVE_HITBOX_WIDTH_RATIO;
    float hitboxH = visualRect.height * MM_SHOCKWAVE_HITBOX_HEIGHT_RATIO;
    float bottomMargin = visualRect.height * MM_SHOCKWAVE_HITBOX_BOTTOM_MARGIN_RATIO;

    return (Rectangle)
    {
        visualRect.x + (visualRect.width - hitboxW) * 0.5f,
        visualRect.y + visualRect.height - hitboxH - bottomMargin,
        hitboxW,
        hitboxH
    };
}

static MMShockwave MakeMMShockwave(Rectangle visualRect, Vector2 speed)
{
    MMShockwave wave = {0};
    wave.rect = visualRect;
    wave.hitbox = GetMMShockwaveHitbox(visualRect);
    wave.speed = speed;
    wave.active = true;
    return wave;
}

static void SyncMMShockwaveHitbox(MMShockwave *wave)
{
    wave->hitbox = GetMMShockwaveHitbox(wave->rect);
}

static void UpdateMMShockwaveAnimation(MMShockwave *wave, const MidnightMan *mm, float deltaTime)
{
    if (!wave->active)
    {
        return;
    }

    int height = mm->texShockwave.height;
    int width = mm->texShockwave.width;
    int frameCount = 1;

    if (height > 0 && width >= height * 2 && (int)width % height == 0)
    {
        frameCount = (int)width / height;
    }

    if (frameCount <= 1)
    {
        wave->currentFrame = 0;
        wave->frameTimer = 0.0f;
        return;
    }

    wave->frameTimer += deltaTime;
    while (wave->frameTimer >= MM_SHOCKWAVE_FRAME_TIME)
    {
        wave->frameTimer -= MM_SHOCKWAVE_FRAME_TIME;
        wave->currentFrame++;
        if (wave->currentFrame >= frameCount)
        {
            wave->currentFrame = 0;
        }
    }
}

static void DrawMMShockwave(const MidnightMan *mm, const MMShockwave *wave, bool flipX)
{
    Texture2D shockwave = mm->texShockwave;
    if (shockwave.id <= 0)
    {
        return;
    }

    int height = shockwave.height;
    int width = shockwave.width;
    int frameCount = 1;

    if (height > 0 && width >= height * 2 && (int)width % height == 0)
    {
        frameCount = (int)width / height;
    }

    int currentFrame = wave->currentFrame;
    if (currentFrame < 0 || currentFrame >= frameCount)
    {
        currentFrame = 0;
    }

    float frameWidth = GetMMShockwaveFrameWidth(mm);
    Rectangle source = {currentFrame * frameWidth, 0.0f, flipX ? -frameWidth : frameWidth, (float)height};
    DrawTexturePro(shockwave, source, wave->rect, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
}

static float Lerpf(float from, float to, float weight)
{
    return from + (to - from) * weight;
}

static float LerpAngle(float from, float to, float weight)
{
    float difference = to - from;
    while (difference < -180.0f)
    {
        difference += 360.0f;
    }
    while (difference > 180.0f)
    {
        difference -= 360.0f;
    }
    return from + difference * weight;
}

static void RefreshHandsLayout(MidnightMan *mm, int screenWidth, int screenHeight, float groundY)
{
    (void)screenWidth;

    if (mm->texHandOpen.id <= 0)
    {
        return;
    }

    float handTargetDrawHeight = (float)screenHeight * MM_HAND_DRAW_HEIGHT_RATIO;
    float scale = handTargetDrawHeight / (float)mm->texHandOpen.height;
    mm->handDrawHeight = handTargetDrawHeight;
    mm->handDrawWidth = (float)mm->texHandOpen.width * scale;
    mm->telegraphY = groundY;
    mm->riseStopY = fmaxf(0.0f, (float)screenHeight - mm->handDrawWidth);
}

void InitMidnightMan(MidnightMan *mm, int screenWidth, int screenHeight, float groundY)
{
    mm->active = true;
    mm->health = 200;
    mm->hitFlashTimer = 0.0f;
    mm->state = MM_IDLE;
    mm->timer = 0.0f;
    mm->handsY = (float)screenHeight;
    mm->telegraphY = groundY;
    mm->riseStopY = 0.0f;
    mm->handDrawWidth = 0.0f;
    mm->handDrawHeight = 0.0f;
    mm->umbrellaSpawnTimer = 0.0f;
    mm->attackCycle = 0;

    for (int i = 0; i < MM_HAND_COUNT; i++)
    {
        mm->handXPositions[i] = 0.0f;
        mm->handActive[i] = false;
        mm->handHitboxes[i] = (Rectangle){0};
    }

    if (mm->texHandOpen.id <= 0)
    {
        mm->texHandOpen = LoadTexture("assets/sprites/Boss/Spr_MidnightMan/Hand_open.png");
    }
    if (mm->texFist.id <= 0)
    {
        mm->texFist = LoadTexture("assets/sprites/Boss/Spr_MidnightMan/Punch.png");
    }
    if (mm->texUmbrella.id <= 0)
    {
        mm->texUmbrella = LoadTexture("assets/sprites/Boss/Spr_MidnightMan/sombrinha-Sheet.png");
    }
    if (mm->texShockwave.id <= 0)
    {
        mm->texShockwave = LoadTexture("assets/sprites/Boss/shockwave.png");
    }
    if (mm->texArm.id <= 0)
    {
        mm->texArm = LoadTexture("assets/sprites/Boss/Spr_MidnightMan/Arms_idle.png");
    }
    if (mm->animShadow.sheet.id <= 0)
    {
        mm->animShadow = LoadAnimation("assets/sprites/Boss/Shadow-Sheet.png", 4, 0.1f);
    }

    for (int i = 0; i < MM_MAX_UMBRELLAS; i++)
    {
        mm->umbrellas[i].active = false;
    }

    mm->sweepX = 0.0f;
    mm->sweepY = 0.0f;
    mm->sweepWidth = 0.0f;
    mm->sweepHeight = 0.0f;
    mm->sweepDirection = 1;
    mm->follow2X = 0.0f;
    mm->follow2Y = 0.0f;
    mm->follow2VelX = 0.0f;
    mm->follow2VelY = 0.0f;
    mm->follow2Angle = 0.0f;
    mm->ceilingPhase = 0;
    mm->waveLeft = (MMShockwave){0};
    mm->waveRight = (MMShockwave){0};

    RefreshHandsLayout(mm, screenWidth, screenHeight, groundY);
    mm->handsY = (float)screenHeight;
}

void UpdateMidnightMan(MidnightMan *mm, Rectangle playerRect, float deltaTime, int screenWidth, int screenHeight, float groundY)
{
    if (!mm->active)
    {
        return;
    }

    if (mm->hitFlashTimer > 0.0f)
    {
        mm->hitFlashTimer -= deltaTime;
        if (mm->hitFlashTimer < 0.0f)
        {
            mm->hitFlashTimer = 0.0f;
        }
    }

    if (mm->health <= 0 && mm->state != MM_DEAD)
    {
        mm->state = MM_DEAD;
        mm->timer = 0.0f;
        for (int i = 0; i < MM_HAND_COUNT; i++)
        {
            mm->handActive[i] = false;
        }
        for (int i = 0; i < MM_MAX_UMBRELLAS; i++)
        {
            mm->umbrellas[i].active = false;
        }
        return;
    }

    RefreshHandsLayout(mm, screenWidth, screenHeight, groundY);

    for (int i = 0; i < MM_MAX_UMBRELLAS; i++)
    {
        if (mm->umbrellas[i].active)
        {
            mm->umbrellas[i].position.x += mm->umbrellas[i].velocity.x * deltaTime;
            mm->umbrellas[i].position.y += mm->umbrellas[i].velocity.y * deltaTime;

            mm->umbrellas[i].animTimer += deltaTime;
            if (mm->umbrellas[i].animTimer >= 0.05f)
            {
                mm->umbrellas[i].animTimer = 0.0f;
                mm->umbrellas[i].animFrame = (mm->umbrellas[i].animFrame + 1) % 20;
            }

            if (mm->umbrellas[i].isBig && mm->umbrellas[i].position.y >= (float)screenHeight * 0.40f)
            {
                mm->umbrellas[i].active = false;

                Vector2 origin = mm->umbrellas[i].position;
                Vector2 playerCenter =
                {
                    playerRect.x + playerRect.width / 2.0f,
                    playerRect.y + playerRect.height / 2.0f
                };

                Vector2 dir = {playerCenter.x - origin.x, playerCenter.y - origin.y};
                float dist = sqrtf(dir.x * dir.x + dir.y * dir.y);
                if (dist > 0.0f)
                {
                    dir.x /= dist;
                    dir.y /= dist;
                }
                else
                {
                    dir.x = 0.0f;
                    dir.y = 1.0f;
                }

                float baseAngle = atan2f(dir.y, dir.x);
                float spreadAngles[] = {-0.3f, -0.15f, 0.0f, 0.15f, 0.3f};
                int count = sizeof(spreadAngles) / sizeof(spreadAngles[0]);

                int spawned = 0;
                for (int u = 0; u < MM_MAX_UMBRELLAS; u++)
                {
                    if (spawned >= count)
                    {
                        break;
                    }
                    if (!mm->umbrellas[u].active)
                    {
                        float angle = baseAngle + spreadAngles[spawned];
                        float speed = 260.0f + (float)GetRandomValue(0, 70);
                        mm->umbrellas[u].active = true;
                        mm->umbrellas[u].isBig = false;
                        mm->umbrellas[u].position = origin;
                        mm->umbrellas[u].velocity.x = cosf(angle) * speed;
                        mm->umbrellas[u].velocity.y = sinf(angle) * speed;
                        mm->umbrellas[u].scale = 0.25f;
                        mm->umbrellas[u].animFrame = GetRandomValue(0, 19);
                        mm->umbrellas[u].animTimer = 0.0f;
                        spawned++;
                    }
                }
            }

            if (mm->umbrellas[i].position.y > (float)screenHeight + 100.0f ||
                mm->umbrellas[i].position.x < -100.0f ||
                mm->umbrellas[i].position.x > (float)screenWidth + 100.0f ||
                mm->umbrellas[i].position.y < -150.0f)
            {
                mm->umbrellas[i].active = false;
            }
        }
    }

    if (mm->waveLeft.active)
    {
        mm->waveLeft.rect.x -= mm->waveLeft.speed.x * deltaTime;
        SyncMMShockwaveHitbox(&mm->waveLeft);
        UpdateMMShockwaveAnimation(&mm->waveLeft, mm, deltaTime);
        if (mm->waveLeft.rect.x < -200.0f)
        {
            mm->waveLeft.active = false;
        }
    }

    if (mm->waveRight.active)
    {
        mm->waveRight.rect.x += mm->waveRight.speed.x * deltaTime;
        SyncMMShockwaveHitbox(&mm->waveRight);
        UpdateMMShockwaveAnimation(&mm->waveRight, mm, deltaTime);
        if (mm->waveRight.rect.x > (float)screenWidth + 200.0f)
        {
            mm->waveRight.active = false;
        }
    }

    switch (mm->state)
    {
        case MM_ARM_STORM_ENTER:
        {
            mm->timer += deltaTime;
            float progress = mm->timer / MM_ARM_STORM_ENTER_DURATION;
            if (progress > 1.0f) progress = 1.0f;

            float armW = mm->texArm.width > 0 ? (float)mm->texArm.width : 413.0f;
            float armH = mm->texArm.height > 0 ? (float)mm->texArm.height : 252.0f;
            float scale = ((float)screenHeight * 0.35f) / armH;
            float drawW = armW * scale;

            float centerX = (float)screenWidth / 2.0f;
            float destLeft  = 0.0f;                            // cola na borda esquerda
            float destRight = (float)screenWidth - drawW; 

            mm->handXPositions[0] = Lerpf(-drawW, destLeft, progress);
            mm->handXPositions[1] = Lerpf((float)screenWidth, destRight, progress);

            if (mm->timer >= MM_ARM_STORM_ENTER_DURATION)
            {
                mm->handXPositions[0] = destLeft;
                mm->handXPositions[1] = destRight;
                mm->state = MM_ARM_STORM_ACTIVE;
                mm->timer = 0.0f;
                mm->umbrellaSpawnTimer = 0.0f;
            }
            break;
        }

        case MM_ARM_STORM_ACTIVE:
        {
            mm->timer += deltaTime;

            float armW = mm->texArm.width > 0 ? (float)mm->texArm.width : 413.0f;
            float armH = mm->texArm.height > 0 ? (float)mm->texArm.height : 252.0f;
            float scale = ((float)screenHeight * 0.35f) / armH;
            float drawW = armW * scale;
            float drawH = armH * scale;

            float handLeftX  = mm->handXPositions[0] + drawW;
            float handRightX = mm->handXPositions[1];
            float handY = mm->handsY + drawH * 0.75f;

            mm->umbrellaSpawnTimer += deltaTime;
            if (mm->umbrellaSpawnTimer >= MM_ARM_STORM_UMBRELLA_INTERVAL)
            {
                mm->umbrellaSpawnTimer = 0.0f;

                float spawnPoints[2] = { handLeftX, handRightX };
                for (int s = 0; s < 2; s++)
                {
                    for (int i = 0; i < MM_MAX_UMBRELLAS; i++)
                    {
                        if (!mm->umbrellas[i].active)
                        {
                            mm->umbrellas[i].active   = true;
                            mm->umbrellas[i].isBig    = false;
                            mm->umbrellas[i].position.x = spawnPoints[s] + (float)GetRandomValue(-15, 15);
                            mm->umbrellas[i].position.y = handY;
                            mm->umbrellas[i].velocity.x = (float)GetRandomValue(-150, 150);
                            mm->umbrellas[i].velocity.y = 220.0f + (float)GetRandomValue(0, 60);
                            mm->umbrellas[i].scale      = 0.35f;
                            mm->umbrellas[i].animFrame  = GetRandomValue(0, 19);
                            mm->umbrellas[i].animTimer  = 0.0f;
                            break;
                        }
                    }
                }
            }

            // hitbox na região da mão de cada braço
            float hitboxSize = drawH * 0.3f;
            mm->handHitboxes[0] = (Rectangle){ handLeftX  - hitboxSize * 0.5f, handY - hitboxSize, hitboxSize, hitboxSize };
            mm->handHitboxes[1] = (Rectangle){ handRightX - hitboxSize * 0.5f, handY - hitboxSize, hitboxSize, hitboxSize };

            if (mm->timer >= MM_ARM_STORM_ACTIVE_DURATION)
            {
                mm->state = MM_ARM_STORM_RETREAT;
                mm->timer = 0.0f;
            }
            break;
        }

        case MM_ARM_STORM_RETREAT:
        {
            mm->timer += deltaTime;
            float progress = mm->timer / MM_ARM_STORM_RETREAT_DURATION;
            if (progress > 1.0f) progress = 1.0f;

            float armW = mm->texArm.width > 0 ? (float)mm->texArm.width : 413.0f;
            float armH = mm->texArm.height > 0 ? (float)mm->texArm.height : 252.0f;
            float scale = ((float)screenHeight * 0.35f) / armH;
            float drawW = armW * scale;
            float destLeft  = 0.0f;                            // cola na borda esquerda
            float destRight = (float)screenWidth - drawW;       

            mm->handXPositions[0] = Lerpf(destLeft,  -drawW, progress);
            mm->handXPositions[1] = Lerpf(destRight, (float)screenWidth, progress);

            if (mm->timer >= MM_ARM_STORM_RETREAT_DURATION)
            {
                mm->handActive[0] = false;
                mm->handActive[1] = false;
                mm->handHitboxes[0] = (Rectangle){0};
                mm->handHitboxes[1] = (Rectangle){0};
                mm->handsY = (float)screenHeight;
                mm->state = MM_IDLE;
                mm->timer = 0.0f;
            }
            break;
        }

        case MM_DEAD:
            mm->handsY = (float)screenHeight + 300.0f;
            for (int i = 0; i < MM_HAND_COUNT; i++)
            {
                mm->handActive[i] = false;
                mm->handHitboxes[i] = (Rectangle){0};
            }
            break;

        case MM_IDLE:
        {
            mm->timer += deltaTime;
            mm->handsY = (float)screenHeight;
            for (int i = 0; i < MM_HAND_COUNT; i++)
            {
                mm->handActive[i] = false;
                mm->handHitboxes[i] = (Rectangle){0};
            }

            if (mm->timer >= MM_IDLE_DURATION)
            {
                mm->timer = 0.0f;
                int nextAttack = GetRandomValue(0, 4);

                if (nextAttack == 0)
                {
                    float visualW = mm->handDrawHeight;
                    float halfVis = visualW / 2.0f;
                    float W = (float)screenWidth;

                    if (GetRandomValue(0, 1) == 0)
                    {
                        mm->handXPositions[0] = W / 6.0f - halfVis;
                        mm->handXPositions[1] = W / 2.0f - halfVis;
                        mm->handXPositions[2] = W * 5.0f / 6.0f - halfVis;
                        mm->handActive[0] = true;
                        mm->handActive[1] = true;
                        mm->handActive[2] = true;
                    }
                    else
                    {
                        mm->handXPositions[0] = W / 3.0f - halfVis;
                        mm->handXPositions[1] = W * 2.0f / 3.0f - halfVis;
                        mm->handXPositions[2] = -9999.0f;
                        mm->handActive[0] = true;
                        mm->handActive[1] = true;
                        mm->handActive[2] = false;
                    }

                    mm->state = MM_GROUND_TELEGRAPH;
                    mm->handsY = mm->telegraphY;
                    mm->animShadow.currentFrame = 0;
                    mm->animShadow.timer = 0.0f;
                }
                else if (nextAttack == 1)
                {
                    float visualW = mm->handDrawHeight;
                    float halfVis = visualW / 2.0f;
                    float playerX = playerRect.x + playerRect.width / 2.0f;

                    mm->handXPositions[0] = playerX - halfVis;
                    mm->handActive[0] = true;
                    mm->handActive[1] = false;
                    mm->handActive[2] = false;
                    mm->ceilingPhase = 0;
                    mm->waveLeft.active = false;
                    mm->waveRight.active = false;
                    mm->state = MM_CEILING_TELEGRAPH;
                    mm->handsY = -mm->handDrawHeight;
                    mm->animShadow.currentFrame = 0;
                    mm->animShadow.timer = 0.0f;
                }
                else if (nextAttack == 2)
                {
                    float playerY = playerRect.y + playerRect.height / 2.0f;
                    mm->sweepDirection = (GetRandomValue(0, 1) == 0) ? 1 : -1;

                    float startX1 = (mm->sweepDirection == 1) ? -150.0f : ((float)screenWidth + 150.0f);
                    mm->handXPositions[0] = startX1;
                    mm->handsY = playerY;
                    mm->handActive[0] = true;

                    float initVelX1 = (mm->sweepDirection == 1) ? MM_FOLLOW_SPEED : -MM_FOLLOW_SPEED;
                    mm->sweepY = initVelX1;
                    mm->sweepWidth = 0.0f;
                    mm->sweepX = (mm->sweepDirection == 1) ? 0.0f : 180.0f;
                    mm->handActive[1] = false;
                    mm->handActive[2] = false;
                    mm->state = MM_FOLLOW_SLAM;
                    mm->timer = 0.0f;
                }
                else if (nextAttack == 3)
                {
                    mm->state = MM_UMBRELLA_STORM;
                    mm->umbrellaSpawnTimer = 0.0f;
                }
                else
                {
                    mm->handsY = 0.0f;
                    mm->handXPositions[0] = -(float)(mm->texArm.width);
                    mm->handXPositions[1] = (float)screenWidth;
                    mm->handActive[0] = true;
                    mm->handActive[1] = true;
                    mm->handActive[2] = false;
                    mm->umbrellaSpawnTimer = 0.0f;
                    mm->state = MM_ARM_STORM_ENTER;
                    mm->timer = 0.0f;
                }
            }
            break;
        }

        case MM_GROUND_TELEGRAPH:
            mm->timer += deltaTime;
            mm->handsY = mm->telegraphY;
            {
                float progress = mm->timer / MM_TELEGRAPH_DURATION;
                if (progress > 1.0f)
                {
                    progress = 1.0f;
                }
                int frame = (int)(progress * mm->animShadow.frameCount);
                if (frame >= mm->animShadow.frameCount)
                {
                    frame = mm->animShadow.frameCount - 1;
                }
                mm->animShadow.currentFrame = frame;
            }
            if (mm->timer >= MM_TELEGRAPH_DURATION)
            {
                mm->state = MM_GROUND_RISE;
                mm->timer = 0.0f;
            }
            break;

        case MM_GROUND_RISE:
        {
            mm->timer += deltaTime;
            float progress = mm->timer / MM_ATTACK_PHASE_DURATION;
            if (progress > 1.0f)
            {
                progress = 1.0f;
            }
            mm->handsY = Lerpf(mm->telegraphY, mm->riseStopY, progress);

            for (int i = 0; i < MM_HAND_COUNT; i++)
            {
                if (mm->handActive[i])
                {
                    mm->handHitboxes[i] = (Rectangle){mm->handXPositions[i], mm->handsY, mm->handDrawHeight, mm->handDrawWidth};
                }
            }

            if (mm->timer >= MM_ATTACK_PHASE_DURATION)
            {
                mm->handsY = mm->riseStopY;
                mm->state = MM_GROUND_RETREAT;
                mm->timer = 0.0f;
            }
            break;
        }

        case MM_GROUND_RETREAT:
        {
            mm->timer += deltaTime;
            float progress = mm->timer / MM_ATTACK_PHASE_DURATION;
            if (progress > 1.0f)
            {
                progress = 1.0f;
            }
            mm->handsY = Lerpf(mm->riseStopY, (float)screenHeight, progress);

            for (int i = 0; i < MM_HAND_COUNT; i++)
            {
                if (mm->handActive[i])
                {
                    mm->handHitboxes[i] = (Rectangle){mm->handXPositions[i], mm->handsY, mm->handDrawHeight, mm->handDrawWidth};
                }
            }

            if (mm->timer >= MM_ATTACK_PHASE_DURATION)
            {
                mm->handsY = (float)screenHeight;

                bool wasThreeHands = mm->handActive[2];
                float visualW = mm->handDrawHeight;
                float halfVis = visualW / 2.0f;
                float W = (float)screenWidth;

                if (wasThreeHands)
                {
                    mm->handXPositions[0] = W / 3.0f - halfVis;
                    mm->handXPositions[1] = W * 2.0f / 3.0f - halfVis;
                    mm->handXPositions[2] = -9999.0f;
                    mm->handActive[0] = true;
                    mm->handActive[1] = true;
                    mm->handActive[2] = false;
                }
                else
                {
                    mm->handXPositions[0] = W / 6.0f - halfVis;
                    mm->handXPositions[1] = W / 2.0f - halfVis;
                    mm->handXPositions[2] = W * 5.0f / 6.0f - halfVis;
                    mm->handActive[0] = true;
                    mm->handActive[1] = true;
                    mm->handActive[2] = true;
                }

                mm->state = MM_GROUND_PHASE2_TELEGRAPH;
                mm->handsY = mm->telegraphY;
                mm->timer = 0.0f;
                mm->animShadow.currentFrame = 0;
                mm->animShadow.timer = 0.0f;
            }
            break;
    }

    case MM_GROUND_PHASE2_TELEGRAPH:
        mm->timer += deltaTime;
        mm->handsY = mm->telegraphY;
        {
            float progress = mm->timer / MM_PHASE2_TELEGRAPH_DURATION;
            if (progress > 1.0f)
            {
                progress = 1.0f;
            }
            int frame = (int)(progress * mm->animShadow.frameCount);
            if (frame >= mm->animShadow.frameCount)
            {
                frame = mm->animShadow.frameCount - 1;
            }
            mm->animShadow.currentFrame = frame;
        }
        if (mm->timer >= MM_PHASE2_TELEGRAPH_DURATION)
        {
            mm->state = MM_GROUND_PHASE2_RISE;
            mm->timer = 0.0f;
        }
        break;

    case MM_GROUND_PHASE2_RISE:
    {
        mm->timer += deltaTime;
        float progress = mm->timer / MM_PHASE2_RISE_DURATION;
        if (progress > 1.0f)
        {
            progress = 1.0f;
        }
        mm->handsY = Lerpf(mm->telegraphY, mm->riseStopY, progress);

        float smallerWidth = mm->handDrawWidth * MM_PHASE2_FIST_SCALE;
        float smallerHeight = mm->handDrawHeight * MM_PHASE2_FIST_SCALE;
        float visualW = mm->handDrawHeight;

        for (int i = 0; i < MM_HAND_COUNT; i++)
        {
            if (mm->handActive[i])
            {
                float offsetColumnX = mm->handXPositions[i] + (visualW - smallerHeight) / 2.0f;
                mm->handHitboxes[i] = (Rectangle){offsetColumnX, mm->handsY, smallerHeight, smallerWidth};
            }
        }

        if (mm->timer >= MM_PHASE2_RISE_DURATION)
        {
            mm->handsY = mm->riseStopY;
            mm->state = MM_GROUND_PHASE2_PAUSE;
            mm->timer = 0.0f;
        }
        break;
    }

    case MM_GROUND_PHASE2_PAUSE:
    {
        mm->timer += deltaTime;
        mm->handsY = mm->riseStopY;

        float smallerWidth = mm->handDrawWidth * MM_PHASE2_FIST_SCALE;
        float smallerHeight = mm->handDrawHeight * MM_PHASE2_FIST_SCALE;
        float visualW = mm->handDrawHeight;

        for (int i = 0; i < MM_HAND_COUNT; i++)
        {
            if (mm->handActive[i])
            {
                float offsetColumnX = mm->handXPositions[i] + (visualW - smallerHeight) / 2.0f;
                mm->handHitboxes[i] = (Rectangle){offsetColumnX, mm->handsY, smallerHeight, smallerWidth};
            }
        }

        if (mm->timer >= MM_PHASE2_PAUSE_DURATION)
        {
            mm->state = MM_GROUND_PHASE2_RETREAT;
            mm->timer = 0.0f;
        }
        break;
    }

    case MM_GROUND_PHASE2_RETREAT:
    {
        mm->timer += deltaTime;
        float progress = mm->timer / MM_PHASE2_RETREAT_DURATION;
        if (progress > 1.0f)
        {
            progress = 1.0f;
        }
        mm->handsY = Lerpf(mm->riseStopY, (float)screenHeight, progress);

        float smallerWidth = mm->handDrawWidth * MM_PHASE2_FIST_SCALE;
        float smallerHeight = mm->handDrawHeight * MM_PHASE2_FIST_SCALE;
        float visualW = mm->handDrawHeight;

        for (int i = 0; i < MM_HAND_COUNT; i++)
        {
            if (mm->handActive[i])
            {
                float offsetColumnX = mm->handXPositions[i] + (visualW - smallerHeight) / 2.0f;
                mm->handHitboxes[i] = (Rectangle){offsetColumnX, mm->handsY, smallerHeight, smallerWidth};
            }
        }

        if (mm->timer >= MM_PHASE2_RETREAT_DURATION)
        {
            mm->handsY = (float)screenHeight;
            mm->state = MM_IDLE;
            mm->timer = 0.0f;
        }
        break;
    }

    case MM_FOLLOW_SLAM:
    {
        mm->timer += deltaTime;

        float playerX = playerRect.x + playerRect.width / 2.0f;
        float playerY = playerRect.y + playerRect.height / 2.0f;

        if (mm->timer >= 1.6f && !mm->handActive[1])
        {
            float startX2 = (mm->sweepDirection == 1) ? ((float)screenWidth + 150.0f) : -150.0f;
            mm->handXPositions[1] = startX2;
            mm->follow2Y = playerY;
            mm->handActive[1] = true;

            float initVelX2 = (mm->sweepDirection == 1) ? -MM_FOLLOW_SPEED : MM_FOLLOW_SPEED;
            mm->follow2VelX = initVelX2;
            mm->follow2VelY = 0.0f;
            mm->follow2Angle = (mm->sweepDirection == 1) ? 180.0f : 0.0f;
        }

        if (mm->handActive[0])
        {
            if (mm->timer < 1.6f)
            {
                Vector2 targetDir1 = {playerX - mm->handXPositions[0], playerY - mm->handsY};
                float dist1 = sqrtf(targetDir1.x * targetDir1.x + targetDir1.y * targetDir1.y);
                if (dist1 > 0.0f)
                {
                    targetDir1.x /= dist1;
                    targetDir1.y /= dist1;
                }
                else
                {
                    targetDir1.x = (mm->sweepDirection == 1) ? 1.0f : -1.0f;
                    targetDir1.y = 0.0f;
                }

                float targetAngle1 = atan2f(targetDir1.y, targetDir1.x) * (180.0f / 3.14159265f);
                mm->sweepX = LerpAngle(mm->sweepX, targetAngle1, MM_FOLLOW_TURN_SPEED * deltaTime);

                float rad1 = mm->sweepX * (3.14159265f / 180.0f);
                mm->sweepY = cosf(rad1) * MM_FOLLOW_SPEED;
                mm->sweepWidth = sinf(rad1) * MM_FOLLOW_SPEED;
            }

            mm->sweepX = atan2f(mm->sweepWidth, mm->sweepY) * (180.0f / 3.14159265f);
            mm->handXPositions[0] += mm->sweepY * deltaTime;
            mm->handsY += mm->sweepWidth * deltaTime;

            float hitboxSize = mm->handDrawHeight * MM_FOLLOW_FIST_SCALE * 0.70f;
            mm->handHitboxes[0] = (Rectangle)
            {
                mm->handXPositions[0] - hitboxSize / 2.0f,
                mm->handsY - hitboxSize / 2.0f,
                hitboxSize,
                hitboxSize
            };
        }

        if (mm->handActive[1])
        {
            if (mm->timer < 3.2f)
            {
                Vector2 targetDir2 = {playerX - mm->handXPositions[1], playerY - mm->follow2Y};
                float dist2 = sqrtf(targetDir2.x * targetDir2.x + targetDir2.y * targetDir2.y);
                if (dist2 > 0.0f)
                {
                    targetDir2.x /= dist2;
                    targetDir2.y /= dist2;
                }
                else
                {
                    targetDir2.x = (mm->sweepDirection == 1) ? -1.0f : 1.0f;
                    targetDir2.y = 0.0f;
                }

                float targetAngle2 = atan2f(targetDir2.y, targetDir2.x) * (180.0f / 3.14159265f);
                mm->follow2Angle = LerpAngle(mm->follow2Angle, targetAngle2, MM_FOLLOW_TURN_SPEED * deltaTime);

                float rad2 = mm->follow2Angle * (3.14159265f / 180.0f);
                mm->follow2VelX = cosf(rad2) * MM_FOLLOW_SPEED;
                mm->follow2VelY = sinf(rad2) * MM_FOLLOW_SPEED;
            }

            mm->follow2Angle = atan2f(mm->follow2VelY, mm->follow2VelX) * (180.0f / 3.14159265f);
            mm->handXPositions[1] += mm->follow2VelX * deltaTime;
            mm->follow2Y += mm->follow2VelY * deltaTime;

            float hitboxSize = mm->handDrawHeight * MM_FOLLOW_FIST_SCALE * 0.70f;
            mm->handHitboxes[1] = (Rectangle)
            {
                mm->handXPositions[1] - hitboxSize / 2.0f,
                mm->follow2Y - hitboxSize / 2.0f,
                hitboxSize,
                hitboxSize
            };
        }

        if (mm->timer >= MM_FOLLOW_SLAM_DURATION)
        {
            mm->state = MM_FOLLOW_RETREAT;
            mm->timer = 0.0f;
        }
        break;
    }

    case MM_FOLLOW_RETREAT:
    {
        mm->timer += deltaTime;

        if (mm->handActive[0])
        {
            mm->handXPositions[0] += mm->sweepY * 1.5f * deltaTime;
            mm->handsY += mm->sweepWidth * 1.5f * deltaTime;

            float hitboxSize = mm->handDrawHeight * MM_FOLLOW_FIST_SCALE * 0.70f;
            mm->handHitboxes[0] = (Rectangle)
            {
                mm->handXPositions[0] - hitboxSize / 2.0f,
                mm->handsY - hitboxSize / 2.0f,
                hitboxSize,
                hitboxSize
            };
        }

        if (mm->handActive[1])
        {
            mm->handXPositions[1] += mm->follow2VelX * 1.5f * deltaTime;
            mm->follow2Y += mm->follow2VelY * 1.5f * deltaTime;

            float hitboxSize = mm->handDrawHeight * MM_FOLLOW_FIST_SCALE * 0.70f;
            mm->handHitboxes[1] = (Rectangle)
            {
                mm->handXPositions[1] - hitboxSize / 2.0f,
                mm->follow2Y - hitboxSize / 2.0f,
                hitboxSize,
                hitboxSize
            };
        }

        float margin = 200.0f;
        bool fist1Exited = !mm->handActive[0] ||
            (mm->handXPositions[0] < -margin ||
             mm->handXPositions[0] > (float)screenWidth + margin ||
             mm->handsY < -margin ||
             mm->handsY > (float)screenHeight + margin);
        bool fist2Exited = !mm->handActive[1] ||
            (mm->handXPositions[1] < -margin ||
             mm->handXPositions[1] > (float)screenWidth + margin ||
             mm->follow2Y < -margin ||
             mm->follow2Y > (float)screenHeight + margin);

        if (fist1Exited && fist2Exited)
        {
            mm->handActive[0] = false;
            mm->handActive[1] = false;
            mm->handHitboxes[0] = (Rectangle){0};
            mm->handHitboxes[1] = (Rectangle){0};
            mm->handsY = (float)screenHeight;
            mm->state = MM_IDLE;
            mm->timer = 0.0f;
        }
        break;
    }

    case MM_CEILING_TELEGRAPH:
        mm->timer += deltaTime;
        mm->handsY = -mm->handDrawHeight;
        {
            float progress = mm->timer / MM_CEILING_TELEGRAPH_DURATION;
            if (progress > 1.0f)
            {
                progress = 1.0f;
            }
            int frame = (int)(progress * mm->animShadow.frameCount);
            if (frame >= mm->animShadow.frameCount)
            {
                frame = mm->animShadow.frameCount - 1;
            }
            mm->animShadow.currentFrame = frame;
        }
        if (mm->timer >= MM_CEILING_TELEGRAPH_DURATION)
        {
            if (mm->ceilingPhase == 0)
            {
                mm->ceilingPhase = 1;
            }
            else if (mm->ceilingPhase == 3)
            {
                mm->ceilingPhase = 4;
            }
            mm->state = MM_CEILING_SLAM;
            mm->timer = 0.0f;
        }
        break;

    case MM_CEILING_SLAM:
    {
        mm->timer += deltaTime;
        float progress = mm->timer / MM_CEILING_SLAM_DURATION;
        if (progress > 1.0f)
        {
            progress = 1.0f;
        }

        float startY = -mm->handDrawHeight;
        float targetY = groundY - mm->handDrawWidth + 30.0f;
        mm->handsY = Lerpf(startY, targetY, progress);

        int activeHandIdx = (mm->ceilingPhase == 4) ? 1 : 0;
        for (int i = 0; i < MM_HAND_COUNT; i++)
        {
            if (i == activeHandIdx && mm->handActive[i])
            {
                mm->handHitboxes[i] = (Rectangle){mm->handXPositions[i], mm->handsY, mm->handDrawHeight, mm->handDrawWidth};
            }
            else
            {
                mm->handHitboxes[i] = (Rectangle){0};
            }
        }

        if (mm->timer >= MM_CEILING_SLAM_DURATION)
        {
            mm->handsY = targetY;
            PlayLegShockwaveSound();

            float scale = ((float)screenHeight * 0.65f / 252.0f) * 0.8f;
            float waveW = GetMMShockwaveWidth(mm, scale);
            float waveH = GetMMShockwaveHeight(mm, scale);
            float legHitboxTipY = targetY + mm->handDrawWidth;
            float waveY = legHitboxTipY - waveH + (MM_SHOCKWAVE_VERTICAL_OFFSET * scale);

            mm->waveLeft = MakeMMShockwave(
                (Rectangle){mm->handXPositions[activeHandIdx] - waveW, waveY, waveW, waveH},
                (Vector2){MM_SHOCKWAVE_SPEED, 0}
            );
            mm->waveRight = MakeMMShockwave(
                (Rectangle){mm->handXPositions[activeHandIdx] + mm->handDrawHeight, waveY, waveW, waveH},
                (Vector2){MM_SHOCKWAVE_SPEED, 0}
            );

            if (mm->ceilingPhase == 1)
            {
                mm->ceilingPhase = 2;
            }
            else if (mm->ceilingPhase == 4)
            {
                mm->ceilingPhase = 5;
            }

            mm->state = MM_CEILING_RETREAT;
            mm->timer = 0.0f;
        }
        break;
    }

    case MM_CEILING_RETREAT:
    {
        mm->timer += deltaTime;
        float progress = mm->timer / (MM_CEILING_PAUSE_DURATION + MM_CEILING_RETREAT_DURATION);
        if (progress > 1.0f)
        {
            progress = 1.0f;
        }

        float targetY = groundY - mm->handDrawWidth + 30.0f;
        float endY = -mm->handDrawHeight;

        if (mm->timer < MM_CEILING_PAUSE_DURATION)
        {
            mm->handsY = targetY;
        }
        else
        {
            float retreatProgress = (mm->timer - MM_CEILING_PAUSE_DURATION) / MM_CEILING_RETREAT_DURATION;
            if (retreatProgress > 1.0f)
            {
                retreatProgress = 1.0f;
            }
            mm->handsY = Lerpf(targetY, endY, retreatProgress);
        }

        int activeHandIdx = (mm->ceilingPhase == 5) ? 1 : 0;
        for (int i = 0; i < MM_HAND_COUNT; i++)
        {
            if (i == activeHandIdx && mm->handActive[i])
            {
                mm->handHitboxes[i] = (Rectangle){mm->handXPositions[i], mm->handsY, mm->handDrawHeight, mm->handDrawWidth};
            }
            else
            {
                mm->handHitboxes[i] = (Rectangle){0};
            }
        }

        if (progress >= 1.0f)
        {
            if (mm->ceilingPhase == 2)
            {
                mm->handActive[0] = false;
                mm->handHitboxes[0] = (Rectangle){0};

                float playerX = playerRect.x + playerRect.width / 2.0f;
                float visualW = mm->handDrawHeight;
                float halfVis = visualW / 2.0f;

                mm->handXPositions[1] = playerX - halfVis;
                mm->handActive[1] = true;
                mm->ceilingPhase = 3;
                mm->state = MM_CEILING_TELEGRAPH;
                mm->timer = 0.0f;
                mm->animShadow.currentFrame = 0;
                mm->animShadow.timer = 0.0f;
            }
            else if (mm->ceilingPhase == 5)
            {
                mm->handActive[1] = false;
                mm->handHitboxes[1] = (Rectangle){0};
                mm->handsY = (float)screenHeight;
                mm->state = MM_IDLE;
                mm->timer = 0.0f;
            }
        }
        break;
    }

    case MM_UMBRELLA_STORM:
    {
        mm->timer += deltaTime;

        mm->umbrellaSpawnTimer += deltaTime;
        if (mm->umbrellaSpawnTimer >= MM_UMBRELLA_SPAWN_INTERVAL && mm->timer < MM_UMBRELLA_STORM_DURATION - 1.5f)
        {
            mm->umbrellaSpawnTimer = 0.0f;
            for (int i = 0; i < MM_MAX_UMBRELLAS; i++)
            {
                if (!mm->umbrellas[i].active)
                {
                    mm->umbrellas[i].active = true;
                    mm->umbrellas[i].isBig = true;
                    mm->umbrellas[i].position.x = (float)GetRandomValue(100, screenWidth - 100);
                    mm->umbrellas[i].position.y = -100.0f;
                    mm->umbrellas[i].velocity.x = (float)GetRandomValue(-20, 20);
                    mm->umbrellas[i].velocity.y = 120.0f;
                    mm->umbrellas[i].scale = 0.65f;
                    mm->umbrellas[i].animFrame = 0;
                    mm->umbrellas[i].animTimer = 0.0f;
                    break;
                }
            }
        }

        float hoverY = (float)screenHeight * 0.15f;
        mm->handsY = hoverY;
        mm->handXPositions[0] = 80.0f;
        mm->handXPositions[1] = (float)screenWidth - 80.0f - mm->handDrawHeight;
        mm->handActive[0] = true;
        mm->handActive[1] = true;
        mm->handActive[2] = false;

        for (int i = 0; i < MM_HAND_COUNT; i++)
        {
            if (mm->handActive[i])
            {
                mm->handHitboxes[i] = (Rectangle){mm->handXPositions[i], mm->handsY, mm->handDrawHeight, mm->handDrawWidth};
            }
            else
            {
                mm->handHitboxes[i] = (Rectangle){0};
            }
        }

        if (mm->timer >= MM_UMBRELLA_STORM_DURATION)
        {
            mm->state = MM_IDLE;
            mm->timer = 0.0f;
        }
        break;
    }
    }
}

void DrawMidnightMan(const MidnightMan *mm)
{
    if (!mm->active)
    {
        return;
    }

    int screenHeight = GetScreenHeight();
    int screenWidth = GetScreenWidth();
    float groundY = (float)screenHeight * 0.82f;
    Color bossTint = mm->hitFlashTimer > 0.0f ? RED : WHITE;

    if (mm->state == MM_GROUND_TELEGRAPH || mm->state == MM_CEILING_TELEGRAPH || mm->state == MM_GROUND_PHASE2_TELEGRAPH)
    {
        float visualW = mm->handDrawHeight;
        if (mm->animShadow.sheet.id > 0)
        {
            float scale = (mm->state == MM_GROUND_PHASE2_TELEGRAPH)
                ? ((float)screenHeight * 0.65f / 252.0f) * 1.5f * MM_PHASE2_FIST_SCALE
                : (float)screenHeight * 0.65f / 252.0f;
            float shadowW = (float)mm->animShadow.frameWidth * scale;
            float shadowH = (float)mm->animShadow.sheet.height * scale;

            for (int i = 0; i < MM_HAND_COUNT; i++)
            {
                if (mm->handActive[i])
                {
                    Vector2 warningPosition =
                    {
                        mm->handXPositions[i] + (visualW * 0.5f) - (shadowW * 0.5f),
                        groundY - shadowH - (42.0f * scale)
                    };
                    DrawAnimationFrame((Animation *)&mm->animShadow, warningPosition, scale, false, WHITE);
                }
            }
        }
    }

    if (mm->state == MM_CEILING_SLAM || mm->state == MM_CEILING_RETREAT)
    {
        if (mm->texFist.id > 0)
        {
            float texW = (float)mm->texFist.width;
            float texH = (float)mm->texFist.height;
            float scale = mm->handDrawHeight / texH;
            float drawW = texW * scale;
            float drawH = mm->handDrawHeight;

            for (int i = 0; i < MM_HAND_COUNT; i++)
            {
                if (mm->handActive[i])
                {
                    float centerX = mm->handXPositions[i] + mm->handDrawHeight / 2.0f;
                    float centerY = mm->handsY + mm->handDrawWidth / 2.0f;
                    Rectangle src = {0.0f, 0.0f, texW, texH};
                    Rectangle dest = {centerX, centerY, drawW, drawH};
                    Vector2 pivot = {drawW / 2.0f, drawH / 2.0f};
                    DrawTexturePro(mm->texFist, src, dest, pivot, 90.0f, bossTint);
                }
            }
        }
    }
    else if (mm->state == MM_GROUND_PHASE2_RISE || mm->state == MM_GROUND_PHASE2_PAUSE || mm->state == MM_GROUND_PHASE2_RETREAT)
    {
        if (mm->texFist.id > 0)
        {
            float texW = (float)mm->texFist.width;
            float texH = (float)mm->texFist.height;
            float scale = mm->handDrawHeight / texH;
            float drawW = texW * scale * MM_PHASE2_FIST_SCALE;
            float drawH = mm->handDrawHeight * MM_PHASE2_FIST_SCALE;
            float visualW = mm->handDrawHeight;
            float smallerWidth = mm->handDrawWidth * MM_PHASE2_FIST_SCALE;

            for (int i = 0; i < MM_HAND_COUNT; i++)
            {
                if (mm->handActive[i])
                {
                    float centerX = mm->handXPositions[i] + visualW / 2.0f;
                    float centerY = mm->handsY + smallerWidth / 2.0f;
                    Rectangle src = {0.0f, 0.0f, texW, texH};
                    Rectangle dest = {centerX, centerY, drawW, drawH};
                    Vector2 pivot = {drawW / 2.0f, drawH / 2.0f};
                    DrawTexturePro(mm->texFist, src, dest, pivot, -90.0f, bossTint);
                }
            }
        }
    }
    else if (mm->state == MM_FOLLOW_SLAM || mm->state == MM_FOLLOW_RETREAT)
    {
        if (mm->texFist.id > 0)
        {
            float texW = (float)mm->texFist.width;
            float texH = (float)mm->texFist.height;
            float scale = mm->handDrawHeight / texH;
            float drawW = texW * scale * MM_FOLLOW_FIST_SCALE;
            float drawH = mm->handDrawHeight * MM_FOLLOW_FIST_SCALE;

            if (mm->handActive[0])
            {
                Rectangle src = {0.0f, 0.0f, texW, texH};
                Rectangle dest = {mm->handXPositions[0], mm->handsY, drawW, drawH};
                Vector2 pivot = {drawW / 2.0f, drawH / 2.0f};
                DrawTexturePro(mm->texFist, src, dest, pivot, mm->sweepX, bossTint);
            }

            if (mm->handActive[1])
            {
                Rectangle src = {0.0f, 0.0f, texW, texH};
                Rectangle dest = {mm->handXPositions[1], mm->follow2Y, drawW, drawH};
                Vector2 pivot = {drawW / 2.0f, drawH / 2.0f};
                DrawTexturePro(mm->texFist, src, dest, pivot, mm->follow2Angle, bossTint);
            }
        }
    }
    else
    {
        if (mm->texHandOpen.id > 0)
        {
            float texW = (float)mm->texHandOpen.width;
            float texH = (float)mm->texHandOpen.height;
            float halfOrigW = mm->handDrawWidth / 2.0f;
            float halfOrigH = mm->handDrawHeight / 2.0f;
            float visualW = mm->handDrawHeight;

            for (int i = 0; i < MM_HAND_COUNT; i++)
            {
                if ((mm->handActive[i] || mm->state == MM_GROUND_RISE || mm->state == MM_GROUND_RETREAT) && mm->state != MM_CEILING_TELEGRAPH && mm->state != MM_ARM_STORM_ENTER && mm->state != MM_ARM_STORM_ACTIVE && mm->state != MM_ARM_STORM_RETREAT)
                {
                    float centerX = mm->handXPositions[i] + visualW / 2.0f;
                    float centerY = mm->handsY + mm->handDrawWidth / 2.0f;
                    float currentTexH = (mm->state == MM_UMBRELLA_STORM && i == 1) ? -texH : texH;
                    Rectangle src = {0.0f, 0.0f, texW, currentTexH};
                    Rectangle dest = {centerX, centerY, mm->handDrawWidth, mm->handDrawHeight};
                    Vector2 pivot = {halfOrigW, halfOrigH};
                    DrawTexturePro(mm->texHandOpen, src, dest, pivot, -90.0f, bossTint);
                }
            }
        }
    }

    if (mm->state == MM_ARM_STORM_ENTER  || mm->state == MM_ARM_STORM_ACTIVE || mm->state == MM_ARM_STORM_RETREAT)
    {
        if (mm->texArm.id > 0)
        {
            float texW = (float)mm->texArm.width;
            float texH = (float)mm->texArm.height;
            float scale = ((float)screenHeight * 0.35f) / texH;
            float drawW = texW * scale;
            float drawH = texH * scale;

            Rectangle srcLeft  = { 0.0f, 0.0f, texW, texH };
            Rectangle destLeft = { mm->handXPositions[0], mm->handsY, drawW, drawH };
            DrawTexturePro(mm->texArm, srcLeft, destLeft, (Vector2){0.0f, 0.0f}, 0.0f, bossTint);

            Rectangle srcRight  = { 0.0f, 0.0f, -texW, texH };
            Rectangle destRight = { mm->handXPositions[1], mm->handsY, drawW, drawH };
            DrawTexturePro(mm->texArm, srcRight, destRight, (Vector2){0.0f, 0.0f}, 0.0f, bossTint);
        }
    }
    if (mm->texUmbrella.id > 0)
    {
        for (int i = 0; i < MM_MAX_UMBRELLAS; i++)
        {
            if (mm->umbrellas[i].active)
            {
                float uW = 186.0f;
                float uH = 141.0f;
                Rectangle src = {mm->umbrellas[i].animFrame * uW, 0.0f, uW, uH};
                float drawW = uW * mm->umbrellas[i].scale;
                float drawH = uH * mm->umbrellas[i].scale;
                Rectangle dest = {mm->umbrellas[i].position.x, mm->umbrellas[i].position.y, drawW, drawH};
                Vector2 pivot = {drawW / 2.0f, drawH / 2.0f};
                DrawTexturePro(mm->texUmbrella, src, dest, pivot, 0.0f, WHITE);
            }
        }
    }

    if (mm->waveLeft.active)
    {
        DrawMMShockwave(mm, &mm->waveLeft, true);
    }
    if (mm->waveRight.active)
    {
        DrawMMShockwave(mm, &mm->waveRight, false);
    }

    for (int i = 0; i < MM_HAND_COUNT; i++)
    {
        if (mm->handActive[i])
        {
            DrawRectangleLinesEx(mm->handHitboxes[i], 2.0f, LIME);
        }
    }

    for (int i = 0; i < MM_MAX_UMBRELLAS; i++)
    {
        if (mm->umbrellas[i].active)
        {
            float uW = 186.0f * mm->umbrellas[i].scale;
            float uH = 141.0f * mm->umbrellas[i].scale;
            Rectangle umbrellaHitbox =
            {
                mm->umbrellas[i].position.x - uW * 0.35f,
                mm->umbrellas[i].position.y - uH * 0.35f,
                uW * 0.7f,
                uH * 0.7f
            };
            DrawRectangleLinesEx(umbrellaHitbox, 2.0f, RED);
        }
    }

    if (mm->waveLeft.active)
    {
        DrawRectangleLinesEx(mm->waveLeft.hitbox, 2.0f, RED);
    }
    if (mm->waveRight.active)
    {
        DrawRectangleLinesEx(mm->waveRight.hitbox, 2.0f, RED);
    }
}

void UnloadMidnightMan(MidnightMan *mm)
{
    if (mm->texHandOpen.id > 0)
    {
        UnloadTexture(mm->texHandOpen);
        mm->texHandOpen.id = 0;
    }
    if (mm->texFist.id > 0)
    {
        UnloadTexture(mm->texFist);
        mm->texFist.id = 0;
    }
    if (mm->texUmbrella.id > 0)
    {
        UnloadTexture(mm->texUmbrella);
        mm->texUmbrella.id = 0;
    }
    if (mm->texShockwave.id > 0)
    {
        UnloadTexture(mm->texShockwave);
        mm->texShockwave.id = 0;
    }
    if (mm->animShadow.sheet.id > 0)
    {
        UnloadAnimation(&mm->animShadow);
        mm->animShadow.sheet.id = 0;
    }
    mm->active = false;
}

bool TryDamageMidnightManFromPlayerAttack(MidnightMan *mm, Player *player, float playerScale)
{
    if (!mm->active || mm->health <= 0)
    {
        return false;
    }

    if (!IsPlayerAttackHitboxActive(player) || player->weapon.hitConnected)
    {
        return false;
    }

    Rectangle attackHitbox = GetPlayerAttackHitbox(player, playerScale);

    bool hit = false;
    for (int i = 0; i < MM_HAND_COUNT; i++)
    {
        if (mm->handActive[i])
        {
            if (CheckCollisionRecs(attackHitbox, mm->handHitboxes[i]))
            {
                hit = true;
                break;
            }
        }
    }

    if (hit)
    {
        mm->health -= (int)player->weapon.damage;
        mm->hitFlashTimer = MM_HIT_FLASH_DURATION;
        if (mm->health < 0)
        {
            mm->health = 0;
        }
        player->weapon.hitConnected = true;
    }

    bool hitUmbrella = false;
    for (int i = 0; i < MM_MAX_UMBRELLAS; i++)
    {
        if (mm->umbrellas[i].active)
        {
            float uW = 186.0f * mm->umbrellas[i].scale;
            float uH = 141.0f * mm->umbrellas[i].scale;
            Rectangle umbrellaHitbox =
            {
                mm->umbrellas[i].position.x - uW * 0.45f,
                mm->umbrellas[i].position.y - uH * 0.45f,
                uW * 0.9f,
                uH * 0.9f
            };
            if (CheckCollisionRecs(attackHitbox, umbrellaHitbox))
            {
                mm->umbrellas[i].active = false;
                hitUmbrella = true;
            }
        }
    }

    if (hitUmbrella)
    {
        player->weapon.hitConnected = true;
    }

    return (hit || hitUmbrella);
}

bool IsMidnightManColliding(const MidnightMan *mm, Rectangle playerHitbox)
{
    if (!mm->active || mm->health <= 0)
    {
        return false;
    }

    if (mm->waveLeft.active && CheckCollisionRecs(playerHitbox, mm->waveLeft.hitbox))
    {
        return true;
    }
    if (mm->waveRight.active && CheckCollisionRecs(playerHitbox, mm->waveRight.hitbox))
    {
        return true;
    }

    if (mm->state == MM_GROUND_RISE || mm->state == MM_GROUND_RETREAT ||
        mm->state == MM_GROUND_PHASE2_RISE || mm->state == MM_GROUND_PHASE2_PAUSE ||
        mm->state == MM_GROUND_PHASE2_RETREAT || mm->state == MM_CEILING_SLAM ||
        mm->state == MM_CEILING_RETREAT || mm->state == MM_FOLLOW_SLAM ||
        mm->state == MM_FOLLOW_RETREAT || mm->state == MM_ARM_STORM_ACTIVE)

    {
        for (int i = 0; i < MM_HAND_COUNT; i++)
        {
            if (mm->handActive[i] && CheckCollisionRecs(playerHitbox, mm->handHitboxes[i]))
            {
                return true;
            }
        }
    }

    if (mm->state == MM_UMBRELLA_STORM)
    {
        for (int i = 0; i < MM_MAX_UMBRELLAS; i++)
        {
            if (mm->umbrellas[i].active)
            {
                float uW = 186.0f * mm->umbrellas[i].scale;
                float uH = 141.0f * mm->umbrellas[i].scale;
                Rectangle umbrellaHitbox =
                {
                    mm->umbrellas[i].position.x - uW * 0.35f,
                    mm->umbrellas[i].position.y - uH * 0.35f,
                    uW * 0.7f,
                    uH * 0.7f
                };
                if (CheckCollisionRecs(playerHitbox, umbrellaHitbox))
                {
                    return true;
                }
            }
        }

        for (int i = 0; i < MM_HAND_COUNT; i++)
        {
            if (mm->handActive[i] && CheckCollisionRecs(playerHitbox, mm->handHitboxes[i]))
            {
                return true;
            }
        }
    }

    return false;
}