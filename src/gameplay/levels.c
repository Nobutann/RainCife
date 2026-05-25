#include "gameplay/levels.h"
#include <stdlib.h>

Level* CreateLevel(int id, int enemyConfigId, float spawnInterval, float duration, float barExtraDuration, int bossId) {
    Level *newLevel = (Level*)malloc(sizeof(Level));
    if (newLevel) {
        newLevel->id = id;
        newLevel->enemyConfigId = enemyConfigId;
        newLevel->spawnInterval = spawnInterval;
        newLevel->duration = duration;
        newLevel->barExtraDuration = barExtraDuration;
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
        Level *last = GetLastLevel(*head);
        last->next = newLevel;
        newLevel->prev = last;
    }
}

Level* GetLastLevel(Level *head) {
    if (head == NULL) {
        return NULL;
    }

    Level *current = head;
    while (current->next != NULL && current->next != head) {
        current = current->next;
    }
    return current;
}

Level* FindLevelById(Level *head, int levelId) {
    Level *current = head;
    while (current != NULL) {
        if (current->id == levelId) {
            return current;
        }

        current = current->next;
        if (current == head) {
            break;
        }
    }
    return head;
}

bool CanAdvanceLevel(Level *current) {
    return current != NULL && current->next != NULL;
}

Level* GetNextLevel(Level *head, Level *current, bool wrapAround) {
    if (current == NULL) {
        return head;
    }
    if (current->next != NULL) {
        return current->next;
    }
    return wrapAround ? head : NULL;
}

Level* GetPreviousLevel(Level *head, Level *current, bool wrapAround) {
    if (current == NULL) {
        return head;
    }
    if (current->prev != NULL) {
        return current->prev;
    }
    return wrapAround ? GetLastLevel(head) : NULL;
}

void MakeLevelsCircular(Level *head) {
    Level *last = GetLastLevel(head);
    if (head == NULL || last == NULL || last == head) {
        return;
    }

    last->next = head;
    head->prev = last;
}

void FreeLevels(Level *head) {
    Level *current = head;
    while (current != NULL) {
        Level *next = current->next;
        free(current);
        if (next == head) {
            break;
        }
        current = next;
    }
}

Level* InitGameLevels() {
    Level *head = NULL;

    AddLevel(&head, CreateLevel(1, 1, 1.2f, 30.0f, 3.0f, 1));
    AddLevel(&head, CreateLevel(2, 2, 1.0f, 45.0f, 5.0f, 2));
    AddLevel(&head, CreateLevel(3, 3, 0.8f, 60.0f, 7.0f, 3));

    // debug levels: duration=0 pula direto ao boss
    AddLevel(&head, CreateLevel(4, 1, 1.0f, 0.0f, 0.0f, 1));
    AddLevel(&head, CreateLevel(5, 2, 1.0f, 0.0f, 0.0f, 2));
    AddLevel(&head, CreateLevel(6, 3, 1.0f, 0.0f, 0.0f, 3));

    return head;
}

Level* InitInfiniteLevels() {
    Level *head = NULL;

    AddLevel(&head, CreateLevel(1, 1, 1.2f, 0.0f, 0.0f, 0));
    AddLevel(&head, CreateLevel(2, 2, 1.0f, 0.0f, 0.0f, 0));
    AddLevel(&head, CreateLevel(3, 3, 0.8f, 0.0f, 0.0f, 0));
    MakeLevelsCircular(head);

    return head;
}
