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

#define MAX_ACTIVE_ENEMIES 30

int main(void)
{
    Config config = CarregarConfig();

    config.telaCheia = 1;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "RatTsunami");

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
            float initGroundY = initH * ENTITY_GROUND_RATIO;
            float initBossScale = (float)initH * 0.65f / 252.0f;

            Player player;
            Vector2 startPos = { 100, initGroundY };
            InitPlayer(&player, startPos, SPEED);

            Level *levels = InitGameLevels();
            Level *currentLevel = levels;
            player.isBossFighting = (currentLevel->bossId != 0);

            HairyLeg pernaCabeluda;
            InitHairyLeg(&pernaCabeluda, (Vector2){ (float)initW * 0.6f, initGroundY }, initGroundY, initBossScale);

            Shark shark;
            InitShark(&shark, initW, initH);

            Enemy enemies[MAX_ACTIVE_ENEMIES] = {0};
            EnemyAssets enemyAssets = {0};
            enemyAssets.textures[ENEMY_BIRD1] = LoadTexture("assets/sprites/Enemys_obstacles/Bird.png");
            enemyAssets.textures[ENEMY_BIRD2] = LoadTexture("assets/sprites/Enemys_obstacles/Bird.png");
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

            bool autoSpawn = false;
            float spawnTimer = 0.0f;

            while (!WindowShouldClose() && currentScreen == SCREEN_GAME)
            {
                if (IsKeyPressed(KEY_ESCAPE))
                {
                    currentScreen = SCREEN_START;
                }

                float dt = GetFrameTime();
                if (dt > 0.1f)
                {
                    dt = 0.1f;
                }

                int currentWidth = GetScreenWidth();
                int currentHeight = GetScreenHeight();
                float groundY = currentHeight * ENTITY_GROUND_RATIO;
                float bgGroundY = currentHeight * BG_GROUND_RATIO;
                float playerScale = (float)currentHeight * 0.45f / 300.0f;
                float bossScale = (float)currentHeight * 0.65f / 252.0f;

                bool spawnBird1 = IsKeyPressed(KEY_ONE);
                bool spawnBird2 = IsKeyPressed(KEY_TWO);
                bool spawnBike = IsKeyPressed(KEY_B);
                bool spawnWood = IsKeyPressed(KEY_M);
                bool spawnPoste = IsKeyPressed(KEY_C);
                bool spawnSafePoste = IsKeyPressed(KEY_I);
                bool spawnFish = IsKeyPressed(KEY_P);

                if (IsKeyPressed(KEY_ENTER))
                {
                    autoSpawn = !autoSpawn;
                }

                if (autoSpawn)
                {
                    spawnTimer -= dt;
                    if (spawnTimer <= 0)
                    {
                        int groundObstacleCount = 0;
                        for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
                        {
                            if (enemies[i].active && (enemies[i].type == ENEMY_BIKE || enemies[i].type == ENEMY_WOOD))
                            {
                                groundObstacleCount++;
                            }
                        }

                        EnemyType sorteado = SortearInimigoFase(currentLevel->enemyConfigId);
                        
                        if ((sorteado == ENEMY_BIKE || sorteado == ENEMY_WOOD) && groundObstacleCount >= 2)
                        {
                            spawnTimer = 0.5f;
                        }
                        else
                        {
                            if (sorteado == ENEMY_BIRD1) spawnBird1 = true;
                            else if (sorteado == ENEMY_BIRD2) spawnBird2 = true;
                            else if (sorteado == ENEMY_BIKE) spawnBike = true;
                            else if (sorteado == ENEMY_WOOD) spawnWood = true;
                            else if (sorteado == ENEMY_POSTE) spawnPoste = true;
                            else if (sorteado == ENEMY_SAFE_POSTE) spawnSafePoste = true;
                            else if (sorteado == ENEMY_FISH) spawnFish = true;
                            
                            spawnTimer = currentLevel->spawnInterval;
                        }
                    }
                }

                if (spawnBird1)
                {
                    for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
                    {
                        if (!enemies[i].active)
                        {
                            InitEnemy(&enemies[i], ENEMY_BIRD1, currentWidth, currentHeight, 0);
                            break;
                        }
                    }
                }

                if (spawnBird2)
                {
                    for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
                    {
                        if (!enemies[i].active)
                        {
                            InitEnemy(&enemies[i], ENEMY_BIRD2, currentWidth, currentHeight, 0);
                            break;
                        }
                    }
                }

                if (spawnBike)
                {
                    for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
                    {
                        if (!enemies[i].active)
                        {
                            InitEnemy(&enemies[i], ENEMY_BIKE, currentWidth, currentHeight, 0);
                            break;
                        }
                    }
                }

                if (spawnWood)
                {
                    for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
                    {
                        if (!enemies[i].active)
                        {
                            InitEnemy(&enemies[i], ENEMY_WOOD, currentWidth, currentHeight, 0);
                            break;
                        }
                    }
                }

                if (spawnPoste)
                {
                    for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
                    {
                        if (!enemies[i].active)
                        {
                            InitEnemy(&enemies[i], ENEMY_POSTE, currentWidth, currentHeight, 0);
                            break;
                        }
                    }
                }

                if (spawnFish)
                {
                    for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
                    {
                        if (!enemies[i].active)
                        {
                            InitEnemy(&enemies[i], ENEMY_FISH, currentWidth, currentHeight, 0);
                            break;
                        }
                    }
                }

                if (spawnSafePoste)
                {
                    for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
                    {
                        if (!enemies[i].active)
                        {
                            InitEnemy(&enemies[i], ENEMY_SAFE_POSTE, currentWidth, currentHeight, 0);
                            break;
                        }
                    }
                }

                Rectangle playerHitbox = GetPlayerHitbox(&player, playerScale);

                for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
                {
                    UpdateEnemy(&enemies[i], currentWidth, currentHeight, 0, playerHitbox);
                    if (enemies[i].active)
                    {
                        Rectangle enemyRect = 
                        { 
                            enemies[i].position.x, 
                            enemies[i].position.y, 
                            enemies[i].size.x, 
                            enemies[i].size.y 
                        };

                        if (enemies[i].type != ENEMY_SAFE_POSTE && CheckCollisionRecs(playerHitbox, enemyRect))
                        {
                            currentScreen = SCREEN_START;
                        }
                    }
                }

                UpdateBackground(&bg, dt);

                float standingY = groundY + (currentHeight * SIDEWALK_THICKNESS_RATIO * -0.2f);
                float playerStandingY = groundY + (currentHeight * SIDEWALK_THICKNESS_RATIO * 0.1f);
                UpdatePlayer(&player, dt, playerStandingY, playerScale);

                if (IsKeyPressed(KEY_RIGHT) && currentLevel->next)
                {
                    currentLevel = currentLevel->next;
                    for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
                    {
                        enemies[i].active = false;
                    }
                    InitHairyLeg(&pernaCabeluda, (Vector2){ (float)currentWidth * 0.6f, groundY }, groundY, bossScale);
                    InitShark(&shark, currentWidth, currentHeight);
                    player.isBossFighting = (currentLevel->bossId != 0);
                }
                if (IsKeyPressed(KEY_LEFT) && currentLevel->prev)
                {
                    currentLevel = currentLevel->prev;
                    for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
                    {
                        enemies[i].active = false;
                    }
                    InitHairyLeg(&pernaCabeluda, (Vector2){ (float)currentWidth * 0.6f, groundY }, groundY, bossScale);
                    InitShark(&shark, currentWidth, currentHeight);
                    player.isBossFighting = (currentLevel->bossId != 0);
                }

                if (currentLevel->bossId == 1)
                {
                    UpdateHairyLeg(&pernaCabeluda, playerHitbox, dt, standingY, bossScale);
                }
                if (currentLevel->bossId == 2)
                {
                    UpdateShark(&shark, playerHitbox, dt, currentWidth, currentHeight);
                }

                if (currentLevel->bossId == 2 && CheckCollisionRecs(playerHitbox, shark.rect))
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

                if (currentLevel->bossId == 1)
                {
                    if (CheckCollisionRecs(playerHitbox, pernaCabeluda.rect))
                    {
                        currentScreen = SCREEN_START;
                    }
                    if (pernaCabeluda.isKickActive && CheckCollisionRecs(playerHitbox, pernaCabeluda.kickHitbox))
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

                BeginDrawing();
                    ClearBackground(BLACK);
                    DrawBackground(&bg, currentWidth, currentHeight, bgGroundY);

                    for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
                    {
                        if (enemies[i].active)
                        {
                            DrawEnemy(&enemies[i], &enemyAssets);
                        }
                    }

                    DrawPlayer(&player, playerScale);

                    if (currentLevel->bossId == 1)
                    {
                        DrawHairyLeg(&pernaCabeluda, bossScale);
                    }
                    if (currentLevel->bossId == 2)
                    {
                        DrawShark(&shark);
                    }
                    
                    char faseText[32];
                    TextCopy(faseText, TextFormat("FASE %d", currentLevel->id));
                    DrawText(faseText, currentWidth / 2 - MeasureText(faseText, 30) / 2, 50, 30, WHITE);

                    DrawText("SETAS: Trocar Fase | 1,2,B,M,C,P: Spawn | ENTER: AutoSpawn", 20, 20, 20, GRAY);

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
            UnloadTexture(enemyAssets.bikeSkin2);
            UnloadTexture(enemyAssets.bikeSkinItau);
            UnloadShark(&shark);
        }
    }

    CloseWindow();

    return 0;
}
