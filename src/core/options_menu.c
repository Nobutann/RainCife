#include <raylib.h>
#include "core/screens.h"
#include "utils.h"
#include "core/config_manager.h"
#include "core/cursor.h"
#include "core/sounds.h"

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
        case KEY_SPACE: return "ESP";
        case KEY_UP: return "CIMA";
        case KEY_DOWN: return "BAIXO";
        case KEY_LEFT: return "ESQ";
        case KEY_RIGHT: return "DIR";
        default: return "?";
    }
}

static void UnloadOptionsTextures(Texture2D blueBackground, Texture2D optionsScreen, Texture2D volumeBar, Texture2D backButton, Texture2D backHover)
{
    UnloadTexture(blueBackground);
    UnloadTexture(optionsScreen);
    UnloadTexture(volumeBar);
    UnloadTexture(backButton);
    UnloadTexture(backHover);
}

static Rectangle GetBasePoint(float x, float y, int screenWidth, int screenHeight)
{
    return ScaleUiRect(x, y, 1.0f, 1.0f, screenWidth, screenHeight);
}

static void DrawVolumeFill(Texture2D volumeBar, float value, float baseY, int screenWidth, int screenHeight)
{
    float width = 187.0f * value;
    if (width <= 0.0f)
    {
        return;
    }

    DrawTexturePro(
        volumeBar,
        (Rectangle){213.0f, 147.0f, width, 12.0f},
        ScaleUiRect(222.0f, baseY, width, 12.0f, screenWidth, screenHeight),
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE
    );
}

GameScreen RunOptions(Config *config, GameScreen returnScreen)
{
    bool draggingSom = false;
    bool draggingMusica = false;
    bool aceitarClique = false;
    int controleSelecionado = -1;
    Texture2D blueBackground = LoadTexture("assets/sprites/background/fundo_azul.png");
    Texture2D optionsScreen = LoadTexture("assets/sprites/ui/options/options_screen.png");
    Texture2D volumeBar = LoadTexture("assets/sprites/ui/options/volume_bar.png");
    Texture2D backButtonTexture = LoadTexture("assets/sprites/ui/options/back.png");
    Texture2D backHoverTexture = LoadTexture("assets/sprites/ui/options/back_hover.png");

    while (!WindowShouldClose())
    {
        AtualizarPreferenciaEspaco();
        UpdateSoundtrack();
        int currentWidth = GetScreenWidth();
        int currentHeight = GetScreenHeight();
        int smallFontSize = currentHeight / 31;
        Vector2 mouse = GetMousePosition();

        Rectangle barSom = ScaleUiRect(213.0f, 94.0f, 200.0f, 22.0f, currentWidth, currentHeight);
        Rectangle barMusica = ScaleUiRect(213.0f, 147.0f, 200.0f, 22.0f, currentWidth, currentHeight);
        Rectangle backButton = ScaleUiRect(7.0f, 8.0f, 24.0f, 30.0f, currentWidth, currentHeight);
        Rectangle controlBoxes[4] =
        {
            ScaleUiRect(213.0f, 208.0f, 55.0f, 82.0f, currentWidth, currentHeight),
            ScaleUiRect(269.0f, 208.0f, 55.0f, 82.0f, currentWidth, currentHeight),
            ScaleUiRect(324.0f, 208.0f, 55.0f, 82.0f, currentWidth, currentHeight),
            ScaleUiRect(380.0f, 208.0f, 55.0f, 82.0f, currentWidth, currentHeight)
        };
        int *teclas[4] =
        {
            &config->teclaPular,
            &config->teclaTras,
            &config->teclaFrente,
            &config->teclaAgachar
        };

        if (!aceitarClique && !IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            aceitarClique = true;
        }

        bool hoveringBack = CheckCollisionPointRec(mouse, backButton);
        bool hoveringControl = false;
        for (int i = 0; i < 4; i++)
        {
            if (CheckCollisionPointRec(mouse, controlBoxes[i]))
            {
                hoveringControl = true;
            }
        }
        bool hoveringSlider = CheckCollisionPointRec(mouse, barSom) || CheckCollisionPointRec(mouse, barMusica);
        bool hoveringInteractive = hoveringBack || hoveringControl || hoveringSlider;

        if (aceitarClique && hoveringBack && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            SalvarConfig(*config);
            UnloadOptionsTextures(blueBackground, optionsScreen, volumeBar, backButtonTexture, backHoverTexture);
            return returnScreen;
        }

        if (aceitarClique)
        {
            float prevVolume = config->volume;
            float prevMusic = config->musica;
            config->volume = UpdateSlider(barSom, config->volume, &draggingSom);
            config->musica = UpdateSlider(barMusica, config->musica, &draggingMusica);
            if (config->volume != prevVolume)
            {
                SetSoundSystemVolume(config->volume);
            }
            if (config->musica != prevMusic)
            {
                SetMusicSystemVolume(config->musica);
            }
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
            SalvarConfig(*config);
            UnloadOptionsTextures(blueBackground, optionsScreen, volumeBar, backButtonTexture, backHoverTexture);
            return returnScreen;
        }

        BeginDrawing();
            ClearBackground((Color){43, 56, 106, 255});
            DrawFullscreenTexture(blueBackground, currentWidth, currentHeight);
            DrawFullscreenTexture(optionsScreen, currentWidth, currentHeight);
            DrawVolumeFill(volumeBar, config->volume, 99.0f, currentWidth, currentHeight);
            DrawVolumeFill(volumeBar, config->musica, 153.0f, currentWidth, currentHeight);
            DrawFullscreenTexture(hoveringBack ? backHoverTexture : backButtonTexture, currentWidth, currentHeight);

            Rectangle soundPercentPos = GetBasePoint(386.0f, 99.0f, currentWidth, currentHeight);
            Rectangle musicPercentPos = GetBasePoint(386.0f, 152.0f, currentWidth, currentHeight);
            DrawText(TextFormat("%d%%", (int)(config->volume * 100 + 0.5f)), (int)soundPercentPos.x, (int)soundPercentPos.y, smallFontSize, DARKGRAY);
            DrawText(TextFormat("%d%%", (int)(config->musica * 100 + 0.5f)), (int)musicPercentPos.x, (int)musicPercentPos.y, smallFontSize, DARKGRAY);

            for (int i = 0; i < 4; i++)
            {
                const char *teclaAtual = GetControlKeyLabel(*teclas[i]);
                int keyFontSize = smallFontSize + 3;
                Rectangle keySlots[4] =
                {
                    ScaleUiRect(216.0f, 250.0f, 39.0f, 35.0f, currentWidth, currentHeight),
                    ScaleUiRect(271.0f, 250.0f, 39.0f, 35.0f, currentWidth, currentHeight),
                    ScaleUiRect(325.0f, 250.0f, 39.0f, 35.0f, currentWidth, currentHeight),
                    ScaleUiRect(380.0f, 250.0f, 38.0f, 35.0f, currentWidth, currentHeight)
                };
                int keyTextWidth = MeasureText(teclaAtual, keyFontSize);
                int keyTextX = (int)(keySlots[i].x + (keySlots[i].width - keyTextWidth) / 2.0f);
                int keyTextY = (int)(keySlots[i].y + (keySlots[i].height - keyFontSize) / 2.0f);
                DrawText(teclaAtual, keyTextX, keyTextY, keyFontSize, DARKGRAY);

            }

            DrawMenuCursor(hoveringInteractive);
        EndDrawing();
    }

    UnloadOptionsTextures(blueBackground, optionsScreen, volumeBar, backButtonTexture, backHoverTexture);
    return SCREEN_EXIT;
}
