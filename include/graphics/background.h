#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <raylib.h>
#include "sprites.h"

typedef enum
{
    PHASE_RUNNING,
    PHASE_BOSS
} GamePhase;

typedef struct {
    float time;
    float scrollX;
    Texture2D floor;
    Texture2D bossHairyLeg;
    Texture2D bossMidnightMan;
    Texture2D waterFrames[38];
    int waterFrameCount;
    int waterCurrentFrame;
    float waterFrameTimer;
    float waterFrameTime;
    Texture2D waterStatic;
    float waterScrollX;
    Texture2D barFrame;
    Texture2D barBackground;
    Texture2D barFill;
} Background;

void InitBackground(Background *bg);
void UpdateBackground(Background *bg, float dt, GamePhase phase);
void DrawBackground(Background *bg, int levelId, float level6IntroProgress, int screenWidth, int screenHeight, float groundY);
void DrawProgressBar(Background *bg, float progress, int screenWidth, int screenHeight);
void UnloadBackground(Background *bg);

#endif
