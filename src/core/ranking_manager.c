#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core/ranking_manager.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#define RANKING_INFINITO_ARQUIVO "ranking_infinito.bin"
#define RANKING_SUPABASE_RESPONSE_FILE "ranking_supabase_response.json"
#define RANKING_SUPABASE_REQUEST_FILE "ranking_supabase_request.json"
#define SUPABASE_URL_MAX 256
#define SUPABASE_KEY_MAX 512
#define SUPABASE_COMMAND_MAX 2048
#define SUPABASE_RESPONSE_MAX 8192
#ifdef _WIN32
#define SUPABASE_NULL_OUTPUT "NUL"
#else
#define SUPABASE_NULL_OUTPUT "/dev/null"
#endif

struct RankingInfinitoFetch
{
    RankingInfinito ranking;
    bool success;
#ifdef _WIN32
    HANDLE thread;
#else
    pthread_t thread;
#endif
};

typedef struct
{
    char nome[INFINITE_PLAYER_NAME_MAX];
    float metros;
} RankingInfinitoSubmit;

static void CopiarNomeJogador(char destino[INFINITE_PLAYER_NAME_MAX], const char *origem)
{
    if (origem == NULL || origem[0] == '\0')
    {
        origem = "Jogador";
    }

    strncpy(destino, origem, INFINITE_PLAYER_NAME_MAX - 1);
    destino[INFINITE_PLAYER_NAME_MAX - 1] = '\0';
}

static char *TrimConfigValue(char *text)
{
    while (*text == ' ' || *text == '\t')
    {
        text++;
    }

    char *end = text + strlen(text);
    while (end > text && (end[-1] == ' ' || end[-1] == '\t' || end[-1] == '\r' || end[-1] == '\n'))
    {
        end--;
    }
    *end = '\0';
    return text;
}

static bool IsPlaceholderValue(const char *value)
{
    return value == NULL ||
           value[0] == '\0' ||
           strstr(value, "SEU_PROJECT_REF") != NULL ||
           strstr(value, "SUA_CHAVE") != NULL;
}

static bool GetConfigValue(const char *key, char *output, int outputSize)
{
    const char *envValue = getenv(key);
    if (!IsPlaceholderValue(envValue))
    {
        strncpy(output, envValue, outputSize - 1);
        output[outputSize - 1] = '\0';
        return true;
    }

    FILE *file = fopen(".env", "r");
    if (file == NULL)
    {
        return false;
    }

    char line[768];
    bool found = false;
    int keyLength = (int)strlen(key);
    while (fgets(line, sizeof(line), file) != NULL)
    {
        char *trimmed = TrimConfigValue(line);
        if (trimmed[0] == '#' || strncmp(trimmed, key, keyLength) != 0 || trimmed[keyLength] != '=')
        {
            continue;
        }

        char *value = TrimConfigValue(trimmed + keyLength + 1);
        if (!IsPlaceholderValue(value))
        {
            strncpy(output, value, outputSize - 1);
            output[outputSize - 1] = '\0';
            found = true;
            break;
        }
    }

    fclose(file);
    return found;
}

