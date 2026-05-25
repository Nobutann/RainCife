#include <raylib.h>
#include <stddef.h>
#include <stdio.h>
#include "core/screens.h"
#include "core/config_manager.h"
#include "core/ranking_manager.h"
#include "core/daily_challenges.h"
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
#include "core/sounds.h"
#include "core/intro_screen.h"
#define MAX_ACTIVE_ENEMIES 12
#define LEVEL6_INTRO_DURATION 2.5f
#define BOSS_DEFEAT_TRANSITION_DELAY 2.5f
#define INFINITE_SPEED_STEP_METERS 5000.0f
#define INFINITE_SPEED_MULTIPLIER_STEP 1.10f
#define INFINITE_PHASE_1_DURATION 40.0f
#define INFINITE_PHASE_2_DURATION 50.0f
#define INFINITE_PHASE_3_DURATION 60.0f
#define INFINITE_PHASE_4_DURATION 70.0f
#define INFINITE_VISUAL_PHASE_COUNT 3
#define INFINITE_MAX_DIFFICULTY_LEVEL 3

static float GetInfinitePhaseDuration(float meters)
{
    if (meters < INFINITE_SPEED_STEP_METERS)
    {
        return INFINITE_PHASE_1_DURATION;
    }
    if (meters < INFINITE_SPEED_STEP_METERS * 2.0f)
    {
        return INFINITE_PHASE_2_DURATION;
    }
    if (meters < INFINITE_SPEED_STEP_METERS * 3.0f)
    {
        return INFINITE_PHASE_3_DURATION;
    }
    return INFINITE_PHASE_4_DURATION;
}

static float GetInfiniteMetersPerSecond(float meters)
{
    return INFINITE_SPEED_STEP_METERS / GetInfinitePhaseDuration(meters);
}

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

    ResetHairyLeg(pernaCabeluda, (Vector2){ (float)currentWidth * 0.6f, groundY }, groundY, bossScale);
    ResetShark(shark, currentWidth, currentHeight);
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

static int GetInfiniteVisualLevelId(float meters)
{
    int phaseIndex = (int)(meters / INFINITE_SPEED_STEP_METERS) % INFINITE_VISUAL_PHASE_COUNT;
    return phaseIndex + 1;
}

static int GetInfiniteDifficultyLevelId(float meters)
{
    int levelId = (int)(meters / INFINITE_SPEED_STEP_METERS) + 1;
    if (levelId > INFINITE_MAX_DIFFICULTY_LEVEL)
    {
        levelId = INFINITE_MAX_DIFFICULTY_LEVEL;
    }
    return levelId;
}

static void SyncInfiniteLevels(Level *levels, float meters, Level **visualLevel, Level **difficultyLevel)
{
    *visualLevel = FindLevelById(levels, GetInfiniteVisualLevelId(meters));
    *difficultyLevel = FindLevelById(levels, GetInfiniteDifficultyLevelId(meters));
}

static void AdvanceInfiniteMeters(
    Level *levels,
    float metersToAdvance,
    float *meters,
    float *nextSpeedStepMeters,
    float *speedMultiplier,
    Level **visualLevel,
    Level **difficultyLevel
)
{
    float targetMeters = *meters + metersToAdvance;
    while (*nextSpeedStepMeters <= targetMeters)
    {
        *speedMultiplier *= INFINITE_SPEED_MULTIPLIER_STEP;
        *nextSpeedStepMeters += INFINITE_SPEED_STEP_METERS;
    }

    *meters = targetMeters;
    SyncInfiniteLevels(levels, *meters, visualLevel, difficultyLevel);
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
    MidnightMan *midnightMan,
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
            ResetHairyLeg(pernaCabeluda, (Vector2){(float)currentWidth * 0.6f, groundY}, groundY, bossScale);
            PlacePlayerForBossIntro(player, pernaCabeluda->rect, playerStandingY, playerScale);
        }
        else if (currentLevel->bossId == 2)
        {
            ResetShark(shark, currentWidth, currentHeight);
            PlacePlayerForBossIntro(player, GetSharkHitbox(shark), playerStandingY, playerScale);
        }
        else if (currentLevel->bossId == 3)
        {
            InitMidnightMan(midnightMan, currentWidth, currentHeight, groundY);
            Rectangle bossPlaceholder = { (float)currentWidth * 0.7f, groundY - 150.0f, 100.0f, 150.0f };
            PlacePlayerForBossIntro(player, bossPlaceholder, playerStandingY, playerScale);
        }
        return;
    }

    *phase = PHASE_RUNNING;
    *progressTimer = 0.0f;
    *autoSpawn = true;
    ResetHairyLeg(pernaCabeluda, (Vector2){ (float)currentWidth * 0.6f, groundY }, groundY, bossScale);
    ResetShark(shark, currentWidth, currentHeight);
    if (currentLevel->bossId == 3)
    {
        InitMidnightMan(midnightMan, currentWidth, currentHeight, groundY);
    }
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
        ResetHairyLeg(pernaCabeluda, (Vector2){(float)currentWidth * 0.6f, groundY}, groundY, bossScale);
        PlacePlayerForBossIntro(player, pernaCabeluda->rect, playerStandingY, playerScale);
    }
    else if (currentLevel->bossId == 2)
    {
        ResetShark(shark, currentWidth, currentHeight);
        PlacePlayerForBossIntro(player, GetSharkHitbox(shark), playerStandingY, playerScale);
    }
}

