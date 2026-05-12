#include <raylib.h>
#include "graphics/background.h"
#include "entities/player.h"
#include <math.h>
#include <stdio.h>

#define FLOOR_SCROLL_SPEED 600.0f
#define BAR_WIDTH_RATIO 0.5f
#define BAR_Y_RATIO 0.03f
#define LEVEL6_BOTTOM_TRIM_RATIO 0.030f

void InitBackground(Background *bg)
{
    bg->time = 0.0f;
    bg->scrollX = 0.0f;
    bg->waterScrollX = 0.0f;
    bg->floor = LoadTexture("assets/sprites/background/Floor.png");
    bg->bossHairyLeg = LoadTexture("assets/sprites/fundo/Background_1Boss.png");
    bg->bossMidnightMan = LoadTexture("assets/sprites/Boss/Spr_MidnightMan/Homem_da_meia_noite_background.png");
    bg->waterFrameCount = 38;
    bg->waterCurrentFrame = 0;
    bg->waterFrameTimer = 0.0f;
    bg->waterFrameTime = 0.05f;
    for (int i = 0; i < bg->waterFrameCount; i++)
    {
        char path[64];
        snprintf(path, sizeof(path), "assets/sprites/background/Water_frame_%02d.png", i);
        bg->waterFrames[i] = LoadTexture(path);
    }
    bg->waterStatic = LoadTexture("assets/sprites/background/Water.png");
    bg->barFrame = LoadTexture("assets/sprites/background/Barra_Boss.png");
    bg->barBackground = LoadTexture("assets/sprites/background/Fundo_Barra_Boss.png");
    bg->barFill = LoadTexture("assets/sprites/background/Porcentagem_Barra_Boss.png");
}

void UpdateBackground(Background *bg, float dt, GamePhase phase)
{
    bg->time += dt;

    bg->waterFrameTimer += dt;
    if (bg->waterFrameTimer >= bg->waterFrameTime)
    {
        bg->waterFrameTimer = 0.0f;
        bg->waterCurrentFrame = (bg->waterCurrentFrame + 1) % bg->waterFrameCount;
    }

    if (phase == PHASE_RUNNING)
    {
        bg->scrollX += FLOOR_SCROLL_SPEED * dt;
        bg->waterScrollX += FLOOR_SCROLL_SPEED * dt;
    }
}

