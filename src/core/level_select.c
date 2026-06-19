#include <raylib.h>
#include <stdio.h>
#include "core/screens.h"
#include "core/ranking_manager.h"
#include "core/daily_challenges.h"
#include "core/cursor.h"
#include "core/sounds.h"
#include "utils.h"
#include "graphics/sprites.h"

#define STORY_LEVEL_COUNT 3
#define ITEMS_CHARACTER_SLOT_COUNT 3
#define ITEMS_EQUIPMENT_SLOT_COUNT 3
#define ITEMS_CLOTHING_SLOT_COUNT 2

static void UnloadStoryLevelSelectTextures(
    Texture2D maps[STORY_LEVEL_COUNT],
    Texture2D selectorArrow,
    Texture2D playButton,
    Texture2D playHoverButton,
    Texture2D itemsButton,
    Texture2D itemsHoverButton,
    Texture2D optionsButton,
    Texture2D optionsHoverButton,
    Texture2D menuButton,
    Texture2D menuHoverButton)
{
    for (int i = 0; i < STORY_LEVEL_COUNT; i++)
    {
        UnloadTexture(maps[i]);
    }
    UnloadTexture(selectorArrow);
    UnloadTexture(playButton);
    UnloadTexture(playHoverButton);
    UnloadTexture(itemsButton);
    UnloadTexture(itemsHoverButton);
    UnloadTexture(optionsButton);
    UnloadTexture(optionsHoverButton);
    UnloadTexture(menuButton);
    UnloadTexture(menuHoverButton);
}

static void UnloadInfiniteMenuTextures(
    Texture2D blueBackground,
    Texture2D rankingScreen,
    Texture2D playButton,
    Texture2D playHoverButton,
    Texture2D challengeButton,
    Texture2D challengeHoverButton,
    Texture2D itemsButton,
    Texture2D itemsHoverButton,
    Texture2D optionsButton,
    Texture2D optionsHoverButton,
    Texture2D menuButton,
    Texture2D menuHoverButton)
{
    UnloadTexture(blueBackground);
    UnloadTexture(rankingScreen);
    UnloadTexture(playButton);
    UnloadTexture(playHoverButton);
    UnloadTexture(challengeButton);
    UnloadTexture(challengeHoverButton);
    UnloadTexture(itemsButton);
    UnloadTexture(itemsHoverButton);
    UnloadTexture(optionsButton);
    UnloadTexture(optionsHoverButton);
    UnloadTexture(menuButton);
    UnloadTexture(menuHoverButton);
}

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

