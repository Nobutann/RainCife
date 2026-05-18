#include <stdio.h>
#include <string.h>
#include "core/ranking_manager.h"

#define RANKING_INFINITO_ARQUIVO "ranking_infinito.bin"

static void CopiarNomeJogador(char destino[INFINITE_PLAYER_NAME_MAX], const char *origem)
{
    if (origem == NULL || origem[0] == '\0')
    {
        origem = "Jogador";
    }

    strncpy(destino, origem, INFINITE_PLAYER_NAME_MAX - 1);
    destino[INFINITE_PLAYER_NAME_MAX - 1] = '\0';
}

static void TrocarEntradas(RankingInfinitoEntrada *a, RankingInfinitoEntrada *b)
{
    RankingInfinitoEntrada temp = *a;
    *a = *b;
    *b = temp;
}

static int ParticionarRanking(RankingInfinitoEntrada entradas[], int inicio, int fim)
{
    float pivo = entradas[fim].metros;
    int i = inicio - 1;

    for (int j = inicio; j < fim; j++)
    {
        if (entradas[j].metros >= pivo)
        {
            i++;
            TrocarEntradas(&entradas[i], &entradas[j]);
        }
    }

    TrocarEntradas(&entradas[i + 1], &entradas[fim]);
    return i + 1;
}

static void QuickSortRanking(RankingInfinitoEntrada entradas[], int inicio, int fim)
{
    if (inicio < fim)
    {
        int pivo = ParticionarRanking(entradas, inicio, fim);
        QuickSortRanking(entradas, inicio, pivo - 1);
        QuickSortRanking(entradas, pivo + 1, fim);
    }
}

static void OrdenarRanking(RankingInfinito *ranking)
{
    if (ranking->quantidade > 1)
    {
        QuickSortRanking(ranking->entradas, 0, ranking->quantidade - 1);
    }
}

void SalvarRankingInfinito(RankingInfinito ranking)
{
    FILE *arquivo = fopen(RANKING_INFINITO_ARQUIVO, "wb");
    if (arquivo)
    {
        fwrite(&ranking, sizeof(RankingInfinito), 1, arquivo);
        fclose(arquivo);
    }
}

RankingInfinito CarregarRankingInfinito(void)
{
    RankingInfinito ranking = {0};
    FILE *arquivo = fopen(RANKING_INFINITO_ARQUIVO, "rb");
    if (arquivo)
    {
        RankingInfinito lido = {0};
        size_t bytesLidos = fread(&lido, 1, sizeof(RankingInfinito), arquivo);
        if (bytesLidos == sizeof(RankingInfinito) &&
            lido.quantidade >= 0 &&
            lido.quantidade <= INFINITE_RANKING_MAX)
        {
            ranking = lido;
            for (int i = 0; i < ranking.quantidade; i++)
            {
                ranking.entradas[i].nome[INFINITE_PLAYER_NAME_MAX - 1] = '\0';
            }
            OrdenarRanking(&ranking);
        }
        fclose(arquivo);
    }
    return ranking;
}

bool PontuacaoEntraNoTop10(const RankingInfinito *ranking, float metros)
{
    if (ranking->quantidade < INFINITE_RANKING_MAX)
    {
        return true;
    }

    return metros > ranking->entradas[ranking->quantidade - 1].metros;
}

void AdicionarPontuacaoRankingInfinito(RankingInfinito *ranking, const char *nome, float metros)
{
    OrdenarRanking(ranking);

    if (ranking->quantidade < INFINITE_RANKING_MAX)
    {
        CopiarNomeJogador(ranking->entradas[ranking->quantidade].nome, nome);
        ranking->entradas[ranking->quantidade].metros = metros;
        ranking->quantidade++;
    }
    else if (metros > ranking->entradas[INFINITE_RANKING_MAX - 1].metros)
    {
        CopiarNomeJogador(ranking->entradas[INFINITE_RANKING_MAX - 1].nome, nome);
        ranking->entradas[INFINITE_RANKING_MAX - 1].metros = metros;
    }

    OrdenarRanking(ranking);
    SalvarRankingInfinito(*ranking);
}