void DrawBackground(Background *bg, int levelId, float level6IntroProgress, int screenWidth, int screenHeight, float groundY)
{
    Texture2D levelBackground;
    if (levelId == 6)
    {
        levelBackground = bg->bossMidnightMan;
    }
    else
    {
        levelBackground = bg->bossHairyLeg;
    }

    if (levelBackground.id > 0)
    {
        Rectangle sourceRec = { 0.0f, 0.0f, (float)levelBackground.width, (float)levelBackground.height };
        Rectangle destRec = { 0.0f, 0.0f, (float)screenWidth, (float)screenHeight };
        Vector2 origin = { 0.0f, 0.0f };

        if (levelId == 6)
        {
            float texW = (float)levelBackground.width;
            float texH = (float)levelBackground.height;
            float scale = (float)screenWidth / texW;
            float texHeightVisibleOnScreen = (float)screenHeight / scale;

            if (level6IntroProgress < 0.0f)
            {
                level6IntroProgress = 0.0f;
            }
            if (level6IntroProgress > 1.0f)
            {
                level6IntroProgress = 1.0f;
            }

            float bottomTrimTex = texH * LEVEL6_BOTTOM_TRIM_RATIO;
            float maxSrcY = fmaxf(0.0f, texH - texHeightVisibleOnScreen - bottomTrimTex);
            float srcY = maxSrcY * level6IntroProgress;
            float srcH = fminf(texHeightVisibleOnScreen, fmaxf(0.0f, texH - srcY));

            sourceRec.x = 0.0f;
            sourceRec.y = srcY;
            sourceRec.width = texW;
            sourceRec.height = srcH;
            destRec.x = 0.0f;
            destRec.y = 0.0f;
            destRec.width = (float)screenWidth;
            destRec.height = srcH * scale;
        }

        DrawTexturePro(levelBackground, sourceRec, destRec, origin, 0.0f, WHITE);
    }


    if (bg->floor.id > 0)
    {
        float scale = (float)screenWidth / bg->floor.width;
        float tileW = bg->floor.width * scale;
        float tileH = bg->floor.height * scale;
        float visibleTopOff = tileH * 0.85f;
        float posY = groundY - visibleTopOff;

        float offset = fmodf(bg->scrollX, tileW);

        float startX = -offset;

        if (startX > 0)
        {
            startX -= tileW;
        }

        for (float x = startX; x < screenWidth; x += tileW)
        {
            DrawTextureEx(bg->floor, (Vector2){x, posY}, 0.0f, scale, WHITE);
        }
    }

    DrawRectangleGradientV(0, 0, screenWidth, screenHeight / 4, (Color){ 0, 0, 0, 180 }, (Color){ 0, 0, 0, 0 });
    DrawRectangleGradientV(0, screenHeight * 3 / 4, screenWidth, screenHeight / 4, (Color){ 0, 0, 0, 0 }, (Color){ 0, 0, 0, 180 });

    if (bg->waterStatic.id > 0)
    {
        float tileW = (float)bg->waterStatic.width;
        float tileH = (float)bg->waterStatic.height;
        float scale = (float)screenWidth / tileW;
        float scaledW = tileW * scale;
        float scaledH = tileH * scale;
        float waterY = groundY;
        float offset = fmodf(bg->waterScrollX, scaledW);
        float startX = -offset;
        if (startX > 0)
        {
            startX -= scaledW;
        }

        for (float x = startX; x < screenWidth + scaledW; x += scaledW)
        {
            DrawTextureEx(bg->waterStatic, (Vector2){x, waterY}, 0.0f, scale, WHITE);
        }
    }

    Texture2D wTex = bg->waterFrames[bg->waterCurrentFrame];
    if (wTex.id > 0)
    {
        float waterY = groundY + (screenHeight * -0.2f);
        float waterH = screenHeight - waterY;
        if (waterH > 0)
        {
            Rectangle src = 
            {
                0,
                0,
                (float)wTex.width,
                (float)wTex.height
            };

            Rectangle dest = 
            {
                0,
                waterY,
                (float)screenWidth,
                waterH
            };

            DrawTexturePro(wTex, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
        }
    }
}

void DrawProgressBar(Background *bg, float progress, int screenWidth, int screenHeight)
{
    if (progress < 0.0f)
    {
        progress = 0.0f;
    }

    if (progress > 1.0f)
    {
        progress = 1.0f;
    }

    float barW = screenHeight * BAR_WIDTH_RATIO;
    float barH = (bg->barFrame.id > 0) ? barW * ((float)bg->barFrame.height / bg->barFrame.width) : screenHeight * 0.06f;
    float barX = (screenWidth - barW) / 2.0f;
    float barY = screenHeight * BAR_Y_RATIO;

    Rectangle dest = 
    {
        barX,
        barY,
        barW,
        barH
    };

    if (bg->barBackground.id > 0)
    {
        Rectangle src = 
        {
            0,
            0,
            (float)bg->barBackground.width,
            (float)bg->barBackground.height
        };

        DrawTexturePro(bg->barBackground, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
    }

    if (bg->barFill.id > 0 && progress > 0.0f)
    {
        float fillSrcW = bg->barFill.width * progress;
        Rectangle src = 
        {
            0,
            0,
            fillSrcW,
            (float)bg->barFill.height
        };

        Rectangle fillDest = 
        {
            barX,
            barY,
            barW * progress,
            barH
        };

        DrawTexturePro(bg->barFill, src, fillDest, (Vector2){0, 0}, 0.0f, WHITE);
    }

    if (bg->barFrame.id > 0)
    {
        Rectangle src = 
        {
            0,
            0,
            (float)bg->barFrame.width,
            (float)bg->barFrame.height
        };

        DrawTexturePro(bg->barFrame, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
    }
}

void UnloadBackground(Background *bg)
{
    UnloadTexture(bg->floor);
    UnloadTexture(bg->bossHairyLeg);
    UnloadTexture(bg->bossMidnightMan);
    for (int i = 0; i < bg->waterFrameCount; i++)
    {
        UnloadTexture(bg->waterFrames[i]);
    }
    UnloadTexture(bg->waterStatic);
    UnloadTexture(bg->barFrame);
    UnloadTexture(bg->barBackground);
    UnloadTexture(bg->barFill);
}