static void DrawDeathScreenOverlay(int currentWidth, int currentHeight, Rectangle optionRects[], const char **options, int optionCount, bool hoveringButton)
{
    Vector2 mouse = GetMousePosition();

    static Texture2D deathScreen = {0};
    static Texture2D deathHome = {0};
    static Texture2D deathRetry = {0};
    if (deathScreen.id <= 0)
    {
        deathScreen = LoadTexture("assets/sprites/ui/result/morreu.png");
        deathHome = LoadTexture("assets/sprites/ui/result/morreu_home.png");
        deathRetry = LoadTexture("assets/sprites/ui/result/morreu_retry.png");
    }
    Texture2D deathOverlay = deathScreen;
    if (optionCount >= 2 && CheckCollisionPointRec(mouse, optionRects[0]))
    {
        deathOverlay = deathRetry;
    }
    else if (optionCount >= 2 && CheckCollisionPointRec(mouse, optionRects[1]))
    {
        deathOverlay = deathHome;
    }
    DrawRectangle(0, 0, currentWidth, currentHeight, Fade(BLACK, 0.72f));
    DrawFullscreenTexture(deathOverlay, currentWidth, currentHeight);
    DrawMenuCursor(hoveringButton);
    (void)options;
}

static void DrawPauseScreenOverlay(int currentWidth, int currentHeight, Rectangle optionRects[], int optionCount, bool hoveringButton)
{
    static Texture2D pauseScreen = {0};
    static Texture2D pauseBack = {0};
    static Texture2D pauseHome = {0};
    if (pauseScreen.id <= 0)
    {
        pauseScreen = LoadTexture("assets/sprites/ui/result/pausado.png");
        pauseBack = LoadTexture("assets/sprites/ui/result/pausado_voltar.png");
        pauseHome = LoadTexture("assets/sprites/ui/result/pausado_home.png");
    }

    Vector2 mouse = GetMousePosition();
    Texture2D pauseOverlay = pauseScreen;
    if (optionCount >= 2 && CheckCollisionPointRec(mouse, optionRects[0]))
    {
        pauseOverlay = pauseBack;
    }
    else if (optionCount >= 2 && CheckCollisionPointRec(mouse, optionRects[1]))
    {
        pauseOverlay = pauseHome;
    }

    DrawRectangle(0, 0, currentWidth, currentHeight, Fade(BLACK, 0.55f));
    DrawFullscreenTexture(pauseOverlay, currentWidth, currentHeight);
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
    bool showProgressBar,
    bool showStageFront
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
    DrawWaterSplashes(bg);
    DrawObjetos(bg, currentWidth, currentHeight, groundY);
    DrawRain(bg, currentWidth, currentHeight);
    if (showStageFront)
    {
        DrawStageFront(bg, currentWidth, currentHeight);
    }
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

static void DrawInfiniteRankingNameOverlay(int currentWidth, int currentHeight, float meters, const char *name)
{
    DrawRectangle(0, 0, currentWidth, currentHeight, Fade(BLACK, 0.82f));

    const char *title = "Novo Top 10!";
    const char *subtitle = "Digite seu nome";
    const char *hint = "ENTER salva";
    int titleSize = currentHeight / 12;
    int subtitleSize = currentHeight / 28;
    int inputSize = currentHeight / 22;
    int hintSize = currentHeight / 34;

    char scoreText[48];
    snprintf(scoreText, sizeof(scoreText), "%.0fm", meters);

    DrawText(
        title,
        (currentWidth / 2) - (MeasureText(title, titleSize) / 2),
        currentHeight / 4,
        titleSize,
        GOLD
    );
    DrawText(
        scoreText,
        (currentWidth / 2) - (MeasureText(scoreText, subtitleSize) / 2),
        currentHeight / 4 + titleSize + 8,
        subtitleSize,
        RAYWHITE
    );
    DrawText(
        subtitle,
        (currentWidth / 2) - (MeasureText(subtitle, subtitleSize) / 2),
        currentHeight / 4 + titleSize + subtitleSize + 18,
        subtitleSize,
        LIGHTGRAY
    );

    int boxW = currentWidth * 0.44f;
    int boxH = inputSize + 24;
    int boxX = (currentWidth - boxW) / 2;
    int boxY = currentHeight / 2;
    DrawRectangleRounded((Rectangle){boxX, boxY, boxW, boxH}, 0.12f, 8, RAYWHITE);
    DrawRectangleRoundedLines((Rectangle){boxX, boxY, boxW, boxH}, 0.12f, 8, GOLD);

    const char *shownName = name[0] == '\0' ? "Jogador" : name;
    int nameW = MeasureText(shownName, inputSize);
    DrawText(shownName, boxX + (boxW - nameW) / 2, boxY + 12, inputSize, DARKGRAY);

    DrawText(
        hint,
        (currentWidth / 2) - (MeasureText(hint, hintSize) / 2),
        boxY + boxH + 18,
        hintSize,
        LIGHTGRAY
    );
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

static float GetGameplayBarValue(Level *currentLevel, GamePhase phase, float progressTimer, HairyLeg *pernaCabeluda, Shark *shark, MidnightMan *midnightMan)
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
        else if (currentLevel->bossId == 3)
        {
            barValue = (float)midnightMan->health / 200.0f;
        }
    }

    return barValue;
}

