#include <raylib.h>
#include "graphics/background.h"
#include "entities/player.h"
#include <math.h>
#include <stdio.h>

#define FLOOR_SCROLL_SPEED 900.0f
#define RUNNING_BG_SCROLL_SPEED 487.5f
#define FLOOR_VISIBLE_TOP_RATIO 0.85f
#define FLOOR_WATER_START_Y 317.0f
#define BAR_WIDTH_RATIO 0.5f
#define BAR_Y_RATIO 0.03f
#define BAR_FRAME_WIDTH_EXTRA_RATIO 0.05f
#define BAR_TRACK_SRC_X 18.0f
#define BAR_TRACK_SRC_RIGHT 330.0f
#define LEVEL6_BOTTOM_TRIM_RATIO 0.030f
#define BUEIRO_INTERVAL 2000.0f
#define COLOR_FRONT_TINT_ALPHA 145
#define RAIN_MIN_DROPS 140
#define RAIN_MAX_DROPS 260
#define RAIN_FALL_SPEED 980.0f
#define RAIN_WIND_SPEED -250.0f
#define WATER_SPLASH_FRAME_COUNT 4
#define WATER_SPLASH_FRAME_TIME 0.055f

void ResetBackgroundState(Background *bg)
{
    bg->time = 0.0f;
    bg->scrollX = 0.0f;
    bg->waterScrollX = 0.0f;
    bg->waterCurrentFrame = 0;
    bg->waterFrameTimer = 0.0f;
    bg->waterFrameTime = 0.01f;
    for (int i = 0; i < MAX_WATER_SPLASHES; i++) bg->waterSplashes[i].active = false;
    bg->waterSplashSpawnTimer = 0.0f;
    bg->waterSplashSpawnInterval = 0.12f;
    bg->runningBgScrollX = 0.0f;
    for (int i = 0; i < MAX_OBJETOS; i++) bg->objetosList[i].active = false;
    bg->objetoSpawnTimer = 0.0f;
    bg->objetoSpawnInterval = 2.0f;
}

void InitBackground(Background *bg)
{
    ResetBackgroundState(bg);
    bg->floor = LoadTexture("assets/sprites/background/Floor.png");
    bg->bossHairyLeg = LoadTexture("assets/sprites/fundo/Background_1Boss.png");
    bg->bossMidnightMan = LoadTexture("assets/sprites/Boss/Spr_MidnightMan/Homem_da_meia_noite_background.png");
    bg->bossShark = LoadTexture("assets/sprites/fundo/Fase_2_fundos/Fase2_background_1.png");
    bg->waterFrameCount = 38;
    for (int i = 0; i < bg->waterFrameCount; i++)
    {
        char path[64];
        snprintf(path, sizeof(path), "assets/sprites/background/Water_frame_%02d.png", i);
        bg->waterFrames[i] = LoadTexture(path);
    }
    bg->waterStatic = LoadTexture("assets/sprites/background/Water.png");
    bg->waterSplashSheet = LoadTexture("assets/sprites/background/Water_splash-Sheet.png");
    bg->barFrame = LoadTexture("assets/sprites/background/Barra_Boss.png");
    bg->barBackground = LoadTexture("assets/sprites/background/Fundo_Barra_Boss.png");
    bg->barFill = LoadTexture("assets/sprites/background/Porcentagem_Barra_Boss.png");

    bg->runningPhase2[0] = LoadTexture("assets/sprites/fundo/Fase_2_fundos/Fase2_background_1.png");
    bg->runningPhase2[1] = LoadTexture("assets/sprites/fundo/Fase_2_fundos/Fase2_background_2.png");
    bg->runningPhase2[2] = LoadTexture("assets/sprites/fundo/Fase_2_fundos/Fase2_background_3.png");
    bg->runningPhase3[0] = LoadTexture("assets/sprites/fundo/Fase_3_fundos/Ultima_fase_background_1.png");
    bg->runningPhase3[1] = LoadTexture("assets/sprites/fundo/Fase_3_fundos/Ultima_fase_background_2.png");
    bg->runningPhase3[2] = LoadTexture("assets/sprites/fundo/Fase_3_fundos/Ultima_fase_background_3.png");
    bg->bueiro = LoadTexture("assets/sprites/background/Bueiro.png");
    bg->objetos = LoadTexture("assets/sprites/background/objetos.png");
    bg->colorFront = LoadTexture("assets/sprites/background/Color_front.png");
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
        bg->runningBgScrollX += RUNNING_BG_SCROLL_SPEED * dt;
    }
}

