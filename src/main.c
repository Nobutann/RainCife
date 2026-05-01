#include <raylib.h>
#include "screens.h"
#include "config_manager.h"

int main(void)
{
    InitWindow(800, 600, "RatTsunami");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);
    SetExitKey(KEY_DELETE);

    Config config = CarregarConfig();

    GameScreen currentScreen = SCREEN_START;

    while (currentScreen != SCREEN_EXIT && !WindowShouldClose())
    {
        if (IsKeyPressed(KEY_F11))
        {
            ToggleFullscreen();
        }

        if (currentScreen == SCREEN_START)
        {
            currentScreen = RunStart();
        }

        if (currentScreen == SCREEN_OPTIONS)
        {
            currentScreen = RunOptions(&config);
        }

        if (currentScreen == SCREEN_GAME)
        {
            currentScreen = RunGame(&config);
        }

        if (currentScreen == SCREEN_CREDITS)
        {
            currentScreen = RunCredits();
        }
    }

    CloseWindow();

    return 0;
}
