#include <raylib.h>
#include <stddef.h>
#include <stdio.h>
#include "core/screens.h"
#include "core/config_manager.h"
#include "entities/player.h"
#include "entities/hairy_leg.h"
#include "entities/shark.h"
#include "entities/midnight_man.h"
#include "entities/enemy.h"
#include "graphics/background.h"
#include "enemy_caller.h"
#include "gameplay/levels.h"
#include "core/cursor.h"
#include "core/window_mode.h"
#include "utils.h"
#define MAX_ACTIVE_ENEMIES 12
#define LEVEL6_INTRO_DURATION 2.5f
#define HAIRY_LEG_DEATH_ADVANCE_DELAY 1.0f
#define INFINITE_METERS_PER_SECOND (5000.0f / 60.0f)
#define INFINITE_SPEED_STEP_METERS 5000.0f
#define INFINITE_SPEED_MULTIPLIER_STEP 1.05f

static void StartLevel(
    Level **currentLevel,
    Level *targetLevel,
    GamePhase *phase,
    float *progressTimer,
    bool *autoSpawn,
    float *spawnTimer,
    Enemy enemies[],
    Player *player,
    HairyLeg *pernaCabeluda,
    Shark *shark,
    int currentWidth,
    int currentHeight,
    float groundY,
    float bossScale
)
{
    *currentLevel = targetLevel;
    *phase = PHASE_RUNNING;
    *progressTimer = 0.0f;
    *autoSpawn = true;
    *spawnTimer = 0.0f;

    for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
    {
        enemies[i].active = false;
    }

    InitHairyLeg(pernaCabeluda, (Vector2){ (float)currentWidth * 0.6f, groundY }, groundY, bossScale);
    InitShark(shark, currentWidth, currentHeight);
    player->isBossFighting = false;
}

static Level *FindLevelById(Level *levels, int levelId)
{
    Level *current = levels;
    while (current != NULL)
    {
        if (current->id == levelId)
        {
            return current;
        }
        current = current->next;
    }
    return levels;
}

static void ResetPlayerForRunningRetry(Player *player, float groundY, float playerScale)
{
    player->position = (Vector2){100.0f, groundY};
    player->velocity = (Vector2){0.0f, 0.0f};
    player->onGround = true;
    player->isBossFighting = false;
    player->isJumping = false;
    player->jumpHoldTimer = 0.0f;
    player->weapon.attacking = false;
    player->weapon.attackTimer = 0.0f;
    player->weapon.hitConnected = false;
    player->facingRight = false;

    if (player->sprites.walkFront.layerCount > 0)
    {
        player->currentAnim = &player->sprites.walkFront;
        float spriteH = player->sprites.walkFront.layers[0].sheet.height * playerScale;
        player->position.y = groundY - spriteH * 1.1f;
    }
}

static void RestartCurrentEncounter(
    Level *currentLevel,
    GamePhase retryPhase,
    GamePhase *phase,
    float *progressTimer,
    bool *autoSpawn,
    float *spawnTimer,
    Enemy enemies[],
    Player *player,
    HairyLeg *pernaCabeluda,
    Shark *shark,
    int currentWidth,
    int currentHeight,
    float groundY,
    float bossScale,
    float playerStandingY,
    float playerScale
)
{
    for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
    {
        enemies[i].active = false;
    }

    *spawnTimer = 0.0f;

    if (retryPhase == PHASE_BOSS && currentLevel->bossId != 0)
    {
        *phase = PHASE_BOSS;
        *progressTimer = currentLevel->duration;
        *autoSpawn = false;

        if (currentLevel->bossId == 1)
        {
            InitHairyLeg(pernaCabeluda, (Vector2){(float)currentWidth * 0.6f, groundY}, groundY, bossScale);
            PlacePlayerForBossIntro(player, pernaCabeluda->rect, playerStandingY, playerScale);
        }
        else if (currentLevel->bossId == 2)
        {
            InitShark(shark, currentWidth, currentHeight);
            PlacePlayerForBossIntro(player, GetSharkHitbox(shark), playerStandingY, playerScale);
        }
        return;
    }

    *phase = PHASE_RUNNING;
    *progressTimer = 0.0f;
    *autoSpawn = true;
    InitHairyLeg(pernaCabeluda, (Vector2){ (float)currentWidth * 0.6f, groundY }, groundY, bossScale);
    InitShark(shark, currentWidth, currentHeight);
    ResetPlayerForRunningRetry(player, playerStandingY, playerScale);
}

