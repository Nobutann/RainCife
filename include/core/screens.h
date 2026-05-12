#ifndef SCREENS_H
#define SCREENS_H

#include <raylib.h>
#include "core/config_manager.h"

typedef enum
{
    SCREEN_GAME,
    SCREEN_START,
    SCREEN_CHARACTER_SELECT,
    SCREEN_LEVEL_SELECT,
    SCREEN_ITEMS,
    SCREEN_INFINITE_SOON,
    SCREEN_CREDITS,
    SCREEN_OPTIONS,
    SCREEN_EXIT
} GameScreen;

GameScreen RunStart();
GameScreen RunCharacterSelect();
GameScreen RunLevelSelect();
GameScreen RunItems();
GameScreen RunInfiniteSoon();
GameScreen RunOptions(Config *config);
GameScreen RunCredits();
GameScreen RunGame(Config *config);

int GetSelectedStoryLevelId(void);
int GetMaxUnlockedStoryLevelId(void);
void SetSelectedStoryLevelId(int levelId);
void UnlockStoryLevel(int levelId);
int GetSelectedCharacterId(void);
void SetSelectedCharacterId(int characterId);
int GetSelectedClothingId(void);

#endif
