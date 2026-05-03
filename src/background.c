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
    // 1. Fundo (Parede com Checkerboard)
    int tileSize = 40;
    for (int y = 0; y < (int)groundY; y += tileSize)
    {
        for (int x = 0; x < screenWidth; x += tileSize)
        {
            // Cores baseadas no print (roxo escuro / cinza azulado)
            Color c1 = (Color){ 55, 50, 65, 255 };
            Color c2 = (Color){ 45, 40, 55, 255 };
            Color c = ((x / tileSize + y / tileSize) % 2 == 0) ? c1 : c2;
            
            int drawH = (y + tileSize > groundY) ? (int)(groundY - y) : tileSize;
            DrawRectangle(x, y, tileSize, drawH, c);
        }
    }

    // 2. Calçada e Água (Floor.png)
    if (bg->floor.id > 0)
    {
        float scale = (float)screenWidth / bg->floor.width;
        
        // Queremos que a parte cinza (o chão) coincida com a posição dos personagens.
        // Aumentando o offset para subir a textura na tela.
        float visiblePartTopOffset = bg->floor.height * 0.85f * scale;
        
        Vector2 pos = { 0, groundY - visiblePartTopOffset };
        DrawTextureEx(bg->floor, pos, 0.0f, scale, WHITE);
    }

    // 6. Camada de "Escuridão" (Vignette simples procedural)
    // Para manter o clima sombrio sem usar sprite
    DrawRectangleGradientV(0, 0, screenWidth, screenHeight / 4, (Color){ 0, 0, 0, 180 }, (Color){ 0, 0, 0, 0 });
    DrawRectangleGradientV(0, screenHeight * 3 / 4, screenWidth, screenHeight / 4, (Color){ 0, 0, 0, 0 }, (Color){ 0, 0, 0, 180 });
}

void UnloadBackground(Background *bg)
{
    UnloadTexture(bg->floor);
}