static void EnterBossPhase(
    Level *currentLevel,
    GamePhase *phase,
    float *progressTimer,
    bool *autoSpawn,
    float *safePosteFollowUpTimer,
    Enemy enemies[],
    Player *player,
    HairyLeg *pernaCabeluda,
    Shark *shark,
    int currentWidth,
    int currentHeight,
    float groundY,
    float bossScale,
    float playerStandingY,
    float playerScale
)
{
    *phase = PHASE_BOSS;
    *progressTimer = currentLevel->duration;
    *autoSpawn = false;
    *safePosteFollowUpTimer = -1.0f;

    for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
    {
        enemies[i].active = false;
    }

    player->isBossFighting = currentLevel->bossId != 0;

    if (currentLevel->bossId == 1)
    {
        InitHairyLeg(pernaCabeluda, (Vector2){(float)currentWidth * 0.6f, groundY}, groundY, bossScale);
        PlacePlayerForBossIntro(player, pernaCabeluda->rect, playerStandingY, playerScale);
    }
    else if (currentLevel->bossId == 2)
    {
        InitShark(shark, currentWidth, currentHeight);
        PlacePlayerForBossIntro(player, GetSharkHitbox(shark), playerStandingY, playerScale);
    }
}

static void DrawDeathScreenOverlay(int currentWidth, int currentHeight, Rectangle optionRects[], const char **options, int optionCount, bool hoveringButton)
{
    DrawRectangle(0, 0, currentWidth, currentHeight, Fade(BLACK, 0.78f));

    const char *title = "Você morreu";
    int titleSize = currentHeight / 11;
    int subtitleSize = currentHeight / 28;
    const char *subtitle = "Escolha como continuar";

    DrawText(
        title,
        (currentWidth / 2) - (MeasureText(title, titleSize) / 2),
        currentHeight / 4,
        titleSize,
        RAYWHITE
    );

    DrawText(
        subtitle,
        (currentWidth / 2) - (MeasureText(subtitle, subtitleSize) / 2),
        currentHeight / 4 + titleSize + 14,
        subtitleSize,
        LIGHTGRAY
    );

    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < optionCount; i++)
    {
        bool hover = CheckCollisionPointRec(mouse, optionRects[i]);
        Color color = hover ? YELLOW : RAYWHITE;
        DrawText(options[i], optionRects[i].x, optionRects[i].y, currentHeight / 20, color);
    }

    DrawMenuCursor(hoveringButton);
}

static void DrawPauseScreenOverlay(int currentWidth, int currentHeight, Rectangle optionRects[], const char **options, int optionCount, bool hoveringButton)
{
    DrawRectangle(0, 0, currentWidth, currentHeight, Fade(BLACK, 0.62f));

    const char *title = "Pausado";
    int titleSize = currentHeight / 11;
    int subtitleSize = currentHeight / 31;
    const char *subtitle = "Aperte ESC ou P para continuar";

    DrawText(
        title,
        (currentWidth / 2) - (MeasureText(title, titleSize) / 2),
        currentHeight / 4,
        titleSize,
        RAYWHITE
    );

    DrawText(
        subtitle,
        (currentWidth / 2) - (MeasureText(subtitle, subtitleSize) / 2),
        currentHeight / 4 + titleSize + 12,
        subtitleSize,
        LIGHTGRAY
    );

    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < optionCount; i++)
    {
        bool hover = CheckCollisionPointRec(mouse, optionRects[i]);
        Color color = hover ? YELLOW : RAYWHITE;
        DrawText(options[i], optionRects[i].x, optionRects[i].y, currentHeight / 20, color);
    }

    DrawMenuCursor(hoveringButton);
}

static void DrawGameplayScene(
    Background *bg,
    Level *currentLevel,
    GamePhase phase,
    float level6IntroProgress,
    int currentWidth,
    int currentHeight,
    float groundY,
    Enemy enemies[],
    EnemyAssets *enemyAssets,
    Player *player,
    float playerScale,
    HairyLeg *pernaCabeluda,
    float bossScale,
    Shark *shark,
    MidnightMan *midnightMan,
    float barValue,
    bool showProgressBar
)
{
    ClearBackground(BLACK);
    DrawBackground(bg, currentLevel->id, currentLevel->bossId, level6IntroProgress, currentWidth, currentHeight, groundY, phase);

    for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
    {
        if (enemies[i].active)
        {
            DrawEnemy(&enemies[i], enemyAssets);
        }
    }

    if (phase == PHASE_BOSS && currentLevel->bossId == 1)
    {
        DrawHairyLegShadowWarning(pernaCabeluda, bossScale);
    }

    if (!(currentLevel->id == 6 && level6IntroProgress < 1.0f))
    {
        DrawPlayer(player, playerScale);
    }

    if (phase == PHASE_BOSS)
    {
        if (currentLevel->bossId == 1)
        {
            DrawHairyLeg(pernaCabeluda, bossScale);
        }

        if (currentLevel->bossId == 2)
        {
            DrawShark(shark);
        }

        if (currentLevel->bossId == 3)
        {
            DrawMidnightMan(midnightMan);
        }
    }

    DrawWater(bg, currentWidth, currentHeight, groundY);
    DrawObjetos(bg, currentWidth, currentHeight, groundY);
    if (showProgressBar)
    {
        DrawProgressBar(bg, barValue, currentWidth, currentHeight);
    }
}

