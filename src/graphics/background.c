#include <raylib.h>
#include "graphics/background.h"
#include "entities/player.h"

void InitBackground(Background *bg)
{
    bg->time = 0.0f;
    bg->floor = LoadTexture("assets/sprites/background/Floor.png");
    bg->bossHairyLeg = LoadTexture("assets/sprites/fundo/Background_1Boss.png");
}

void UpdateBackground(Background *bg, float dt)
{
    bg->time += dt;
}

void DrawBackground(Background *bg, int screenWidth, int screenHeight, float groundY)
{
    if(bg->bossHairyLeg.id > 0){
        Rectangle sourceRec = { 0.0f, 0.0f, (float)bg->bossHairyLeg.width, (float)bg->bossHairyLeg.height };
        Rectangle destRec = { 0.0f, 0.0f, (float)screenWidth, (float)screenHeight };
        Vector2 origin = { 0.0f, 0.0f };

        DrawTexturePro(bg->bossHairyLeg, sourceRec, destRec, origin, 0.0f, WHITE);
    }


    if (bg->floor.id > 0)
    {
        float scale = (float)screenWidth / bg->floor.width;
        float visiblePartTopOffset = bg->floor.height * 0.85f * scale;

        Vector2 pos = { 0, groundY - visiblePartTopOffset };
        DrawTextureEx(bg->floor, pos, 0.0f, scale, WHITE);
    }

    DrawRectangleGradientV(0, 0, screenWidth, screenHeight / 4, (Color){ 0, 0, 0, 180 }, (Color){ 0, 0, 0, 0 });
    DrawRectangleGradientV(0, screenHeight * 3 / 4, screenWidth, screenHeight / 4, (Color){ 0, 0, 0, 0 }, (Color){ 0, 0, 0, 180 });
}

void UnloadBackground(Background *bg)
{
    UnloadTexture(bg->floor);
    UnloadTexture(bg->bossHairyLeg);
}
