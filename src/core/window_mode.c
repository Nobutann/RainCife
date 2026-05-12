#include "core/window_mode.h"

#include <raylib.h>

#define WINDOWED_WIDTH 1280
#define WINDOWED_HEIGHT 720
#define WINDOWED_MARGIN 80

static void GetCenteredWindowRect(int monitor, int *width, int *height, int *x, int *y)
{
    int monitorWidth = GetMonitorWidth(monitor);
    int monitorHeight = GetMonitorHeight(monitor);
    Vector2 monitorPos = GetMonitorPosition(monitor);

    int maxWidth = monitorWidth - WINDOWED_MARGIN;
    int maxHeight = monitorHeight - WINDOWED_MARGIN;

    *width = WINDOWED_WIDTH;
    *height = WINDOWED_HEIGHT;

    if (*width > maxWidth)
    {
        *width = maxWidth;
        *height = (*width * WINDOWED_HEIGHT) / WINDOWED_WIDTH;
    }

    if (*height > maxHeight)
    {
        *height = maxHeight;
        *width = (*height * WINDOWED_WIDTH) / WINDOWED_HEIGHT;
    }

    if (*width < 640) *width = 640;
    if (*height < 360) *height = 360;

    *x = (int)monitorPos.x + (monitorWidth - *width) / 2;
    *y = (int)monitorPos.y + (monitorHeight - *height) / 2;
}

void ApplyWindowMode(bool fullscreen)
{
    int monitor = GetCurrentMonitor();

    if (fullscreen)
    {
        if (!IsWindowFullscreen())
        {
            SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
            ToggleFullscreen();
        }
        return;
    }

    int width;
    int height;
    int x;
    int y;
    GetCenteredWindowRect(monitor, &width, &height, &x, &y);

    if (IsWindowFullscreen())
    {
        ToggleFullscreen();
    }

    SetWindowSize(width, height);
    SetWindowPosition(x, y);
}

void ToggleWindowMode(bool *fullscreen)
{
    *fullscreen = !*fullscreen;
    ApplyWindowMode(*fullscreen);
}
