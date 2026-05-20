#ifndef INTRO_SCREEN_H
#define INTRO_SCREEN_H

#include "core/screens.h"

// Plays the pre-extracted intro frames and fades out to SCREEN_START.
// SPACE or ENTER skips the video immediately (triggers the fade-out).
// Returns SCREEN_START when complete.
GameScreen RunIntro(void);

#endif
