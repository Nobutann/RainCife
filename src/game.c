#include <raylib.h>
#include "screens.h"
#include "game.h"
/*
GameScreen RunGame(Config *config) {
    static Inimigo passaro1[MAX_ENEMIES] = { 0 };
    static Inimigo passaro2[MAX_ENEMIES] = { 0 };
    static Inimigo bike[MAX_ENEMIES] = { 0 };
    static Inimigo madeira[MAX_ENEMIES] = { 0 };
    static Inimigo caboclo[MAX_ENEMIES] = { 0 };
    static Inimigo peixe[MAX_ENEMIES] = { 0 };

    Texture2D texPassaro1 = { 0 };
    Texture2D texPassaro2 = { 0 };
    Texture2D texBike = { 0 };
    Texture2D texMadeira = { 0 };
    Texture2D texCaboclo = { 0 };
    Texture2D texPeixe = { 0 };

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) return SCREEN_START;

        int currentWidth = GetScreenWidth();
        int currentHeight = GetScreenHeight();

        bool sinalPassaro1 = IsKeyPressed(KEY_SPACE);
        bool sinalPassaro2 = IsKeyPressed(KEY_ENTER);
        bool sinalBike = IsKeyPressed(KEY_B);
        bool sinalMadeira = IsKeyPressed(KEY_M);
        bool sinalCaboclo = IsKeyPressed(KEY_C);
        bool sinalPeixe = IsKeyPressed(KEY_P);

        BeginDrawing();
            ClearBackground(RAYWHITE);

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
            }

            const char* help = config->idioma ? 
                "ESC: Back | P1: ESP | P2: ENT | B: Bike | M: Wood | C: Dash | P: Fish" : 
                "ESC: Voltar | P1: ESP | P2: ENT | B: Bike | M: Madeira | C: Caboclo | P: Peixe";
            
            DrawText(help, 20, 20, 20, GRAY);
        EndDrawing();
    }
    return SCREEN_EXIT;
}

*/
