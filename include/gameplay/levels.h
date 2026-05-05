#ifndef LEVELS_H
#define LEVELS_H

#include <stdbool.h>

typedef struct Level {
    int id;
    int enemyConfigId;
    float spawnInterval;
    float duration;
    int bossId;
    struct Level *next;
    struct Level *prev;
} Level;

Level* CreateLevel(int id, int enemyConfigId, float spawnInterval, float duration, int bossId);
void AddLevel(Level **head, Level *newLevel);
void FreeLevels(Level *head);
Level* InitGameLevels();

#endif
