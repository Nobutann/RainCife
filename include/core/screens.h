#ifndef SCREENS_H
#define SCREENS_H

#include <raylib.h>
#include "core/config_manager.h"

typedef enum
{
    SCREEN_INTRO,
    SCREEN_GAME,
    SCREEN_START,
    SCREEN_HISTORY_ANIMATION,
    SCREEN_CHARACTER_SELECT,
    SCREEN_LEVEL_SELECT,
    SCREEN_ITEMS,
    SCREEN_INFINITE_MENU,
    SCREEN_DAILY_CHALLENGES,
    SCREEN_INFINITE_SOON,
    SCREEN_INFINITE_GAME,
    SCREEN_CREDITS,
    SCREEN_OPTIONS,
    SCREEN_EXIT
} GameScreen;

GameScreen RunIntro();
GameScreen RunHistoryAnimation();
void SetHistoryAnimationReturnScreen(GameScreen returnScreen);
GameScreen RunStart();
GameScreen RunCharacterSelect();
void SetCharacterSelectNextScreen(GameScreen nextScreen);
GameScreen RunLevelSelect();
GameScreen RunInfiniteMenu();
GameScreen RunDailyChallenges();
GameScreen RunItems();
void SetItemsReturnScreen(GameScreen returnScreen);
GameScreen RunInfiniteSoon();
GameScreen RunOptions(Config *config, GameScreen returnScreen);
GameScreen RunCredits();
GameScreen RunGame(Config *config);

int GetSelectedStoryLevelId(void);
int GetMaxUnlockedStoryLevelId(void);
void SetSelectedStoryLevelId(int levelId);
void UnlockStoryLevel(int levelId);
int GetSelectedCharacterId(void);
void SetSelectedCharacterId(int characterId);
int GetSelectedClothingId(void);
int GetSelectedWeaponId(void);

#endif
