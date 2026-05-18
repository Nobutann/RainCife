#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <raylib.h>
#include "sprites.h"

#define MAX_OBJETOS 10

typedef struct {
    float x, y;
    int frame;
    bool active;
} ObjetoInstance;

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
    Texture2D bossShark;
    Animation water;
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
    Texture2D runningPhase2[3];
    Texture2D runningPhase3[3];
    float runningBgScrollX;
    Texture2D bueiro;
    Texture2D objetos;
    ObjetoInstance objetosList[MAX_OBJETOS];
    float objetoSpawnTimer;
    float objetoSpawnInterval;
} Background;

void InitBackground(Background *bg);
void UpdateBackground(Background *bg, float dt, GamePhase phase);
void UpdateObjetos(Background *bg, float dt, int screenWidth, int screenHeight, float groundY, GamePhase phase);
void DrawBackground(Background *bg, int levelId, int bossId, float level6IntroProgress, int screenWidth, int screenHeight, float groundY, GamePhase phase);
void DrawWater(Background *bg, int screenWidth, int screenHeight, float groundY);
void DrawObjetos(Background *bg, int screenWidth, int screenHeight, float groundY);
void DrawProgressBar(Background *bg, float progress, int screenWidth, int screenHeight);
void UnloadBackground(Background *bg);

#endif
