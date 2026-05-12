#include <raylib.h>
#include "core/screens.h"
#include "core/cursor.h"

GameScreen RunCredits()
{
    while (!WindowShouldClose())
    {
        int currentHeight = GetScreenHeight();
        int currentWidth = GetScreenWidth();

        if (IsKeyPressed(KEY_ESCAPE))
        {
            return SCREEN_START;
        }
    
        BeginDrawing();
            ClearBackground(RAYWHITE);
            const char* title = "CRÉDITOS";
            int titleFontSize = currentHeight / 10;
            int titleWidth = MeasureText(title, titleFontSize);
            DrawText(title, (currentWidth / 2) - (titleWidth / 2), currentHeight / 8, titleFontSize, DARKBLUE);
    
            int nameFontSize = currentHeight / 25;
            int startY = currentHeight / 3;
            int spacing = nameFontSize + 15;
    
            const char* nomes[] = 
            { 
                "Bruno Augusto", 
                "Gustavo Castro", 
                "Igor Gabriel", 
                "Luiz Gonzaga", 
                "Marco Aurélio" 
            };
            
            for (int i = 0; i < 5; i++)
            {
                int textWidth = MeasureText(nomes[i], nameFontSize);
                DrawText(nomes[i], (currentWidth / 2) - (textWidth / 2), startY + (i * spacing), nameFontSize, DARKGRAY);
            }
            DrawMenuCursor(false);
        EndDrawing();
    }

    return SCREEN_EXIT;
}
