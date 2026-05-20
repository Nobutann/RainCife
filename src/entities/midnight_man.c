#include "entities/midnight_man.h"
#include "entities/player.h"
#include <math.h>

#define MM_ATTACK_PHASE_DURATION 0.72f
#define MM_IDLE_DURATION 1.1f
#define MM_TELEGRAPH_DURATION 1.2f
#define MM_HAND_DRAW_HEIGHT_RATIO 0.38f

#define MM_CEILING_TELEGRAPH_DURATION 1.2f
#define MM_CEILING_SLAM_DURATION 0.3f
#define MM_CEILING_PAUSE_DURATION 0.5f
#define MM_CEILING_RETREAT_DURATION 0.7f

#define MM_SWEEP_TELEGRAPH_DURATION 1.2f
#define MM_SWEEP_MOVE_DURATION 1.4f

#define MM_UMBRELLA_STORM_DURATION 5.5f
#define MM_UMBRELLA_SPAWN_INTERVAL 0.45f

static float Lerpf(float from, float to, float weight)
{
    return from + (to - from) * weight;
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
    if (mm->animShadow.sheet.id <= 0)
    {
        mm->animShadow = LoadAnimation("assets/sprites/Boss/Shadow-Sheet.png", 4, 0.1f);
    }

    for (int i = 0; i < MM_MAX_UMBRELLAS; i++)
    {
        mm->umbrellas[i].active = false;
    }

    mm->sweepHitbox = (Rectangle){0};
    mm->sweepX = 0.0f;
    mm->sweepY = 0.0f;
    mm->sweepWidth = 0.0f;
    mm->sweepHeight = 0.0f;
    mm->sweepDirection = 1;

    RefreshHandsLayout(mm, screenWidth, screenHeight, groundY);
    mm->handsY = (float)screenHeight;
}

