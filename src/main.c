#include <raylib.h>
#include "screens.h"
#include "config_manager.h"

int main(void)
{
    Config config = CarregarConfig();
    
    config.telaCheia = 1;
    
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "RatTsunami");
    
    if (config.telaCheia)
    {
        int monitor = GetCurrentMonitor();
        SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
        ToggleFullscreen();
    }

    SetTargetFPS(60);
    SetExitKey(KEY_DELETE);

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
