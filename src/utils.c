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

Rectangle ScaleUiRect(float x, float y, float width, float height, int screenWidth, int screenHeight)
{
    const float baseWidth = 640.0f;
    const float baseHeight = 360.0f;

    return (Rectangle)
    {
        x * screenWidth / baseWidth,
        y * screenHeight / baseHeight,
        width * screenWidth / baseWidth,
        height * screenHeight / baseHeight
    };
}

void DrawFullscreenTexture(Texture2D texture, int screenWidth, int screenHeight)
{
    if (texture.id <= 0)
    {
        return;
    }

    DrawTexturePro(
        texture,
        (Rectangle){0.0f, 0.0f, (float)texture.width, (float)texture.height},
        (Rectangle){0.0f, 0.0f, (float)screenWidth, (float)screenHeight},
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE
    );
}

void DrawFullscreenTextureOffset(Texture2D texture, float baseOffsetX, float baseOffsetY, int screenWidth, int screenHeight)
{
    if (texture.id <= 0)
    {
        return;
    }

    DrawTexturePro(
        texture,
        (Rectangle){0.0f, 0.0f, (float)texture.width, (float)texture.height},
        (Rectangle)
        {
            baseOffsetX * screenWidth / 640.0f,
            baseOffsetY * screenHeight / 360.0f,
            (float)screenWidth,
            (float)screenHeight
        },
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE
    );
}

void DrawFullscreenTextureClipped(Texture2D texture, Rectangle baseClip, int screenWidth, int screenHeight)
{
    if (texture.id <= 0 || baseClip.width <= 0.0f || baseClip.height <= 0.0f)
    {
        return;
    }

    DrawTexturePro(
        texture,
        baseClip,
        (Rectangle)
        {
            baseClip.x * screenWidth / 640.0f,
            baseClip.y * screenHeight / 360.0f,
            baseClip.width * screenWidth / 640.0f,
            baseClip.height * screenHeight / 360.0f
        },
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE
    );
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
