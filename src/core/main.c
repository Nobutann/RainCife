#include <raylib.h>
#include "core/screens.h"
#include "core/config_manager.h"
#include "entities/player.h"
#include "entities/hairy_leg.h"
#include "entities/shark.h"
#include "entities/enemy.h"
#include "graphics/background.h"
#include "enemy_caller.h"
#include "gameplay/levels.h"
#include "core/cursor.h"

#define MAX_ACTIVE_ENEMIES 12
#define HAIRY_LEG_DEATH_ADVANCE_DELAY 1.0f

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

int main(void)
{
    Config config = CarregarConfig();

    config.telaCheia = 1;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "RainCife");
    HideCursor();
    InitCustomCursor();

    if (config.telaCheia)
    {
        int monitor = GetCurrentMonitor();
        SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
        ToggleFullscreen();
    }

    SetTargetFPS(60);
    SetExitKey(KEY_DELETE);

    GameScreen currentScreen = SCREEN_START;

    while (currentScreen != SCREEN_EXIT && !WindowShouldClose())
    {
        if (IsKeyPressed(KEY_F11))
        {
            ToggleFullscreen();
        }

        if (currentScreen == SCREEN_START)
        {
            currentScreen = RunStart();
        }

        if (currentScreen == SCREEN_OPTIONS)
        {
            currentScreen = RunOptions(&config);
        }

        if (currentScreen == SCREEN_CREDITS)
        {
            currentScreen = RunCredits();
        }

        if (currentScreen == SCREEN_GAME)
        {
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

            Level *levels = InitGameLevels();
            Level *currentLevel = levels;

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
            enemyAssets.bikeSkin2 = LoadTexture("assets/sprites/Enemys_obstacles/Bike_2.png");
            enemyAssets.bikeSkinItau = LoadTexture("assets/sprites/Enemys_obstacles/Bike_itau.png");

            GamePhase phase = PHASE_RUNNING;
            float progressTimer = 0.0f;

            // player.isBossFighting = (currentLevel->bossId != 0);
            player.isBossFighting = false;

            bool autoSpawn = true;
            float spawnTimer = 0.0f;
            // const float spawnInterval = 1.5f;
            float safePosteFollowUpTimer = -1.0f;

            while (!WindowShouldClose() && currentScreen == SCREEN_GAME)
            {
                if (IsKeyPressed(KEY_ESCAPE))
                {
                    currentScreen = SCREEN_START;
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

                if (phase == PHASE_RUNNING)
                {
                    progressTimer += dt;

                    if (progressTimer >= currentLevel->duration)
                    {
                        phase = PHASE_BOSS;
                        progressTimer = currentLevel->duration;
                        autoSpawn = false;
                        
                        for ( int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
                        {
                            enemies[i].active = false;
                        }

                        player.isBossFighting = (currentLevel)->bossId != 0;

                        if (currentLevel->bossId == 1)
                        {
                            InitHairyLeg(&pernaCabeluda, (Vector2){(float)currentWidth * 0.6f, groundY}, groundY, bossScale);
                            PlacePlayerForBossIntro(&player, pernaCabeluda.rect, playerStandingY, playerScale);
                        }

                        if (currentLevel->bossId == 2)
                        {
                            InitShark(&shark, currentWidth, currentHeight);
                            PlacePlayerForBossIntro(&player, GetSharkHitbox(&shark), playerStandingY, playerScale);
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
                                InitEnemy(&enemies[i], chosen, currentWidth, currentHeight, 0);
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
                                    InitEnemy(&enemies[i], followUp, currentWidth, currentHeight, 0);
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
                    UpdateEnemy(&enemies[i], currentWidth, currentHeight, 0, playerHitbox);
                    if (enemies[i].active && enemies[i].type != ENEMY_SAFE_POSTE)
                    {
                        if (CheckCollisionRecs(playerHitbox, GetEnemyHitbox(&enemies[i])))
                        {
                            currentScreen = SCREEN_START;
                        }
                    }
                }

                UpdateBackground(&bg, dt, phase);
                UpdatePlayer(&player, dt, playerStandingY, playerScale);
                playerHitbox = GetPlayerHitbox(&player, playerScale);

                if (IsKeyPressed(KEY_RIGHT) && currentLevel->next) {
                    StartLevel(&currentLevel, currentLevel->next, &phase, &progressTimer, &autoSpawn, &spawnTimer, enemies, &player, &pernaCabeluda, &shark, currentWidth, currentHeight, groundY, bossScale);
                }
                if (IsKeyPressed(KEY_LEFT) && currentLevel->prev) {
                    StartLevel(&currentLevel, currentLevel->prev, &phase, &progressTimer, &autoSpawn, &spawnTimer, enemies, &player, &pernaCabeluda, &shark, currentWidth, currentHeight, groundY, bossScale);
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
                            StartLevel(&currentLevel, currentLevel->next, &phase, &progressTimer, &autoSpawn, &spawnTimer, enemies, &player, &pernaCabeluda, &shark, currentWidth, currentHeight, groundY, bossScale);
                            bossDefeatedThisFrame = true;
                        }
                    }

                    if (!bossDefeatedThisFrame && currentLevel->bossId == 2)
                    {
                        UpdateShark(&shark, playerHitbox, dt, currentWidth, currentHeight);
                    }

                    if (!bossDefeatedThisFrame && currentLevel->bossId == 1 && pernaCabeluda.state != HL_DEAD)
                    {
                        if (CheckCollisionRecs(playerHitbox, pernaCabeluda.rect))
                        {
                            currentScreen = SCREEN_START;
                        }

                        if (IsHairyLegKickColliding(&pernaCabeluda, playerHitbox))
                        {
                            currentScreen = SCREEN_START;
                        }

                        if (pernaCabeluda.waveLeft.active && CheckCollisionRecs(playerHitbox, pernaCabeluda.waveLeft.rect))
                        {
                            currentScreen = SCREEN_START;
                        }

                        if (pernaCabeluda.waveRight.active && CheckCollisionRecs(playerHitbox, pernaCabeluda.waveRight.rect))
                        {
                            currentScreen = SCREEN_START;
                        }
                    }

                    if (!bossDefeatedThisFrame && currentLevel->bossId == 2)
                    {
                        if (CheckCollisionRecs(playerHitbox, GetSharkHitbox(&shark)))
                        {
                            currentScreen = SCREEN_START;
                        }

                        for (int i = 0; i < MAX_WATER_BALLS; i++)
                        {
                            if (shark.balls[i].active && CheckCollisionRecs(playerHitbox, shark.balls[i].rect))
                            {
                                currentScreen = SCREEN_START;
                            }
                        }
                    }
                }

                float barValue = 1.0f;
                if (phase == PHASE_RUNNING && currentLevel->duration > 0.0f)
                {
                    barValue = progressTimer / currentLevel->duration;
                }
                else if (phase == PHASE_BOSS)
                {
                    if (currentLevel->bossId == 1)
                    {
                        barValue = (float)pernaCabeluda.health / 100.0f;
                    }
                    else if (currentLevel->bossId == 2)
                    {
                        barValue = (float)shark.health / 200.0f;
                    }
                }

                BeginDrawing();
                    ClearBackground(BLACK);
                    DrawBackground(&bg, currentWidth, currentHeight, groundY);

                    for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
                    {
                        if (enemies[i].active)
                        {
                            DrawEnemy(&enemies[i], &enemyAssets);
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

                    DrawProgressBar(&bg, barValue, currentWidth, currentHeight);
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
            UnloadShark(&shark);
            FreeLevels(levels);
        }
    }

    UnloadCustomCursor();
    ShowCursor();
    CloseWindow();

    return 0;
}
