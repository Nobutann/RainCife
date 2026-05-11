#include <raylib.h>
#include "core/screens.h"
#include "utils.h"
#include "core/cursor.h"

GameScreen RunStart()
{
    const char* optionsPT[] = 
    {
        "Jogar",
        "Opções",
        "Créditos",
        "Sair"
    };

    const char optionsEN[4][8] = 
    {
        "Play",
        "Options",
        "Credits",
        "Exit"
    };

    while (!WindowShouldClose())
    {
        int currentWidth = GetScreenWidth();
        int currentHeight = GetScreenHeight();

        int startY = currentHeight / 2.5;
        int menuFontSize = currentHeight / 20;
        int spacing = menuFontSize + 20;

        int optionsCount = sizeof(optionsPT) / sizeof(optionsPT[0]);
        Rectangle optionsRects[optionsCount];
        BuildOptionRects(optionsRects, optionsPT, optionsCount, menuFontSize, currentWidth / 2, startY, spacing);
        Vector2 mouse = GetMousePosition();
        bool hoveringButton = false;

        for (int i = 0; i < optionsCount; i++)
        {
            if (CheckCollisionPointRec(mouse, optionsRects[i]))
            {
                hoveringButton = true;
                break;
            }
        }

        int clicked = GetClickedOption(optionsRects, optionsCount);
        if (clicked >= 0)
        {
            if (clicked == 0)
            {
                return SCREEN_GAME;
            }
            if (clicked == 1)
            {
                return SCREEN_OPTIONS; 
            }
            if (clicked == 2)
            {
                return SCREEN_CREDITS;
            }
            if (clicked == 3)
            {
                return SCREEN_EXIT;
            }
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            const char* title = "RatTsunami";
            int titleSize = currentHeight / 10;
            DrawText(title, (currentWidth / 2) - (MeasureText(title, titleSize) / 2), currentHeight / 6, titleSize, DARKBLUE);

            for (int i = 0; i < optionsCount; i++)
            {
                bool hover = CheckCollisionPointRec(mouse, optionsRects[i]);
                Color color = hover ? YELLOW : DARKGRAY;
                DrawText(optionsPT[i], optionsRects[i].x, optionsRects[i].y, menuFontSize, color);
            }
            DrawMenuCursor(hoveringButton);
        EndDrawing();
    }

    return SCREEN_EXIT;
}