static void DrawWeaponPreview(Texture2D texture, Rectangle bounds, int frameCount, int frameIndex)
{
    if (texture.id <= 0 || frameCount <= 0)
    {
        return;
    }

    if (frameIndex < 0)
    {
        frameIndex = 0;
    }
    if (frameIndex >= frameCount)
    {
        frameIndex = frameCount - 1;
    }

    float frameWidth = (float)texture.width / frameCount;
    Rectangle source =
    {
        frameWidth * frameIndex,
        0.0f,
        frameWidth,
        (float)texture.height
    };
    float maxWidth = bounds.width * 0.92f;
    float maxHeight = bounds.height * 0.92f;
    float scaleX = maxWidth / source.width;
    float scaleY = maxHeight / source.height;
    float scale = scaleX < scaleY ? scaleX : scaleY;
    Rectangle dest =
    {
        bounds.x + (bounds.width - source.width * scale) * 0.5f,
        bounds.y + (bounds.height - source.height * scale) * 0.5f,
        source.width * scale,
        source.height * scale
    };

    DrawTexturePro(texture, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
}

static const char *GetPlayerGunSpritePath(int characterId)
{
    static const char *gunSpritePaths[ITEMS_CHARACTER_SLOT_COUNT] =
    {
        "assets/sprites/Player/Spr_Mouse/Spr_rat/Mouse_Arm_gun.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Arm_gun.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Arm_gun.png"
    };

    int index = characterId - 1;
    if (index < 0 || index >= ITEMS_CHARACTER_SLOT_COUNT)
    {
        index = 0;
    }
    return gunSpritePaths[index];
}

static int selectedStoryLevelId = 1;
static int maxUnlockedStoryLevelId = 1;
static int selectedClothingId = 1;
static int selectedWeaponId = 1;
static GameScreen itemsReturnScreen = SCREEN_LEVEL_SELECT;

int GetSelectedClothingId(void)
{
    return selectedClothingId;
}

int GetSelectedWeaponId(void)
{
    return selectedWeaponId;
}

void SetItemsReturnScreen(GameScreen returnScreen)
{
    itemsReturnScreen = returnScreen;
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

static Rectangle ScaleBaseRect(Rectangle rect, int screenWidth, int screenHeight)
{
    return ScaleUiRect(rect.x, rect.y, rect.width, rect.height, screenWidth, screenHeight);
}

static void DrawScreenButton(Texture2D normal, Texture2D hover, Rectangle baseSource, Rectangle baseDest, bool hovered, int screenWidth, int screenHeight)
{
    Texture2D texture = hovered && hover.id > 0 ? hover : normal;
    if (texture.id <= 0)
    {
        return;
    }

    DrawTexturePro(texture, baseSource, ScaleBaseRect(baseDest, screenWidth, screenHeight), (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
}

static void DrawInfiniteRankingPanel(Rectangle panel, const RankingInfinito *ranking, int screenHeight)
{
    int titleSize = screenHeight / 30;
    int headerSize = screenHeight / 48;
    int rowSize = screenHeight / 46;
    const char *title = "Top 10";

    DrawRectangleRec(panel, (Color){232, 232, 232, 255});
    DrawRectangleLinesEx(panel, 3.0f, DARKGRAY);

    DrawText(
        title,
        (int)(panel.x + (panel.width - MeasureText(title, titleSize)) * 0.5f),
        (int)(panel.y + panel.height * 0.07f),
        titleSize,
        DARKBLUE
    );

    float tableX = panel.x + panel.width * 0.12f;
    float tableY = panel.y + panel.height * 0.23f;
    float rankX = tableX;
    float nameX = tableX + panel.width * 0.13f;
    float scoreX = panel.x + panel.width * 0.70f;
    float tableW = panel.width * 0.76f;
    float rowH = rowSize + 7.0f;

    DrawText("POS", (int)rankX, (int)tableY, headerSize, GRAY);
    DrawText("NOME", (int)nameX, (int)tableY, headerSize, GRAY);
    DrawText("METROS", (int)scoreX, (int)tableY, headerSize, GRAY);
    DrawLine(
        (int)tableX,
        (int)(tableY + headerSize + 7),
        (int)(tableX + tableW),
        (int)(tableY + headerSize + 7),
        GRAY
    );

    if (ranking->quantidade == 0)
    {
        const char *emptyText = "Sem recordes ainda";
        DrawText(
            emptyText,
            (int)(panel.x + (panel.width - MeasureText(emptyText, rowSize)) * 0.5f),
            (int)(panel.y + panel.height * 0.52f),
            rowSize,
            DARKGRAY
        );
        return;
    }

    for (int i = 0; i < ranking->quantidade; i++)
    {
        char posText[8];
        char scoreText[24];
        snprintf(posText, sizeof(posText), "%d.", i + 1);
        snprintf(scoreText, sizeof(scoreText), "%.0fm", ranking->entradas[i].metros);

        float y = tableY + headerSize + 16.0f + i * rowH;
        if (i % 2 == 0)
        {
            DrawRectangle(
                (int)(tableX - 8.0f),
                (int)(y - 3.0f),
                (int)(tableW + 16.0f),
                (int)(rowH - 1.0f),
                (Color){222, 222, 222, 255}
            );
        }

        Color rowColor = i == 0 ? DARKBLUE : DARKGRAY;
        DrawText(posText, (int)rankX, (int)y, rowSize, rowColor);
        DrawText(ranking->entradas[i].nome, (int)nameX, (int)y, rowSize, rowColor);
        DrawText(scoreText, (int)scoreX, (int)y, rowSize, rowColor);
    }
}

#define INFINITE_RANKING_DISPLAY_MAX 5

static void DrawInfiniteRankingRows(const RankingInfinito *ranking, int screenWidth, int screenHeight)
{
    int fontSize = screenHeight / 34;
    int rowGap = screenHeight / 25;
    int startY = (int)(92.0f * screenHeight / 360.0f);
    int posX = (int)(114.0f * screenWidth / 640.0f);
    int nameX = (int)(198.0f * screenWidth / 640.0f);
    int metersX = (int)(420.0f * screenWidth / 640.0f);

    if (ranking->quantidade == 0)
    {
        const char *emptyText = "Sem recordes ainda";
        DrawText(emptyText, (screenWidth - MeasureText(emptyText, fontSize)) / 2, startY + rowGap * 2, fontSize, (Color){31, 35, 64, 255});
        return;
    }

    for (int i = 0; i < ranking->quantidade && i < INFINITE_RANKING_DISPLAY_MAX; i++)
    {
        char posText[8];
        char scoreText[24];
        snprintf(posText, sizeof(posText), "%d.", i + 1);
        snprintf(scoreText, sizeof(scoreText), "%.0fm", ranking->entradas[i].metros);
        int y = startY + i * rowGap;
        DrawText(posText, posX, y, fontSize, (Color){31, 35, 64, 255});
        DrawText(ranking->entradas[i].nome, nameX, y, fontSize, (Color){31, 35, 64, 255});
        DrawText(scoreText, metersX, y, fontSize, (Color){31, 35, 64, 255});
    }
}

GameScreen RunLevelSelect()
{
    const char *menuOptions[] =
    {
        "Play",
        "Itens",
        "Opções",
        "Menu"
    };
    bool acceptClick = false;
    bool acceptEscape = !IsKeyDown(KEY_ESCAPE);
    Vector2 lastMousePosition = GetMousePosition();
    Texture2D maps[STORY_LEVEL_COUNT] =
    {
        LoadTexture("assets/sprites/ui/story_map/map_stage_1.png"),
        LoadTexture("assets/sprites/ui/story_map/map_stage_2.png"),
        LoadTexture("assets/sprites/ui/story_map/map_stage_3.png")
    };
    Texture2D selectorArrow = LoadTexture("assets/sprites/ui/story_map/selector_arrow.png");
    Texture2D playButton = LoadTexture("assets/sprites/ui/main_menu/play.png");
    Texture2D playHoverButton = LoadTexture("assets/sprites/ui/main_menu/play_hover.png");
    Texture2D itemsButton = LoadTexture("assets/sprites/ui/main_menu/items.png");
    Texture2D itemsHoverButton = LoadTexture("assets/sprites/ui/main_menu/items_hover.png");
    Texture2D optionsButton = LoadTexture("assets/sprites/ui/main_menu/options.png");
    Texture2D optionsHoverButton = LoadTexture("assets/sprites/ui/main_menu/options_hover.png");
    Texture2D menuButton = LoadTexture("assets/sprites/ui/main_menu/menu.png");
    Texture2D menuHoverButton = LoadTexture("assets/sprites/ui/main_menu/menu_hover.png");
    Rectangle stageBaseRects[STORY_LEVEL_COUNT] =
    {
        {184.0f, 86.0f, 30.0f, 30.0f},
        {314.0f, 86.0f, 30.0f, 30.0f},
        {445.0f, 86.0f, 30.0f, 30.0f}
    };
    Rectangle optionBaseRects[4] =
    {
        {288.0f, 217.0f, 60.0f, 31.0f},
        {288.0f, 252.0f, 57.0f, 29.0f},
        {279.0f, 286.0f, 75.0f, 32.0f},
        {285.0f, 323.0f, 59.0f, 27.0f}
    };
    Rectangle optionBaseSources[4] =
    {
        {288.0f, 217.0f, 60.0f, 31.0f},
        {288.0f, 252.0f, 57.0f, 29.0f},
        {279.0f, 286.0f, 75.0f, 32.0f},
        {285.0f, 323.0f, 59.0f, 27.0f}
    };

    while (!WindowShouldClose())
    {
        AtualizarPreferenciaEspaco();
        UpdateSoundtrack();
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
        for (int i = 0; i < STORY_LEVEL_COUNT; i++)
        {
            stageRects[i] = ScaleBaseRect(stageBaseRects[i], currentWidth, currentHeight);
        }
        for (int i = 0; i < 4; i++)
        {
            optionRects[i] = ScaleBaseRect(optionBaseRects[i], currentWidth, currentHeight);
        }

        if (!acceptEscape && !IsKeyDown(KEY_ESCAPE))
        {
            acceptEscape = true;
        }

        if (acceptEscape && IsKeyPressed(KEY_ESCAPE))
        {
            UnloadStoryLevelSelectTextures(maps, selectorArrow, playButton, playHoverButton, itemsButton, itemsHoverButton, optionsButton, optionsHoverButton, menuButton, menuHoverButton);
            return SCREEN_CHARACTER_SELECT;
        }

        if (IsKeyPressed(KEY_ENTER))
        {
            UnloadStoryLevelSelectTextures(maps, selectorArrow, playButton, playHoverButton, itemsButton, itemsHoverButton, optionsButton, optionsHoverButton, menuButton, menuHoverButton);
            SetHistoryAnimationReturnScreen(SCREEN_LEVEL_SELECT);
            return SCREEN_HISTORY_ANIMATION;
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
            UnloadStoryLevelSelectTextures(maps, selectorArrow, playButton, playHoverButton, itemsButton, itemsHoverButton, optionsButton, optionsHoverButton, menuButton, menuHoverButton);
            return SCREEN_GAME;
        }
        if (clicked == 1)
        {
            SetItemsReturnScreen(SCREEN_LEVEL_SELECT);
            UnloadStoryLevelSelectTextures(maps, selectorArrow, playButton, playHoverButton, itemsButton, itemsHoverButton, optionsButton, optionsHoverButton, menuButton, menuHoverButton);
            return SCREEN_ITEMS;
        }
        if (clicked == 2)
        {
            UnloadStoryLevelSelectTextures(maps, selectorArrow, playButton, playHoverButton, itemsButton, itemsHoverButton, optionsButton, optionsHoverButton, menuButton, menuHoverButton);
            return SCREEN_OPTIONS;
        }
        if (clicked == 3)
        {
            UnloadStoryLevelSelectTextures(maps, selectorArrow, playButton, playHoverButton, itemsButton, itemsHoverButton, optionsButton, optionsHoverButton, menuButton, menuHoverButton);
            return SCREEN_START;
        }

        BeginDrawing();
            ClearBackground((Color){43, 56, 106, 255});
            int mapIndex = maxUnlockedStoryLevelId - 1;
            if (mapIndex < 0) mapIndex = 0;
            if (mapIndex >= STORY_LEVEL_COUNT) mapIndex = STORY_LEVEL_COUNT - 1;
            DrawFullscreenTexture(maps[mapIndex], currentWidth, currentHeight);
            DrawFullscreenTextureOffset(selectorArrow, (float)(selectedStoryLevelId - 1) * 130.0f, 0.0f, currentWidth, currentHeight);
            DrawScreenButton(playButton, playHoverButton, optionBaseSources[0], optionBaseRects[0], CheckCollisionPointRec(mouse, optionRects[0]), currentWidth, currentHeight);
            DrawScreenButton(itemsButton, itemsHoverButton, optionBaseSources[1], optionBaseRects[1], CheckCollisionPointRec(mouse, optionRects[1]), currentWidth, currentHeight);
            DrawScreenButton(optionsButton, optionsHoverButton, optionBaseSources[2], optionBaseRects[2], CheckCollisionPointRec(mouse, optionRects[2]), currentWidth, currentHeight);
            DrawScreenButton(menuButton, menuHoverButton, optionBaseSources[3], optionBaseRects[3], CheckCollisionPointRec(mouse, optionRects[3]), currentWidth, currentHeight);

            DrawMenuCursor(hoveringInteractive);
        EndDrawing();

        lastMousePosition = mouse;
    }

    UnloadStoryLevelSelectTextures(maps, selectorArrow, playButton, playHoverButton, itemsButton, itemsHoverButton, optionsButton, optionsHoverButton, menuButton, menuHoverButton);
    return SCREEN_EXIT;
}

GameScreen RunInfiniteMenu()
{
    const char *menuOptions[] =
    {
        "Jogar",
        "Desafios",
        "Itens",
        "Opções",
        "Menu"
    };
    bool acceptClick = false;
    bool acceptEscape = !IsKeyDown(KEY_ESCAPE);
    RankingInfinito ranking = CarregarRankingInfinitoLocal();
    RankingInfinitoFetch *rankingFetch = IniciarCarregamentoRankingInfinitoOnline();
    Texture2D blueBackground = LoadTexture("assets/sprites/background/fundo_azul.png");
    Texture2D rankingScreen = LoadTexture("assets/sprites/ui/infinite_menu/ranking_screen.png");
    Texture2D playButton = LoadTexture("assets/sprites/ui/main_menu/play.png");
    Texture2D playHoverButton = LoadTexture("assets/sprites/ui/main_menu/play_hover.png");
    Texture2D challengeButton = LoadTexture("assets/sprites/ui/infinite_menu/daily_challenges_button.png");
    Texture2D challengeHoverButton = LoadTexture("assets/sprites/ui/infinite_menu/daily_challenges_button_hover.png");
    Texture2D itemsButton = LoadTexture("assets/sprites/ui/main_menu/items.png");
    Texture2D itemsHoverButton = LoadTexture("assets/sprites/ui/main_menu/items_hover.png");
    Texture2D optionsButton = LoadTexture("assets/sprites/ui/main_menu/options.png");
    Texture2D optionsHoverButton = LoadTexture("assets/sprites/ui/main_menu/options_hover.png");
    Texture2D menuButton = LoadTexture("assets/sprites/ui/main_menu/menu.png");
    Texture2D menuHoverButton = LoadTexture("assets/sprites/ui/main_menu/menu_hover.png");
    Rectangle optionBaseRects[5] =
    {
        {288.0f, 216.0f, 60.0f, 31.0f},
        {276.0f, 248.0f, 88.0f, 27.0f},
        {288.0f, 278.0f, 57.0f, 29.0f},
        {279.0f, 309.0f, 75.0f, 32.0f},
        {285.0f, 338.0f, 59.0f, 27.0f}
    };
    Rectangle sourceRects[5] =
    {
        {288.0f, 217.0f, 60.0f, 31.0f},
        {6.0f, 12.0f, 88.0f, 27.0f},
        {288.0f, 252.0f, 57.0f, 29.0f},
        {279.0f, 286.0f, 75.0f, 32.0f},
        {285.0f, 323.0f, 59.0f, 27.0f}
    };

    while (!WindowShouldClose())
    {
        AtualizarPreferenciaEspaco();
        UpdateSoundtrack();
        int currentWidth = GetScreenWidth();
        int currentHeight = GetScreenHeight();
        int titleSize = currentHeight / 10;
        int menuFontSize = currentHeight / 21;
        int menuSpacing = menuFontSize + 18;
        int menuStartY = (int)(currentHeight * 0.62f);
        Rectangle optionRects[5];
        Rectangle panel =
        {
            currentWidth * 0.14f,
            currentHeight * 0.18f,
            currentWidth * 0.72f,
            currentHeight * 0.40f
        };
        Vector2 mouse = GetMousePosition();
        bool hoveringInteractive = false;
        RankingInfinito rankingOnline = {0};
        bool rankingFetchSuccess = false;

        BuildOptionRects(optionRects, menuOptions, 5, menuFontSize, currentWidth / 2, menuStartY, menuSpacing);
        for (int i = 0; i < 5; i++)
        {
            optionRects[i] = ScaleBaseRect(optionBaseRects[i], currentWidth, currentHeight);
        }

        if (rankingFetch != NULL &&
            FinalizarCarregamentoRankingInfinitoOnline(rankingFetch, &rankingOnline, &rankingFetchSuccess))
        {
            if (rankingFetchSuccess)
            {
                ranking = rankingOnline;
            }
            rankingFetch = NULL;
        }

        if (!acceptEscape && !IsKeyDown(KEY_ESCAPE))
        {
            acceptEscape = true;
        }

        if (acceptEscape && IsKeyPressed(KEY_ESCAPE))
        {
            CancelarCarregamentoRankingInfinitoOnline(rankingFetch);
            UnloadInfiniteMenuTextures(blueBackground, rankingScreen, playButton, playHoverButton, challengeButton, challengeHoverButton, itemsButton, itemsHoverButton, optionsButton, optionsHoverButton, menuButton, menuHoverButton);
            return SCREEN_CHARACTER_SELECT;
        }

        if (!acceptClick && !IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            acceptClick = true;
        }

        for (int i = 0; i < 5; i++)
        {
            if (CheckCollisionPointRec(mouse, optionRects[i]))
            {
                hoveringInteractive = true;
            }
        }

        int clicked = acceptClick ? GetClickedOption(optionRects, 5) : -1;
        if (clicked == 0)
        {
            CancelarCarregamentoRankingInfinitoOnline(rankingFetch);
            UnloadInfiniteMenuTextures(blueBackground, rankingScreen, playButton, playHoverButton, challengeButton, challengeHoverButton, itemsButton, itemsHoverButton, optionsButton, optionsHoverButton, menuButton, menuHoverButton);
            return SCREEN_INFINITE_GAME;
        }
        if (clicked == 1)
        {
            CancelarCarregamentoRankingInfinitoOnline(rankingFetch);
            UnloadInfiniteMenuTextures(blueBackground, rankingScreen, playButton, playHoverButton, challengeButton, challengeHoverButton, itemsButton, itemsHoverButton, optionsButton, optionsHoverButton, menuButton, menuHoverButton);
            return SCREEN_DAILY_CHALLENGES;
        }
        if (clicked == 2)
        {
            SetItemsReturnScreen(SCREEN_INFINITE_MENU);
            CancelarCarregamentoRankingInfinitoOnline(rankingFetch);
            UnloadInfiniteMenuTextures(blueBackground, rankingScreen, playButton, playHoverButton, challengeButton, challengeHoverButton, itemsButton, itemsHoverButton, optionsButton, optionsHoverButton, menuButton, menuHoverButton);
            return SCREEN_ITEMS;
        }
        if (clicked == 3)
        {
            CancelarCarregamentoRankingInfinitoOnline(rankingFetch);
            UnloadInfiniteMenuTextures(blueBackground, rankingScreen, playButton, playHoverButton, challengeButton, challengeHoverButton, itemsButton, itemsHoverButton, optionsButton, optionsHoverButton, menuButton, menuHoverButton);
            return SCREEN_OPTIONS;
        }
        if (clicked == 4)
        {
            CancelarCarregamentoRankingInfinitoOnline(rankingFetch);
            UnloadInfiniteMenuTextures(blueBackground, rankingScreen, playButton, playHoverButton, challengeButton, challengeHoverButton, itemsButton, itemsHoverButton, optionsButton, optionsHoverButton, menuButton, menuHoverButton);
            return SCREEN_START;
        }

        BeginDrawing();
            ClearBackground((Color){43, 56, 106, 255});
            DrawFullscreenTexture(blueBackground, currentWidth, currentHeight);
            DrawFullscreenTexture(rankingScreen, currentWidth, currentHeight);
            DrawInfiniteRankingRows(&ranking, currentWidth, currentHeight);
            DrawScreenButton(playButton, playHoverButton, sourceRects[0], optionBaseRects[0], CheckCollisionPointRec(mouse, optionRects[0]), currentWidth, currentHeight);
            DrawScreenButton(challengeButton, challengeHoverButton, sourceRects[1], optionBaseRects[1], CheckCollisionPointRec(mouse, optionRects[1]), currentWidth, currentHeight);
            DrawScreenButton(itemsButton, itemsHoverButton, sourceRects[2], optionBaseRects[2], CheckCollisionPointRec(mouse, optionRects[2]), currentWidth, currentHeight);
            DrawScreenButton(optionsButton, optionsHoverButton, sourceRects[3], optionBaseRects[3], CheckCollisionPointRec(mouse, optionRects[3]), currentWidth, currentHeight);
            DrawScreenButton(menuButton, menuHoverButton, sourceRects[4], optionBaseRects[4], CheckCollisionPointRec(mouse, optionRects[4]), currentWidth, currentHeight);

            DrawMenuCursor(hoveringInteractive);
        EndDrawing();
    }

    CancelarCarregamentoRankingInfinitoOnline(rankingFetch);
    UnloadInfiniteMenuTextures(blueBackground, rankingScreen, playButton, playHoverButton, challengeButton, challengeHoverButton, itemsButton, itemsHoverButton, optionsButton, optionsHoverButton, menuButton, menuHoverButton);
    return SCREEN_EXIT;
}

static void DrawDailyChallengeRow(Rectangle row, const DailyChallenge *challenge, int fontSize, int smallFontSize)
{
    const char *check = challenge->completed ? "X" : "";
    Color textColor = (Color){31, 37, 65, 255};
    char progressValue[16];
    char targetValue[16];

    snprintf(progressValue, sizeof(progressValue), "%d", challenge->progress);
    snprintf(targetValue, sizeof(targetValue), "%d", challenge->target);

    if (challenge->completed)
    {
        Rectangle completedFill = (Rectangle){row.x + 3.0f, row.y + 3.0f, row.width - 6.0f, row.height - 6.0f};
        DrawRectangleRec(completedFill, (Color){36, 174, 89, 55});

        Vector2 checkSize = MeasureTextEx(GetFontDefault(), check, fontSize, 1.0f);
        Vector2 checkCenter = (Vector2){row.x + row.width * 0.059f, row.y + row.height * 0.50f};
        Vector2 checkPosition = (Vector2){checkCenter.x - checkSize.x * 0.5f, checkCenter.y - checkSize.y * 0.5f};
        DrawTextEx(GetFontDefault(), check, checkPosition, fontSize, 1.0f, textColor);
    }

    int textX = (int)(row.x + row.width * 0.16f);
    int maxTextW = (int)(row.width * 0.48f);
    int challengeFontSize = fontSize;
    while (challengeFontSize > 14 && MeasureText(challenge->text, challengeFontSize) > maxTextW)
    {
        challengeFontSize--;
    }

    int textY = (int)(row.y + (row.height - challengeFontSize) * 0.42f);
    DrawText(challenge->text, textX, textY, challengeFontSize, textColor);

    int progressY = (int)(row.y + (row.height - smallFontSize) * 0.46f);
    int slashX = (int)(row.x + row.width * 0.833f);
    int gap = smallFontSize;
    int progressW = MeasureText(progressValue, smallFontSize);
    DrawText(progressValue, slashX - gap - progressW, progressY, smallFontSize, textColor);
    DrawText(targetValue, slashX + gap, progressY, smallFontSize, textColor);
}

GameScreen RunDailyChallenges()
{
    bool acceptClick = false;
    GameScreen nextScreen = SCREEN_EXIT;
    DailyChallengeState state = CarregarDesafiosDiarios();
    Texture2D blueBackground = LoadTexture("assets/sprites/background/fundo_azul.png");
    Texture2D challengesScreen = LoadTexture("assets/sprites/ui/infinite_menu/daily_challenges_screen.png");
    Texture2D backButtonTexture = LoadTexture("assets/sprites/ui/options/back.png");
    Texture2D backHoverTexture = LoadTexture("assets/sprites/ui/options/back_hover.png");
    Rectangle challengeBaseRows[DAILY_CHALLENGE_COUNT] =
    {
        {105.0f, 76.0f, 431.0f, 56.0f},
        {105.0f, 143.0f, 431.0f, 56.0f},
        {105.0f, 210.0f, 431.0f, 56.0f}
    };

    while (!WindowShouldClose() && nextScreen == SCREEN_EXIT)
    {
        AtualizarPreferenciaEspaco();
        UpdateSoundtrack();
        GarantirDesafiosDiarios(&state);

        int currentWidth = GetScreenWidth();
        int currentHeight = GetScreenHeight();
        int rowFontSize = currentHeight / 27;
        int smallFontSize = currentHeight / 29;
        Rectangle backButton = ScaleUiRect(7.0f, 8.0f, 24.0f, 30.0f, currentWidth, currentHeight);
        Vector2 mouse = GetMousePosition();
        bool hoveringInteractive = CheckCollisionPointRec(mouse, backButton);

        if (!acceptClick && !IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            acceptClick = true;
        }

        if (IsKeyPressed(KEY_ESCAPE))
        {
            nextScreen = SCREEN_INFINITE_MENU;
        }

        if (IsKeyPressed(KEY_BACKSPACE))
        {
            state = ResetarDesafiosDiariosConcluidos();
        }

        if (hoveringInteractive && acceptClick && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            nextScreen = SCREEN_INFINITE_MENU;
        }

        BeginDrawing();
            ClearBackground((Color){43, 56, 106, 255});

            bool backHovered = CheckCollisionPointRec(mouse, backButton);
            DrawFullscreenTexture(blueBackground, currentWidth, currentHeight);
            DrawFullscreenTexture(challengesScreen, currentWidth, currentHeight);
            DrawFullscreenTexture(backHovered ? backHoverTexture : backButtonTexture, currentWidth, currentHeight);

            for (int i = 0; i < DAILY_CHALLENGE_COUNT; i++)
            {
                Rectangle row = ScaleBaseRect(challengeBaseRows[i], currentWidth, currentHeight);
                DrawDailyChallengeRow(row, &state.challenges[i], rowFontSize, smallFontSize);
            }

            DrawMenuCursor(hoveringInteractive);
        EndDrawing();
    }

    UnloadTexture(blueBackground);
    UnloadTexture(challengesScreen);
    UnloadTexture(backButtonTexture);
    UnloadTexture(backHoverTexture);
    return nextScreen;
}

GameScreen RunItems()
{
    bool acceptClick = false;
    Texture2D blueBackground = LoadTexture("assets/sprites/background/fundo_azul.png");
    Texture2D itemText = LoadTexture("assets/sprites/ui/items/texto.png");
    Texture2D itemButtons = LoadTexture("assets/sprites/ui/items/botoes.png");
    Texture2D itemIcons = LoadTexture("assets/sprites/ui/items/icons.png");
    Texture2D itemSelected = LoadTexture("assets/sprites/ui/items/botao_ligado.png");
    Texture2D itemHover = LoadTexture("assets/sprites/ui/items/botao_hover.png");
    Texture2D itemBack = LoadTexture("assets/sprites/ui/items/sair.png");
    Texture2D itemBackHover = LoadTexture("assets/sprites/ui/items/sair_hover.png");
    Texture2D itemWeapons[ITEMS_EQUIPMENT_SLOT_COUNT] =
    {
        LoadTexture("assets/sprites/ui/items/martelo_grande.png"),
        LoadTexture("assets/sprites/ui/items/sword_grande.png"),
        LoadTexture("assets/sprites/ui/items/gun_grande.png")
    };
    Texture2D itemCharacters[ITEMS_CHARACTER_SLOT_COUNT][ITEMS_CLOTHING_SLOT_COUNT] =
    {
        {
            LoadTexture("assets/sprites/ui/items/rato_grande.png"),
            LoadTexture("assets/sprites/ui/items/rato_grande_cesar.png")
        },
        {
            LoadTexture("assets/sprites/ui/items/capi_grande.png"),
            LoadTexture("assets/sprites/ui/items/capi_grande_cesar.png")
        },
        {
            LoadTexture("assets/sprites/ui/items/guai_grande.png"),
            LoadTexture("assets/sprites/ui/items/guai_grande_cesar.png")
        }
    };
    Rectangle uiSlots[8] =
    {
        {335.0f, 68.0f, 69.0f, 68.0f},
        {413.0f, 68.0f, 69.0f, 68.0f},
        {491.0f, 68.0f, 69.0f, 68.0f},
        {374.0f, 157.0f, 69.0f, 68.0f},
        {452.0f, 157.0f, 69.0f, 68.0f},
        {335.0f, 245.0f, 69.0f, 68.0f},
        {413.0f, 245.0f, 69.0f, 68.0f},
        {491.0f, 245.0f, 69.0f, 68.0f}
    };
    GameScreen nextScreen = SCREEN_EXIT;

    while (!WindowShouldClose() && nextScreen == SCREEN_EXIT)
    {
        AtualizarPreferenciaEspaco();
        UpdateSoundtrack();
        int currentWidth = GetScreenWidth();
        int currentHeight = GetScreenHeight();
        Vector2 mouse = GetMousePosition();
        Rectangle backButton = ScaleUiRect(7.0f, 8.0f, 24.0f, 30.0f, currentWidth, currentHeight);
        bool hoveringInteractive = false;
        bool backHovered = CheckCollisionPointRec(mouse, backButton);

        if (backHovered)
        {
            hoveringInteractive = true;
            if (acceptClick && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                nextScreen = itemsReturnScreen;
            }
        }

        if (IsKeyPressed(KEY_ESCAPE))
        {
            nextScreen = itemsReturnScreen;
        }

        if (!acceptClick && !IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            acceptClick = true;
        }

        for (int i = 0; i < 8; i++)
        {
            Rectangle slot = ScaleBaseRect(uiSlots[i], currentWidth, currentHeight);
            if (CheckCollisionPointRec(mouse, slot))
            {
                hoveringInteractive = true;
                if (acceptClick && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    if (i < 3)
                    {
                        SetSelectedCharacterId(i + 1);
                    }
                    else if (i < 5)
                    {
                        selectedClothingId = i - 2;
                    }
                    else
                    {
                        selectedWeaponId = i - 4;
                    }
                }
            }
        }

        BeginDrawing();
            ClearBackground((Color){43, 56, 106, 255});
            DrawFullscreenTexture(blueBackground, currentWidth, currentHeight);
            int selectedCharacterIndexForUi = GetSelectedCharacterId() - 1;
            int selectedClothingIndexForUi = selectedClothingId - 1;
            int selectedWeaponIndexForUi = selectedWeaponId - 1;
            DrawFullscreenTexture(itemCharacters[selectedCharacterIndexForUi][selectedClothingIndexForUi], currentWidth, currentHeight);
            DrawFullscreenTexture(itemWeapons[selectedWeaponIndexForUi], currentWidth, currentHeight);
            DrawFullscreenTexture(itemButtons, currentWidth, currentHeight);
            int selectedUiSlots[3] = { GetSelectedCharacterId() - 1, 3 + selectedClothingId - 1, 5 + selectedWeaponId - 1 };
            for (int i = 0; i < 3; i++)
            {
                int slotIndex = selectedUiSlots[i];
                DrawFullscreenTextureOffset(itemSelected, uiSlots[slotIndex].x - 335.0f, uiSlots[slotIndex].y - 68.0f, currentWidth, currentHeight);
            }
            for (int i = 0; i < 8; i++)
            {
                if (CheckCollisionPointRec(mouse, ScaleBaseRect(uiSlots[i], currentWidth, currentHeight)))
                {
                    DrawFullscreenTextureOffset(itemHover, uiSlots[i].x - 335.0f, uiSlots[i].y - 68.0f, currentWidth, currentHeight);
                }
            }
            DrawFullscreenTexture(itemIcons, currentWidth, currentHeight);
            DrawFullscreenTexture(itemText, currentWidth, currentHeight);
            DrawFullscreenTexture(backHovered ? itemBackHover : itemBack, currentWidth, currentHeight);
            DrawMenuCursor(hoveringInteractive);
        EndDrawing();
    }

    UnloadTexture(blueBackground);
    UnloadTexture(itemText);
    UnloadTexture(itemButtons);
    UnloadTexture(itemIcons);
    UnloadTexture(itemSelected);
    UnloadTexture(itemHover);
    UnloadTexture(itemBack);
    UnloadTexture(itemBackHover);
    for (int i = 0; i < ITEMS_EQUIPMENT_SLOT_COUNT; i++)
    {
        UnloadTexture(itemWeapons[i]);
    }
    for (int i = 0; i < ITEMS_CHARACTER_SLOT_COUNT; i++)
    {
        for (int j = 0; j < ITEMS_CLOTHING_SLOT_COUNT; j++)
        {
            UnloadTexture(itemCharacters[i][j]);
        }
    }
    return nextScreen;
}
