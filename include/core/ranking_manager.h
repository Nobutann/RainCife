#ifndef RANKING_MANAGER_H
#define RANKING_MANAGER_H

#include <stdbool.h>

#define INFINITE_RANKING_MAX 10
#define INFINITE_PLAYER_NAME_MAX 24

typedef struct {
    char nome[INFINITE_PLAYER_NAME_MAX];
    float metros;
} RankingInfinitoEntrada;

typedef struct {
    RankingInfinitoEntrada entradas[INFINITE_RANKING_MAX];
    int quantidade;
} RankingInfinito;

void SalvarRankingInfinito(RankingInfinito ranking);
RankingInfinito CarregarRankingInfinito(void);
bool PontuacaoEntraNoTop10(const RankingInfinito *ranking, float metros);
void AdicionarPontuacaoRankingInfinito(RankingInfinito *ranking, const char *nome, float metros);

#endif
