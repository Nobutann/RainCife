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

    AddLevel(&head, CreateLevel(1, 1, 1.2f, 30.0f, 1));
    AddLevel(&head, CreateLevel(2, 2, 1.0f, 45.0f, 2));
    AddLevel(&head, CreateLevel(3, 1, 0.8f, 60.0f, 0));

    // debug levels: duration=0 pula direto ao boss
    AddLevel(&head, CreateLevel(4, 1, 1.0f, 0.0f, 1));
    AddLevel(&head, CreateLevel(5, 2, 1.0f, 0.0f, 2));
    AddLevel(&head, CreateLevel(6, 1, 1.0f, 0.0f, 0));

    return head;
}
