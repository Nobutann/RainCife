#include <raylib.h>
#include "core/screens.h"
#include "core/cursor.h"
#include "utils.h"
#include "graphics/sprites.h"

#define STORY_LEVEL_COUNT 3
#define ITEMS_CHARACTER_SLOT_COUNT 3
#define ITEMS_EQUIPMENT_SLOT_COUNT 3
#define ITEMS_CLOTHING_SLOT_COUNT 2

static void DrawRatPreview(Animation *animation, Rectangle bounds, float widthRatio, float heightRatio, float yRatio)
{
    Rectangle source = GetAnimationFrameSource(animation, false);
    float maxWidth = bounds.width * widthRatio;
    float maxHeight = bounds.height * heightRatio;
    float scaleX = maxWidth / source.width;
    float scaleY = maxHeight / source.height;
    float scale = scaleX < scaleY ? scaleX : scaleY;
    Rectangle dest =
    {
        bounds.x + (bounds.width - source.width * scale) * 0.5f,
        bounds.y + bounds.height * yRatio,
        source.width * scale,
        source.height * scale
    };
    DrawTexturePro(animation->sheet, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
}

static int selectedStoryLevelId = 1;
static int maxUnlockedStoryLevelId = 1;
static int selectedClothingId = 1;
static int selectedWeaponId = 1;

int GetSelectedClothingId(void)
{
    return selectedClothingId;
}

int GetSelectedStoryLevelId(void)
{
    return selectedStoryLevelId;
}

int GetMaxUnlockedStoryLevelId(void)
{
    return maxUnlockedStoryLevelId;
}

void SetSelectedStoryLevelId(int levelId)
{
    if (levelId < 1)
    {
        levelId = 1;
    }
    if (levelId > maxUnlockedStoryLevelId)
    {
        levelId = maxUnlockedStoryLevelId;
    }
    if (levelId > STORY_LEVEL_COUNT)
    {
        levelId = STORY_LEVEL_COUNT;
    }
    selectedStoryLevelId = levelId;
}

void UnlockStoryLevel(int levelId)
{
    if (levelId < 1)
    {
        return;
    }
    if (levelId > STORY_LEVEL_COUNT)
    {
        levelId = STORY_LEVEL_COUNT;
    }
    if (levelId > maxUnlockedStoryLevelId)
    {
        maxUnlockedStoryLevelId = levelId;
        selectedStoryLevelId = levelId;
    }
}

static void BuildStageRects(Rectangle stages[], int screenWidth, int screenHeight)
{
    float islandX = screenWidth * 0.16f;
    float islandY = screenHeight * 0.15f;
    float islandW = screenWidth * 0.68f;
    float islandH = screenHeight * 0.30f;
    float nodeSize = screenHeight * 0.055f;
    float nodeY = islandY + islandH * 0.35f;

    for (int i = 0; i < STORY_LEVEL_COUNT; i++)
    {
        float nodeX = islandX + islandW * (0.20f + i * 0.30f);
        stages[i] = (Rectangle){nodeX, nodeY, nodeSize, nodeSize};
    }
}

static void DrawStagePanel(int screenWidth, int screenHeight)
{
    Rectangle panel =
    {
        screenWidth * 0.14f,
        screenHeight * 0.14f,
        screenWidth * 0.72f,
        screenHeight * 0.34f
    };

    DrawRectangleRec(panel, (Color){232, 232, 232, 255});
    DrawRectangleLinesEx(panel, 3.0f, DARKGRAY);
}

static void DrawStageNode(Rectangle rect, int stageId, bool unlocked, bool selected, bool hovered, int fontSize)
{
    Color fill = unlocked ? (selected ? GOLD : RAYWHITE) : LIGHTGRAY;
    Color border = unlocked ? DARKGRAY : GRAY;
    if (hovered && unlocked)
    {
        fill = (Color){245, 232, 150, 255};
    }

    DrawEllipse(
        (int)(rect.x + rect.width * 0.5f),
        (int)(rect.y + rect.height * 0.5f),
        rect.width * 0.5f,
        rect.height * 0.44f,
        fill
    );
    DrawEllipseLines(
        (int)(rect.x + rect.width * 0.5f),
        (int)(rect.y + rect.height * 0.5f),
        rect.width * 0.5f,
        rect.height * 0.44f,
        border
    );

    const char *label = unlocked ? TextFormat("%d", stageId) : "X";
    int textWidth = MeasureText(label, fontSize);
    DrawText(
        label,
        (int)(rect.x + (rect.width - textWidth) * 0.5f),
        (int)(rect.y + (rect.height - fontSize) * 0.5f),
        fontSize,
        unlocked ? DARKBLUE : GRAY
    );
}

static void DrawSelectionArrow(Rectangle stageRect)
{
    Vector2 tip = {stageRect.x + stageRect.width * 0.5f, stageRect.y - 12.0f};
    Vector2 left = {tip.x - 18.0f, tip.y - 38.0f};
    Vector2 right = {tip.x + 18.0f, tip.y - 38.0f};
    DrawTriangle(left, right, tip, RED);
    DrawLineEx((Vector2){tip.x, tip.y - 72.0f}, tip, 7.0f, RED);
}

GameScreen RunLevelSelect()
{
    const char *menuOptions[] =
    {
        "Play",
        "Itens",
        "Opcoes",
        "Menu"
    };
    bool acceptClick = false;
    Vector2 lastMousePosition = GetMousePosition();

    while (!WindowShouldClose())
    {
        int currentWidth = GetScreenWidth();
        int currentHeight = GetScreenHeight();
        int stageFontSize = currentHeight / 28;
        int menuFontSize = currentHeight / 21;
        int menuSpacing = menuFontSize + 18;
        int menuStartY = (int)(currentHeight * 0.70f);
        Rectangle stageRects[STORY_LEVEL_COUNT];
        Rectangle optionRects[4];
        Vector2 mouse = GetMousePosition();
        bool mouseMoved = mouse.x != lastMousePosition.x || mouse.y != lastMousePosition.y;
        bool hoveringInteractive = false;

        BuildStageRects(stageRects, currentWidth, currentHeight);
        BuildOptionRects(optionRects, menuOptions, 4, menuFontSize, currentWidth / 2, menuStartY, menuSpacing);

        if (IsKeyPressed(KEY_ESCAPE))
        {
            return SCREEN_CHARACTER_SELECT;
        }

        if (!acceptClick && !IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            acceptClick = true;
        }

        if (IsKeyPressed(KEY_LEFT) && selectedStoryLevelId > 1)
        {
            selectedStoryLevelId--;
        }
        if (IsKeyPressed(KEY_RIGHT) && selectedStoryLevelId < maxUnlockedStoryLevelId)
        {
            selectedStoryLevelId++;
        }

        for (int i = 0; i < STORY_LEVEL_COUNT; i++)
        {
            bool unlocked = (i + 1) <= maxUnlockedStoryLevelId;
            if (unlocked && CheckCollisionPointRec(mouse, stageRects[i]))
            {
                hoveringInteractive = true;
                if (mouseMoved)
                {
                    selectedStoryLevelId = i + 1;
                }
            }
        }

        for (int i = 0; i < 4; i++)
        {
            if (CheckCollisionPointRec(mouse, optionRects[i]))
            {
                hoveringInteractive = true;
            }
        }

        int clicked = acceptClick ? GetClickedOption(optionRects, 4) : -1;
        if (clicked == 0)
        {
            return SCREEN_GAME;
        }
        if (clicked == 1)
        {
            return SCREEN_ITEMS;
        }
        if (clicked == 2)
        {
            return SCREEN_OPTIONS;
        }
        if (clicked == 3)
        {
            return SCREEN_START;
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawStagePanel(currentWidth, currentHeight);

            for (int i = 0; i < STORY_LEVEL_COUNT; i++)
            {
                bool unlocked = (i + 1) <= maxUnlockedStoryLevelId;
                bool selected = selectedStoryLevelId == (i + 1);
                bool hovered = unlocked && CheckCollisionPointRec(mouse, stageRects[i]);
                DrawStageNode(stageRects[i], i + 1, unlocked, selected, hovered, stageFontSize);
            }

            DrawSelectionArrow(stageRects[selectedStoryLevelId - 1]);

            for (int i = 0; i < 4; i++)
            {
                bool hovered = CheckCollisionPointRec(mouse, optionRects[i]);
                Color color = hovered ? YELLOW : DARKGRAY;
                DrawText(menuOptions[i], optionRects[i].x, optionRects[i].y, menuFontSize, color);
            }

            DrawMenuCursor(hoveringInteractive);
        EndDrawing();

        lastMousePosition = mouse;
    }

    return SCREEN_EXIT;
}

GameScreen RunItems()
{
    Rectangle characterSlots[ITEMS_CHARACTER_SLOT_COUNT];
    Rectangle clothingSlots[ITEMS_CLOTHING_SLOT_COUNT];
    Rectangle weaponSlots[ITEMS_EQUIPMENT_SLOT_COUNT];
    bool acceptClick = false;
    Animation ratIdleAnimation = LoadAnimation("assets/sprites/Player/idle/Idle_complete-Sheet.png", 6, 0.10f);
    Animation ratCesarIdleAnimation = LoadAnimation("assets/sprites/Player/idle/Idle_complete_cesar-Sheet.png", 6, 0.10f);

    while (!WindowShouldClose())
    {
        int currentWidth = GetScreenWidth();
        int currentHeight = GetScreenHeight();
        Vector2 mouse = GetMousePosition();
        int titleSize = currentHeight / 14;
        int sectionSize = currentHeight / 27;
        int slotLabelSize = currentHeight / 38;
        int bodySize = currentHeight / 42;
        float frameMargin = currentWidth * 0.035f;
        Rectangle outerFrame =
        {
            frameMargin,
            frameMargin,
            currentWidth - frameMargin * 2.0f,
            currentHeight - frameMargin * 2.0f
        };
        Rectangle previewPanel =
        {
            outerFrame.x + outerFrame.width * 0.035f,
            outerFrame.y + outerFrame.height * 0.14f,
            outerFrame.width * 0.40f,
            outerFrame.height * 0.74f
        };
        Rectangle rightPanel =
        {
            outerFrame.x + outerFrame.width * 0.50f,
            outerFrame.y + outerFrame.height * 0.12f,
            outerFrame.width * 0.43f,
            outerFrame.height * 0.78f
        };
        float characterSlotW = rightPanel.width * 0.25f;
        float characterSlotGap = rightPanel.width * 0.055f;
        float characterSlotsWidth = characterSlotW * ITEMS_CHARACTER_SLOT_COUNT + characterSlotGap * (ITEMS_CHARACTER_SLOT_COUNT - 1);
        float equipmentSlotW = rightPanel.width * 0.25f;
        float equipmentSlotGap = rightPanel.width * 0.055f;
        float clothingSlotsWidth = equipmentSlotW * ITEMS_CLOTHING_SLOT_COUNT + equipmentSlotGap * (ITEMS_CLOTHING_SLOT_COUNT - 1);
        float weaponSlotsWidth = equipmentSlotW * ITEMS_EQUIPMENT_SLOT_COUNT + equipmentSlotGap * (ITEMS_EQUIPMENT_SLOT_COUNT - 1);
        Rectangle backButton = { 28.0f, 26.0f, 70.0f, 54.0f };
        bool hoveringInteractive = false;
        UpdateAnimation(&ratIdleAnimation, GetFrameTime());
        UpdateAnimation(&ratCesarIdleAnimation, GetFrameTime());

        if (CheckCollisionPointRec(mouse, backButton))
        {
            hoveringInteractive = true;
            if (acceptClick && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                UnloadAnimation(&ratIdleAnimation);
                UnloadAnimation(&ratCesarIdleAnimation);
                return SCREEN_LEVEL_SELECT;
            }
        }

        if (IsKeyPressed(KEY_ESCAPE))
        {
            UnloadAnimation(&ratIdleAnimation);
            UnloadAnimation(&ratCesarIdleAnimation);
            return SCREEN_LEVEL_SELECT;
        }

        if (!acceptClick && !IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            acceptClick = true;
        }

        for (int i = 0; i < ITEMS_CHARACTER_SLOT_COUNT; i++)
        {
            characterSlots[i] = (Rectangle)
            {
                rightPanel.x + (rightPanel.width - characterSlotsWidth) * 0.5f + i * (characterSlotW + characterSlotGap),
                rightPanel.y + rightPanel.height * 0.16f,
                characterSlotW,
                characterSlotW
            };

            if (CheckCollisionPointRec(mouse, characterSlots[i]))
            {
                hoveringInteractive = true;
                if (acceptClick && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    SetSelectedCharacterId(i + 1);
                }
            }
        }

        for (int i = 0; i < ITEMS_CLOTHING_SLOT_COUNT; i++)
        {
            clothingSlots[i] = (Rectangle)
            {
                rightPanel.x + (rightPanel.width - clothingSlotsWidth) * 0.5f + i * (equipmentSlotW + equipmentSlotGap),
                rightPanel.y + rightPanel.height * 0.47f,
                equipmentSlotW,
                equipmentSlotW
            };
            if (CheckCollisionPointRec(mouse, clothingSlots[i]))
            {
                hoveringInteractive = true;
                if (acceptClick && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    selectedClothingId = i + 1;
                }
            }
        }

        for (int i = 0; i < ITEMS_EQUIPMENT_SLOT_COUNT; i++)
        {
            weaponSlots[i] = (Rectangle)
            {
                rightPanel.x + (rightPanel.width - weaponSlotsWidth) * 0.5f + i * (equipmentSlotW + equipmentSlotGap),
                rightPanel.y + rightPanel.height * 0.77f,
                equipmentSlotW,
                equipmentSlotW
            };
            if (CheckCollisionPointRec(mouse, weaponSlots[i]))
            {
                hoveringInteractive = true;
                if (acceptClick && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    selectedWeaponId = i + 1;
                }
            }
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            bool backHovered = CheckCollisionPointRec(mouse, backButton);
            DrawText("<", 40, 18, 58, backHovered ? RED : DARKGRAY);

            const char *title = "Itens";
            int textWidth = MeasureText(title, titleSize);
            DrawText(
                title,
                (currentWidth / 2) - (textWidth / 2),
                (int)(outerFrame.y + outerFrame.height * 0.035f),
                titleSize,
                DARKBLUE
            );

            const char *previewLabel = GetSelectedCharacterId() == 1
                ? "Rato"
                : TextFormat("Personagem %d", GetSelectedCharacterId());
            int previewLabelWidth = MeasureText(previewLabel, sectionSize);
            DrawText(
                previewLabel,
                (int)(previewPanel.x + (previewPanel.width - previewLabelWidth) * 0.5f),
                (int)(previewPanel.y + previewPanel.height * 0.08f),
                sectionSize,
                DARKBLUE
            );

            Rectangle stage =
            {
                previewPanel.x + previewPanel.width * 0.16f,
                previewPanel.y + previewPanel.height * 0.23f,
                previewPanel.width * 0.68f,
                previewPanel.height * 0.50f
            };
            DrawEllipse(
                (int)(stage.x + stage.width * 0.5f),
                (int)(stage.y + stage.height * 0.88f),
                stage.width * 0.38f,
                stage.height * 0.10f,
                BLACK
            );
            if (GetSelectedCharacterId() == 1)
            {
                Animation *previewAnimation = selectedClothingId == 2
                    ? &ratCesarIdleAnimation
                    : &ratIdleAnimation;
                DrawRatPreview(previewAnimation, stage, 1.18f, 1.18f, -0.10f);
            }

            const char *charactersTitle = "Personagens";
            DrawText(
                charactersTitle,
                (int)(rightPanel.x + (rightPanel.width - MeasureText(charactersTitle, sectionSize)) * 0.5f),
                (int)(characterSlots[0].y - sectionSize - 10.0f),
                sectionSize,
                DARKBLUE
            );

            for (int i = 0; i < ITEMS_CHARACTER_SLOT_COUNT; i++)
            {
                bool selected = GetSelectedCharacterId() == (i + 1);
                bool hovered = CheckCollisionPointRec(mouse, characterSlots[i]);
                Color fill = selected ? (Color){245, 232, 150, 255} : (Color){232, 232, 232, 255};
                if (hovered)
                {
                    fill = (Color){255, 242, 170, 255};
                }
                DrawRectangleRec(characterSlots[i], fill);
                DrawRectangleLinesEx(characterSlots[i], selected ? 4.0f : 3.0f, selected ? GOLD : DARKGRAY);

                if (i > 0)
                {
                    const char *slotTitle = TextFormat("%d", i + 1);
                    DrawText(
                        slotTitle,
                        (int)(characterSlots[i].x + (characterSlots[i].width - MeasureText(slotTitle, slotLabelSize + 8)) * 0.5f),
                        (int)(characterSlots[i].y + characterSlots[i].height * 0.22f),
                        slotLabelSize + 8,
                        DARKBLUE
                    );
                }
                if (i == 0)
                {
                    DrawRatPreview(&ratIdleAnimation, characterSlots[i], 1.08f, 1.00f, 0.02f);
                }
                else
                {
                    DrawText(
                        "agente",
                        (int)(characterSlots[i].x + (characterSlots[i].width - MeasureText("agente", bodySize)) * 0.5f),
                        (int)(characterSlots[i].y + characterSlots[i].height * 0.62f),
                        bodySize,
                        GRAY
                    );
                }
            }

            const char *clothingTitle = "Roupas";
            DrawText(
                clothingTitle,
                (int)(rightPanel.x + (rightPanel.width - MeasureText(clothingTitle, sectionSize)) * 0.5f),
                (int)(clothingSlots[0].y - sectionSize - 8.0f),
                sectionSize,
                DARKBLUE
            );

            const char *weaponTitle = "Armas";
            DrawText(
                weaponTitle,
                (int)(rightPanel.x + (rightPanel.width - MeasureText(weaponTitle, sectionSize)) * 0.5f),
                (int)(weaponSlots[0].y - sectionSize - 8.0f),
                sectionSize,
                DARKBLUE
            );

            for (int i = 0; i < ITEMS_CLOTHING_SLOT_COUNT; i++)
            {
                bool hovered = CheckCollisionPointRec(mouse, clothingSlots[i]);
                bool selected = selectedClothingId == (i + 1);
                Color fill = selected ? (Color){245, 232, 150, 255} : (Color){236, 236, 236, 255};
                if (hovered)
                {
                    fill = (Color){255, 242, 170, 255};
                }
                DrawRectangleRec(clothingSlots[i], fill);
                DrawRectangleLinesEx(clothingSlots[i], selected ? 4.0f : 3.0f, selected ? GOLD : DARKGRAY);
                if (i == 0)
                {
                    DrawRatPreview(&ratIdleAnimation, clothingSlots[i], 0.96f, 0.96f, 0.00f);
                }
                else if (i == 1)
                {
                    DrawRatPreview(&ratCesarIdleAnimation, clothingSlots[i], 0.96f, 0.96f, 0.00f);
                }
            }

            for (int i = 0; i < ITEMS_EQUIPMENT_SLOT_COUNT; i++)
            {
                bool hovered = CheckCollisionPointRec(mouse, weaponSlots[i]);
                bool selected = selectedWeaponId == (i + 1);
                Color fill = selected ? (Color){245, 232, 150, 255} : (Color){236, 236, 236, 255};
                if (hovered)
                {
                    fill = (Color){255, 242, 170, 255};
                }
                DrawRectangleRec(weaponSlots[i], fill);
                DrawRectangleLinesEx(weaponSlots[i], selected ? 4.0f : 3.0f, selected ? GOLD : DARKGRAY);
                DrawText(
                    "arma",
                    (int)(weaponSlots[i].x + (weaponSlots[i].width - MeasureText("arma", bodySize)) * 0.5f),
                    (int)(weaponSlots[i].y + weaponSlots[i].height * 0.43f),
                    bodySize,
                    GRAY
                );
            }

            DrawMenuCursor(hoveringInteractive);
        EndDrawing();
    }

    UnloadAnimation(&ratIdleAnimation);
    UnloadAnimation(&ratCesarIdleAnimation);
    return SCREEN_EXIT;
}
