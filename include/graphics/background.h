#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <raylib.h>

typedef struct {
    float time;
    Texture2D floor;
    Texture2D bossHairyLeg;
} Background;

void InitBackground(Background *bg);
void UpdateBackground(Background *bg, float dt);
void DrawBackground(Background *bg, int screenWidth, int screenHeight, float groundY);
void UnloadBackground(Background *bg);

#endif
