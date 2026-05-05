#include "enemy_caller.h"
#include <raylib.h>

ConfiguracaoFase ObterConfiguracaoFase(int faseId) {
    ConfiguracaoFase config = {0};
    config.faseId = faseId;

    for (int i = 0; i < ENEMY_COUNT; i++) {
        config.inimigosPermitidos[i] = false;
    }

    switch (faseId) {
        case 1:
            config.inimigosPermitidos[ENEMY_BIRD1] = true;
            config.inimigosPermitidos[ENEMY_BIKE] = true;
            config.inimigosPermitidos[ENEMY_WOOD] = true;
            config.inimigosPermitidos[ENEMY_POSTE] = true;
            config.inimigosPermitidos[ENEMY_SAFE_POSTE] = true;
            break;
        case 2:
            config.inimigosPermitidos[ENEMY_BIRD1] = true;
            config.inimigosPermitidos[ENEMY_BIRD2] = true;
            config.inimigosPermitidos[ENEMY_BIKE] = true;
            config.inimigosPermitidos[ENEMY_WOOD] = true;
            config.inimigosPermitidos[ENEMY_POSTE] = true;
            config.inimigosPermitidos[ENEMY_FISH] = true;
            config.inimigosPermitidos[ENEMY_SAFE_POSTE] = true;
            break;
        default:
            config.inimigosPermitidos[ENEMY_BIRD1] = true;
            break;
    }

    return config;
}

bool InimigoPermitidoNaFase(int faseId, EnemyType tipo) {
    ConfiguracaoFase config = ObterConfiguracaoFase(faseId);
    if (tipo >= 0 && tipo < ENEMY_COUNT) {
        return config.inimigosPermitidos[tipo];
    }
    return false;
}

EnemyType SortearInimigoFase(int faseId) {
    ConfiguracaoFase config = ObterConfiguracaoFase(faseId);
    
    EnemyType permitidos[ENEMY_COUNT];
    int qtdPermitidos = 0;
    
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (config.inimigosPermitidos[i]) {
            permitidos[qtdPermitidos++] = (EnemyType)i;
        }
    }
    
    if (qtdPermitidos == 0) return ENEMY_COUNT;
    
    int sorteado = GetRandomValue(0, qtdPermitidos - 1);
    return permitidos[sorteado];
}