static void DrawInfiniteMetersCounter(float meters, int currentWidth, int currentHeight)
{
    char text[32];
    snprintf(text, sizeof(text), "%.0fm", meters);

    int fontSize = currentHeight / 18;
    int paddingX = currentWidth / 60;
    int paddingY = currentHeight / 80;
    int textW = MeasureText(text, fontSize);
    int boxW = textW + paddingX * 2;
    int boxH = fontSize + paddingY * 2;
    int boxX = (currentWidth - boxW) / 2;
    int boxY = currentHeight * 0.03f;

    DrawRectangleRounded((Rectangle){boxX, boxY, boxW, boxH}, 0.18f, 8, Fade(BLACK, 0.48f));
    DrawRectangleRoundedLines((Rectangle){boxX, boxY, boxW, boxH}, 0.18f, 8, WHITE);
    DrawText(text, boxX + paddingX, boxY + paddingY, fontSize, RAYWHITE);
}

static int GetInfiniteEnemyBaseSpeed(float gameSpeedMultiplier)
{
    float bonus = 15.0f * (gameSpeedMultiplier - 1.0f);
    if (bonus < 0.0f)
    {
        bonus = 0.0f;
    }
    return (int)(bonus + 0.5f);
}

static float GetGameplayBarValue(Level *currentLevel, GamePhase phase, float progressTimer, HairyLeg *pernaCabeluda, Shark *shark)
{
    float barValue = 1.0f;
    if (phase == PHASE_RUNNING && currentLevel->duration > 0.0f)
    {
        barValue = progressTimer / currentLevel->duration;
    }
    else if (phase == PHASE_BOSS)
    {
        if (currentLevel->bossId == 1)
        {
            barValue = (float)pernaCabeluda->health / 100.0f;
        }
        else if (currentLevel->bossId == 2)
        {
            barValue = (float)shark->health / 100.0f;
        }
    }

    return barValue;
}

