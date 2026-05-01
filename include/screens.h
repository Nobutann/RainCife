#ifndef SCREENS_H
#define SCREENS_H

#include <raylib.h>
#include "config_manager.h"

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

#endif