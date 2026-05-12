#include "entities/midnight_man.h"
#include <math.h>

#define MM_ATTACK_PHASE_DURATION 0.72f
#define MM_IDLE_DURATION 1.1f
#define MM_TELEGRAPH_DURATION 0.5f
#define MM_HAND_DRAW_HEIGHT_RATIO 0.38f

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

    for (int i = 0; i < MM_HAND_COUNT; i++)
    {
        mm->handXPositions[i] = 0.0f;
    }

    if (mm->texHandOpen.id <= 0)
    {
        mm->texHandOpen = LoadTexture("assets/sprites/Boss/Spr_MidnightMan/Hand_open.png");
    }

    RefreshHandsLayout(mm, screenWidth, screenHeight, groundY);
    mm->handsY = (float)screenHeight;
}

void UpdateMidnightMan(MidnightMan *mm, Rectangle playerRect, float deltaTime, int screenWidth, int screenHeight, float groundY)
{
    if (!mm->active || mm->texHandOpen.id <= 0)
    {
        return;
    }

    RefreshHandsLayout(mm, screenWidth, screenHeight, groundY);

    switch (mm->state)
    {
        case MM_IDLE:
            mm->timer += deltaTime;
            mm->handsY = (float)screenHeight;
            if (mm->timer >= MM_IDLE_DURATION)
            {
                float visualW = mm->handDrawHeight;
                float halfVis = visualW / 2.0f;
                float W = (float)screenWidth;

                if (GetRandomValue(0, 1) == 0)
                {
                    mm->handXPositions[0] = W / 6.0f - halfVis;
                    mm->handXPositions[1] = W / 2.0f - halfVis;
                    mm->handXPositions[2] = W * 5.0f / 6.0f - halfVis;
                }
                else
                {
                    mm->handXPositions[0] = W / 3.0f - halfVis;
                    mm->handXPositions[1] = W * 2.0f / 3.0f - halfVis;
                    mm->handXPositions[2] = -(visualW * 2.0f);
                }

                mm->state = MM_TELEGRAPH;
                mm->timer = 0.0f;
                mm->handsY = mm->telegraphY;
            }
            break;

        case MM_TELEGRAPH:
            mm->timer += deltaTime;
            mm->handsY = mm->telegraphY;
            if (mm->timer >= MM_TELEGRAPH_DURATION)
            {
                mm->state = MM_RISING;
                mm->timer = 0.0f;
            }
            break;

        case MM_RISING:
        {
            mm->timer += deltaTime;
            float progress = mm->timer / MM_ATTACK_PHASE_DURATION;
            if (progress > 1.0f) progress = 1.0f;
            mm->handsY = Lerpf(mm->telegraphY, mm->riseStopY, progress);
            if (mm->timer >= MM_ATTACK_PHASE_DURATION)
            {
                mm->handsY = mm->riseStopY;
                mm->state = MM_RETREATING;
                mm->timer = 0.0f;
            }
            break;
        }

        case MM_RETREATING:
        {
            mm->timer += deltaTime;
            float progress = mm->timer / MM_ATTACK_PHASE_DURATION;
            if (progress > 1.0f) progress = 1.0f;
            mm->handsY = Lerpf(mm->riseStopY, (float)screenHeight, progress);
            if (mm->timer >= MM_ATTACK_PHASE_DURATION)
            {
                mm->handsY = (float)screenHeight;
                mm->state = MM_IDLE;
                mm->timer = 0.0f;
            }
            break;
        }
    }
}

void DrawMidnightMan(const MidnightMan *mm)
{
    if (!mm->active || mm->texHandOpen.id <= 0)
    {
        return;
    }

    float texW = (float)mm->texHandOpen.width;
    float texH = (float)mm->texHandOpen.height;

    float halfOrigW = mm->handDrawWidth / 2.0f;
    float halfOrigH = mm->handDrawHeight / 2.0f;
    float visualW = mm->handDrawHeight;

    for (int i = 0; i < MM_HAND_COUNT; i++)
    {
        float centerX = mm->handXPositions[i] + visualW / 2.0f;
        float centerY = mm->handsY + mm->handDrawWidth / 2.0f;
        Rectangle src = { 0.0f, 0.0f, texW, texH };
        Rectangle dest = { centerX - halfOrigW, centerY - halfOrigH, mm->handDrawWidth, mm->handDrawHeight };
        Vector2 pivot = { halfOrigW, halfOrigH };
        DrawTexturePro(mm->texHandOpen, src, dest, pivot, -90.0f, WHITE);
    }
}

void UnloadMidnightMan(MidnightMan *mm)
{
    if (mm->texHandOpen.id > 0)
    {
        UnloadTexture(mm->texHandOpen);
        mm->texHandOpen.id = 0;
    }
    mm->active = false;
}