typedef enum { TRANSITION_RUNNING_TO_BOSS, TRANSITION_BOSS_TO_RUNNING } PhaseTransitionType;

static void DrawPhaseTransitionOverlay(int currentWidth, int currentHeight, Rectangle advanceRect, Rectangle menuRect, int btnFontSize, const char *title, bool hoveringButton)
{
    Vector2 mouse = GetMousePosition();

    static Texture2D passedScreen = {0};
    static Texture2D passedHome = {0};
    static Texture2D passedNext = {0};
    if (passedScreen.id <= 0)
    {
        passedScreen = LoadTexture("assets/sprites/ui/result/passou.png");
        passedHome = LoadTexture("assets/sprites/ui/result/passou_home.png");
        passedNext = LoadTexture("assets/sprites/ui/result/passou_passou.png");
    }
    Texture2D passedOverlay = passedScreen;
    if (CheckCollisionPointRec(mouse, menuRect))
    {
        passedOverlay = passedHome;
    }
    else if (CheckCollisionPointRec(mouse, advanceRect))
    {
        passedOverlay = passedNext;
    }
    DrawRectangle(0, 0, currentWidth, currentHeight, Fade(BLACK, 0.55f));
    DrawFullscreenTexture(passedOverlay, currentWidth, currentHeight);

    DrawMenuCursor(hoveringButton);
    (void)btnFontSize;
    (void)title;
}

