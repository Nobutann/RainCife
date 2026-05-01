#include <raylib.h>
#include "screens.h"
#include "game.h"
#include "hitbox_enemy.h"

GameScreen RunGame(Config *config) {
    static Inimigo passaro1 = { 0 };
    static Inimigo passaro2 = { 0 };
    static Inimigo bike = { 0 };
    static Inimigo madeira = { 0 };
    static Inimigo caboclo = { 0 };
    static Inimigo peixe = { 0 };

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

            AtualizarDesenharPassaro1(&passaro1, currentWidth, currentHeight, sinalPassaro1, 0);
            AtualizarDesenharPassaro2(&passaro2, currentWidth, currentHeight, sinalPassaro2, 0);
            AtualizarDesenharBike(&bike, currentWidth, currentHeight, sinalBike, 0);
            AtualizarDesenharMadeira(&madeira, currentWidth, currentHeight, sinalMadeira, 0);
            AtualizarDesenharCaboclo(&caboclo, currentWidth, currentHeight, sinalCaboclo, 0);
            AtualizarDesenharPeixe(&peixe, currentWidth, currentHeight, sinalPeixe, 0);

            const char* help = config->idioma ? 
                "ESC: Back | P1: ESP | P2: ENT | B: Bike | M: Wood | C: Dash | P: Fish" : 
                "ESC: Voltar | P1: ESP | P2: ENT | B: Bike | M: Madeira | C: Caboclo | P: Peixe";
            
            DrawText(help, 20, 20, 20, GRAY);
        EndDrawing();
    }
    return SCREEN_EXIT;
}
