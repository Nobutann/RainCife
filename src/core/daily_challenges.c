#include "core/daily_challenges.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DAILY_CHALLENGES_FILE "desafios_diarios.bin"
#define GEMINI_REQUEST_FILE "desafios_gemini_request.json"
#define GEMINI_RESPONSE_FILE "desafios_gemini_response.json"

static int GetDailyChallengeDayKey(void)
{
    time_t now = time(NULL) - (3 * 60 * 60);
    struct tm *local = localtime(&now);
    if (local == NULL)
    {
        return 0;
    }
    return (local->tm_year + 1900) * 10000 + (local->tm_mon + 1) * 100 + local->tm_mday;
}

static DailyChallengeType ParseType(const char *text)
{
    if (strcmp(text, "DISTANCE") == 0) return DAILY_CHALLENGE_DISTANCE;
    if (strcmp(text, "KILL_TOTAL") == 0) return DAILY_CHALLENGE_KILL_TOTAL;
    if (strcmp(text, "KILL_BIRD") == 0) return DAILY_CHALLENGE_KILL_BIRD;
    if (strcmp(text, "KILL_BIKE") == 0) return DAILY_CHALLENGE_KILL_BIKE;
    if (strcmp(text, "KILL_WOOD") == 0) return DAILY_CHALLENGE_KILL_WOOD;
    if (strcmp(text, "KILL_POSTE") == 0) return DAILY_CHALLENGE_KILL_POSTE;
    if (strcmp(text, "KILL_FISH") == 0) return DAILY_CHALLENGE_KILL_FISH;
    if (strcmp(text, "RUNS") == 0) return DAILY_CHALLENGE_RUNS;
    return DAILY_CHALLENGE_NONE;
}

static char *TrimText(char *text)
{
    while (isspace((unsigned char)*text))
    {
        text++;
    }

    char *end = text + strlen(text);
    while (end > text && isspace((unsigned char)*(end - 1)))
    {
        end--;
    }
    *end = '\0';
    return text;
}

static bool IsSupportedChallenge(DailyChallengeType type, int target)
{
    if (target <= 0)
    {
        return false;
    }
    return type >= DAILY_CHALLENGE_DISTANCE && type <= DAILY_CHALLENGE_RUNS;
}

static void CopyChallengeText(char destination[DAILY_CHALLENGE_TEXT_MAX], const char *source)
{
    if (source == NULL || source[0] == '\0')
    {
        source = "Complete o desafio diario";
    }
    strncpy(destination, source, DAILY_CHALLENGE_TEXT_MAX - 1);
    destination[DAILY_CHALLENGE_TEXT_MAX - 1] = '\0';
}

static void SetChallenge(DailyChallenge *challenge, DailyChallengeType type, int target, const char *text)
{
    challenge->type = type;
    challenge->target = target;
    challenge->progress = 0;
    challenge->completed = false;
    CopyChallengeText(challenge->text, text);
}

