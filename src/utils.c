#include "utils.h"
#include <raylib.h>

int GetClickedOption(Rectangle* rects, int count)
{
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        return -1;
    }

    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < count; i++)
    {
        if (CheckCollisionPointRec(mouse, rects[i]))
        {
            return i;
        }
    }

    return -1;
}

void BuildOptionRects(Rectangle* rects, const char** options, int count, int fontSize, int startX, int startY, int spacing)
{
    for (int i = 0; i < count; i++)
    { 
        int textWidth = MeasureText(options[i], fontSize);
        rects[i] = (Rectangle)
        {
            startX - textWidth / 2,
            startY + i * spacing,
            textWidth,
            fontSize
        };
    }
}

void DrawSlider(Rectangle bounds, float value)
{
    Rectangle fill = { bounds.x, bounds.y, bounds.width * value, bounds.height };
    DrawRectangleRec(bounds, LIGHTGRAY);
    DrawRectangleRec(fill, BLUE);
}

float UpdateSlider(Rectangle bounds, float value, bool *isDragging)
{
    Vector2 mouse = GetMousePosition();
    Rectangle hitbox =
    {
        bounds.x,
        bounds.y - 10.0f,
        bounds.width,
        bounds.height + 20.0f
    };

    if (CheckCollisionPointRec(mouse, hitbox) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        *isDragging = true;
        value = (mouse.x - bounds.x) / bounds.width;
        if (value < 0.0f) value = 0.0f;
        if (value > 1.0f) value = 1.0f;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        *isDragging = false;
    }

    if (*isDragging && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        value = (mouse.x - bounds.x) / bounds.width;
        if (value < 0.0f) value = 0.0f;
        if (value > 1.0f) value = 1.0f;
    }
    return value;
}
