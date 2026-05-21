#include <raylib.h>
#include "core/screens.h"
#include "core/cursor.h"
#include "utils.h"

#define CHARACTER_SLOT_COUNT 3

static int selectedCharacterId = 1;
static GameScreen characterSelectNextScreen = SCREEN_LEVEL_SELECT;

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
        Vector2 mouse = GetMousePosition();
        Rectangle slots[CHARACTER_SLOT_COUNT] =
        {
            ScaleUiRect(58.0f, 132.0f, 130.0f, 126.0f, currentWidth, currentHeight),
            ScaleUiRect(223.0f, 132.0f, 130.0f, 126.0f, currentWidth, currentHeight),
            ScaleUiRect(388.0f, 132.0f, 130.0f, 126.0f, currentWidth, currentHeight)
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
            ClearBackground(RAYWHITE);
            DrawFullscreenTexture(hoveredSlot >= 0 ? hoverScreens[hoveredSlot] : baseScreen, currentWidth, currentHeight);
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
