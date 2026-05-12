#include <raylib.h>
#include "core/screens.h"
#include "utils.h"
#include "core/config_manager.h"
#include "core/cursor.h"
#include "core/window_mode.h"

static const char *GetControlKeyLabel(int key)
{
    switch (key)
    {
        case KEY_A: return "A";
        case KEY_B: return "B";
        case KEY_C: return "C";
        case KEY_D: return "D";
        case KEY_E: return "E";
        case KEY_F: return "F";
        case KEY_G: return "G";
        case KEY_H: return "H";
        case KEY_I: return "I";
        case KEY_J: return "J";
        case KEY_K: return "K";
        case KEY_L: return "L";
        case KEY_M: return "M";
        case KEY_N: return "N";
        case KEY_O: return "O";
        case KEY_P: return "P";
        case KEY_Q: return "Q";
        case KEY_R: return "R";
        case KEY_S: return "S";
        case KEY_T: return "T";
        case KEY_U: return "U";
        case KEY_V: return "V";
        case KEY_W: return "W";
        case KEY_X: return "X";
        case KEY_Y: return "Y";
        case KEY_Z: return "Z";
        case KEY_ZERO: return "0";
        case KEY_ONE: return "1";
        case KEY_TWO: return "2";
        case KEY_THREE: return "3";
        case KEY_FOUR: return "4";
        case KEY_FIVE: return "5";
        case KEY_SIX: return "6";
        case KEY_SEVEN: return "7";
        case KEY_EIGHT: return "8";
        case KEY_NINE: return "9";
        case KEY_SPACE: return "ESPACO";
        case KEY_UP: return "CIMA";
        case KEY_DOWN: return "BAIXO";
        case KEY_LEFT: return "ESQ";
        case KEY_RIGHT: return "DIR";
        default: return "?";
    }
}

static int GetFittedFontSize(const char *text, int preferredSize, int maxWidth, int minSize)
{
    int fontSize = preferredSize;
    while (fontSize > minSize && MeasureText(text, fontSize) > maxWidth)
    {
        fontSize--;
    }
    return fontSize;
}