int main(void)
{
    Config config = CarregarConfig();

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "RainCife");
    HideCursor();
    InitCustomCursor();

    ApplyWindowMode(config.telaCheia != 0);

    SetTargetFPS(60);
    SetExitKey(KEY_DELETE);

    GameScreen currentScreen = SCREEN_START;
    GameScreen optionsReturnScreen = SCREEN_START;

    while (currentScreen != SCREEN_EXIT && !WindowShouldClose())
    {
        if (IsKeyPressed(KEY_F11))
        {
            bool fullscreen = config.telaCheia != 0;
            ToggleWindowMode(&fullscreen);
            config.telaCheia = fullscreen ? 1 : 0;
            SalvarConfig(config);
        }

        if (currentScreen == SCREEN_START)
        {
            GameScreen nextScreen = RunStart();
            if (nextScreen == SCREEN_OPTIONS)
            {
                optionsReturnScreen = SCREEN_START;
            }
            currentScreen = nextScreen;
        }
        else if (currentScreen == SCREEN_OPTIONS)
        {
            currentScreen = RunOptions(&config, optionsReturnScreen);
        }
        else if (currentScreen == SCREEN_CHARACTER_SELECT)
        {
            currentScreen = RunCharacterSelect();
        }
        else if (currentScreen == SCREEN_LEVEL_SELECT)
        {
            GameScreen nextScreen = RunLevelSelect();
            if (nextScreen == SCREEN_OPTIONS)
            {
                optionsReturnScreen = SCREEN_LEVEL_SELECT;
            }
            currentScreen = nextScreen;
        }
        else if (currentScreen == SCREEN_INFINITE_MENU)
        {
            GameScreen nextScreen = RunInfiniteMenu();
            if (nextScreen == SCREEN_OPTIONS)
            {
                optionsReturnScreen = SCREEN_INFINITE_MENU;
            }
            currentScreen = nextScreen;
        }
        else if (currentScreen == SCREEN_ITEMS)
        {
            currentScreen = RunItems();
        }
        else if (currentScreen == SCREEN_INFINITE_SOON)
        {
            currentScreen = RunInfiniteSoon();
        }
        else if (currentScreen == SCREEN_CREDITS)
        {
            currentScreen = RunCredits();
        }
        else if (currentScreen == SCREEN_GAME || currentScreen == SCREEN_INFINITE_GAME)
        {
            bool infiniteMode = currentScreen == SCREEN_INFINITE_GAME;
            Background bg;
            InitBackground(&bg);

            int initW = GetScreenWidth();
            int initH = GetScreenHeight();
            float initGroundY = initH * GROUND_RATIO;
            float initBossScale = (float)initH * 0.65f / 252.0f;

            Player player;
            Vector2 startPos = { 100, initGroundY };
            InitPlayer(&player, startPos, SPEED);

            HairyLeg pernaCabeluda;
            InitHairyLeg(&pernaCabeluda, (Vector2){ (float)initW * 0.6f, initGroundY }, initGroundY, initBossScale);

            Shark shark;
            InitShark(&shark, initW, initH);

            MidnightMan midnightMan;
            InitMidnightMan(&midnightMan, initW, initH, initGroundY);

            Level *levels = infiniteMode ? InitInfiniteLevels() : InitGameLevels();
            Level *currentLevel = infiniteMode ? levels : FindLevelById(levels, GetSelectedStoryLevelId());

            Enemy enemies[MAX_ACTIVE_ENEMIES] = {0};
            EnemyAssets enemyAssets = {0};
            enemyAssets.birdAnimation = LoadAnimation("assets/sprites/Enemys_obstacles/Bird-Sheet.png", 7, 0.1f);
            enemyAssets.textures[ENEMY_BIKE] = LoadTexture("assets/sprites/Enemys_obstacles/Bike.png");
            enemyAssets.textures[ENEMY_WOOD] = LoadTexture("assets/sprites/Enemys_obstacles/Tree.png");
            enemyAssets.textures[ENEMY_POSTE] = LoadTexture("assets/sprites/Enemys_obstacles/Poste_mal_completo.png");
            enemyAssets.textures[ENEMY_FISH] = LoadTexture("assets/sprites/Enemys_obstacles/Fish.png");
            enemyAssets.textures[ENEMY_SAFE_POSTE] = LoadTexture("assets/sprites/Enemys_obstacles/Poste_inofensivo.png");

            enemyAssets.posteSemCabeca = LoadTexture("assets/sprites/Enemys_obstacles/Poste_mal_sem_cabeca.png");
            enemyAssets.posteCabecas = LoadTexture("assets/sprites/Enemys_obstacles/Poste_mal_cabecas.png");
            enemyAssets.fishWaterJump = LoadTexture("assets/sprites/Enemys_obstacles/Fish_water_jump.png");
            enemyAssets.fishAnticipation = LoadAnimation("assets/sprites/Enemys_obstacles/Fish_antecipation_water-Sheet.png", 3, 0.2f);
            enemyAssets.birdDeath = LoadTexture("assets/sprites/Enemys_obstacles/Bird_death.png");
            enemyAssets.fishDeath = LoadTexture("assets/sprites/Enemys_obstacles/Fish_death.png");
            enemyAssets.destroyedSheet = LoadTexture("assets/sprites/Enemys_obstacles/Destroyed-Sheet.png");
            enemyAssets.bikeSkin2 = LoadTexture("assets/sprites/Enemys_obstacles/Bike_2.png");
            enemyAssets.bikeSkinItau = LoadTexture("assets/sprites/Enemys_obstacles/Bike_itau.png");

            GamePhase phase = PHASE_RUNNING;
            float progressTimer = 0.0f;
            bool level6IntroActive = false;
            float level6IntroTimer = 0.0f;

            // player.isBossFighting = (currentLevel->bossId != 0);
            player.isBossFighting = false;

            bool autoSpawn = true;
            float spawnTimer = 0.0f;
            // const float spawnInterval = 1.5f;
            float safePosteFollowUpTimer = -1.0f;
            bool deathScreenActive = false;
            bool gamePaused = false;
            GamePhase retryPhase = PHASE_RUNNING;
            float infiniteMeters = 0.0f;
            float infiniteNextSpeedStepMeters = INFINITE_SPEED_STEP_METERS;
            float infiniteSpeedMultiplier = 1.0f;

            while (!WindowShouldClose() && (currentScreen == SCREEN_GAME || currentScreen == SCREEN_INFINITE_GAME))
            {
                if (!deathScreenActive && (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P)))
                {
                    gamePaused = !gamePaused;
                }

                float dt = GetFrameTime();
                UpdateCustomCursor(dt);
                int currentWidth = GetScreenWidth();
                int currentHeight = GetScreenHeight();
                float groundY = currentHeight * GROUND_RATIO;
                float playerScale = (float)currentHeight * 0.45f / 300.0f;
                float bossScale = (float)currentHeight * 0.65f / 252.0f;
                float standingY = groundY + (currentHeight * SIDEWALK_THICKNESS_RATIO * -0.2f);
                float playerStandingY = groundY + (currentHeight * SIDEWALK_THICKNESS_RATIO * 0.1f);
                float level6IntroProgress = 1.0f;

                if (currentLevel->id == 6)
                {
                    level6IntroProgress = level6IntroTimer / LEVEL6_INTRO_DURATION;
                }

                if (gamePaused)
                {
                    const char *pauseOptions[] =
                    {
                        "Continuar",
                        "Menu"
                    };
                    int pauseOptionCount = sizeof(pauseOptions) / sizeof(pauseOptions[0]);
                    int pauseFontSize = currentHeight / 20;
                    int pauseSpacing = pauseFontSize + 22;
                    int pauseStartY = currentHeight / 2;
                    Rectangle pauseOptionRects[pauseOptionCount];
                    BuildOptionRects(pauseOptionRects, pauseOptions, pauseOptionCount, pauseFontSize, currentWidth / 2, pauseStartY, pauseSpacing);

                    Vector2 mouse = GetMousePosition();
                    bool hoveringButton = false;
                    for (int i = 0; i < pauseOptionCount; i++)
                    {
                        if (CheckCollisionPointRec(mouse, pauseOptionRects[i]))
                        {
                            hoveringButton = true;
                            break;
                        }
                    }

                    int clickedPauseOption = GetClickedOption(pauseOptionRects, pauseOptionCount);
                    if (clickedPauseOption == 0)
                    {
                        gamePaused = false;
                    }
                    else if (clickedPauseOption == 1)
                    {
                        currentScreen = SCREEN_START;
                    }

                    float barValue = GetGameplayBarValue(currentLevel, phase, progressTimer, &pernaCabeluda, &shark);

                    BeginDrawing();
                        DrawGameplayScene(&bg, currentLevel, phase, level6IntroProgress, currentWidth, currentHeight, groundY, enemies, &enemyAssets, &player, playerScale, &pernaCabeluda, bossScale, &shark, &midnightMan, barValue, !infiniteMode);
                        if (infiniteMode)
                        {
                            DrawInfiniteMetersCounter(infiniteMeters, currentWidth, currentHeight);
                        }
                        DrawPauseScreenOverlay(currentWidth, currentHeight, pauseOptionRects, pauseOptions, pauseOptionCount, hoveringButton);
                    EndDrawing();
                    continue;
                }

                if (currentLevel->id == 6 && level6IntroActive)
                {
                    level6IntroTimer += dt;
                    if (level6IntroTimer >= LEVEL6_INTRO_DURATION)
                    {
                        level6IntroTimer = LEVEL6_INTRO_DURATION;
                        level6IntroActive = false;
                    }
                    level6IntroProgress = level6IntroTimer / LEVEL6_INTRO_DURATION;
                }

                if (deathScreenActive)
                {
                    const char *deathOptions[] =
                    {
                        "Continuar",
                        "Menu"
                    };
                    int deathOptionCount = sizeof(deathOptions) / sizeof(deathOptions[0]);
                    int deathFontSize = currentHeight / 20;
                    int deathSpacing = deathFontSize + 22;
                    int deathStartY = currentHeight / 2;
                    Rectangle deathOptionRects[deathOptionCount];
                    BuildOptionRects(deathOptionRects, deathOptions, deathOptionCount, deathFontSize, currentWidth / 2, deathStartY, deathSpacing);

                    Vector2 mouse = GetMousePosition();
                    bool hoveringButton = false;
                    for (int i = 0; i < deathOptionCount; i++)
                    {
                        if (CheckCollisionPointRec(mouse, deathOptionRects[i]))
                        {
                            hoveringButton = true;
                            break;
                        }
                    }

                    int clickedDeathOption = GetClickedOption(deathOptionRects, deathOptionCount);
                    if (clickedDeathOption == 0)
                    {
                        if (infiniteMode)
                        {
                            currentLevel = levels;
                            phase = PHASE_RUNNING;
                            progressTimer = 0.0f;
                            autoSpawn = true;
                            spawnTimer = 0.0f;
                            infiniteMeters = 0.0f;
                            infiniteNextSpeedStepMeters = INFINITE_SPEED_STEP_METERS;
                            infiniteSpeedMultiplier = 1.0f;
                            for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
                            {
                                enemies[i].active = false;
                                enemies[i].dying = false;
                            }
                            ResetPlayerForRunningRetry(&player, playerStandingY, playerScale);
                        }
                        else
                        {
                            RestartCurrentEncounter(
                                currentLevel,
                                retryPhase,
                                &phase,
                                &progressTimer,
                                &autoSpawn,
                                &spawnTimer,
                                enemies,
                                &player,
                                &pernaCabeluda,
                                &shark,
                                currentWidth,
                                currentHeight,
                                groundY,
                                bossScale,
                                playerStandingY,
                                playerScale
                            );
                        }
                        safePosteFollowUpTimer = -1.0f;
                        deathScreenActive = false;
                    }
                    else if (clickedDeathOption == 1)
                    {
                        currentScreen = SCREEN_START;
                    }

                    BeginDrawing();
                        ClearBackground(BLACK);
                        DrawBackground(&bg, currentLevel->id, currentLevel->bossId, level6IntroProgress, currentWidth, currentHeight, groundY, phase);

                        for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
                        {
                            if (enemies[i].active)
                            {
                                DrawEnemy(&enemies[i], &enemyAssets);
                            }
                        }

                        if (phase == PHASE_BOSS)
                        {
                            if (currentLevel->bossId == 1)
                            {
                                DrawHairyLegShadowWarning(&pernaCabeluda, bossScale);
                            }
                        }

                        DrawPlayer(&player, playerScale);

                        if (phase == PHASE_BOSS)
                        {
                            if (currentLevel->bossId == 1)
                            {
                                DrawHairyLeg(&pernaCabeluda, bossScale);
                            }

                            if (currentLevel->bossId == 2)
                            {
                                DrawShark(&shark);
                            }
                        }

                        DrawWater(&bg, currentWidth, currentHeight, groundY);
                        DrawObjetos(&bg, currentWidth, currentHeight, groundY);

                        DrawDeathScreenOverlay(currentWidth, currentHeight, deathOptionRects, deathOptions, deathOptionCount, hoveringButton);
                    EndDrawing();
                    continue;
                }

                int infiniteEnemyBaseSpeed = infiniteMode ? GetInfiniteEnemyBaseSpeed(infiniteSpeedMultiplier) : 0;

                if (phase == PHASE_RUNNING)
                {
                    progressTimer += dt;

                    if (infiniteMode)
                    {
                        infiniteMeters += INFINITE_METERS_PER_SECOND * infiniteSpeedMultiplier * dt;
                        while (infiniteMeters >= infiniteNextSpeedStepMeters)
                        {
                            infiniteSpeedMultiplier *= INFINITE_SPEED_MULTIPLIER_STEP;
                            infiniteNextSpeedStepMeters += INFINITE_SPEED_STEP_METERS;
                            if (currentLevel->next != NULL)
                            {
                                currentLevel = currentLevel->next;
                            }
                        }
                    }
                    else if (progressTimer >= currentLevel->duration)
                    {
                        EnterBossPhase(
                            currentLevel,
                            &phase,
                            &progressTimer,
                            &autoSpawn,
                            &safePosteFollowUpTimer,
                            enemies,
                            &player,
                            &pernaCabeluda,
                            &shark,
                            currentWidth,
                            currentHeight,
                            groundY,
                            bossScale,
                            playerStandingY,
                            playerScale
                        );
                        if (currentLevel->bossId == 3)
                        {
                            InitMidnightMan(&midnightMan, currentWidth, currentHeight, groundY);
                        }
                    }

                    spawnTimer -= dt;
                    if (spawnTimer <= 0.0f)
                    {
                        EnemyType chosen = SortearInimigoFase(currentLevel->enemyConfigId);

                        if (chosen == ENEMY_BIRD1)
                        {
                            for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
                            {
                                if (enemies[i].active && enemies[i].type == ENEMY_BIRD1)
                                {
                                    chosen = SortearInimigoFase(currentLevel->enemyConfigId);
                                    break;
                                }
                            }
                        }

                        for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
                        {
                            if (!enemies[i].active)
                            {
                                if (infiniteMode)
                                {
                                    InitEnemyTuned(&enemies[i], chosen, currentWidth, currentHeight, infiniteEnemyBaseSpeed, 0.55, -30.0f);
                                }
                                else
                                {
                                    InitEnemy(&enemies[i], chosen, currentWidth, currentHeight, infiniteEnemyBaseSpeed);
                                }
                                if (chosen == ENEMY_SAFE_POSTE)
                                {
                                    safePosteFollowUpTimer = 0.5f;
                                }
                                break;
                            }
                        }
                        spawnTimer = currentLevel->spawnInterval;
                    }

                    if (safePosteFollowUpTimer > 0.0f)
                    {
                        safePosteFollowUpTimer -= dt;
                        if (safePosteFollowUpTimer <= 0.0f)
                        {
                            safePosteFollowUpTimer = -1.0f;
                            EnemyType followUp;

                            do
                            {
                                followUp = SortearInimigoFase(currentLevel->enemyConfigId);
                            }
                            while (followUp == ENEMY_SAFE_POSTE);
                            
                            for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
                            {
                                if (!enemies[i].active)
                                {
                                    if (infiniteMode)
                                    {
                                        InitEnemyTuned(&enemies[i], followUp, currentWidth, currentHeight, infiniteEnemyBaseSpeed, 0.55, -30.0f);
                                    }
                                    else
                                    {
                                        InitEnemy(&enemies[i], followUp, currentWidth, currentHeight, infiniteEnemyBaseSpeed);
                                    }
                                    break;
                                }
                            }

                            spawnTimer = currentLevel->spawnInterval;
                        }
                    }
                }

                Rectangle playerHitbox = GetPlayerHitbox(&player, playerScale);

                for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
                {
                    UpdateEnemy(&enemies[i], currentWidth, currentHeight, infiniteEnemyBaseSpeed, playerHitbox);
                    if (enemies[i].active && enemies[i].type != ENEMY_SAFE_POSTE)
                    {
                        if (CheckCollisionRecs(playerHitbox, GetEnemyHitbox(&enemies[i])))
                        {
                            deathScreenActive = true;
                            retryPhase = phase;
                        }
                    }
                }

                if (IsPlayerAttackHitboxActive(&player))
                {
                    Rectangle hammerHitbox = GetPlayerAttackHitbox(&player, playerScale);
                    Rectangle otherHitbox  = GetPlayerAttackHitbox(&player, playerScale);
                    for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
                    {
                        if (!enemies[i].active) continue;
                        Rectangle attackHitbox = (player.weapon.type == WEAPON_HAMMER) ? hammerHitbox : otherHitbox;
                        if (!CanWeaponBreakEnemy(player.weapon.type, enemies[i].type)) continue;
                        if (CheckCollisionRecs(attackHitbox, GetEnemyHitbox(&enemies[i])))
                        {
                            enemies[i].dying = true;
                            enemies[i].velocity.x = -8.0f;
                            enemies[i].velocity.y = -5.0f;
                            enemies[i].animationTimer = 0.0f;
                            enemies[i].currentFrame = 0;
                            if (enemies[i].type == ENEMY_POSTE)
                            {
                                enemies[i].headDetached = true;
                                enemies[i].headLanded = false;
                            }
                        }
                    }
                }

                UpdateBackground(&bg, infiniteMode ? dt * infiniteSpeedMultiplier : dt, phase);
                UpdateObjetos(&bg, infiniteMode ? dt * infiniteSpeedMultiplier : dt, currentWidth, currentHeight, groundY, phase);
                UpdatePlayer(&player, dt, playerStandingY, playerScale, &config);
                playerHitbox = GetPlayerHitbox(&player, playerScale);

                if (!infiniteMode && IsKeyPressed(KEY_RIGHT) && phase == PHASE_RUNNING && currentLevel->bossId != 0) {
                    EnterBossPhase(
                        currentLevel,
                        &phase,
                        &progressTimer,
                        &autoSpawn,
                        &safePosteFollowUpTimer,
                        enemies,
                        &player,
                        &pernaCabeluda,
                        &shark,
                        currentWidth,
                        currentHeight,
                        groundY,
                        bossScale,
                        playerStandingY,
                        playerScale
                    );
                    if (currentLevel->bossId == 3)
                    {
                        InitMidnightMan(&midnightMan, currentWidth, currentHeight, groundY);
                    }
                }
                else if (!infiniteMode && IsKeyPressed(KEY_RIGHT) && phase == PHASE_BOSS && currentLevel->next) {
                    UnlockStoryLevel(currentLevel->next->id);
                    StartLevel(
                        &currentLevel,
                        currentLevel->next,
                        &phase,
                        &progressTimer,
                        &autoSpawn,
                        &spawnTimer,
                        enemies,
                        &player,
                        &pernaCabeluda,
                        &shark,
                        currentWidth,
                        currentHeight,
                        groundY,
                        bossScale
                    );
                    safePosteFollowUpTimer = -1.0f;
                    ResetPlayerForRunningRetry(&player, playerStandingY, playerScale);
                    if (currentLevel->bossId == 3)
                    {
                        InitMidnightMan(&midnightMan, currentWidth, currentHeight, groundY);
                    }
                    level6IntroActive = (currentLevel->id == 6);
                    level6IntroTimer = 0.0f;
                }
                if (!infiniteMode && IsKeyPressed(KEY_LEFT) && currentLevel->prev) {
                    StartLevel(
                        &currentLevel,
                        currentLevel->prev,
                        &phase,
                        &progressTimer,
                        &autoSpawn,
                        &spawnTimer,
                        enemies,
                        &player,
                        &pernaCabeluda,
                        &shark,
                        currentWidth,
                        currentHeight,
                        groundY,
                        bossScale
                    );
                    safePosteFollowUpTimer = -1.0f;
                    ResetPlayerForRunningRetry(&player, playerStandingY, playerScale);
                    if (currentLevel->bossId == 3)
                    {
                        InitMidnightMan(&midnightMan, currentWidth, currentHeight, groundY);
                    }
                    level6IntroActive = (currentLevel->id == 6);
                    level6IntroTimer = 0.0f;
                }

                if (phase == PHASE_BOSS)
                {
                    bool bossDefeatedThisFrame = false;

                    if (currentLevel->bossId == 1)
                    {
                        UpdateHairyLeg(&pernaCabeluda, playerHitbox, dt, standingY, bossScale);
                        TryDamageHairyLegFromPlayerAttack(&pernaCabeluda, &player, playerScale);

                        if (pernaCabeluda.state == HL_DEAD && pernaCabeluda.timer >= HAIRY_LEG_DEATH_ADVANCE_DELAY && currentLevel->next)
                        {
                            UnlockStoryLevel(currentLevel->next->id);
                            StartLevel(&currentLevel, currentLevel->next, &phase, &progressTimer, &autoSpawn, &spawnTimer, enemies, &player, &pernaCabeluda, &shark, currentWidth, currentHeight, groundY, bossScale);
                            bossDefeatedThisFrame = true;
                        }
                    }

                    if (!bossDefeatedThisFrame && currentLevel->bossId == 2)
                    {
                        UpdateShark(&shark, playerHitbox, dt, currentWidth, currentHeight);
                        TryDamageSharkFromPlayerAttack(&shark, &player, playerScale);

                        if (!shark.active && currentLevel->next)
                        {
                            UnlockStoryLevel(currentLevel->next->id);
                            StartLevel(&currentLevel, currentLevel->next, &phase, &progressTimer, &autoSpawn, &spawnTimer, enemies, &player, &pernaCabeluda, &shark, currentWidth, currentHeight, groundY, bossScale);
                            bossDefeatedThisFrame = true;
                        }
                    }

                    if (currentLevel->bossId == 3)
                    {
                        UpdateMidnightMan(&midnightMan, playerHitbox, dt, currentWidth, currentHeight, groundY);
                    }

                    if (!bossDefeatedThisFrame && currentLevel->bossId == 1 && pernaCabeluda.state != HL_DEAD)
                    {
                        if (CheckCollisionRecs(playerHitbox, pernaCabeluda.rect))
                        {
                            deathScreenActive = true;
                            retryPhase = PHASE_BOSS;
                        }

                        if (IsHairyLegKickColliding(&pernaCabeluda, playerHitbox))
                        {
                            deathScreenActive = true;
                            retryPhase = PHASE_BOSS;
                        }

                        if (pernaCabeluda.waveLeft.active && CheckCollisionRecs(playerHitbox, pernaCabeluda.waveLeft.rect))
                        {
                            deathScreenActive = true;
                            retryPhase = PHASE_BOSS;
                        }

                        if (pernaCabeluda.waveRight.active && CheckCollisionRecs(playerHitbox, pernaCabeluda.waveRight.rect))
                        {
                            deathScreenActive = true;
                            retryPhase = PHASE_BOSS;
                        }
                    }

                    if (!bossDefeatedThisFrame && currentLevel->bossId == 2)
                    {
                        if (CheckCollisionRecs(playerHitbox, GetSharkHitbox(&shark)))
                        {
                            deathScreenActive = true;
                            retryPhase = PHASE_BOSS;
                        }

                        for (int i = 0; i < MAX_WATER_BALLS; i++)
                        {
                            if (shark.balls[i].active && CheckCollisionRecs(playerHitbox, shark.balls[i].hitbox))
                            {
                                deathScreenActive = true;
                                retryPhase = PHASE_BOSS;
                            }
                        }
                    }

                }

                float barValue = GetGameplayBarValue(currentLevel, phase, progressTimer, &pernaCabeluda, &shark);

                BeginDrawing();
                    DrawGameplayScene(&bg, currentLevel, phase, level6IntroProgress, currentWidth, currentHeight, groundY, enemies, &enemyAssets, &player, playerScale, &pernaCabeluda, bossScale, &shark, &midnightMan, barValue, !infiniteMode);
                    if (infiniteMode)
                    {
                        DrawInfiniteMetersCounter(infiniteMeters, currentWidth, currentHeight);
                    }
                    DrawGameplayCursor(player.weapon.attacking);
                EndDrawing();
            }

            UnloadPlayer(&player);
            UnloadBackground(&bg);
            for (int i = 0; i < ENEMY_COUNT; i++)
            {
                UnloadTexture(enemyAssets.textures[i]);
            }
            UnloadTexture(enemyAssets.posteSemCabeca);
            UnloadTexture(enemyAssets.posteCabecas);
            UnloadTexture(enemyAssets.fishWaterJump);
            UnloadAnimation(&enemyAssets.fishAnticipation);
            UnloadAnimation(&enemyAssets.birdAnimation);
            UnloadTexture(enemyAssets.bikeSkin2);
            UnloadTexture(enemyAssets.bikeSkinItau);
            UnloadTexture(enemyAssets.birdDeath);
            UnloadTexture(enemyAssets.fishDeath);
            UnloadTexture(enemyAssets.destroyedSheet);
            UnloadShark(&shark);
            UnloadMidnightMan(&midnightMan);
            FreeLevels(levels);
        }
    }

    UnloadCustomCursor();
    ShowCursor();
    CloseWindow();

    return 0;
}
