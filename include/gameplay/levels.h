#ifndef LEVELS_H
#define LEVELS_H

#include <stdbool.h>

typedef struct Level {
    int id;
    int enemyConfigId;
    float spawnInterval;
    float duration;
    float barExtraDuration;
    int bossId;
    struct Level *next;
    struct Level *prev;
} Level;

Level* CreateLevel(int id, int enemyConfigId, float spawnInterval, float duration, float barExtraDuration, int bossId);
void AddLevel(Level **head, Level *newLevel);
Level* GetLastLevel(Level *head);
Level* FindLevelById(Level *head, int levelId);
bool CanAdvanceLevel(Level *current);
Level* GetNextLevel(Level *head, Level *current, bool wrapAround);
Level* GetPreviousLevel(Level *head, Level *current, bool wrapAround);
void MakeLevelsCircular(Level *head);
void FreeLevels(Level *head);
Level* InitGameLevels();
Level* InitInfiniteLevels();

#endif
