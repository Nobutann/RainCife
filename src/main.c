#include <raylib.h>
#include "screens.h"
#include "config_manager.h"
#include "player.h"
#include "hairy_leg.h"
#include "hitbox_enemy.h"
#include "background.h"
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
            float initBossScale = (float)initH * 0.32f / 252.0f;

            Player player;
            Vector2 startPos = { 100, initGroundY };
            InitPlayer(&player, startPos, SPEED);

            HairyLeg pernaCabeluda;
            InitHairyLeg(&pernaCabeluda, (Vector2){ (float)initW * 0.6f, initGroundY }, initGroundY, initBossScale);

            Inimigo passaro1[MAX_ENEMIES] = { 0 };
            Inimigo passaro2[MAX_ENEMIES] = { 0 };
            Inimigo bike[MAX_ENEMIES] = { 0 };
            Inimigo madeira[MAX_ENEMIES] = { 0 };
            Inimigo caboclo[MAX_ENEMIES] = { 0 };
            Inimigo peixe[MAX_ENEMIES] = { 0 };

            Texture2D texPassaro1 = { 0 };
            Texture2D texPassaro2 = { 0 };
            Texture2D texBike = { 0 };
            Texture2D texMadeira = { 0 };
            Texture2D texCaboclo = { 0 };
            Texture2D texPeixe = { 0 };

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
                float bossScale = (float)currentHeight * 0.40f / 252.0f;

                bool sinalPassaro1 = IsKeyPressed(KEY_ONE);
                bool sinalPassaro2 = IsKeyPressed(KEY_TWO);
                bool sinalBike = IsKeyPressed(KEY_B);
                bool sinalMadeira = IsKeyPressed(KEY_M);
                bool sinalCaboclo = IsKeyPressed(KEY_C);
                bool sinalPeixe = IsKeyPressed(KEY_P);

                UpdateBackground(&bg, dt);

                float standingY = groundY + (currentHeight * SIDEWALK_THICKNESS_RATIO * -0.2f);
                float playerStandingY = groundY + (currentHeight * SIDEWALK_THICKNESS_RATIO * 0.1f);
                UpdatePlayer(&player, dt, playerStandingY, playerScale);
                Rectangle playerHitbox = GetPlayerHitbox(&player, playerScale);

                UpdateHairyLeg(&pernaCabeluda, playerHitbox, dt, standingY, bossScale);

                BeginDrawing();
                    ClearBackground(BLACK);
                    DrawBackground(&bg, currentWidth, currentHeight, groundY);

                    bool spawnP1 = false, spawnP2 = false, spawnB = false, spawnM = false, spawnC = false, spawnPx = false;

                    for (int i = 0; i < MAX_ENEMIES; i++) {
                        bool triggerP1 = (sinalPassaro1 && !spawnP1 && !passaro1[i].ativo) ? (spawnP1 = true) : false;
                        bool triggerP2 = (sinalPassaro2 && !spawnP2 && !passaro2[i].ativo) ? (spawnP2 = true) : false;
                        bool triggerB = (sinalBike && !spawnB && !bike[i].ativo) ? (spawnB = true) : false;
                        bool triggerM = (sinalMadeira && !spawnM && !madeira[i].ativo) ? (spawnM = true) : false;
                        bool triggerC = (sinalCaboclo && !spawnC && !caboclo[i].ativo) ? (spawnC = true) : false;
                        bool triggerPx = (sinalPeixe && !spawnPx && !peixe[i].ativo) ? (spawnPx = true) : false;

                        AtualizarDesenharPassaro1(&passaro1[i], currentWidth, currentHeight, triggerP1, 0, texPassaro1);
                        AtualizarDesenharPassaro2(&passaro2[i], currentWidth, currentHeight, triggerP2, 0, texPassaro2);
                        AtualizarDesenharBike(&bike[i], currentWidth, currentHeight, triggerB, 0, texBike);
                        AtualizarDesenharMadeira(&madeira[i], currentWidth, currentHeight, triggerM, 0, texMadeira);
                        AtualizarDesenharCaboclo(&caboclo[i], currentWidth, currentHeight, triggerC, 0, texCaboclo);
                        AtualizarDesenharPeixe(&peixe[i], currentWidth, currentHeight, triggerPx, 0, texPeixe);

                        // Verificação de colisão com inimigos comuns
                        if (passaro1[i].ativo && CheckCollisionRecs(playerHitbox, (Rectangle){passaro1[i].posicao.x, passaro1[i].posicao.y, passaro1[i].tamanho.x, passaro1[i].tamanho.y})) currentScreen = SCREEN_START;
                        if (passaro2[i].ativo && CheckCollisionRecs(playerHitbox, (Rectangle){passaro2[i].posicao.x, passaro2[i].posicao.y, passaro2[i].tamanho.x, passaro2[i].tamanho.y})) currentScreen = SCREEN_START;
                        if (bike[i].ativo && CheckCollisionRecs(playerHitbox, (Rectangle){bike[i].posicao.x, bike[i].posicao.y, bike[i].tamanho.x, bike[i].tamanho.y})) currentScreen = SCREEN_START;
                        if (madeira[i].ativo && CheckCollisionRecs(playerHitbox, (Rectangle){madeira[i].posicao.x, madeira[i].posicao.y, madeira[i].tamanho.x, madeira[i].tamanho.y})) currentScreen = SCREEN_START;
                        if (caboclo[i].ativo && CheckCollisionRecs(playerHitbox, (Rectangle){caboclo[i].posicao.x, caboclo[i].posicao.y, caboclo[i].tamanho.x, caboclo[i].tamanho.y})) currentScreen = SCREEN_START;
                        if (peixe[i].ativo && CheckCollisionRecs(playerHitbox, (Rectangle){peixe[i].posicao.x, peixe[i].posicao.y, peixe[i].tamanho.x, peixe[i].tamanho.y})) currentScreen = SCREEN_START;
                    }

                    DrawPlayer(&player, playerScale);

                    DrawHairyLeg(&pernaCabeluda, bossScale);

                    // Verificação de colisão com o Boss (Perna Cabeluda) e seus ataques
                    if (CheckCollisionRecs(playerHitbox, pernaCabeluda.rect)) currentScreen = SCREEN_START;
                    if (pernaCabeluda.isKickActive && CheckCollisionRecs(playerHitbox, pernaCabeluda.kickHitbox)) currentScreen = SCREEN_START;
                    if (pernaCabeluda.waveLeft.active && CheckCollisionRecs(playerHitbox, pernaCabeluda.waveLeft.rect)) currentScreen = SCREEN_START;
                    if (pernaCabeluda.waveRight.active && CheckCollisionRecs(playerHitbox, pernaCabeluda.waveRight.rect)) currentScreen = SCREEN_START;

                    DrawText("1: P1 | 2: P2 | B: Bike | M: Madeira | C: Caboclo | P: Peixe", 20, 20, 20, GRAY);

                EndDrawing();
            }

            UnloadPlayer(&player);
            UnloadBackground(&bg);
        }
    }

    CloseWindow();

    return 0;
}
