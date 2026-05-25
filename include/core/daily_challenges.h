#ifndef DAILY_CHALLENGES_H
#define DAILY_CHALLENGES_H

#include <stdbool.h>
#include "entities/enemy.h"

#define DAILY_CHALLENGE_COUNT 3
#define DAILY_CHALLENGE_TEXT_MAX 128

typedef enum
{
    DAILY_CHALLENGE_NONE = 0,
    DAILY_CHALLENGE_DISTANCE = 1,
    DAILY_CHALLENGE_KILL_TOTAL = 2,
    DAILY_CHALLENGE_KILL_BIRD = 3,
    DAILY_CHALLENGE_KILL_BIKE = 4,
    DAILY_CHALLENGE_KILL_WOOD = 5,
    DAILY_CHALLENGE_KILL_POSTE = 6,
    DAILY_CHALLENGE_KILL_FISH = 7,
    DAILY_CHALLENGE_RUNS = 8
} DailyChallengeType;

typedef struct
{
    DailyChallengeType type;
    int target;
    int progress;
    bool completed;
    char text[DAILY_CHALLENGE_TEXT_MAX];
} DailyChallenge;

typedef struct
{
    int dayKey;
    bool generatedByAi;
    DailyChallenge challenges[DAILY_CHALLENGE_COUNT];
} DailyChallengeState;

DailyChallengeState CarregarDesafiosDiarios(void);
void SalvarDesafiosDiarios(DailyChallengeState state);
void GarantirDesafiosDiarios(DailyChallengeState *state);
DailyChallengeState ResetarDesafiosDiariosConcluidos(void);
void RegistrarDistanciaDesafioDiario(float meters);
void RegistrarInimigoDesafioDiario(EnemyType type);
void RegistrarRunDesafioDiario(void);

#endif
