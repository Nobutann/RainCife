#ifndef HITBOX_ENEMY_H
#define HITBOX_ENEMY_H

#include <raylib.h>

typedef struct {
  Vector2 posicao;
  float velocidadeX;
  float velocidadeY;
  bool ativo;
  int estado;
  double tempoEstado;
} Inimigo;

void AtualizarDesenharPassaro1(Inimigo *passaro, int currentWidth, int currentHeight, bool sinalAtivar, int velocidadeBase);
void AtualizarDesenharPassaro2(Inimigo *passaro, int currentWidth, int currentHeight, bool sinalAtivar, int velocidadeBase);
void AtualizarDesenharBike(Inimigo *bike, int currentWidth, int currentHeight, bool sinalAtivar, int velocidadeBase);
void AtualizarDesenharMadeira(Inimigo *madeira, int currentWidth, int currentHeight, bool sinalAtivar, int velocidadeBase);
void AtualizarDesenharCaboclo(Inimigo *caboclo, int currentWidth, int currentHeight, bool sinalAtivar, int velocidadeBase);
void AtualizarDesenharPeixe(Inimigo *peixe, int currentWidth, int currentHeight, bool sinalAtivar, int velocidadeBase);

#endif
