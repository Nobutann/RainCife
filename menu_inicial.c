#include <raylib.h>
#include "config_manager.h"
#include "hitbox_menu_ini.h"
#include "menu_opcoes.h"
#include "menu_creditos.h"

typedef enum { TELA_MENU, TELA_OPCOES, TELA_CREDITOS } Tela;

int main(void) {
  InitWindow(800, 450, "Jogo AED");
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetExitKey(KEY_DELETE);

  Config config = CarregarConfig();

  int monitor = GetCurrentMonitor();
  SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
  if (config.telaCheia && !IsWindowFullscreen())
    ToggleFullscreen();

  SetTargetFPS(60);
  Tela telaAtual = TELA_MENU;

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_F11))
      ToggleFullscreen();

    int currentWidth = GetScreenWidth();
    int currentHeight = GetScreenHeight();

    if (telaAtual == TELA_MENU) {
      int clique = VerificarCliqueMenu(currentWidth, currentHeight, config.idioma);
      if (clique == 1)
        telaAtual = TELA_OPCOES;
      if (clique == 2)
        telaAtual = TELA_CREDITOS;
      if (clique == 3)
        break;
    } else {
      if (IsKeyPressed(KEY_ESCAPE))
        telaAtual = TELA_MENU;
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (telaAtual == TELA_MENU) {
      const char *titulo = config.idioma ? "AED GAME" : "JOGO AED";
      int tSize = currentHeight / 10;
      DrawText(titulo, (currentWidth / 2) - (MeasureText(titulo, tSize) / 2),
               currentHeight / 6, tSize, DARKBLUE);

      int menuFontSize = currentHeight / 20;
      int startY = currentHeight / 2.5;
      int spacing = menuFontSize + 20;

      const char *optsPT[] = {"Jogar", "Opções", "Créditos", "Sair"};
      const char *optsEN[] = {"Play", "Options", "Credits", "Exit"};

      for (int i = 0; i < 4; i++) {
        const char *txt = config.idioma ? optsEN[i] : optsPT[i];
        DrawText(txt, (currentWidth / 2) - (MeasureText(txt, menuFontSize) / 2),
                 startY + (i * spacing), menuFontSize, DARKGRAY);
      }
    } else if (telaAtual == TELA_OPCOES) {
      DesenharMenuOpcoes(currentWidth, currentHeight, &config);
    } else if (telaAtual == TELA_CREDITOS) {
      DesenharMenuCreditos(currentWidth, currentHeight);
    }

    const char *help = config.idioma
                           ? "F11: Fullscreen | ESC: Back | DEL: Exit"
                           : "F11: Tela Cheia | ESC: Voltar | DEL: Sair";
    DrawText(help, 20, currentHeight - 40, 20, GRAY);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
