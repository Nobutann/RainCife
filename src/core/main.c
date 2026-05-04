#include <raylib.h>
#include "core/screens.h"
#include "core/config_manager.h"
#include "entities/player.h"
#include "entities/hairy_leg.h"
#include "entities/enemy.h"
#include "graphics/background.h"
#include "enemy_caller.h"

#define MAX_ACTIVE_ENEMIES 12

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
            float initGroundY = initH * GROUND_RATIO;
            float initBossScale = (float)initH * 0.65f / 252.0f;

            Player player;
            Vector2 startPos = { 100, initGroundY };
            InitPlayer(&player, startPos, SPEED);

            HairyLeg pernaCabeluda;
            InitHairyLeg(&pernaCabeluda, (Vector2){ (float)initW * 0.6f, initGroundY }, initGroundY, initBossScale);

            Enemy enemies[MAX_ACTIVE_ENEMIES] = {0};
            Texture2D enemyTextures[ENEMY_COUNT];
            enemyTextures[ENEMY_BIRD1] = LoadTexture("assets/sprites/Enemys_obstacles/Bird.png");
            enemyTextures[ENEMY_BIRD2] = LoadTexture("assets/sprites/Enemys_obstacles/Bird.png");
            enemyTextures[ENEMY_BIKE] = LoadTexture("assets/sprites/Enemys_obstacles/Bike.png");
            enemyTextures[ENEMY_WOOD] = LoadTexture("assets/sprites/Enemys_obstacles/Tree.png");
            enemyTextures[ENEMY_POSTE] = LoadTexture("assets/sprites/Enemys_obstacles/Poste_mal_completo.png");
            enemyTextures[ENEMY_FISH] = LoadTexture("assets/sprites/Enemys_obstacles/Fish.png");

            Texture2D texPosteSemCabeca = LoadTexture("assets/sprites/Enemys_obstacles/Poste_mal_sem_cabeca.png");
            Texture2D texPosteCabecas = LoadTexture("assets/sprites/Enemys_obstacles/Poste_mal_cabecas.png");

            bool autoSpawn = false;
            float spawnTimer = 0.0f;
            const float spawnInterval = 2.0f;

            while (!WindowShouldClose() && currentScreen == SCREEN_GAME)
            {
                if (IsKeyPressed(KEY_ESCAPE))
                {
                    currentScreen = SCREEN_START;
                }

                float dt = GetFrameTime();
                int currentWidth = GetScreenWidth();
                int currentHeight = GetScreenHeight();
                float groundY = currentHeight * GROUND_RATIO;
                float playerScale = (float)currentHeight * 0.45f / 252.0f;
                float bossScale = (float)currentHeight * 0.65f / 252.0f;

                bool spawnBird1 = IsKeyPressed(KEY_ONE);
                bool spawnBird2 = IsKeyPressed(KEY_TWO);
                bool spawnBike = IsKeyPressed(KEY_B);
                bool spawnWood = IsKeyPressed(KEY_M);
                bool spawnPoste = IsKeyPressed(KEY_C);
                bool spawnFish = IsKeyPressed(KEY_P);

                if (IsKeyPressed(KEY_ENTER)) autoSpawn = !autoSpawn;

                if (autoSpawn) {
                    spawnTimer -= dt;
                    if (spawnTimer <= 0) {
                        EnemyType sorteado = SortearInimigoFase(1);
                        if (sorteado == ENEMY_BIRD1) spawnBird1 = true;
                        else if (sorteado == ENEMY_BIRD2) spawnBird2 = true;
                        else if (sorteado == ENEMY_BIKE) spawnBike = true;
                        else if (sorteado == ENEMY_WOOD) spawnWood = true;
                        else if (sorteado == ENEMY_POSTE) spawnPoste = true;
                        else if (sorteado == ENEMY_FISH) spawnFish = true;
                        
                        spawnTimer = spawnInterval;
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

                        if (CheckCollisionRecs(playerHitbox, enemyRect))
                        {
                            currentScreen = SCREEN_START;
                        }
                    }
                }

                UpdateBackground(&bg, dt);

                float standingY = groundY + (currentHeight * SIDEWALK_THICKNESS_RATIO * -0.2f);
                float playerStandingY = groundY + (currentHeight * SIDEWALK_THICKNESS_RATIO * 0.1f);
                UpdatePlayer(&player, dt, playerStandingY, playerScale);

                UpdateHairyLeg(&pernaCabeluda, playerHitbox, dt, standingY, bossScale);

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


                BeginDrawing();
                    ClearBackground(BLACK);
                    DrawBackground(&bg, currentWidth, currentHeight, groundY);

                    for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++)
                    {
                        if (enemies[i].active)
                        {
                            DrawEnemy(&enemies[i], enemyTextures, texPosteSemCabeca, texPosteCabecas);
                        }
                    }

                    DrawPlayer(&player, playerScale);

                    DrawHairyLeg(&pernaCabeluda, bossScale);
                    DrawText("1: P1 | 2: P2 | B: Bike | M: Madeira | C: Caboclo | P: Peixe", 20, 20, 20, GRAY);

                EndDrawing();
            }

            UnloadPlayer(&player);
            UnloadBackground(&bg);
            for (int i = 0; i < ENEMY_COUNT; i++) UnloadTexture(enemyTextures[i]);
            UnloadTexture(texPosteSemCabeca);
            UnloadTexture(texPosteCabecas);
        }
    }

    CloseWindow();

    return 0;
}