GameScreen RunOptions(Config *config)
{
    bool draggingSom = false;
    bool draggingMusica = false;
    bool aceitarClique = false;
    int controleSelecionado = -1;

    while (!WindowShouldClose())
    {
        int currentWidth = GetScreenWidth();
        int currentHeight = GetScreenHeight();

        int labelFontSize = currentHeight / 16;
        int smallFontSize = currentHeight / 31;
        float centerX = currentWidth / 2.0f;

        int titleFontSize = currentHeight / 13;
        int titleY = currentHeight / 28;
        int contentShiftY = currentHeight / 28;
        int somLabelY = titleY + titleFontSize + currentHeight / 18 + contentShiftY;
        Rectangle barSom =
        {
            centerX - 180.0f,
            somLabelY + labelFontSize + 18.0f,
            360.0f,
            24.0f
        };

        int musicaLabelY = (int)barSom.y + 74;
        Rectangle barMusica =
        {
            centerX - 180.0f,
            musicaLabelY + labelFontSize + 18.0f,
            360.0f,
            24.0f
        };

        int telaLabelY = (int)barMusica.y + 84;
        float screenButtonWidth = currentWidth * 0.34f;
        if (screenButtonWidth < 220.0f) screenButtonWidth = 220.0f;
        if (screenButtonWidth > 320.0f) screenButtonWidth = 320.0f;

        Rectangle btnScreen =
        {
            centerX - screenButtonWidth / 2.0f,
            telaLabelY + labelFontSize + 16.0f,
            screenButtonWidth,
            58.0f
        };
        Rectangle btnScreenHitbox = btnScreen;
        Rectangle leftArrowHitbox =
        {
            btnScreen.x - 58.0f,
            btnScreen.y,
            44.0f,
            btnScreen.height
        };
        Rectangle rightArrowHitbox =
        {
            btnScreen.x + btnScreen.width + 14.0f,
            btnScreen.y,
            44.0f,
            btnScreen.height
        };

        int controlesY = (int)btnScreen.y + (int)btnScreen.height + 68;
        float boxSize = 108.0f;
        float boxGap = 22.0f;
        float totalBoxesWidth = boxSize * 4.0f + boxGap * 3.0f;
        float boxesStartX = centerX - totalBoxesWidth / 2.0f;
        Rectangle controlBoxes[4] =
        {
            { boxesStartX, controlesY + labelFontSize + 18.0f, boxSize, boxSize },
            { boxesStartX + boxSize + boxGap, controlesY + labelFontSize + 18.0f, boxSize, boxSize },
            { boxesStartX + (boxSize + boxGap) * 2.0f, controlesY + labelFontSize + 18.0f, boxSize, boxSize },
            { boxesStartX + (boxSize + boxGap) * 3.0f, controlesY + labelFontSize + 18.0f, boxSize, boxSize }
        };
        Rectangle backButton = { 28.0f, 26.0f, 70.0f, 54.0f };
        int *teclas[4] =
        {
            &config->teclaPular,
            &config->teclaTras,
            &config->teclaAgachar,
            &config->teclaFrente
        };

        Vector2 mouse = GetMousePosition();
        if (!aceitarClique && !IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            aceitarClique = true;
        }

        bool hoveringScreenToggle =
            CheckCollisionPointRec(mouse, btnScreenHitbox) ||
            CheckCollisionPointRec(mouse, leftArrowHitbox) ||
            CheckCollisionPointRec(mouse, rightArrowHitbox);
        bool hoveringBack = CheckCollisionPointRec(mouse, backButton);
        bool hoveringControl = false;
        for (int i = 0; i < 4; i++)
        {
            if (CheckCollisionPointRec(mouse, controlBoxes[i]))
            {
                hoveringControl = true;
            }
        }
        bool hoveringInteractive = hoveringScreenToggle || hoveringBack || hoveringControl;

        if (aceitarClique && hoveringScreenToggle && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            bool fullscreen = config->telaCheia != 0;
            ToggleWindowMode(&fullscreen);
            config->telaCheia = fullscreen ? 1 : 0;
            SalvarConfig(*config);
        }

        if (aceitarClique && hoveringBack && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            return SCREEN_START;
        }

        if (aceitarClique)
        {
            config->volume = UpdateSlider(barSom, config->volume, &draggingSom);
            config->musica = UpdateSlider(barMusica, config->musica, &draggingMusica);
        }

        for (int i = 0; i < 4; i++)
        {
            if (aceitarClique && CheckCollisionPointRec(mouse, controlBoxes[i]) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                controleSelecionado = i;
            }
        }

        if (controleSelecionado >= 0)
        {
            int teclaPressionada = GetKeyPressed();
            if (teclaPressionada != 0)
            {
                *teclas[controleSelecionado] = teclaPressionada;
                controleSelecionado = -1;
                SalvarConfig(*config);
            }
        }

        if (IsKeyPressed(KEY_ESCAPE))
        {
            return SCREEN_START;
        }
        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawText("<", 40, 18, 58, hoveringBack ? RED : DARKGRAY);

            const char *titleLabel = "Opções";
            DrawText(
                titleLabel,
                (int)(centerX - MeasureText(titleLabel, titleFontSize) / 2),
                titleY,
                titleFontSize,
                DARKBLUE
            );

            const char *somLabel = "SOM";
            DrawText(somLabel, (int)(centerX - MeasureText(somLabel, labelFontSize) / 2), somLabelY, labelFontSize, DARKGRAY);
            DrawSlider(barSom, config->volume);
            DrawText(TextFormat("%d%%", (int)(config->volume * 100 + 0.5f)), (int)barSom.x + (int)barSom.width + 18, (int)barSom.y - 1, smallFontSize, DARKGRAY);

            const char *musicaLabel = "MUSICA";
            DrawText(musicaLabel, (int)(centerX - MeasureText(musicaLabel, labelFontSize) / 2), musicaLabelY, labelFontSize, DARKGRAY);
            DrawSlider(barMusica, config->musica);
            DrawText(TextFormat("%d%%", (int)(config->musica * 100 + 0.5f)), (int)barMusica.x + (int)barMusica.width + 18, (int)barMusica.y - 1, smallFontSize, DARKGRAY);

            const char *telaLabel = "TELA";
            DrawText(telaLabel, (int)(centerX - MeasureText(telaLabel, labelFontSize) / 2), telaLabelY, labelFontSize, DARKGRAY);
            DrawRectangleRec(btnScreen, WHITE);
            DrawRectangleLinesEx(btnScreen, 3.0f, hoveringScreenToggle ? RED : DARKGRAY);
            DrawText("<", (int)leftArrowHitbox.x + 8, (int)leftArrowHitbox.y + 2, 46, hoveringScreenToggle ? RED : DARKGRAY);
            DrawText(">", (int)rightArrowHitbox.x + 8, (int)rightArrowHitbox.y + 2, 46, hoveringScreenToggle ? RED : DARKGRAY);
            const char *screenLabel = config->telaCheia ? "FULLSCREEN" : "JANELA";
            int screenFontSize = GetFittedFontSize(screenLabel, smallFontSize + 4, (int)btnScreen.width - 28, 14);
            DrawText(
                screenLabel,
                (int)(btnScreen.x + (btnScreen.width - MeasureText(screenLabel, screenFontSize)) / 2.0f),
                (int)(btnScreen.y + (btnScreen.height - screenFontSize) / 2.0f),
                screenFontSize,
                DARKGRAY
            );

            const char *controlesLabel = "CONTROLES";
            DrawText(controlesLabel, (int)(centerX - MeasureText(controlesLabel, labelFontSize) / 2), controlesY, labelFontSize, DARKGRAY);
            for (int i = 0; i < 4; i++)
            {
                bool hoverBox = CheckCollisionPointRec(mouse, controlBoxes[i]);
                Color borderColor = LIGHTGRAY;
                if (hoverBox)
                {
                    borderColor = RED;
                }
                if (controleSelecionado == i)
                {
                    borderColor = BLUE;
                }

                DrawRectangleRec(controlBoxes[i], WHITE);
                DrawRectangleLinesEx(controlBoxes[i], 3.0f, borderColor);

                const char *teclaAtual = GetControlKeyLabel(*teclas[i]);
                int keyFontSize = smallFontSize + 3;
                int keyTextWidth = MeasureText(teclaAtual, keyFontSize);
                int keyTextX = (int)(controlBoxes[i].x + (controlBoxes[i].width - keyTextWidth) / 2.0f);
                int keyTextY = (int)(controlBoxes[i].y + controlBoxes[i].height + 12.0f);
                DrawText(teclaAtual, keyTextX, keyTextY, keyFontSize, DARKGRAY);
            }

            DrawMenuCursor(hoveringInteractive);
        EndDrawing();
    }
    return SCREEN_EXIT;
}
