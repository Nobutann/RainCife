#ifndef CURSOR_H
#define CURSOR_H

#include <raylib.h>

void InitCustomCursor(void);
void UnloadCustomCursor(void);
void UpdateCustomCursor(float dt);
void DrawGameplayCursor(bool attacking);
void DrawMenuCursor(bool hoveringButton);

#endif
