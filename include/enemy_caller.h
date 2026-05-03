#ifndef ENEMY_CALLER_H
#define ENEMY_CALLER_H

#include <stdbool.h>

typedef enum {
    ENEMY_PASSARO1,
    ENEMY_PASSARO2,
    ENEMY_BIKE,
    ENEMY_MADEIRA,
    ENEMY_CABOCLO,
    ENEMY_PEIXE,
    ENEMY_COUNT
} EnemyType;

typedef struct {
    int faseId;
    bool inimigosPermitidos[ENEMY_COUNT];
} ConfiguracaoFase;

ConfiguracaoFase ObterConfiguracaoFase(int faseId);
bool InimigoPermitidoNaFase(int faseId, EnemyType tipo);
EnemyType SortearInimigoFase(int faseId);

#endif
