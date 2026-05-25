#include "core/window_mode.h"

#include <raylib.h>

#define WINDOW_ASPECT_WIDTH 16
#define WINDOW_ASPECT_HEIGHT 9
#define WINDOWED_MARGIN 80
#define MIN_WINDOWED_WIDTH 640
#define MIN_WINDOWED_HEIGHT 360

static void GetCenteredWindowRect(int monitor, int *width, int *height, int *x, int *y)
{
    int monitorWidth = GetMonitorWidth(monitor);
    int monitorHeight = GetMonitorHeight(monitor);
    Vector2 monitorPos = GetMonitorPosition(monitor);

    int maxWidth = monitorWidth - WINDOWED_MARGIN;
    int maxHeight = monitorHeight - WINDOWED_MARGIN;

    if (maxWidth < MIN_WINDOWED_WIDTH) maxWidth = monitorWidth;
    if (maxHeight < MIN_WINDOWED_HEIGHT) maxHeight = monitorHeight;

    *width = maxWidth;
    *height = (*width * WINDOW_ASPECT_HEIGHT) / WINDOW_ASPECT_WIDTH;

    if (*height > maxHeight)
    {
        *height = maxHeight;
        *width = (*height * WINDOW_ASPECT_WIDTH) / WINDOW_ASPECT_HEIGHT;
    }

    if (*width < MIN_WINDOWED_WIDTH) *width = MIN_WINDOWED_WIDTH;
    if (*height < MIN_WINDOWED_HEIGHT) *height = MIN_WINDOWED_HEIGHT;

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