int main(void)
{
    Config config = CarregarConfig();

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(640, 360, "RainCife");
    HideCursor();
    InitCustomCursor();
    InitSoundSystem();
    SetSoundSystemVolume(config.volume);

    ApplyWindowMode(config.telaCheia != 0);

    SetTargetFPS(60);
    SetExitKey(KEY_DELETE);

    GameScreen currentScreen = SCREEN_INTRO;
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

        if (currentScreen == SCREEN_INTRO)
        {
            currentScreen = RunIntro();
            if (currentScreen == SCREEN_START)
            {
                SetHistoryAnimationReturnScreen(SCREEN_START);
                currentScreen = SCREEN_HISTORY_ANIMATION;
            }
        }
        else if (currentScreen == SCREEN_START)
        {
            GameScreen nextScreen = RunStart();
            if (nextScreen == SCREEN_OPTIONS)
            {
                optionsReturnScreen = SCREEN_START;
            }
            currentScreen = nextScreen;
        }
        else if (currentScreen == SCREEN_HISTORY_ANIMATION)
        {
            currentScreen = RunHistoryAnimation();
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
        else if (currentScreen == SCREEN_DAILY_CHALLENGES)
        {
            currentScreen = RunDailyChallenges();
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

            Player player = {0};
            Vector2 startPos = { 100, initGroundY };
            InitPlayer(&player, startPos, SPEED);

            ProjectileSystem projectiles = {0};
            InitProjectileSystem(&projectiles);
            SetProjectileSystem(&projectiles);

            HairyLeg pernaCabeluda = {0};
            Shark shark = {0};
            MidnightMan midnightMan = {0};
            if (!infiniteMode)
            {
                InitHairyLeg(&pernaCabeluda, (Vector2){ (float)initW * 0.6f, initGroundY }, initGroundY, initBossScale);
                InitShark(&shark, initW, initH);
                InitMidnightMan(&midnightMan, initW, initH, initGroundY);
            }

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
            bool level6IntroActive = (currentLevel->id == 6);
            float level6IntroTimer = 0.0f;
            float level6IntroDuration = LEVEL6_INTRO_DURATION;

            // player.isBossFighting = (currentLevel->bossId != 0);
            player.isBossFighting = false;

            bool autoSpawn = true;
            float spawnTimer = 0.0f;
            // const float spawnInterval = 1.5f;
            float safePosteFollowUpTimer = -1.0f;
            bool deathScreenActive = false;
            bool gamePaused = false;
            GamePhase retryPhase = PHASE_RUNNING;
            bool phaseTransitionActive = false;
            PhaseTransitionType transitionType = TRANSITION_RUNNING_TO_BOSS;
            bool bossDefeatTransitionPending = false;
            float bossDefeatTransitionTimer = 0.0f;
            RankingInfinito infiniteRanking = infiniteMode ? CarregarRankingInfinito() : (RankingInfinito){0};
            bool infiniteRankingChecked = false;
            bool infiniteRankingNameActive = false;
            char infinitePlayerName[INFINITE_PLAYER_NAME_MAX] = "";
            int infinitePlayerNameLength = 0;
            float infiniteFinalMeters = 0.0f;
            float infiniteMeters = 0.0f;
            float infiniteNextSpeedStepMeters = INFINITE_SPEED_STEP_METERS;
            float infiniteSpeedMultiplier = 1.0f;
            bool infiniteRunRecorded = false;
            Level *infiniteDifficultyLevel = infiniteMode ? currentLevel : NULL;

            while (!WindowShouldClose() && (currentScreen == SCREEN_GAME || currentScreen == SCREEN_INFINITE_GAME))
            {
                if (!deathScreenActive && !phaseTransitionActive && !bossDefeatTransitionPending && (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P)))
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
                    level6IntroProgress = level6IntroTimer / level6IntroDuration;
                }

                if (gamePaused)
                {
                    int pauseOptionCount = 2;
                    Rectangle pauseOptionRects[pauseOptionCount];
                    pauseOptionRects[0] = ScaleUiRect(332.0f, 154.0f, 68.0f, 56.0f, currentWidth, currentHeight);
                    pauseOptionRects[1] = ScaleUiRect(249.0f, 152.0f, 59.0f, 52.0f, currentWidth, currentHeight);

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

                    float barValue = GetGameplayBarValue(currentLevel, phase, progressTimer, &pernaCabeluda, &shark, &midnightMan);

                    BeginDrawing();
                        DrawGameplayScene(&bg, currentLevel, phase, level6IntroProgress, currentWidth, currentHeight, groundY, enemies, &enemyAssets, &player, playerScale, &pernaCabeluda, bossScale, &shark, &midnightMan, barValue, !infiniteMode, true);
                        if (infiniteMode)
                        {
                            DrawInfiniteMetersCounter(infiniteMeters, currentWidth, currentHeight);
                        }
                        DrawPauseScreenOverlay(currentWidth, currentHeight, pauseOptionRects, pauseOptionCount, hoveringButton);
                    EndDrawing();
                    continue;
                }

                if (currentLevel->id == 6 && level6IntroActive)
                {
                    level6IntroTimer += dt;
                    if (level6IntroTimer >= level6IntroDuration)
                    {
                        level6IntroTimer = level6IntroDuration;
                        level6IntroActive = false;
                    }
                    level6IntroProgress = level6IntroTimer / level6IntroDuration;
                }

                if (deathScreenActive)
                {
                    if (infiniteMode && !infiniteRunRecorded)
                    {
                        RegistrarDistanciaDesafioDiario(infiniteMeters);
                        RegistrarRunDesafioDiario();
                        infiniteRunRecorded = true;
                    }

                    if (infiniteMode && !infiniteRankingChecked)
                    {
                        infiniteFinalMeters = infiniteMeters;
                        infiniteRanking = CarregarRankingInfinito();
                        infiniteRankingNameActive = PontuacaoEntraNoTop10(&infiniteRanking, infiniteFinalMeters);
                        infinitePlayerName[0] = '\0';
                        infinitePlayerNameLength = 0;
                        infiniteRankingChecked = true;
                    }

                    if (infiniteRankingNameActive)
                    {
                        int key = GetCharPressed();
                        while (key > 0)
                        {
                            if (key >= 32 && key <= 126 && infinitePlayerNameLength < INFINITE_PLAYER_NAME_MAX - 1)
                            {
                                infinitePlayerName[infinitePlayerNameLength] = (char)key;
                                infinitePlayerNameLength++;
                                infinitePlayerName[infinitePlayerNameLength] = '\0';
                            }
                            key = GetCharPressed();
                        }

                        if (IsKeyPressed(KEY_BACKSPACE) && infinitePlayerNameLength > 0)
                        {
                            infinitePlayerNameLength--;
                            infinitePlayerName[infinitePlayerNameLength] = '\0';
                        }

                        if (IsKeyPressed(KEY_ENTER))
                        {
                            AdicionarPontuacaoRankingInfinito(&infiniteRanking, infinitePlayerName, infiniteFinalMeters);
                            infiniteRankingNameActive = false;
                        }

                        float barValue = GetGameplayBarValue(currentLevel, phase, progressTimer, &pernaCabeluda, &shark, &midnightMan);

                        BeginDrawing();
                            DrawGameplayScene(&bg, currentLevel, phase, level6IntroProgress, currentWidth, currentHeight, groundY, enemies, &enemyAssets, &player, playerScale, &pernaCabeluda, bossScale, &shark, &midnightMan, barValue, !infiniteMode, true);
                            DrawInfiniteMetersCounter(infiniteFinalMeters, currentWidth, currentHeight);
                            DrawInfiniteRankingNameOverlay(currentWidth, currentHeight, infiniteFinalMeters, infinitePlayerName);
                        EndDrawing();
                        continue;
                    }

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
                    deathOptionRects[0] = ScaleUiRect(334.0f, 151.0f, 70.0f, 54.0f, currentWidth, currentHeight);
                    deathOptionRects[1] = ScaleUiRect(250.0f, 153.0f, 51.0f, 51.0f, currentWidth, currentHeight);

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
                            infiniteDifficultyLevel = levels;
                            phase = PHASE_RUNNING;
                            progressTimer = 0.0f;
                            autoSpawn = true;
                            spawnTimer = 0.0f;
                            infiniteMeters = 0.0f;
                            infiniteNextSpeedStepMeters = INFINITE_SPEED_STEP_METERS;
                            infiniteSpeedMultiplier = 1.0f;
                            infiniteRankingChecked = false;
                            infiniteRankingNameActive = false;
                            infiniteRunRecorded = false;
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
                                &midnightMan,
                                currentWidth,
                                currentHeight,
                                groundY,
                                bossScale,
                                playerStandingY,
                                playerScale
                            );
                            if (currentLevel->bossId == 3)
                            {
                                level6IntroActive = (currentLevel->id == 6);
                                level6IntroTimer = 0.0f;
                                level6IntroDuration = 0.6f;
                            }
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
                        DrawWaterSplashes(&bg);
                        DrawObjetos(&bg, currentWidth, currentHeight, groundY);
                        DrawRain(&bg, currentWidth, currentHeight);
                        DrawStageFront(&bg, currentWidth, currentHeight);

                        DrawDeathScreenOverlay(currentWidth, currentHeight, deathOptionRects, deathOptions, deathOptionCount, hoveringButton);
                    EndDrawing();
                    continue;
                }

                if (phaseTransitionActive)
                {
                    int blockW = 400;
                    int blockH = 300;
                    int blockX = (currentWidth - blockW) / 2;
                    int blockY = (currentHeight - blockH) / 2;
                    int btnFontSize = currentHeight / 24;
                    int padding = 20;

                    const char *advText = "Avançar";
                    const char *menuText = "Menu";
                    int advTextW = MeasureText(advText, btnFontSize);
                    int menuTextW = MeasureText(menuText, btnFontSize);

                    Rectangle advanceRect = {
                        (float)(blockX + blockW - advTextW - padding),
                        (float)(blockY + blockH - btnFontSize - padding),
                        (float)advTextW,
                        (float)btnFontSize
                    };
                    Rectangle menuRect = {
                        (float)(blockX + padding),
                        (float)(blockY + blockH - btnFontSize - padding),
                        (float)menuTextW,
                        (float)btnFontSize
                    };
                    advanceRect = ScaleUiRect(333.0f, 153.0f, 72.0f, 50.0f, currentWidth, currentHeight);
                    menuRect = ScaleUiRect(250.0f, 153.0f, 51.0f, 51.0f, currentWidth, currentHeight);

                    Vector2 mouse = GetMousePosition();
                    bool hoveringButton = CheckCollisionPointRec(mouse, advanceRect) || CheckCollisionPointRec(mouse, menuRect);

                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    {
                        if (CheckCollisionPointRec(mouse, advanceRect))
                        {
                            phaseTransitionActive = false;
                            if (transitionType == TRANSITION_RUNNING_TO_BOSS)
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
                            else if (transitionType == TRANSITION_BOSS_TO_RUNNING)
                            {
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
                                level6IntroDuration = LEVEL6_INTRO_DURATION;
                            }
                        }
                        else if (CheckCollisionPointRec(mouse, menuRect))
                        {
                            currentScreen = SCREEN_LEVEL_SELECT;
                        }
                    }

                    const char *transitionTitle = (transitionType == TRANSITION_RUNNING_TO_BOSS) ? "Fase Concluída!" : "Boss Derrotado!";
                    float barValue = GetGameplayBarValue(currentLevel, phase, progressTimer, &pernaCabeluda, &shark, &midnightMan);
                    BeginDrawing();
                        DrawGameplayScene(&bg, currentLevel, phase, level6IntroProgress, currentWidth, currentHeight, groundY, enemies, &enemyAssets, &player, playerScale, &pernaCabeluda, bossScale, &shark, &midnightMan, barValue, !infiniteMode, true);
                        DrawPhaseTransitionOverlay(currentWidth, currentHeight, advanceRect, menuRect, btnFontSize, transitionTitle, hoveringButton);
                    EndDrawing();
                    continue;
                }

                int infiniteEnemyBaseSpeed = infiniteMode ? GetInfiniteEnemyBaseSpeed(infiniteSpeedMultiplier) : 0;

                if (phase == PHASE_RUNNING)
                {
                    progressTimer += dt;

                    if (infiniteMode)
                    {
                        bool advancedByShortcut = false;
                        if (IsKeyPressed(KEY_F2))
                        {
                            AdvanceInfiniteMeters(
                                levels,
                                INFINITE_SPEED_STEP_METERS,
                                &infiniteMeters,
                                &infiniteNextSpeedStepMeters,
                                &infiniteSpeedMultiplier,
                                &currentLevel,
                                &infiniteDifficultyLevel
                            );
                            infiniteEnemyBaseSpeed = GetInfiniteEnemyBaseSpeed(infiniteSpeedMultiplier);
                            advancedByShortcut = true;
                        }

                        float remainingDt = advancedByShortcut ? 0.0f : dt;
                        while (remainingDt > 0.0f)
                        {
                            float metersPerSecond = GetInfiniteMetersPerSecond(infiniteMeters);
                            float distanceToNextStep = infiniteNextSpeedStepMeters - infiniteMeters;
                            float timeToNextStep = distanceToNextStep / metersPerSecond;

                            if (remainingDt < timeToNextStep)
                            {
                                infiniteMeters += metersPerSecond * remainingDt;
                                remainingDt = 0.0f;
                                break;
                            }

                            infiniteMeters = infiniteNextSpeedStepMeters;
                            remainingDt -= timeToNextStep;
                            infiniteSpeedMultiplier *= INFINITE_SPEED_MULTIPLIER_STEP;
                            infiniteNextSpeedStepMeters += INFINITE_SPEED_STEP_METERS;
                            SyncInfiniteLevels(levels, infiniteMeters, &currentLevel, &infiniteDifficultyLevel);
                        }
                    }
                    else if (progressTimer >= currentLevel->duration)
                    {
                        if (currentLevel->duration > 0.0f)
                        {
                            progressTimer = currentLevel->duration;
                            autoSpawn = false;
                            for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++) enemies[i].active = false;
                            phaseTransitionActive = true;
                            transitionType = TRANSITION_RUNNING_TO_BOSS;
                        }
                        else
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
                    }

                    spawnTimer -= dt;
                    if (spawnTimer <= 0.0f)
                    {
                        Level *spawnLevel = infiniteMode ? infiniteDifficultyLevel : currentLevel;
                        EnemyType chosen = SortearInimigoFase(spawnLevel->enemyConfigId);

                        if (chosen == ENEMY_BIRD1)
                        {
                            for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
                            {
                                if (enemies[i].active && enemies[i].type == ENEMY_BIRD1)
                                {
                                    chosen = SortearInimigoFase(spawnLevel->enemyConfigId);
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
                        spawnTimer = spawnLevel->spawnInterval;
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
                                Level *spawnLevel = infiniteMode ? infiniteDifficultyLevel : currentLevel;
                                followUp = SortearInimigoFase(spawnLevel->enemyConfigId);
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

                            Level *spawnLevel = infiniteMode ? infiniteDifficultyLevel : currentLevel;
                            spawnTimer = spawnLevel->spawnInterval;
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
                        if (!enemies[i].dying && CheckCollisionRecs(attackHitbox, GetEnemyHitbox(&enemies[i])))
                        {
                            if (infiniteMode)
                            {
                                RegistrarInimigoDesafioDiario(enemies[i].type);
                            }
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
                UpdateWaterSplashes(&bg, infiniteMode ? dt * infiniteSpeedMultiplier : dt, currentWidth, currentHeight, groundY);
                UpdatePlayer(&player, dt, playerStandingY, playerScale, &config);
                UpdateProjectile(&projectiles, dt, currentWidth, currentHeight);

                // Bullet collision handling
                for (int j = 0; j < MAX_PROJECTILES; j++)
                {
                    if (!projectiles.items[j].active) continue;
                    Rectangle bulletHitbox = GetProjectileHitbox(&projectiles.items[j]);

                    // 1. Check normal enemies collision (birds and fish, only)
                    for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
                    {
                        if (!enemies[i].active || enemies[i].dying) continue;
                        if (enemies[i].type != ENEMY_BIRD1 && enemies[i].type != ENEMY_BIRD2 && enemies[i].type != ENEMY_FISH) continue;

                        if (CheckCollisionRecs(bulletHitbox, GetEnemyHitbox(&enemies[i])))
                        {
                            if (infiniteMode)
                            {
                                RegistrarInimigoDesafioDiario(enemies[i].type);
                            }
                            enemies[i].dying = true;
                            enemies[i].velocity.x = -8.0f;
                            enemies[i].velocity.y = -5.0f;
                            enemies[i].animationTimer = 0.0f;
                            enemies[i].currentFrame = 0;

                            // Deactivate the bullet
                            projectiles.items[j].active = false;
                            break;
                        }
                    }

                    if (!projectiles.items[j].active) continue;

                    // 2. Check bosses collision
                    if (phase == PHASE_BOSS)
                    {
                        int dmg = (int)player.weapon.damage;
                        if (dmg <= 0) dmg = 3;

                        if (currentLevel->bossId == 1) // Hairy Leg
                        {
                            if (pernaCabeluda.state != HL_DEAD)
                            {
                                if (CheckCollisionRecs(bulletHitbox, pernaCabeluda.rect))
                                {
                                    DamageHairyLeg(&pernaCabeluda, dmg);
                                    projectiles.items[j].active = false;
                                }
                            }
                        }
                        else if (currentLevel->bossId == 2) // Shark
                        {
                            if (shark.active && !shark.dying)
                            {
                                if (CheckCollisionRecs(bulletHitbox, GetSharkHitbox(&shark)))
                                {
                                    DamageShark(&shark, dmg);
                                    projectiles.items[j].active = false;
                                }
                            }
                        }
                        else if (currentLevel->bossId == 3) // Midnight Man
                        {
                            if (midnightMan.active && midnightMan.health > 0)
                            {
                                bool hit = false;
                                // Check all active hands
                                for (int h = 0; h < MM_HAND_COUNT; h++)
                                {
                                    if (midnightMan.handActive[h])
                                    {
                                        if (CheckCollisionRecs(bulletHitbox, midnightMan.handHitboxes[h]))
                                        {
                                            hit = true;
                                            break;
                                        }
                                    }
                                }

                                // Check umbrella collisions for bullets
                                bool umbrellaHit = false;
                                for (int u = 0; u < MM_MAX_UMBRELLAS; u++)
                                {
                                    if (midnightMan.umbrellas[u].active)
                                    {
                                        float uW = 186.0f * midnightMan.umbrellas[u].scale;
                                        float uH = 141.0f * midnightMan.umbrellas[u].scale;
                                        Rectangle umbrellaHitbox = {
                                            midnightMan.umbrellas[u].position.x - uW * 0.45f,
                                            midnightMan.umbrellas[u].position.y - uH * 0.45f,
                                            uW * 0.9f,
                                            uH * 0.9f
                                        };
                                        if (CheckCollisionRecs(bulletHitbox, umbrellaHitbox))
                                        {
                                            midnightMan.umbrellas[u].active = false; // Destroy umbrella
                                            umbrellaHit = true;
                                        }
                                    }
                                }

                                if (umbrellaHit)
                                {
                                    projectiles.items[j].active = false; // Destroy bullet
                                }
                                else if (hit)
                                {
                                    midnightMan.health -= dmg;
                                    midnightMan.hitFlashTimer = 0.12f;
                                    if (midnightMan.health < 0) midnightMan.health = 0;
                                    projectiles.items[j].active = false;
                                }
                            }
                        }
                    }
                }

                playerHitbox = GetPlayerHitbox(&player, playerScale);

                if (!bossDefeatTransitionPending && !infiniteMode && IsKeyPressed(KEY_RIGHT) && phase == PHASE_RUNNING && currentLevel->bossId != 0) {
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
                else if (!bossDefeatTransitionPending && !infiniteMode && IsKeyPressed(KEY_RIGHT) && phase == PHASE_BOSS && currentLevel->next) {
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
                    level6IntroDuration = LEVEL6_INTRO_DURATION;
                }
                if (!bossDefeatTransitionPending && !infiniteMode && IsKeyPressed(KEY_LEFT) && currentLevel->prev) {
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
                    level6IntroDuration = LEVEL6_INTRO_DURATION;
                }

                if (phase == PHASE_BOSS)
                {
                    bool bossDefeatedThisFrame = false;

                    if (currentLevel->bossId == 1)
                    {
                        UpdateHairyLeg(&pernaCabeluda, playerHitbox, dt, standingY, bossScale);
                        if (!bossDefeatTransitionPending)
                        {
                            TryDamageHairyLegFromPlayerAttack(&pernaCabeluda, &player, playerScale);
                        }

                        if (!bossDefeatTransitionPending && pernaCabeluda.state == HL_DEAD && currentLevel->next)
                        {
                            UnlockStoryLevel(currentLevel->next->id);
                            transitionType = TRANSITION_BOSS_TO_RUNNING;
                            bossDefeatTransitionPending = true;
                            bossDefeatTransitionTimer = 0.0f;
                            bossDefeatedThisFrame = true;
                        }
                    }

                    if (!bossDefeatedThisFrame && currentLevel->bossId == 2)
                    {
                        UpdateShark(&shark, playerHitbox, dt, currentWidth, currentHeight);
                        if (!bossDefeatTransitionPending)
                        {
                            if (IsKeyPressed(KEY_N))
                            {
                                DamageShark(&shark, shark.health);
                            }
                            TryDamageSharkFromPlayerAttack(&shark, &player, playerScale);
                        }

                        if (!bossDefeatTransitionPending && !shark.active && currentLevel->next)
                        {
                            UnlockStoryLevel(currentLevel->next->id);
                            transitionType = TRANSITION_BOSS_TO_RUNNING;
                            bossDefeatTransitionPending = true;
                            bossDefeatTransitionTimer = 0.0f;
                            bossDefeatedThisFrame = true;
                        }
                    }

                    if (currentLevel->bossId == 3)
                    {
                        float mmDt = (currentLevel->id == 6 && level6IntroActive) ? 0.0f : dt;
                        UpdateMidnightMan(&midnightMan, playerHitbox, mmDt, currentWidth, currentHeight, groundY);
                        if (!bossDefeatTransitionPending)
                        {
                            TryDamageMidnightManFromPlayerAttack(&midnightMan, &player, playerScale);
                        }

                        if (!bossDefeatTransitionPending && midnightMan.health <= 0 && currentLevel->next)
                        {
                            UnlockStoryLevel(currentLevel->next->id);
                            transitionType = TRANSITION_BOSS_TO_RUNNING;
                            bossDefeatTransitionPending = true;
                            bossDefeatTransitionTimer = 0.0f;
                            bossDefeatedThisFrame = true;
                        }
                    }

                    if (bossDefeatTransitionPending)
                    {
                        bossDefeatTransitionTimer += dt;
                        if (bossDefeatTransitionTimer >= BOSS_DEFEAT_TRANSITION_DELAY)
                        {
                            bossDefeatTransitionPending = false;
                            bossDefeatTransitionTimer = 0.0f;
                            phaseTransitionActive = true;
                            transitionType = TRANSITION_BOSS_TO_RUNNING;
                        }
                        bossDefeatedThisFrame = true;
                    }

                    if (!bossDefeatedThisFrame && currentLevel->bossId == 1 && pernaCabeluda.state != HL_DEAD)
                    {
                        if (CheckCollisionRecs(playerHitbox, pernaCabeluda.bodyHitbox))
                        {
                            deathScreenActive = true;
                            retryPhase = PHASE_BOSS;
                        }

                        if (IsHairyLegKickColliding(&pernaCabeluda, playerHitbox))
                        {
                            deathScreenActive = true;
                            retryPhase = PHASE_BOSS;
                        }

                        if (pernaCabeluda.waveLeft.active && CheckCollisionRecs(playerHitbox, pernaCabeluda.waveLeft.hitbox))
                        {
                            deathScreenActive = true;
                            retryPhase = PHASE_BOSS;
                        }

                        if (pernaCabeluda.waveRight.active && CheckCollisionRecs(playerHitbox, pernaCabeluda.waveRight.hitbox))
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

                    if (!bossDefeatedThisFrame && currentLevel->bossId == 3)
                    {
                        if (IsMidnightManColliding(&midnightMan, playerHitbox))
                        {
                            deathScreenActive = true;
                            retryPhase = PHASE_BOSS;
                        }
                    }
                }

                float barValue = GetGameplayBarValue(currentLevel, phase, progressTimer, &pernaCabeluda, &shark, &midnightMan);

                BeginDrawing();
                    DrawGameplayScene(&bg, currentLevel, phase, level6IntroProgress, currentWidth, currentHeight, groundY, enemies, &enemyAssets, &player, playerScale, &pernaCabeluda, bossScale, &shark, &midnightMan, barValue, false, false);
                    if (!(currentLevel->id == 6 && level6IntroActive))
                    {
                        DrawPlayer(&player, playerScale);
                    }

                    if (phase == PHASE_BOSS)
                    {
                        if (currentLevel->bossId == 1)
                        {
                            DrawHairyLeg(&pernaCabeluda, bossScale);
                        }
                    }

                    DrawRain(&bg, currentWidth, currentHeight);
                    DrawStageFront(&bg, currentWidth, currentHeight);
                    if (infiniteMode)
                    {
                        DrawInfiniteMetersCounter(infiniteMeters, currentWidth, currentHeight);
                    }

                    if (!infiniteMode)
                    {
                        DrawProgressBar(&bg, barValue, currentWidth, currentHeight);
                    }
                    DrawProjectiles(&projectiles);
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
            if (!infiniteMode)
            {
                UnloadHairyLeg(&pernaCabeluda);
                UnloadShark(&shark);
                UnloadMidnightMan(&midnightMan);
            }
            UnloadProjectileSystem(&projectiles);
            FreeLevels(levels);
        }
    }

    UnloadCustomCursor();
    ShowCursor();
    UnloadSoundSystem();
    CloseWindow();

    return 0;
}