void UpdateMidnightMan(MidnightMan *mm, Rectangle playerRect, float deltaTime, int screenWidth, int screenHeight, float groundY)
{
    if (!mm->active)
    {
        return;
    }

    if (mm->health <= 0 && mm->state != MM_DEAD)
    {
        mm->state = MM_DEAD;
        mm->timer = 0.0f;
        for (int i = 0; i < MM_HAND_COUNT; i++) mm->handActive[i] = false;
        for (int i = 0; i < MM_MAX_UMBRELLAS; i++) mm->umbrellas[i].active = false;
        return;
    }

    RefreshHandsLayout(mm, screenWidth, screenHeight, groundY);

    // Update Touhou falling/aiming umbrellas
    for (int i = 0; i < MM_MAX_UMBRELLAS; i++)
    {
        if (mm->umbrellas[i].active)
        {
            mm->umbrellas[i].position.x += mm->umbrellas[i].velocity.x * deltaTime;
            mm->umbrellas[i].position.y += mm->umbrellas[i].velocity.y * deltaTime;

            // Animate umbrella (20 frames)
            mm->umbrellas[i].animTimer += deltaTime;
            if (mm->umbrellas[i].animTimer >= 0.05f)
            {
                mm->umbrellas[i].animTimer = 0.0f;
                mm->umbrellas[i].animFrame = (mm->umbrellas[i].animFrame + 1) % 20;
            }

            // Split logic for big umbrella
            if (mm->umbrellas[i].isBig && mm->umbrellas[i].position.y >= (float)screenHeight * 0.40f)
            {
                mm->umbrellas[i].active = false;

                Vector2 origin = mm->umbrellas[i].position;
                Vector2 playerCenter = { playerRect.x + playerRect.width / 2.0f, playerRect.y + playerRect.height / 2.0f };

                Vector2 dir = { playerCenter.x - origin.x, playerCenter.y - origin.y };
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

                // Touhou spread: 5 small aimed projectiles
                float baseAngle = atan2f(dir.y, dir.x);
                float spreadAngles[] = { -0.3f, -0.15f, 0.0f, 0.15f, 0.3f };
                int count = sizeof(spreadAngles) / sizeof(spreadAngles[0]);

                int spawned = 0;
                for (int u = 0; u < MM_MAX_UMBRELLAS; u++)
                {
                    if (spawned >= count) break;
                    if (!mm->umbrellas[u].active)
                    {
                        float angle = baseAngle + spreadAngles[spawned];
                        float speed = 260.0f + (float)GetRandomValue(0, 70); // varying speeds
                        mm->umbrellas[u].active = true;
                        mm->umbrellas[u].isBig = false;
                        mm->umbrellas[u].position = origin;
                        mm->umbrellas[u].velocity.x = cosf(angle) * speed;
                        mm->umbrellas[u].velocity.y = sinf(angle) * speed;
                        mm->umbrellas[u].scale = 0.25f; // small scale
                        mm->umbrellas[u].animFrame = GetRandomValue(0, 19);
                        mm->umbrellas[u].animTimer = 0.0f;
                        spawned++;
                    }
                }
            }

            // Deactivate when off-screen
            if (mm->umbrellas[i].position.y > (float)screenHeight + 100.0f ||
                mm->umbrellas[i].position.x < -100.0f ||
                mm->umbrellas[i].position.x > (float)screenWidth + 100.0f ||
                mm->umbrellas[i].position.y < -150.0f)
            {
                mm->umbrellas[i].active = false;
            }
        }
    }

    switch (mm->state)
    {
        case MM_DEAD:
            mm->handsY = (float)screenHeight + 300.0f;
            for (int i = 0; i < MM_HAND_COUNT; i++)
            {
                mm->handActive[i] = false;
                mm->handHitboxes[i] = (Rectangle){0};
            }
            mm->sweepHitbox = (Rectangle){0};
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
            mm->sweepHitbox = (Rectangle){0};

            if (mm->timer >= MM_IDLE_DURATION)
            {
                mm->timer = 0.0f;
                // Choose attack: 0=Ground rise, 1=Ceiling slam, 2=Sweep, 3=Umbrella storm
                int nextAttack = GetRandomValue(0, 3);

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

                    // Fist 1 targets player
                    mm->handXPositions[0] = playerX - halfVis;
                    mm->handActive[0] = true;

                    // Fist 2 targets a random position away from player
                    float offset = (float)GetRandomValue(220, 450);
                    if (GetRandomValue(0, 1) == 0) offset = -offset;
                    float secX = playerX + offset;
                    if (secX < 50.0f) secX = (float)screenWidth - 150.0f;
                    if (secX > (float)screenWidth - 150.0f) secX = 150.0f;

                    mm->handXPositions[1] = secX - halfVis;
                    mm->handActive[1] = true;

                    mm->handXPositions[2] = -9999.0f;
                    mm->handActive[2] = false;

                    mm->state = MM_CEILING_TELEGRAPH;
                    mm->handsY = -mm->handDrawHeight;
                    mm->animShadow.currentFrame = 0;
                    mm->animShadow.timer = 0.0f;
                }
                else if (nextAttack == 2)
                {
                    mm->sweepDirection = (GetRandomValue(0, 1) == 0) ? 1 : -1;
                    mm->sweepWidth = mm->handDrawHeight;
                    mm->sweepHeight = mm->handDrawWidth;
                    mm->sweepY = groundY - mm->sweepHeight + 20.0f;

                    if (mm->sweepDirection == 1)
                    {
                        mm->sweepX = -mm->sweepWidth;
                    }
                    else
                    {
                        mm->sweepX = (float)screenWidth;
                    }

                    mm->state = MM_SWEEP_TELEGRAPH;
                }
                else
                {
                    mm->state = MM_UMBRELLA_STORM;
                    mm->umbrellaSpawnTimer = 0.0f;
                }
            }
            break;
        }

        // GROUND ATTACK
        case MM_GROUND_TELEGRAPH:
            mm->timer += deltaTime;
            mm->handsY = mm->telegraphY;
            {
                float progress = mm->timer / MM_TELEGRAPH_DURATION;
                if (progress > 1.0f) progress = 1.0f;
                int frame = (int)(progress * mm->animShadow.frameCount);
                if (frame >= mm->animShadow.frameCount) frame = mm->animShadow.frameCount - 1;
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
            if (progress > 1.0f) progress = 1.0f;
            mm->handsY = Lerpf(mm->telegraphY, mm->riseStopY, progress);

            for (int i = 0; i < MM_HAND_COUNT; i++)
            {
                if (mm->handActive[i])
                {
                    mm->handHitboxes[i] = (Rectangle){
                        mm->handXPositions[i],
                        mm->handsY,
                        mm->handDrawHeight,
                        mm->handDrawWidth
                    };
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
            if (progress > 1.0f) progress = 1.0f;
            mm->handsY = Lerpf(mm->riseStopY, (float)screenHeight, progress);

            for (int i = 0; i < MM_HAND_COUNT; i++)
            {
                if (mm->handActive[i])
                {
                    mm->handHitboxes[i] = (Rectangle){
                        mm->handXPositions[i],
                        mm->handsY,
                        mm->handDrawHeight,
                        mm->handDrawWidth
                    };
                }
            }

            if (mm->timer >= MM_ATTACK_PHASE_DURATION)
            {
                mm->handsY = (float)screenHeight;
                mm->state = MM_IDLE;
                mm->timer = 0.0f;
            }
            break;
        }

        // CEILING SLAM
        case MM_CEILING_TELEGRAPH:
            mm->timer += deltaTime;
            mm->handsY = -mm->handDrawHeight;
            {
                float progress = mm->timer / MM_CEILING_TELEGRAPH_DURATION;
                if (progress > 1.0f) progress = 1.0f;
                int frame = (int)(progress * mm->animShadow.frameCount);
                if (frame >= mm->animShadow.frameCount) frame = mm->animShadow.frameCount - 1;
                mm->animShadow.currentFrame = frame;
            }
            if (mm->timer >= MM_CEILING_TELEGRAPH_DURATION)
            {
                mm->state = MM_CEILING_SLAM;
                mm->timer = 0.0f;
            }
            break;

        case MM_CEILING_SLAM:
        {
            mm->timer += deltaTime;
            float progress = mm->timer / MM_CEILING_SLAM_DURATION;
            if (progress > 1.0f) progress = 1.0f;

            float startY = -mm->handDrawHeight;
            float targetY = groundY - mm->handDrawWidth + 30.0f;
            mm->handsY = Lerpf(startY, targetY, progress);

            for (int i = 0; i < MM_HAND_COUNT; i++)
            {
                if (mm->handActive[i])
                {
                    mm->handHitboxes[i] = (Rectangle){
                        mm->handXPositions[i],
                        mm->handsY,
                        mm->handDrawHeight,
                        mm->handDrawWidth
                    };
                }
            }

            if (mm->timer >= MM_CEILING_SLAM_DURATION)
            {
                mm->handsY = targetY;
                mm->state = MM_CEILING_RETREAT;
                mm->timer = 0.0f;
            }
            break;
        }

        case MM_CEILING_RETREAT:
        {
            mm->timer += deltaTime;
            float progress = mm->timer / (MM_CEILING_PAUSE_DURATION + MM_CEILING_RETREAT_DURATION);
            if (progress > 1.0f) progress = 1.0f;

            float targetY = groundY - mm->handDrawWidth + 30.0f;
            float endY = -mm->handDrawHeight;

            if (mm->timer < MM_CEILING_PAUSE_DURATION)
            {
                mm->handsY = targetY;
            }
            else
            {
                float retreatProgress = (mm->timer - MM_CEILING_PAUSE_DURATION) / MM_CEILING_RETREAT_DURATION;
                if (retreatProgress > 1.0f) retreatProgress = 1.0f;
                mm->handsY = Lerpf(targetY, endY, retreatProgress);
            }

            for (int i = 0; i < MM_HAND_COUNT; i++)
            {
                if (mm->handActive[i])
                {
                    mm->handHitboxes[i] = (Rectangle){
                        mm->handXPositions[i],
                        mm->handsY,
                        mm->handDrawHeight,
                        mm->handDrawWidth
                    };
                }
            }

            if (progress >= 1.0f)
            {
                mm->handsY = (float)screenHeight;
                mm->state = MM_IDLE;
                mm->timer = 0.0f;
            }
            break;
        }

        // SWEEP
        case MM_SWEEP_TELEGRAPH:
            mm->timer += deltaTime;
            if (mm->timer >= MM_SWEEP_TELEGRAPH_DURATION)
            {
                mm->state = MM_SWEEP_MOVE;
                mm->timer = 0.0f;
            }
            break;

        case MM_SWEEP_MOVE:
        {
            mm->timer += deltaTime;
            float progress = mm->timer / MM_SWEEP_MOVE_DURATION;
            if (progress > 1.0f) progress = 1.0f;

            float startX = (mm->sweepDirection == 1) ? -mm->sweepWidth : (float)screenWidth;
            float endX = (mm->sweepDirection == 1) ? (float)screenWidth : -mm->sweepWidth;
            mm->sweepX = Lerpf(startX, endX, progress);

            float hitboxHeight = mm->sweepHeight * 0.30f;
            mm->sweepHitbox = (Rectangle){
                mm->sweepX,
                groundY + 20.0f - hitboxHeight,
                mm->sweepWidth,
                hitboxHeight
            };

            if (mm->timer >= MM_SWEEP_MOVE_DURATION)
            {
                mm->sweepHitbox = (Rectangle){0};
                mm->state = MM_SWEEP_RETREAT;
                mm->timer = 0.0f;
            }
            break;
        }

        case MM_SWEEP_RETREAT:
            mm->state = MM_IDLE;
            mm->timer = 0.0f;
            break;

        // UMBRELLA STORM
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

            // Hover hands at the top corners to be targetable
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
                    mm->handHitboxes[i] = (Rectangle){
                        mm->handXPositions[i],
                        mm->handsY,
                        mm->handDrawHeight,
                        mm->handDrawWidth
                    };
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

    // 1. Draw warning indicators
    if (mm->state == MM_GROUND_TELEGRAPH)
    {
        float visualW = mm->handDrawHeight;
        for (int i = 0; i < MM_HAND_COUNT; i++)
        {
            if (mm->handActive[i])
            {
                DrawRectangle((int)mm->handXPositions[i], 0, (int)visualW, screenHeight, Fade(RED, 0.2f));
            }
        }
        if (mm->animShadow.sheet.id > 0)
        {
            float scale = ((float)screenHeight * 0.65f / 252.0f) * 1.5f;
            float shadowW = (float)mm->animShadow.frameWidth * scale;
            float shadowH = (float)mm->animShadow.sheet.height * scale;
            for (int i = 0; i < MM_HAND_COUNT; i++)
            {
                if (mm->handActive[i])
                {
                    Vector2 warningPosition = {
                        mm->handXPositions[i] + (visualW * 0.5f) - (shadowW * 0.5f),
                        groundY - shadowH - (42.0f * scale)
                    };
                    DrawAnimationFrame((Animation *)&mm->animShadow, warningPosition, scale, false, WHITE);
                }
            }
        }
    }
    else if (mm->state == MM_CEILING_TELEGRAPH)
    {
        float visualW = mm->handDrawHeight;
        for (int i = 0; i < MM_HAND_COUNT; i++)
        {
            if (mm->handActive[i])
            {
                DrawRectangle((int)mm->handXPositions[i], 0, (int)visualW, screenHeight, Fade(RED, 0.2f));
            }
        }
        if (mm->animShadow.sheet.id > 0)
        {
            float scale = ((float)screenHeight * 0.65f / 252.0f) * 1.5f;
            float shadowW = (float)mm->animShadow.frameWidth * scale;
            float shadowH = (float)mm->animShadow.sheet.height * scale;
            for (int i = 0; i < MM_HAND_COUNT; i++)
            {
                if (mm->handActive[i])
                {
                    Vector2 warningPosition = {
                        mm->handXPositions[i] + (visualW * 0.5f) - (shadowW * 0.5f),
                        groundY - shadowH - (42.0f * scale)
                    };
                    DrawAnimationFrame((Animation *)&mm->animShadow, warningPosition, scale, false, WHITE);
                }
            }
        }
    }
    else if (mm->state == MM_SWEEP_TELEGRAPH)
    {
        float telegraphHeight = mm->sweepHeight * 0.30f;
        DrawRectangle(0, (int)(groundY + 20.0f - telegraphHeight), screenWidth, (int)telegraphHeight, Fade(RED, 0.4f));
    }

    // 2. Draw Hands/Fists
    if (mm->state == MM_CEILING_SLAM || mm->state == MM_CEILING_RETREAT)
    {
        // Draw ceiling slams as fists pointing down
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
                    Rectangle src = { 0.0f, 0.0f, texW, texH };
                    Rectangle dest = { centerX, centerY, drawW, drawH };
                    Vector2 pivot = { drawW / 2.0f, drawH / 2.0f };
                    DrawTexturePro(mm->texFist, src, dest, pivot, 90.0f, WHITE);
                }
            }
        }
    }
    else if (mm->state == MM_SWEEP_MOVE)
    {
        // Draw sweeping claw horizontally
        if (mm->texHandOpen.id > 0)
        {
            float texW = (float)mm->texHandOpen.width;
            float texH = (float)mm->texHandOpen.height;
            float hitboxHeight = mm->sweepHeight * 0.30f;
            float drawY = groundY + 20.0f - hitboxHeight;
            float centerX = mm->sweepX + mm->sweepWidth / 2.0f;
            float centerY = drawY + hitboxHeight / 2.0f;
            // Flip texture vertically if sweeping from right to left
            Rectangle src = { 0.0f, 0.0f, texW, (mm->sweepDirection == 1) ? texH : -texH };
            Rectangle dest = { centerX, centerY, mm->sweepWidth, hitboxHeight };
            Vector2 pivot = { mm->sweepWidth / 2.0f, hitboxHeight / 2.0f };
            float rotation = (mm->sweepDirection == 1) ? 0.0f : 180.0f;
            DrawTexturePro(mm->texHandOpen, src, dest, pivot, rotation, WHITE);
        }
    }
    else
    {
        // Draw ground claws / hovering claws
        if (mm->texHandOpen.id > 0)
        {
            float texW = (float)mm->texHandOpen.width;
            float texH = (float)mm->texHandOpen.height;

            float halfOrigW = mm->handDrawWidth / 2.0f;
            float halfOrigH = mm->handDrawHeight / 2.0f;
            float visualW = mm->handDrawHeight;

            for (int i = 0; i < MM_HAND_COUNT; i++)
            {
                if (mm->handActive[i] || mm->state == MM_GROUND_RISE || mm->state == MM_GROUND_RETREAT)
                {
                    float centerX = mm->handXPositions[i] + visualW / 2.0f;
                    float centerY = mm->handsY + mm->handDrawWidth / 2.0f;
                    Rectangle src = { 0.0f, 0.0f, texW, texH };
                    Rectangle dest = { centerX, centerY, mm->handDrawWidth, mm->handDrawHeight };
                    Vector2 pivot = { halfOrigW, halfOrigH };
                    DrawTexturePro(mm->texHandOpen, src, dest, pivot, -90.0f, WHITE);
                }
            }
        }
    }

    // 3. Draw umbrellas
    if (mm->texUmbrella.id > 0)
    {
        for (int i = 0; i < MM_MAX_UMBRELLAS; i++)
        {
            if (mm->umbrellas[i].active)
            {
                float uW = 186.0f;
                float uH = 141.0f;
                Rectangle src = { mm->umbrellas[i].animFrame * uW, 0.0f, uW, uH };
                float drawW = uW * mm->umbrellas[i].scale;
                float drawH = uH * mm->umbrellas[i].scale;
                Rectangle dest = {
                    mm->umbrellas[i].position.x,
                    mm->umbrellas[i].position.y,
                    drawW,
                    drawH
                };
                Vector2 pivot = { drawW / 2.0f, drawH / 2.0f };
                DrawTexturePro(mm->texUmbrella, src, dest, pivot, 0.0f, WHITE);
            }
        }
    }

    // Debug hitboxes
    #ifdef DEBUG
    for (int i = 0; i < MM_HAND_COUNT; i++)
    {
        if (mm->handActive[i])
        {
            DrawRectangleLinesEx(mm->handHitboxes[i], 2.0f, RED);
        }
    }
    if (mm->state == MM_SWEEP_MOVE)
    {
        DrawRectangleLinesEx(mm->sweepHitbox, 2.0f, RED);
    }
    if (mm->state == MM_UMBRELLA_STORM)
    {
        for (int i = 0; i < MM_MAX_UMBRELLAS; i++)
        {
            if (mm->umbrellas[i].active)
            {
                float uW = 186.0f * mm->umbrellas[i].scale;
                float uH = 141.0f * mm->umbrellas[i].scale;
                Rectangle umbrellaHitbox = {
                    mm->umbrellas[i].position.x - uW * 0.35f,
                    mm->umbrellas[i].position.y - uH * 0.35f,
                    uW * 0.7f,
                    uH * 0.7f
                };
                DrawRectangleLinesEx(umbrellaHitbox, 1.0f, ORANGE);
            }
        }
    }
    #endif
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

    if (!hit && mm->state == MM_SWEEP_MOVE)
    {
        if (CheckCollisionRecs(attackHitbox, mm->sweepHitbox))
        {
            hit = true;
        }
    }

    if (hit)
    {
        mm->health -= (int)player->weapon.damage;
        if (mm->health < 0)
        {
            mm->health = 0;
        }
        player->weapon.hitConnected = true;
        return true;
    }

    return false;
}

bool IsMidnightManColliding(const MidnightMan *mm, Rectangle playerHitbox)
{
    if (!mm->active || mm->health <= 0)
    {
        return false;
    }

    if (mm->state == MM_GROUND_RISE || mm->state == MM_GROUND_RETREAT ||
        mm->state == MM_CEILING_SLAM || mm->state == MM_CEILING_RETREAT)
    {
        for (int i = 0; i < MM_HAND_COUNT; i++)
        {
            if (mm->handActive[i] && CheckCollisionRecs(playerHitbox, mm->handHitboxes[i]))
            {
                return true;
            }
        }
    }

    if (mm->state == MM_SWEEP_MOVE && CheckCollisionRecs(playerHitbox, mm->sweepHitbox))
    {
        return true;
    }

    if (mm->state == MM_UMBRELLA_STORM)
    {
        for (int i = 0; i < MM_MAX_UMBRELLAS; i++)
        {
            if (mm->umbrellas[i].active)
            {
                float uW = 186.0f * mm->umbrellas[i].scale;
                float uH = 141.0f * mm->umbrellas[i].scale;
                Rectangle umbrellaHitbox = {
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