static float GetWaterSurfaceY(Background *bg, int screenWidth, float groundY)
{
    float waterY = groundY;
    if (bg->floor.id > 0)
    {
        float floorScale = (float)screenWidth / bg->floor.width;
        float floorY = groundY - (bg->floor.height * floorScale * FLOOR_VISIBLE_TOP_RATIO);
        waterY = floorY + (FLOOR_WATER_START_Y * floorScale);
    }
    return waterY;
}

void UpdateWaterSplashes(Background *bg, float dt, int screenWidth, int screenHeight, float groundY)
{
    if (bg->waterSplashSheet.id == 0) return;

    for (int i = 0; i < MAX_WATER_SPLASHES; i++)
    {
        if (!bg->waterSplashes[i].active) continue;

        bg->waterSplashes[i].frameTimer += dt;
        while (bg->waterSplashes[i].frameTimer >= WATER_SPLASH_FRAME_TIME)
        {
            bg->waterSplashes[i].frameTimer -= WATER_SPLASH_FRAME_TIME;
            bg->waterSplashes[i].frame++;
            if (bg->waterSplashes[i].frame >= WATER_SPLASH_FRAME_COUNT)
            {
                bg->waterSplashes[i].active = false;
                break;
            }
        }
    }

    bg->waterSplashSpawnTimer += dt;
    if (bg->waterSplashSpawnTimer < bg->waterSplashSpawnInterval) return;

    bg->waterSplashSpawnTimer = 0.0f;
    bg->waterSplashSpawnInterval = (float)GetRandomValue(5, 16) / 100.0f;

    for (int i = 0; i < MAX_WATER_SPLASHES; i++)
    {
        if (bg->waterSplashes[i].active) continue;

        float frameW = (float)bg->waterSplashSheet.width / WATER_SPLASH_FRAME_COUNT;
        float frameH = (float)bg->waterSplashSheet.height;
        float scale = (float)screenHeight / 430.0f;
        scale *= (float)GetRandomValue(60, 100) / 100.0f;
        float splashW = frameW * scale;
        float splashH = frameH * scale;
        float waterY = GetWaterSurfaceY(bg, screenWidth, groundY);
        float surfaceBand = fmaxf(10.0f, screenHeight * 0.055f);

        bg->waterSplashes[i].active = true;
        bg->waterSplashes[i].x = (float)GetRandomValue(0, screenWidth) - splashW * 0.5f;
        bg->waterSplashes[i].y = waterY + (float)GetRandomValue(0, (int)surfaceBand) - splashH * 0.55f;
        bg->waterSplashes[i].scale = scale;
        bg->waterSplashes[i].frameTimer = 0.0f;
        bg->waterSplashes[i].frame = 0;
        break;
    }
}

static void DrawBgStrip(Texture2D *frames, int count, float scrollX, int screenWidth, int screenHeight)
{
    float scales[3];
    float widths[3];
    float totalW = 0.0f;
    for (int i = 0; i < count; i++)
    {
        scales[i] = (float)screenHeight / (float)frames[i].height;
        widths[i] = (float)frames[i].width * scales[i];
        totalW += widths[i];
    }

    float offset = fmodf(scrollX, totalW);
    if (offset < 0.0f) offset += totalW;

    float x = -offset;
    while (x < (float)screenWidth)
    {
        for (int i = 0; i < count && x < (float)screenWidth; i++)
        {
            if (frames[i].id > 0)
                DrawTextureEx(frames[i], (Vector2){x, 0.0f}, 0.0f, scales[i], WHITE);
            x += widths[i];
        }
    }
}

