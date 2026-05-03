#include <raylib.h>
#include "background.h"
#include "player.h"

void InitBackground(Background *bg)
{
    bg->time = 0.0f;
    bg->floor = LoadTexture("assets/sprites/background/Floor.png");
}

void UpdateBackground(Background *bg, float dt)
{
    bg->time += dt;
}

void DrawBackground(Background *bg, int screenWidth, int screenHeight, float groundY)
{
    
    int tileSize = 40;
    for (int y = 0; y < (int)groundY; y += tileSize)
    {
        for (int x = 0; x < screenWidth; x += tileSize)
        {
            
            Color c1 = (Color){ 55, 50, 65, 255 };
            Color c2 = (Color){ 45, 40, 55, 255 };
            Color c = ((x / tileSize + y / tileSize) % 2 == 0) ? c1 : c2;
            
            int drawH = (y + tileSize > groundY) ? (int)(groundY - y) : tileSize;
            DrawRectangle(x, y, tileSize, drawH, c);
        }
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
}