static void SetFallbackChallenges(DailyChallengeState *state)
{
    int dayKey = GetDailyChallengeDayKey();
    int variant = dayKey % 3;

    state->dayKey = dayKey;
    state->generatedByAi = false;

    if (variant == 0)
    {
        SetChallenge(&state->challenges[0], DAILY_CHALLENGE_DISTANCE, 1200, "Chegue a 1200m no modo infinito");
        SetChallenge(&state->challenges[1], DAILY_CHALLENGE_KILL_TOTAL, 8, "Destrua 8 obstaculos");
        SetChallenge(&state->challenges[2], DAILY_CHALLENGE_RUNS, 3, "Jogue 3 partidas no modo infinito");
    }
    else if (variant == 1)
    {
        SetChallenge(&state->challenges[0], DAILY_CHALLENGE_DISTANCE, 1800, "Chegue a 1800m no modo infinito");
        SetChallenge(&state->challenges[1], DAILY_CHALLENGE_KILL_BIRD, 3, "Derrube 3 passaros");
        SetChallenge(&state->challenges[2], DAILY_CHALLENGE_KILL_BIKE, 2, "Destrua 2 bicicletas");
    }
    else
    {
        SetChallenge(&state->challenges[0], DAILY_CHALLENGE_DISTANCE, 2500, "Chegue a 2500m no modo infinito");
        SetChallenge(&state->challenges[1], DAILY_CHALLENGE_KILL_FISH, 2, "Derrote 2 peixes");
        SetChallenge(&state->challenges[2], DAILY_CHALLENGE_KILL_POSTE, 2, "Quebre 2 postes malditos");
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

static bool WriteGeminiRequest(void)
{
    FILE *file = fopen(GEMINI_REQUEST_FILE, "w");
    if (file == NULL)
    {
        return false;
    }

    const char *prompt =
        "Gere exatamente 3 desafios diarios para um endless runner chamado RainCife. "
        "Use somente estes tipos: DISTANCE, KILL_TOTAL, KILL_BIRD, KILL_BIKE, KILL_WOOD, KILL_POSTE, KILL_FISH, RUNS. "
        "Metas recomendadas: DISTANCE 800 a 3500, RUNS 1 a 5, kills 1 a 12. "
        "Responda somente em 3 linhas, sem markdown, no formato TIPO|META|TEXTO. "
        "O texto deve ser curto, em portugues, sem acentos, e verificavel pelo codigo.";

    fputs("{\"contents\":[{\"parts\":[{\"text\":\"", file);
    JsonWriteEscaped(file, prompt);
    fputs("\"}]}],\"generationConfig\":{\"temperature\":0.9,\"maxOutputTokens\":180}}", file);
    fclose(file);
    return true;
}

static void DecodeJsonText(char *text)
{
    char decoded[1024];
    int out = 0;

    for (int i = 0; text[i] != '\0' && out < (int)sizeof(decoded) - 1; i++)
    {
        if (text[i] == '\\' && text[i + 1] != '\0')
        {
            i++;
            if (text[i] == 'n')
            {
                decoded[out++] = '\n';
            }
            else if (text[i] == '"' || text[i] == '\\' || text[i] == '/')
            {
                decoded[out++] = text[i];
            }
            else
            {
                decoded[out++] = text[i];
            }
        }
        else
        {
            decoded[out++] = text[i];
        }
    }

    decoded[out] = '\0';
    strcpy(text, decoded);
}

static bool ExtractGeminiText(char *output, int outputSize)
{
    FILE *file = fopen(GEMINI_RESPONSE_FILE, "r");
    if (file == NULL)
    {
        return false;
    }

    char response[8192];
    size_t bytes = fread(response, 1, sizeof(response) - 1, file);
    fclose(file);
    response[bytes] = '\0';

    char *textKey = strstr(response, "\"text\"");
    if (textKey == NULL)
    {
        return false;
    }

    char *colon = strchr(textKey, ':');
    if (colon == NULL)
    {
        return false;
    }

    char *start = strchr(colon, '"');
    if (start == NULL)
    {
        return false;
    }
    start++;

    int out = 0;
    bool escaped = false;
    for (char *c = start; *c != '\0' && out < outputSize - 1; c++)
    {
        if (!escaped && *c == '"')
        {
            break;
        }
        if (!escaped && *c == '\\')
        {
            escaped = true;
        }
        else
        {
            if (escaped)
            {
                output[out++] = '\\';
                escaped = false;
            }
            output[out++] = *c;
        }
    }
    output[out] = '\0';
    DecodeJsonText(output);
    return output[0] != '\0';
}

static bool ParseChallengeLine(char *line, DailyChallenge *challenge)
{
    char *firstPipe = strchr(line, '|');
    if (firstPipe == NULL)
    {
        return false;
    }
    *firstPipe = '\0';

    char *secondPipe = strchr(firstPipe + 1, '|');
    if (secondPipe == NULL)
    {
        return false;
    }
    *secondPipe = '\0';

    char *typeText = line;
    char *targetText = firstPipe + 1;
    char *challengeText = secondPipe + 1;

    if (typeText == NULL || targetText == NULL || challengeText == NULL)
    {
        return false;
    }

    typeText = TrimText(typeText);
    targetText = TrimText(targetText);
    challengeText = TrimText(challengeText);

    DailyChallengeType type = ParseType(typeText);
    int target = atoi(targetText);
    if (!IsSupportedChallenge(type, target))
    {
        return false;
    }

    SetChallenge(challenge, type, target, challengeText);
    return true;
}

static bool ParseAiChallenges(const char *text, DailyChallengeState *state)
{
    char buffer[1024];
    strncpy(buffer, text, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    int count = 0;
    char *line = strtok(buffer, "\n");
    while (line != NULL && count < DAILY_CHALLENGE_COUNT)
    {
        DailyChallenge parsed = {0};
        if (ParseChallengeLine(line, &parsed))
        {
            state->challenges[count++] = parsed;
        }
        line = strtok(NULL, "\n");
    }

    if (count != DAILY_CHALLENGE_COUNT)
    {
        return false;
    }

    state->dayKey = GetDailyChallengeDayKey();
    state->generatedByAi = true;
    return true;
}

static bool TryGenerateAiChallenges(DailyChallengeState *state)
{
    const char *apiKey = getenv("GEMINI_API_KEY");
    if (apiKey == NULL || apiKey[0] == '\0')
    {
        return false;
    }
    if (!WriteGeminiRequest())
    {
        return false;
    }

    char command[1024];
    snprintf(
        command,
        sizeof(command),
        "curl -s -X POST -H \"Content-Type: application/json\" -d @%s \"https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash-lite:generateContent?key=%s\" -o %s",
        GEMINI_REQUEST_FILE,
        apiKey,
        GEMINI_RESPONSE_FILE
    );

    if (system(command) != 0)
    {
        return false;
    }

    char aiText[1024];
    if (!ExtractGeminiText(aiText, sizeof(aiText)))
    {
        return false;
    }

    return ParseAiChallenges(aiText, state);
}

DailyChallengeState CarregarDesafiosDiarios(void)
{
    DailyChallengeState state = {0};
    FILE *file = fopen(DAILY_CHALLENGES_FILE, "rb");
    if (file != NULL)
    {
        DailyChallengeState loaded = {0};
        size_t bytes = fread(&loaded, 1, sizeof(DailyChallengeState), file);
        fclose(file);
        if (bytes == sizeof(DailyChallengeState))
        {
            state = loaded;
            for (int i = 0; i < DAILY_CHALLENGE_COUNT; i++)
            {
                state.challenges[i].text[DAILY_CHALLENGE_TEXT_MAX - 1] = '\0';
            }
        }
    }
    GarantirDesafiosDiarios(&state);
    return state;
}

void SalvarDesafiosDiarios(DailyChallengeState state)
{
    FILE *file = fopen(DAILY_CHALLENGES_FILE, "wb");
    if (file != NULL)
    {
        fwrite(&state, sizeof(DailyChallengeState), 1, file);
        fclose(file);
    }
}

void GarantirDesafiosDiarios(DailyChallengeState *state)
{
    int today = GetDailyChallengeDayKey();
    if (state->dayKey == today && state->challenges[0].type != DAILY_CHALLENGE_NONE)
    {
        return;
    }

    DailyChallengeState generated = {0};
    if (!TryGenerateAiChallenges(&generated))
    {
        SetFallbackChallenges(&generated);
    }

    *state = generated;
    SalvarDesafiosDiarios(*state);
}

static bool ChallengeMatchesEnemy(DailyChallengeType type, EnemyType enemyType)
{
    if (type == DAILY_CHALLENGE_KILL_TOTAL)
    {
        return true;
    }
    if (type == DAILY_CHALLENGE_KILL_BIRD)
    {
        return enemyType == ENEMY_BIRD1 || enemyType == ENEMY_BIRD2;
    }
    if (type == DAILY_CHALLENGE_KILL_BIKE)
    {
        return enemyType == ENEMY_BIKE;
    }
    if (type == DAILY_CHALLENGE_KILL_WOOD)
    {
        return enemyType == ENEMY_WOOD;
    }
    if (type == DAILY_CHALLENGE_KILL_POSTE)
    {
        return enemyType == ENEMY_POSTE;
    }
    if (type == DAILY_CHALLENGE_KILL_FISH)
    {
        return enemyType == ENEMY_FISH;
    }
    return false;
}

static void UpdateChallengeProgress(DailyChallenge *challenge, int progress, bool useMax)
{
    if (challenge->completed)
    {
        return;
    }

    if (useMax)
    {
        if (progress > challenge->progress)
        {
            challenge->progress = progress;
        }
    }
    else
    {
        challenge->progress += progress;
    }

    if (challenge->progress >= challenge->target)
    {
        challenge->progress = challenge->target;
        challenge->completed = true;
    }
}

void RegistrarDistanciaDesafioDiario(float meters)
{
    DailyChallengeState state = CarregarDesafiosDiarios();
    int roundedMeters = (int)(meters + 0.5f);
    bool changed = false;

    for (int i = 0; i < DAILY_CHALLENGE_COUNT; i++)
    {
        if (state.challenges[i].type == DAILY_CHALLENGE_DISTANCE)
        {
            int oldProgress = state.challenges[i].progress;
            bool oldCompleted = state.challenges[i].completed;
            UpdateChallengeProgress(&state.challenges[i], roundedMeters, true);
            changed = changed || oldProgress != state.challenges[i].progress || oldCompleted != state.challenges[i].completed;
        }
    }

    if (changed)
    {
        SalvarDesafiosDiarios(state);
    }
}

void RegistrarInimigoDesafioDiario(EnemyType type)
{
    DailyChallengeState state = CarregarDesafiosDiarios();
    bool changed = false;

    for (int i = 0; i < DAILY_CHALLENGE_COUNT; i++)
    {
        if (ChallengeMatchesEnemy(state.challenges[i].type, type))
        {
            int oldProgress = state.challenges[i].progress;
            bool oldCompleted = state.challenges[i].completed;
            UpdateChallengeProgress(&state.challenges[i], 1, false);
            changed = changed || oldProgress != state.challenges[i].progress || oldCompleted != state.challenges[i].completed;
        }
    }

    if (changed)
    {
        SalvarDesafiosDiarios(state);
    }
}

void RegistrarRunDesafioDiario(void)
{
    DailyChallengeState state = CarregarDesafiosDiarios();
    bool changed = false;

    for (int i = 0; i < DAILY_CHALLENGE_COUNT; i++)
    {
        if (state.challenges[i].type == DAILY_CHALLENGE_RUNS)
        {
            int oldProgress = state.challenges[i].progress;
            bool oldCompleted = state.challenges[i].completed;
            UpdateChallengeProgress(&state.challenges[i], 1, false);
            changed = changed || oldProgress != state.challenges[i].progress || oldCompleted != state.challenges[i].completed;
        }
    }

    if (changed)
    {
        SalvarDesafiosDiarios(state);
    }
}
