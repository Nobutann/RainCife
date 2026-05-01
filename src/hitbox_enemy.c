#include "hitbox_enemy.h"
#include <raylib.h>

void AtualizarDesenharPassaro1(Inimigo *passaro, int currentWidth,
                               int currentHeight, bool sinalAtivar,
                               int velocidadeBase) {
  if (sinalAtivar && !passaro->ativo) {
    passaro->posicao.x = (float)currentWidth;
    passaro->posicao.y = currentHeight * 0.60f;
    passaro->ativo = true;
  }

  if (passaro->ativo) {
    passaro->posicao.x -= (7 + velocidadeBase);
    if (passaro->posicao.x < -50)
      passaro->ativo = false;

    DrawRectangle(passaro->posicao.x, passaro->posicao.y, 50, 50, RED);
  }
}

void AtualizarDesenharPassaro2(Inimigo *passaro, int currentWidth,
                               int currentHeight, bool sinalAtivar,
                               int velocidadeBase) {
  if (sinalAtivar && !passaro->ativo) {
    passaro->posicao.x = (float)currentWidth;
    passaro->posicao.y = currentHeight * 0.55f;
    passaro->velocidadeY = 3.0f;
    passaro->ativo = true;
  }

  if (passaro->ativo) {
    passaro->posicao.x -= (8 + velocidadeBase);
    passaro->posicao.y += passaro->velocidadeY;

    if (passaro->posicao.y < currentHeight * 0.40f) {
      passaro->velocidadeY = 3.0f;
    } else if (passaro->posicao.y > currentHeight * 0.70f) {
      passaro->velocidadeY = -3.0f;
    }

    if (passaro->posicao.x < -50)
      passaro->ativo = false;

    DrawRectangle(passaro->posicao.x, passaro->posicao.y, 50, 50, BLUE);
  }
}

void AtualizarDesenharBike(Inimigo *bike, int currentWidth, int currentHeight,
                           bool sinalAtivar, int velocidadeBase) {
  if (sinalAtivar && !bike->ativo) {
    bike->posicao.x = (float)currentWidth;
    bike->posicao.y = currentHeight * 0.80f - 50;
    bike->ativo = true;
  }

  if (bike->ativo) {
    bike->posicao.x -= (7 + velocidadeBase);
    if (bike->posicao.x < -50)
      bike->ativo = false;

    DrawRectangle(bike->posicao.x, bike->posicao.y, 50, 50, GREEN);
  }
}

void AtualizarDesenharMadeira(Inimigo *madeira, int currentWidth,
                              int currentHeight, bool sinalAtivar,
                              int velocidadeBase) {
  if (sinalAtivar && !madeira->ativo) {
    madeira->posicao.x = (float)currentWidth;
    madeira->posicao.y = currentHeight * 0.80f - 50;
    madeira->ativo = true;
  }

  if (madeira->ativo) {
    madeira->posicao.x -= (7 + velocidadeBase);
    if (madeira->posicao.x < -100)
      madeira->ativo = false;

    DrawRectangle(madeira->posicao.x, madeira->posicao.y, 100, 50, BROWN);
  }
}

void AtualizarDesenharCaboclo(Inimigo *caboclo, int currentWidth,
                              int currentHeight, bool sinalAtivar,
                              int velocidadeBase) {
  if (sinalAtivar && !caboclo->ativo) {
    caboclo->posicao.x = (float)currentWidth;
    caboclo->posicao.y = currentHeight * 0.80f - 80;
    caboclo->estado = 0;
    caboclo->ativo = true;
  }

  if (caboclo->ativo) {
    if (caboclo->estado == 0) {
      caboclo->posicao.x -= (4 + velocidadeBase);
      if (caboclo->posicao.x < currentWidth * 0.7f) {
        caboclo->estado = 1;
        caboclo->tempoEstado = GetTime();
      }
    } else if (caboclo->estado == 1) {
      DrawText("!", caboclo->posicao.x + 20, caboclo->posicao.y - 40, 40,
               ORANGE);
      if (GetTime() - caboclo->tempoEstado > 1.0) {
        caboclo->estado = 2;
      }
    } else if (caboclo->estado == 2) {
      caboclo->posicao.x -= 20;
    }

    if (caboclo->posicao.x < -100)
      caboclo->ativo = false;

    DrawRectangle(caboclo->posicao.x, caboclo->posicao.y, 60, 80, PURPLE);
  }
}

void AtualizarDesenharPeixe(Inimigo *peixe, int currentWidth, int currentHeight,
                            bool sinalAtivar, int velocidadeBase) {
  if (sinalAtivar && !peixe->ativo) {
    peixe->posicao.x = currentWidth * 0.42f;
    peixe->posicao.y = currentHeight * 0.80f - 80;

    peixe->velocidadeY = -18.0f;
    peixe->velocidadeX = -(4.0f + velocidadeBase);
    peixe->ativo = true;
  }

  if (peixe->ativo) {
    peixe->posicao.x += peixe->velocidadeX;
    peixe->posicao.y += peixe->velocidadeY;
    peixe->velocidadeY += 0.55f;

    if (peixe->posicao.y > currentHeight || peixe->posicao.x < -100) {
      peixe->ativo = false;
    }

    DrawRectangle(peixe->posicao.x, peixe->posicao.y, 60, 60, ORANGE);
    DrawCircle(peixe->posicao.x + 30, peixe->posicao.y + 30, 5, YELLOW);
  }
}
