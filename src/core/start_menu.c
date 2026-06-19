#include <raylib.h>
#include "core/screens.h"
#include "utils.h"
#include "core/cursor.h"
#include "core/sounds.h"

#define MENU_CANVAS_WIDTH 640.0f
#define MENU_CANVAS_HEIGHT 360.0f

static Rectangle GetMenuCanvasRect(int screenWidth, int screenHeight)
{
    return (Rectangle)
    {
        0.0f,
        0.0f,
        (float)screenWidth,
        (float)screenHeight
    };
}

static Vector2 GetMenuCanvasMouse(Rectangle canvasRect)
{
    Vector2 mouse = GetMousePosition();
    return (Vector2)
    {
        (mouse.x - canvasRect.x) * MENU_CANVAS_WIDTH / canvasRect.width,
        (mouse.y - canvasRect.y) * MENU_CANVAS_HEIGHT / canvasRect.height
    };
}

static void DrawTextureSourceCentered(Texture2D texture, Rectangle source, float centerX, float y, Color tint)
{
    Rectangle dest =
    {
        centerX - source.width * 0.5f,
        y,
        source.width,
        source.height
    };
    DrawTexturePro(texture, source, dest, (Vector2){0.0f, 0.0f}, 0.0f, tint);
}

GameScreen RunStart()
{
    Texture2D titleScreen = LoadTexture("assets/sprites/ui/title/title_screen.png");
    Texture2D storyLabel = LoadTexture("assets/sprites/ui/main_menu/story_mode.png");
    Texture2D infiniteLabel = LoadTexture("assets/sprites/ui/main_menu/infinite_mode.png");
    Texture2D optionsLabel = LoadTexture("assets/sprites/ui/main_menu/options.png");
    Texture2D exitLabel = LoadTexture("assets/sprites/ui/main_menu/sair.png");
    RenderTexture2D menuCanvas = LoadRenderTexture((int)MENU_CANVAS_WIDTH, (int)MENU_CANVAS_HEIGHT);

    Rectangle optionRects[] =
    {
        {248.0f, 169.0f, 144.0f, 28.0f},
        {248.0f, 211.0f, 144.0f, 28.0f},
        {248.0f, 253.0f, 144.0f, 28.0f},
        {290.0f, 292.0f, 50.0f, 26.0f}
    };
    int optionCount = sizeof(optionRects) / sizeof(optionRects[0]);

    while (!WindowShouldClose())
    {
        AtualizarPreferenciaEspaco();
        UpdateSoundtrack();
        int currentWidth = GetScreenWidth();
        int currentHeight = GetScreenHeight();
        Rectangle canvasRect = GetMenuCanvasRect(currentWidth, currentHeight);
        Vector2 mouse = GetMenuCanvasMouse(canvasRect);
        bool hoveringButton = false;
        int hovered = -1;

        for (int i = 0; i < optionCount; i++)
        {
            if (CheckCollisionPointRec(mouse, optionRects[i]))
            {
                hoveringButton = true;
                hovered = i;
                break;
            }
        }

        int clicked = -1;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            for (int i = 0; i < optionCount; i++)
            {
                if (CheckCollisionPointRec(mouse, optionRects[i]))
                {
                    clicked = i;
                    break;
                }
            }
        }

        // ENTER key anywhere on the start menu launches the history animation
        // and returns to the start menu afterwards.
        if (IsKeyPressed(KEY_ENTER))
        {
            UnloadRenderTexture(menuCanvas);
            UnloadTexture(titleScreen);
            UnloadTexture(storyLabel);
            UnloadTexture(infiniteLabel);
            UnloadTexture(optionsLabel);
            UnloadTexture(exitLabel);
            SetHistoryAnimationReturnScreen(SCREEN_START);
            return SCREEN_HISTORY_ANIMATION;
        }

        if (clicked >= 0)
        {
            UnloadRenderTexture(menuCanvas);
            UnloadTexture(titleScreen);
            UnloadTexture(storyLabel);
            UnloadTexture(infiniteLabel);
            UnloadTexture(optionsLabel);
            UnloadTexture(exitLabel);
            if (clicked == 0)
            {
                SetCharacterSelectNextScreen(SCREEN_LEVEL_SELECT);
                return SCREEN_CHARACTER_SELECT;
            }
            if (clicked == 1)
            {
                SetCharacterSelectNextScreen(SCREEN_INFINITE_MENU);
                return SCREEN_CHARACTER_SELECT;
            }
            if (clicked == 2)
            {
                return SCREEN_OPTIONS;
            }
            if (clicked == 3)
            {
                return SCREEN_EXIT;
            }
        }

        BeginTextureMode(menuCanvas);
            ClearBackground((Color){43, 56, 106, 255});
            DrawTexture(titleScreen, 0, 0, WHITE);
            DrawTextureSourceCentered(
                storyLabel,
                (Rectangle){272.0f, 113.0f, 143.0f, 31.0f},
                MENU_CANVAS_WIDTH * 0.5f,
                optionRects[0].y - 2.0f,
                hovered == 0 ? YELLOW : WHITE
            );
            DrawTextureSourceCentered(
                infiniteLabel,
                (Rectangle){277.0f, 174.0f, 137.0f, 31.0f},
                MENU_CANVAS_WIDTH * 0.5f,
                optionRects[1].y - 2.0f,
                hovered == 1 ? YELLOW : WHITE
            );
            DrawTextureSourceCentered(
                optionsLabel,
                (Rectangle){279.0f, 286.0f, 75.0f, 32.0f},
                MENU_CANVAS_WIDTH * 0.5f,
                optionRects[2].y - 2.0f,
                hovered == 2 ? YELLOW : WHITE
            );
            DrawTextureSourceCentered(
                exitLabel,
                (Rectangle){290.0f, 322.0f, 50.0f, 26.0f},
                MENU_CANVAS_WIDTH * 0.5f,
                optionRects[3].y,
                hovered == 3 ? YELLOW : WHITE
            );
        EndTextureMode();

        BeginDrawing();
            ClearBackground((Color){43, 56, 106, 255});
            DrawTexturePro(
                menuCanvas.texture,
                (Rectangle){0.0f, 0.0f, MENU_CANVAS_WIDTH, -MENU_CANVAS_HEIGHT},
                canvasRect,
                (Vector2){0.0f, 0.0f},
                0.0f,
                WHITE
            );
            DrawMenuCursor(hoveringButton);
        EndDrawing();
    }

    UnloadRenderTexture(menuCanvas);
    UnloadTexture(titleScreen);
    UnloadTexture(storyLabel);
    UnloadTexture(infiniteLabel);
    UnloadTexture(optionsLabel);
    UnloadTexture(exitLabel);
    return SCREEN_EXIT;
}

GameScreen RunInfiniteSoon()
{
    while (!WindowShouldClose())
    {
        AtualizarPreferenciaEspaco();
        UpdateSoundtrack();
        int currentWidth = GetScreenWidth();
        int currentHeight = GetScreenHeight();

        if (IsKeyPressed(KEY_ESCAPE))
        {
            return SCREEN_START;
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            const char* message = "Modo infinito em breve";
            int fontSize = currentHeight / 18;
            DrawText(
                message,
                (currentWidth / 2) - (MeasureText(message, fontSize) / 2),
                (currentHeight / 2) - (fontSize / 2),
                fontSize,
                DARKGRAY
            );
        EndDrawing();
    }

    return SCREEN_EXIT;
}
