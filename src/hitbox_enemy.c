#include "hitbox_enemy.h"
#include <raylib.h>

void AtualizarDesenharPassaro1(Inimigo *passaro, int currentWidth, int currentHeight, bool sinalAtivar, int velocidadeBase, Texture2D textura) {
  if (sinalAtivar && !passaro->ativo) {
    passaro->posicao.x = (float)currentWidth;
    passaro->posicao.y = currentHeight * 0.60f;
    passaro->tamanho = (Vector2){50, 50};
    passaro->ativo = true;
  }

  if (passaro->ativo) {
    passaro->posicao.x -= (7 + velocidadeBase);
    if (passaro->posicao.x < -50) passaro->ativo = false;

    if (textura.id > 0) {
      DrawTextureV(textura, passaro->posicao, WHITE);
    } else {
      DrawRectangleV(passaro->posicao, passaro->tamanho, RED);
    }
  }
}

void AtualizarDesenharPassaro2(Inimigo *passaro, int currentWidth, int currentHeight, bool sinalAtivar, int velocidadeBase, Texture2D textura) {
  if (sinalAtivar && !passaro->ativo) {
    passaro->posicao.x = (float)currentWidth;
    passaro->posicao.y = currentHeight * 0.55f;
    passaro->tamanho = (Vector2){50, 50};
    passaro->velocidadeY = 3.0f;
    passaro->ativo = true;
  }

  if (passaro->ativo) {
    passaro->posicao.x -= (10 + velocidadeBase);
    passaro->posicao.y += passaro->velocidadeY;

    if (passaro->posicao.y < currentHeight * 0.20f) {
      passaro->velocidadeY = 3.0f;
    } else if (passaro->posicao.y > currentHeight * 0.70f) {
      passaro->velocidadeY = -3.0f;
    }

    if (passaro->posicao.x < -50) passaro->ativo = false;

    if (textura.id > 0) {
      DrawTextureV(textura, passaro->posicao, WHITE);
    } else {
      DrawRectangleV(passaro->posicao, passaro->tamanho, BLUE);
    }
  }
}

void AtualizarDesenharBike(Inimigo *bike, int currentWidth, int currentHeight, bool sinalAtivar, int velocidadeBase, Texture2D textura) {
  if (sinalAtivar && !bike->ativo) {
    bike->posicao.x = (float)currentWidth;
    bike->posicao.y = currentHeight * 0.80f - 50;
    bike->tamanho = (Vector2){50, 50};
    bike->ativo = true;
  }

  if (bike->ativo) {
    bike->posicao.x -= (7 + velocidadeBase);
    if (bike->posicao.x < -50) bike->ativo = false;

    if (textura.id > 0) {
      DrawTextureV(textura, bike->posicao, WHITE);
    } else {
      DrawRectangleV(bike->posicao, bike->tamanho, GREEN);
    }
  }
}

void AtualizarDesenharMadeira(Inimigo *madeira, int currentWidth, int currentHeight, bool sinalAtivar, int velocidadeBase, Texture2D textura) {
  if (sinalAtivar && !madeira->ativo) {
    madeira->posicao.x = (float)currentWidth;
    madeira->posicao.y = currentHeight * 0.80f - 50;
    madeira->tamanho = (Vector2){100, 50};
    madeira->ativo = true;
  }

  if (madeira->ativo) {
    madeira->posicao.x -= (7 + velocidadeBase);
    if (madeira->posicao.x < -100) madeira->ativo = false;

    if (textura.id > 0) {
      DrawTextureV(textura, madeira->posicao, WHITE);
    } else {
      DrawRectangleV(madeira->posicao, madeira->tamanho, BROWN);
    }
  }
}

void AtualizarDesenharCaboclo(Inimigo *caboclo, int currentWidth, int currentHeight, bool sinalAtivar, int velocidadeBase, Texture2D textura) {
  if (sinalAtivar && !caboclo->ativo) {
    caboclo->posicao.x = (float)currentWidth;
    caboclo->posicao.y = currentHeight * 0.80f - 80;
    caboclo->tamanho = (Vector2){60, 80};
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
      DrawText("!", caboclo->posicao.x + 20, caboclo->posicao.y - 40, 40, ORANGE);
      if (GetTime() - caboclo->tempoEstado > 1.0) {
        caboclo->estado = 2;
      }
    } else if (caboclo->estado == 2) {
      caboclo->posicao.x -= 20;
    }

    if (caboclo->posicao.x < -100) caboclo->ativo = false;

    if (textura.id > 0) {
      DrawTextureV(textura, caboclo->posicao, WHITE);
    } else {
      DrawRectangleV(caboclo->posicao, caboclo->tamanho, PURPLE);
    }
  }
}

void AtualizarDesenharPeixe(Inimigo *peixe, int currentWidth, int currentHeight, bool sinalAtivar, int velocidadeBase, Texture2D textura) {
  if (sinalAtivar && !peixe->ativo) {
    peixe->posicao.x = currentWidth * 0.42f;
    peixe->posicao.y = currentHeight * 0.80f - 80;
    peixe->tamanho = (Vector2){60, 60};
    
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

    if (textura.id > 0) {
      DrawTextureV(textura, peixe->posicao, WHITE);
    } else {
      DrawRectangleV(peixe->posicao, peixe->tamanho, ORANGE);
      DrawCircle(peixe->posicao.x + peixe->tamanho.x/2, peixe->posicao.y + peixe->tamanho.y/2, 5, YELLOW);
    }
  }
}
