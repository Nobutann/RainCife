#include <raylib.h>
#include "core/screens.h"
#include "utils.h"
#include "core/config_manager.h"

GameScreen RunOptions(Config *config)
{
    while (!WindowShouldClose())
    {
        int currentWidth = GetScreenWidth();
        int currentHeight = GetScreenHeight();

        int startY = currentHeight / 3;
        int spacing = currentHeight / 8;
        int fontSize = currentHeight / 25;

        Rectangle btnLanguage = { (float)currentWidth / 2, (float)startY - 5, 100, 30 };
        Rectangle btnScreen = { (float)currentWidth / 2, (float)startY + spacing - 5, 100, 30 };
        Rectangle barVolume = { (float)currentWidth / 2, (float)startY + spacing * 2 + 5, 200, 25 };

        Vector2 mouse = GetMousePosition();

        if (CheckCollisionPointRec(mouse, btnLanguage) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            config->idioma = !config->idioma;
            SalvarConfig(*config);
        }
        if (CheckCollisionPointRec(mouse, btnScreen) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            config->telaCheia = !config->telaCheia;
            ToggleFullscreen();
            SalvarConfig(*config);
        }

        config->volume = UpdateSlider(barVolume, config->volume);

        if (IsKeyPressed(KEY_ESCAPE))
        {
            return SCREEN_START;
        }
        BeginDrawing();
            ClearBackground(RAYWHITE);

            int titleSize = currentHeight / 10;
            const char *titulo = "OPCOES";
            DrawText(titulo, (currentWidth / 2) - (MeasureText(titulo, titleSize) / 2), currentHeight / 6, titleSize, DARKBLUE);

            DrawText("Idioma:", currentWidth / 4, startY, fontSize, DARKGRAY);
            DrawRectangleRec(btnLanguage, config->idioma ? BLUE : GREEN);
            DrawText(config->idioma ? "EN" : "PT-BR", (int)btnLanguage.x + 10, (int)btnLanguage.y + 5, 20, WHITE);

            DrawText("Tela Cheia:", currentWidth / 4, startY + spacing, fontSize, DARKGRAY);
            DrawRectangleRec(btnScreen, config->telaCheia ? GREEN : RED);
            DrawText(config->telaCheia ? "ON" : "OFF", (int)btnScreen.x + 10, (int)btnScreen.y + 5, 20, WHITE);

            DrawText("Volume:", currentWidth / 4, startY + spacing * 2, fontSize, DARKGRAY);
            DrawSlider(barVolume, config->volume);
            DrawText(TextFormat("%d%%", (int)(config->volume * 100 + 0.5f)), (int)barVolume.x + (int)barVolume.width + 15, (int)barVolume.y + 2, 20, DARKGRAY);

        EndDrawing();
    }
    return SCREEN_EXIT;
}