static bool GetSupabaseConfig(char *url, int urlSize, char *anonKey, int anonKeySize)
{
    return GetConfigValue("SUPABASE_URL", url, urlSize) &&
           GetConfigValue("SUPABASE_ANON_KEY", anonKey, anonKeySize);
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

static void JsonWriteEscaped(FILE *file, const char *text)
{
    for (const char *c = text; *c != '\0'; c++)
    {
        if (*c == '"' || *c == '\\')
        {
            fputc('\\', file);
        }

        if (*c == '\n')
        {
            fputs("\\n", file);
        }
        else if (*c != '\r')
        {
            fputc(*c, file);
        }
    }
}

static const char *FindJsonValueStart(const char *objectStart, const char *fieldName)
{
    char pattern[32];
    snprintf(pattern, sizeof(pattern), "\"%s\"", fieldName);

    const char *field = strstr(objectStart, pattern);
    if (field == NULL)
    {
        return NULL;
    }

    const char *colon = strchr(field, ':');
    if (colon == NULL)
    {
        return NULL;
    }

    colon++;
    while (*colon == ' ' || *colon == '\t' || *colon == '\r' || *colon == '\n')
    {
        colon++;
    }
    return colon;
}

static bool ParseJsonStringField(const char *objectStart, const char *fieldName, char *output, int outputSize)
{
    const char *value = FindJsonValueStart(objectStart, fieldName);
    if (value == NULL || *value != '"')
    {
        return false;
    }
    value++;

    int out = 0;
    bool escaped = false;
    while (*value != '\0' && out < outputSize - 1)
    {
        if (!escaped && *value == '"')
        {
            break;
        }
        if (!escaped && *value == '\\')
        {
            escaped = true;
            value++;
            continue;
        }

        output[out++] = *value;
        escaped = false;
        value++;
    }
    output[out] = '\0';
    return output[0] != '\0';
}

static bool ParseJsonFloatField(const char *objectStart, const char *fieldName, float *output)
{
    const char *value = FindJsonValueStart(objectStart, fieldName);
    if (value == NULL)
    {
        return false;
    }

    char *end = NULL;
    float parsed = strtof(value, &end);
    if (end == value)
    {
        return false;
    }

    *output = parsed;
    return true;
}

static bool ParseSupabaseRankingResponse(const char *response, RankingInfinito *ranking)
{
    RankingInfinito parsed = {0};
    const char *cursor = response;

    while ((cursor = strchr(cursor, '{')) != NULL && parsed.quantidade < INFINITE_RANKING_MAX)
    {
        RankingInfinitoEntrada entry = {0};
        if (ParseJsonStringField(cursor, "nome", entry.nome, sizeof(entry.nome)) &&
            ParseJsonFloatField(cursor, "metros", &entry.metros))
        {
            parsed.entradas[parsed.quantidade++] = entry;
        }
        cursor++;
    }

    if (parsed.quantidade == 0 && strchr(response, '[') == NULL)
    {
        return false;
    }

    OrdenarRanking(&parsed);
    *ranking = parsed;
    return true;
}

static bool FetchSupabaseRanking(RankingInfinito *ranking)
{
    char url[SUPABASE_URL_MAX];
    char anonKey[SUPABASE_KEY_MAX];
    if (!GetSupabaseConfig(url, sizeof(url), anonKey, sizeof(anonKey)))
    {
        return false;
    }

    char command[SUPABASE_COMMAND_MAX];
    snprintf(
        command,
        sizeof(command),
        "curl -s -f --max-time 2 \"%s/rest/v1/ranking_infinito?select=nome,metros&order=metros.desc&limit=10\" -H \"apikey: %s\" -H \"Authorization: Bearer %s\" -o %s",
        url,
        anonKey,
        anonKey,
        RANKING_SUPABASE_RESPONSE_FILE
    );

    if (system(command) != 0)
    {
        return false;
    }

    FILE *file = fopen(RANKING_SUPABASE_RESPONSE_FILE, "r");
    if (file == NULL)
    {
        return false;
    }

    char response[SUPABASE_RESPONSE_MAX];
    size_t bytes = fread(response, 1, sizeof(response) - 1, file);
    fclose(file);
    response[bytes] = '\0';

    return ParseSupabaseRankingResponse(response, ranking);
}

static bool WriteSupabaseRankingRequest(const char *nome, float metros)
{
    FILE *file = fopen(RANKING_SUPABASE_REQUEST_FILE, "w");
    if (file == NULL)
    {
        return false;
    }

    fputs("{\"nome\":\"", file);
    JsonWriteEscaped(file, nome);
    fprintf(file, "\",\"metros\":%.2f}", metros);
    fclose(file);
    return true;
}

static bool InsertSupabaseRankingEntry(const char *nome, float metros)
{
    char url[SUPABASE_URL_MAX];
    char anonKey[SUPABASE_KEY_MAX];
    if (!GetSupabaseConfig(url, sizeof(url), anonKey, sizeof(anonKey)) ||
        !WriteSupabaseRankingRequest(nome, metros))
    {
        return false;
    }

    char command[SUPABASE_COMMAND_MAX];
    snprintf(
        command,
        sizeof(command),
        "curl -s -f --max-time 2 -X POST \"%s/rest/v1/ranking_infinito\" -H \"apikey: %s\" -H \"Authorization: Bearer %s\" -H \"Content-Type: application/json\" -H \"Prefer: return=minimal\" -d @%s -o %s",
        url,
        anonKey,
        anonKey,
        RANKING_SUPABASE_REQUEST_FILE,
        SUPABASE_NULL_OUTPUT
    );

    return system(command) == 0;
}

#ifdef _WIN32
static DWORD WINAPI EnviarPontuacaoRankingInfinitoThread(LPVOID data)
#else
static void *EnviarPontuacaoRankingInfinitoThread(void *data)
#endif
{
    RankingInfinitoSubmit *submit = (RankingInfinitoSubmit *)data;
    InsertSupabaseRankingEntry(submit->nome, submit->metros);
    free(submit);
#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

static void EnviarPontuacaoRankingInfinitoOnlineAsync(const char *nome, float metros)
{
    RankingInfinitoSubmit *submit = (RankingInfinitoSubmit *)calloc(1, sizeof(RankingInfinitoSubmit));
    if (submit == NULL)
    {
        return;
    }

    CopiarNomeJogador(submit->nome, nome);
    submit->metros = metros;

#ifdef _WIN32
    HANDLE thread = CreateThread(NULL, 0, EnviarPontuacaoRankingInfinitoThread, submit, 0, NULL);
    if (thread == NULL)
    {
        free(submit);
        return;
    }
    CloseHandle(thread);
#else
    pthread_t thread;
    if (pthread_create(&thread, NULL, EnviarPontuacaoRankingInfinitoThread, submit) != 0)
    {
        free(submit);
        return;
    }
    pthread_detach(thread);
#endif
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

RankingInfinito CarregarRankingInfinitoLocal(void)
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

RankingInfinito CarregarRankingInfinito(void)
{
    RankingInfinito ranking = CarregarRankingInfinitoLocal();

    RankingInfinito rankingOnline = {0};
    if (FetchSupabaseRanking(&rankingOnline))
    {
        ranking = rankingOnline;
        SalvarRankingInfinito(ranking);
    }

    return ranking;
}

#ifdef _WIN32
static DWORD WINAPI BuscarRankingInfinitoOnlineThread(LPVOID data)
#else
static void *BuscarRankingInfinitoOnlineThread(void *data)
#endif
{
    RankingInfinitoFetch *fetch = (RankingInfinitoFetch *)data;
    fetch->success = FetchSupabaseRanking(&fetch->ranking);
#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

RankingInfinitoFetch *IniciarCarregamentoRankingInfinitoOnline(void)
{
    RankingInfinitoFetch *fetch = (RankingInfinitoFetch *)calloc(1, sizeof(RankingInfinitoFetch));
    if (fetch == NULL)
    {
        return NULL;
    }

#ifdef _WIN32
    fetch->thread = CreateThread(NULL, 0, BuscarRankingInfinitoOnlineThread, fetch, 0, NULL);
    if (fetch->thread == NULL)
    {
        free(fetch);
        return NULL;
    }
#else
    if (pthread_create(&fetch->thread, NULL, BuscarRankingInfinitoOnlineThread, fetch) != 0)
    {
        free(fetch);
        return NULL;
    }
#endif

    return fetch;
}

bool FinalizarCarregamentoRankingInfinitoOnline(RankingInfinitoFetch *fetch, RankingInfinito *ranking, bool *success)
{
    if (fetch == NULL)
    {
        return false;
    }

#ifdef _WIN32
    DWORD waitResult = WaitForSingleObject(fetch->thread, 0);
    if (waitResult != WAIT_OBJECT_0)
    {
        return false;
    }
    CloseHandle(fetch->thread);
#else
    if (pthread_tryjoin_np(fetch->thread, NULL) != 0)
    {
        return false;
    }
#endif

    if (success != NULL)
    {
        *success = fetch->success;
    }
    if (fetch->success && ranking != NULL)
    {
        *ranking = fetch->ranking;
        SalvarRankingInfinito(*ranking);
    }
    free(fetch);
    return true;
}

void CancelarCarregamentoRankingInfinitoOnline(RankingInfinitoFetch *fetch)
{
    if (fetch == NULL)
    {
        return;
    }

#ifdef _WIN32
    if (WaitForSingleObject(fetch->thread, 0) == WAIT_OBJECT_0)
    {
        CloseHandle(fetch->thread);
        free(fetch);
    }
#else
    if (pthread_tryjoin_np(fetch->thread, NULL) == 0)
    {
        free(fetch);
    }
    else
    {
        pthread_detach(fetch->thread);
    }
#endif
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
    char nomeNormalizado[INFINITE_PLAYER_NAME_MAX];
    CopiarNomeJogador(nomeNormalizado, nome);

    OrdenarRanking(ranking);

    if (ranking->quantidade < INFINITE_RANKING_MAX)
    {
        CopiarNomeJogador(ranking->entradas[ranking->quantidade].nome, nomeNormalizado);
        ranking->entradas[ranking->quantidade].metros = metros;
        ranking->quantidade++;
    }
    else if (metros > ranking->entradas[INFINITE_RANKING_MAX - 1].metros)
    {
        CopiarNomeJogador(ranking->entradas[INFINITE_RANKING_MAX - 1].nome, nomeNormalizado);
        ranking->entradas[INFINITE_RANKING_MAX - 1].metros = metros;
    }

    OrdenarRanking(ranking);
    SalvarRankingInfinito(*ranking);
    EnviarPontuacaoRankingInfinitoOnlineAsync(nomeNormalizado, metros);
}
