#include <raylib.h>
#include "menu_opcoes.h"

void DesenharMenuOpcoes(int currentWidth, int currentHeight, Config *config) {
    ClearBackground(RAYWHITE);
    Vector2 mouse = GetMousePosition();

    DrawText("OPÇÕES", currentWidth / 2 - MeasureText("OPÇÕES", 40) / 2, currentHeight / 8, 40, DARKBLUE);

    int startY = currentHeight / 3;
    int spacing = currentHeight / 8;

    DrawText("Idioma:", currentWidth / 4, startY, 25, DARKGRAY);
    Rectangle btnIdioma = { (float)currentWidth / 2, (float)startY - 5, 100, 30 };
    DrawRectangleRec(btnIdioma, config->idioma ? BLUE : GREEN);
    DrawText(config->idioma ? "EN" : "PT-BR", (int)btnIdioma.x + 10, (int)btnIdioma.y + 5, 20, WHITE);
    if (CheckCollisionPointRec(mouse, btnIdioma) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        config->idioma = !config->idioma;
        SalvarConfig(*config);
    }

    DrawText("Tela Cheia:", currentWidth / 4, startY + spacing, 25, DARKGRAY);
    Rectangle btnTela = { (float)currentWidth / 2, (float)startY + spacing - 5, 100, 30 };
    DrawRectangleRec(btnTela, config->telaCheia ? GREEN : RED);
    DrawText(config->telaCheia ? "ON" : "OFF", (int)btnTela.x + 10, (int)btnTela.y + 5, 20, WHITE);
    if (CheckCollisionPointRec(mouse, btnTela) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        config->telaCheia = !config->telaCheia;
        ToggleFullscreen();
        SalvarConfig(*config);
    }

    DrawText("Volume:", currentWidth / 4, startY + spacing * 2, 25, DARKGRAY);
    Rectangle barraTotal = { (float)currentWidth / 2, (float)startY + spacing * 2 + 5, 200, 25 };
    Rectangle barraVolume = { barraTotal.x, barraTotal.y, barraTotal.width * config->volume, barraTotal.height };
    DrawRectangleRec(barraTotal, LIGHTGRAY);
    DrawRectangleRec(barraVolume, BLUE);
    
    if (CheckCollisionPointRec(mouse, barraTotal) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        config->volume = (mouse.x - barraTotal.x) / barraTotal.width;
        if (config->volume < 0.0f) config->volume = 0.0f;
        if (config->volume > 1.0f) config->volume = 1.0f;
        SalvarConfig(*config);
    }
    DrawText(TextFormat("%d%%", (int)(config->volume * 100 + 0.5f)), (int)barraTotal.x + (int)barraTotal.width + 15, (int)barraTotal.y + 2, 20, DARKGRAY);
}
