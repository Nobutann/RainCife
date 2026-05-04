#ifndef ENEMY_CALLER_H
#define ENEMY_CALLER_H

#include <stdbool.h>
#include "entities/enemy.h"

typedef struct {
    int faseId;
    bool inimigosPermitidos[ENEMY_COUNT];
} ConfiguracaoFase;

ConfiguracaoFase ObterConfiguracaoFase(int faseId);
bool InimigoPermitidoNaFase(int faseId, EnemyType tipo);
EnemyType SortearInimigoFase(int faseId);

#endif