void DrawBackground(Background *bg, int levelId, int bossId, float level6IntroProgress, int screenWidth, int screenHeight, float groundY, GamePhase phase)
{
    Texture2D *runningFrames = NULL;
    int runningFrameCount = 0;
    if (phase == PHASE_RUNNING)
    {
        if (levelId == 1 || levelId == 4)
        {
            runningFrames = &bg->bossHairyLeg;
            runningFrameCount = 1;
        }
        else if (levelId == 2 || levelId == 5)
        {
            runningFrames = bg->runningPhase2;
            runningFrameCount = 3;
        }
        else if (levelId == 3 || levelId == 6)
        {
            runningFrames = bg->runningPhase3;
            runningFrameCount = 3;
        }
    }

    if (runningFrames != NULL)
    {
        DrawBgStrip(runningFrames, runningFrameCount, bg->runningBgScrollX, screenWidth, screenHeight);
    }
    else
    {

    bool useMidnightManBossBackground = (bossId == 3 || levelId == 6);
    Texture2D levelBackground;
    if (useMidnightManBossBackground)
    {
        levelBackground = bg->bossMidnightMan;
    }
    else if (bossId == 2)
    {
        levelBackground = bg->bossShark;
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

        if (useMidnightManBossBackground)
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

    }

    if (bg->floor.id > 0)
    {
        float scale = (float)screenWidth / bg->floor.width;
        float tileW = bg->floor.width * scale;
        float tileH = bg->floor.height * scale;
        float visibleTopOff = tileH * FLOOR_VISIBLE_TOP_RATIO;
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

    if (bg->bueiro.id > 0 && phase == PHASE_RUNNING && bg->floor.id > 0)
    {
        float floorScale = (float)screenWidth / bg->floor.width;
        float floorY = groundY - (bg->floor.height * floorScale * FLOOR_VISIBLE_TOP_RATIO);
        float bueiroBaseY = floorY + (FLOOR_WATER_START_Y * floorScale);

        float bueiroScale = floorScale;
        float bueiroW = bg->bueiro.width * bueiroScale;
        float bueiroH = bg->bueiro.height * bueiroScale;

        float offset = fmodf(bg->scrollX, BUEIRO_INTERVAL);
        if (offset < 0.0f) offset += BUEIRO_INTERVAL;

        for (float x = -offset; x < (float)screenWidth + bueiroW; x += BUEIRO_INTERVAL)
        {
            Rectangle src = { 0, 0, (float)bg->bueiro.width, (float)bg->bueiro.height };
            Rectangle dest = { x, bueiroBaseY - bueiroH - 17.0f, bueiroW, bueiroH };
            DrawTexturePro(bg->bueiro, src, dest, (Vector2){ 0, 0 }, 0.0f, WHITE);
        }
    }

    DrawRectangleGradientV(0, 0, screenWidth, screenHeight / 4, (Color){ 0, 0, 0, 180 }, (Color){ 0, 0, 0, 0 });
    DrawRectangleGradientV(0, screenHeight * 3 / 4, screenWidth, screenHeight / 4, (Color){ 0, 0, 0, 0 }, (Color){ 0, 0, 0, 180 });

}

void DrawWater(Background *bg, int screenWidth, int screenHeight, float groundY)
{
    if (bg->waterStatic.id > 0)
    {
        float tileW = (float)bg->waterStatic.width;
        float scale = (float)screenWidth / tileW;
        float scaledW = tileW * scale;
        float waterY = GetWaterSurfaceY(bg, screenWidth, groundY);

        float offset = fmodf(bg->waterScrollX, scaledW);
        float startX = -offset;
        if (startX > 0)
        {
            startX -= scaledW;
        }

        float waterHeight = ((float)screenHeight - waterY) * 1.5f;
        Rectangle waterSrc = { 0, 0, (float)bg->waterStatic.width, (float)bg->waterStatic.height };
        for (float x = startX; x < screenWidth + scaledW; x += scaledW)
        {
            Rectangle waterDest = { x, waterY, scaledW, waterHeight };
            DrawTexturePro(bg->waterStatic, waterSrc, waterDest, (Vector2){0, 0}, 0.0f, WHITE);
        }
    }

    Texture2D wTex = bg->waterFrames[bg->waterCurrentFrame];
    int nextFrame = (bg->waterCurrentFrame + 1) % bg->waterFrameCount;
    Texture2D wTexNext = bg->waterFrames[nextFrame];

    if (wTex.id > 0)
    {
        float waterY = groundY + (screenHeight * -0.14f);
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

            float alpha = bg->waterFrameTimer / bg->waterFrameTime;

            BeginBlendMode(BLEND_ALPHA);
                DrawTexturePro(wTex, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
                if (wTexNext.id > 0)
                {
                    Color tint = {255, 255, 255, (unsigned char)(255 * alpha)};
                    DrawTexturePro(wTexNext, src, dest, (Vector2){0, 0}, 0.0f, tint);
                }
            EndBlendMode();
        }
    }
}

void DrawWaterSplashes(Background *bg)
{
    if (bg->waterSplashSheet.id == 0) return;

    float frameW = (float)bg->waterSplashSheet.width / WATER_SPLASH_FRAME_COUNT;
    float frameH = (float)bg->waterSplashSheet.height;

    BeginBlendMode(BLEND_ALPHA);
    for (int i = 0; i < MAX_WATER_SPLASHES; i++)
    {
        if (!bg->waterSplashes[i].active) continue;

        Rectangle src =
        {
            bg->waterSplashes[i].frame * frameW,
            0.0f,
            frameW,
            frameH
        };
        Rectangle dest =
        {
            bg->waterSplashes[i].x,
            bg->waterSplashes[i].y,
            frameW * bg->waterSplashes[i].scale,
            frameH * bg->waterSplashes[i].scale
        };

        DrawTexturePro(bg->waterSplashSheet, src, dest, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
    }
    EndBlendMode();
}

void UpdateObjetos(Background *bg, float dt, int screenWidth, int screenHeight, float groundY, GamePhase phase)
{
    if (phase != PHASE_RUNNING || bg->objetos.id == 0) return;

    float frameW = (float)bg->objetos.width / 3.0f;
    float floorScale = (bg->floor.id > 0) ? (float)screenWidth / bg->floor.width : 1.0f;
    float objScale = floorScale * 1.7f;
    float objW = frameW * objScale;
    float objH = (float)bg->objetos.height * objScale;

    for (int i = 0; i < MAX_OBJETOS; i++)
    {
        if (!bg->objetosList[i].active) continue;
        bg->objetosList[i].x -= FLOOR_SCROLL_SPEED * dt;
        if (bg->objetosList[i].x + objW < 0.0f)
            bg->objetosList[i].active = false;
    }

    bg->objetoSpawnTimer += dt;
    if (bg->objetoSpawnTimer >= bg->objetoSpawnInterval)
    {
        bg->objetoSpawnTimer = 0.0f;
        bg->objetoSpawnInterval = (float)GetRandomValue(150, 300) / 100.0f;

        for (int i = 0; i < MAX_OBJETOS; i++)
        {
            if (bg->objetosList[i].active) continue;

            float waterTopY = groundY + (screenHeight * -0.04f);
            float waterBottomY = (float)screenHeight - objH;
            float spawnY = waterTopY;
            if (waterBottomY > waterTopY)
                spawnY = waterTopY + (float)GetRandomValue(0, (int)(waterBottomY - waterTopY));

            bg->objetosList[i].active = true;
            bg->objetosList[i].x = (float)screenWidth + 10.0f;
            bg->objetosList[i].y = spawnY;
            bg->objetosList[i].frame = GetRandomValue(0, 2);
            break;
        }
    }
}

void DrawObjetos(Background *bg, int screenWidth, int screenHeight, float groundY)
{
    if (bg->objetos.id == 0) return;

    float frameW = (float)bg->objetos.width / 3.0f;
    float frameH = (float)bg->objetos.height;
    float floorScale = (bg->floor.id > 0) ? (float)screenWidth / bg->floor.width : 1.0f;
    float objScale = floorScale * 1.7f;
    float objW = frameW * objScale;
    float objH = frameH * objScale;

    for (int i = 0; i < MAX_OBJETOS; i++)
    {
        if (!bg->objetosList[i].active) continue;
        Rectangle src = { bg->objetosList[i].frame * frameW, 0, frameW, frameH };
        Rectangle dest = { bg->objetosList[i].x, bg->objetosList[i].y, objW, objH };
        DrawTexturePro(bg->objetos, src, dest, (Vector2){ 0, 0 }, 0.0f, WHITE);
    }
}

void DrawStageFront(Background *bg, int screenWidth, int screenHeight)
{
    if (bg->colorFront.id == 0) return;

    Rectangle src = { 0, 0, (float)bg->colorFront.width, (float)bg->colorFront.height };
    Rectangle dest = { 0, 0, (float)screenWidth, (float)screenHeight };

    BeginBlendMode(BLEND_ALPHA);
        DrawTexturePro(bg->colorFront, src, dest, (Vector2){ 0, 0 }, 0.0f, (Color){ 255, 255, 255, COLOR_FRONT_TINT_ALPHA });
    EndBlendMode();
}

static float RainNoise(int index, float salt)
{
    float value = sinf((float)index * 12.9898f + salt * 78.233f) * 43758.5453f;
    return value - floorf(value);
}

void DrawRain(Background *bg, int screenWidth, int screenHeight)
{
    int dropCount = screenWidth / 11;
    if (dropCount < RAIN_MIN_DROPS)
    {
        dropCount = RAIN_MIN_DROPS;
    }
    if (dropCount > RAIN_MAX_DROPS)
    {
        dropCount = RAIN_MAX_DROPS;
    }

    float pad = screenHeight * 0.18f;
    float travelW = (float)screenWidth + pad * 2.0f;
    float travelH = (float)screenHeight + pad * 2.0f;
    Color rainColor = { 78, 100, 185, 150 };

    BeginBlendMode(BLEND_ALPHA);
    for (int i = 0; i < dropCount; i++)
    {
        float depth = RainNoise(i, 0.7f);
        float speed = RAIN_FALL_SPEED * (0.65f + depth * 0.65f);
        float wind = RAIN_WIND_SPEED * (0.75f + depth * 0.5f);
        float length = screenHeight * (0.035f + depth * 0.035f);
        float thickness = 2.0f + depth * 1.8f;

        float baseX = RainNoise(i, 1.3f) * travelW;
        float baseY = RainNoise(i, 2.1f) * travelH;
        float x = fmodf(baseX + bg->time * wind, travelW);
        float y = fmodf(baseY + bg->time * speed, travelH);
        if (x < 0.0f) x += travelW;
        if (y < 0.0f) y += travelH;
        x -= pad;
        y -= pad;

        Vector2 start = { x, y };
        Vector2 end = { x - length * 0.42f, y + length };
        Color tint = rainColor;
        tint.a = (unsigned char)(70 + depth * 85);
        DrawLineEx(start, end, thickness, tint);
    }
    EndBlendMode();
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
    float barDrawW = barW + barW * BAR_FRAME_WIDTH_EXTRA_RATIO;
    float barTextureW = (bg->barFrame.id > 0) ? (float)bg->barFrame.width : 383.0f;
    float trackSrcW = BAR_TRACK_SRC_RIGHT - BAR_TRACK_SRC_X + 1.0f;
    float trackX = barX + barDrawW * (BAR_TRACK_SRC_X / barTextureW);
    float trackW = barDrawW * (trackSrcW / barTextureW);

    Rectangle dest = 
    {
        barX,
        barY,
        barDrawW,
        barH
    };

    if (bg->barBackground.id > 0)
    {
        Rectangle src = 
        {
            BAR_TRACK_SRC_X,
            0,
            trackSrcW,
            (float)bg->barBackground.height
        };

        Rectangle backgroundDest =
        {
            trackX,
            barY,
            trackW,
            barH
        };

        DrawTexturePro(bg->barBackground, src, backgroundDest, (Vector2){0, 0}, 0.0f, WHITE);
    }

    if (bg->barFill.id > 0 && progress > 0.0f)
    {
        float fillSrcW = trackSrcW * progress;
        Rectangle src = 
        {
            BAR_TRACK_SRC_X,
            0,
            fillSrcW,
            (float)bg->barFill.height
        };

        Rectangle fillDest = 
        {
            trackX,
            barY,
            trackW * progress,
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
    UnloadTexture(bg->bueiro);
    UnloadTexture(bg->objetos);
    UnloadTexture(bg->colorFront);
    UnloadTexture(bg->bossHairyLeg);
    UnloadTexture(bg->bossMidnightMan);
    UnloadTexture(bg->bossShark);
    for (int i = 0; i < bg->waterFrameCount; i++)
    {
        UnloadTexture(bg->waterFrames[i]);
    }
    UnloadTexture(bg->waterStatic);
    UnloadTexture(bg->waterSplashSheet);
    UnloadTexture(bg->barFrame);
    UnloadTexture(bg->barBackground);
    UnloadTexture(bg->barFill);
    for (int i = 0; i < 3; i++)
    {
        UnloadTexture(bg->runningPhase2[i]);
        UnloadTexture(bg->runningPhase3[i]);
    }
}
