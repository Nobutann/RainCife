#ifndef SCREENS_H
#define SCREENS_H

#include <raylib.h>
#include "core/config_manager.h"

typedef enum
{
    SCREEN_GAME,
    SCREEN_START,
    SCREEN_CREDITS,
    SCREEN_OPTIONS,
    SCREEN_EXIT
} GameScreen;

GameScreen RunStart();
GameScreen RunOptions(Config *config);
GameScreen RunCredits();
GameScreen RunGame(Config *config);

#endif