#ifndef BACKGROUND_H
#define BACKGROUND_H

#define BG_GROUND_RATIO 0.9f

#include <raylib.h>

typedef struct {
    float time;
    Texture2D floor;
} Background;

void InitBackground(Background *bg);
void UpdateBackground(Background *bg, float dt);
void DrawBackground(Background *bg, int screenWidth, int screenHeight, float groundY);
void UnloadBackground(Background *bg);

#endif
