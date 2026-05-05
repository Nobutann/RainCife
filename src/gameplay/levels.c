#include "gameplay/levels.h"
#include <stdlib.h>

Level* CreateLevel(int id, int enemyConfigId, float spawnInterval, float duration, int bossId) {
    Level *newLevel = (Level*)malloc(sizeof(Level));
    if (newLevel) {
        newLevel->id = id;
        newLevel->enemyConfigId = enemyConfigId;
        newLevel->spawnInterval = spawnInterval;
        newLevel->duration = duration;
        newLevel->bossId = bossId;
        newLevel->next = NULL;
        newLevel->prev = NULL;
    }
    return newLevel;
}

void AddLevel(Level **head, Level *newLevel) {
    if (*head == NULL) {
        *head = newLevel;
    } else {
        Level *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newLevel;
        newLevel->prev = current;
    }
}

void FreeLevels(Level *head) {
    Level *current = head;
    while (current != NULL) {
        Level *next = current->next;
        free(current);
        current = next;
    }
}

Level* InitGameLevels() {
    Level *head = NULL;
    
    AddLevel(&head, CreateLevel(1, 1, 2.0f, 30.0f, 1));
    AddLevel(&head, CreateLevel(2, 2, 1.5f, 45.0f, 2));
    
    return head;
}
