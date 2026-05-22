#include <raylib.h>
#include "core/screens.h"
#include "core/cursor.h"

#define CHARACTER_SLOT_COUNT 3
#define CHARACTER_CANVAS_WIDTH 640.0f
#define CHARACTER_CANVAS_HEIGHT 360.0f

static int selectedCharacterId = 1;
static GameScreen characterSelectNextScreen = SCREEN_LEVEL_SELECT;

static Rectangle GetCharacterCanvasRect(int screenWidth, int screenHeight)
{
    float scaleX = (float)screenWidth / CHARACTER_CANVAS_WIDTH;
    float scaleY = (float)screenHeight / CHARACTER_CANVAS_HEIGHT;
    float scale = scaleX > scaleY ? scaleX : scaleY;
    float width = CHARACTER_CANVAS_WIDTH * scale;
    float height = CHARACTER_CANVAS_HEIGHT * scale;

    return (Rectangle)
    {
        ((float)screenWidth - width) * 0.5f,
        ((float)screenHeight - height) * 0.5f,
        width,
        height
    };
}

static Vector2 GetCharacterCanvasMouse(Rectangle canvasRect)
{
    Vector2 mouse = GetMousePosition();
    return (Vector2)
    {
        (mouse.x - canvasRect.x) * CHARACTER_CANVAS_WIDTH / canvasRect.width,
        (mouse.y - canvasRect.y) * CHARACTER_CANVAS_HEIGHT / canvasRect.height
    };
}

static void DrawCharacterScreen(Texture2D texture, Rectangle canvasRect)
{
    if (texture.id <= 0)
    {
        return;
    }

    DrawTexturePro(
        texture,
        (Rectangle){0.0f, 0.0f, CHARACTER_CANVAS_WIDTH, CHARACTER_CANVAS_HEIGHT},
        canvasRect,
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE
    );
}

int GetSelectedCharacterId(void)
{
    return selectedCharacterId;
}

void SetSelectedCharacterId(int characterId)
{
    if (characterId < 1)
    {
        characterId = 1;
    }
    if (characterId > CHARACTER_SLOT_COUNT)
    {
        characterId = CHARACTER_SLOT_COUNT;
    }
    selectedCharacterId = characterId;
}

void SetCharacterSelectNextScreen(GameScreen nextScreen)
{
    characterSelectNextScreen = nextScreen;
}

GameScreen RunCharacterSelect()
{
    bool acceptCharacterClick = false;
    Texture2D baseScreen = LoadTexture("assets/sprites/ui/character_select/selecao_de_personagem.png");
    Texture2D hoverScreens[CHARACTER_SLOT_COUNT] =
    {
        LoadTexture("assets/sprites/ui/character_select/selecao_de_personagem_rato_hover.png"),
        LoadTexture("assets/sprites/ui/character_select/selecao_de_personagem_capi_hover.png"),
        LoadTexture("assets/sprites/ui/character_select/selecao_de_personagem_guai_hover.png")
    };

    while (!WindowShouldClose())
    {
        int currentWidth = GetScreenWidth();
        int currentHeight = GetScreenHeight();
        Rectangle canvasRect = GetCharacterCanvasRect(currentWidth, currentHeight);
        Vector2 mouse = GetCharacterCanvasMouse(canvasRect);
        Rectangle slots[CHARACTER_SLOT_COUNT] =
        {
            {30.0f, 72.0f, 192.0f, 214.0f},
            {228.0f, 72.0f, 193.0f, 214.0f},
            {429.0f, 72.0f, 192.0f, 214.0f}
        };
        bool hoveringSlot = false;
        int hoveredSlot = -1;

        if (IsKeyPressed(KEY_ESCAPE))
        {
            UnloadTexture(baseScreen);
            for (int i = 0; i < CHARACTER_SLOT_COUNT; i++)
            {
                UnloadTexture(hoverScreens[i]);
            }
            return SCREEN_START;
        }

        if (!acceptCharacterClick && !IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            acceptCharacterClick = true;
        }

        for (int i = 0; i < CHARACTER_SLOT_COUNT; i++)
        {
            if (CheckCollisionPointRec(mouse, slots[i]))
            {
                hoveringSlot = true;
                hoveredSlot = i;
                if (acceptCharacterClick && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    SetSelectedCharacterId(i + 1);
                    UnloadTexture(baseScreen);
                    for (int j = 0; j < CHARACTER_SLOT_COUNT; j++)
                    {
                        UnloadTexture(hoverScreens[j]);
                    }
                    return characterSelectNextScreen;
                }
            }
        }

        BeginDrawing();
            ClearBackground((Color){43, 56, 106, 255});
            DrawCharacterScreen(hoveredSlot >= 0 ? hoverScreens[hoveredSlot] : baseScreen, canvasRect);
            DrawMenuCursor(hoveringSlot);
        EndDrawing();
    }

    UnloadTexture(baseScreen);
    for (int i = 0; i < CHARACTER_SLOT_COUNT; i++)
    {
        UnloadTexture(hoverScreens[i]);
    }
    return SCREEN_EXIT;
}
