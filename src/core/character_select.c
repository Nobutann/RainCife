#include <raylib.h>
#include <stddef.h>
#include "core/screens.h"
#include "core/cursor.h"
#include "graphics/sprites.h"

#define CHARACTER_SLOT_COUNT 3

static int selectedCharacterId = 1;

typedef struct CharacterSlot
{
    const char *name;
    const char *futureTexturePath;
    Rectangle bounds;
} CharacterSlot;

static void BuildCharacterSlots(CharacterSlot slots[], int screenWidth, int screenHeight)
{
    const float slotSize = screenHeight * 0.34f;
    const float slotGap = screenWidth * 0.045f;
    const float totalWidth = slotSize * CHARACTER_SLOT_COUNT + slotGap * (CHARACTER_SLOT_COUNT - 1);
    const float startX = ((float)screenWidth - totalWidth) * 0.5f;
    const float startY = screenHeight * 0.34f;

    for (int i = 0; i < CHARACTER_SLOT_COUNT; i++)
    {
        slots[i].bounds = (Rectangle)
        {
            startX + i * (slotSize + slotGap),
            startY,
            slotSize,
            slotSize
        };
    }
}

static void DrawCharacterSlot(const CharacterSlot *slot, bool hovered, int labelFontSize)
{
    Color fill = hovered ? (Color){245, 232, 150, 255} : (Color){225, 225, 225, 255};
    Color outline = hovered ? GOLD : DARKGRAY;

    DrawRectangleRec(slot->bounds, fill);
    DrawRectangleLinesEx(slot->bounds, hovered ? 5.0f : 3.0f, outline);

    int labelWidth = MeasureText(slot->name, labelFontSize);
    DrawText(
        slot->name,
        (int)(slot->bounds.x + (slot->bounds.width - labelWidth) * 0.5f),
        (int)(slot->bounds.y + slot->bounds.height - labelFontSize - 18.0f),
        labelFontSize,
        DARKGRAY
    );
}

static void DrawCharacterArtwork(const CharacterSlot *slot, Animation *ratIdleAnimation)
{
    if (slot->futureTexturePath == NULL)
    {
        const char *placeholder = "em breve";
        int placeholderSize = (int)(slot->bounds.height * 0.10f);
        int placeholderWidth = MeasureText(placeholder, placeholderSize);
        DrawText(
            placeholder,
            (int)(slot->bounds.x + (slot->bounds.width - placeholderWidth) * 0.5f),
            (int)(slot->bounds.y + slot->bounds.height * 0.42f),
            placeholderSize,
            GRAY
        );
        return;
    }

    Rectangle source = GetAnimationFrameSource(ratIdleAnimation, false);
    float maxWidth = slot->bounds.width * 0.72f;
    float maxHeight = slot->bounds.height * 0.72f;
    float scaleX = maxWidth / source.width;
    float scaleY = maxHeight / source.height;
    float scale = scaleX < scaleY ? scaleX : scaleY;
    Rectangle dest =
    {
        slot->bounds.x + (slot->bounds.width - source.width * scale) * 0.5f,
        slot->bounds.y + slot->bounds.height * 0.10f,
        source.width * scale,
        source.height * scale
    };
    DrawTexturePro(ratIdleAnimation->sheet, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
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

GameScreen RunCharacterSelect()
{
    CharacterSlot slots[CHARACTER_SLOT_COUNT] =
    {
        {"Rato", "assets/sprites/Player/idle/Idle_complete-Sheet.png", {0}},
        {"Personagem 2", NULL, {0}},
        {"Personagem 3", NULL, {0}}
    };
    bool acceptCharacterClick = false;
    Animation ratIdleAnimation = LoadAnimation("assets/sprites/Player/idle/Idle_complete-Sheet.png", 6, 0.10f);

    while (!WindowShouldClose())
    {
        int currentWidth = GetScreenWidth();
        int currentHeight = GetScreenHeight();
        int titleFontSize = currentHeight / 11;
        int labelFontSize = currentHeight / 28;
        Vector2 mouse = GetMousePosition();
        bool hoveringSlot = false;

        BuildCharacterSlots(slots, currentWidth, currentHeight);
        UpdateAnimation(&ratIdleAnimation, GetFrameTime());

        if (IsKeyPressed(KEY_ESCAPE))
        {
            UnloadAnimation(&ratIdleAnimation);
            return SCREEN_START;
        }

        if (!acceptCharacterClick && !IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            acceptCharacterClick = true;
        }

        for (int i = 0; i < CHARACTER_SLOT_COUNT; i++)
        {
            if (CheckCollisionPointRec(mouse, slots[i].bounds))
            {
                hoveringSlot = true;
                if (acceptCharacterClick && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    SetSelectedCharacterId(i + 1);
                    UnloadAnimation(&ratIdleAnimation);
                    return SCREEN_LEVEL_SELECT;
                }
            }
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            const char *title = "Selecao de personagens";
            int titleWidth = MeasureText(title, titleFontSize);
            DrawText(
                title,
                (currentWidth / 2) - (titleWidth / 2),
                currentHeight / 8,
                titleFontSize,
                DARKBLUE
            );

            for (int i = 0; i < CHARACTER_SLOT_COUNT; i++)
            {
                bool hovered = CheckCollisionPointRec(mouse, slots[i].bounds);
                DrawCharacterSlot(&slots[i], hovered, labelFontSize);
                DrawCharacterArtwork(&slots[i], &ratIdleAnimation);
            }

            DrawMenuCursor(hoveringSlot);
        EndDrawing();
    }

    UnloadAnimation(&ratIdleAnimation);
    return SCREEN_EXIT;
}
