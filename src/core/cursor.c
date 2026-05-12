#include "core/cursor.h"

#define CROSSHAIR_FRAME_TIME 0.08f
#define GAMEPLAY_CURSOR_SCALE 2.0f
#define MENU_CURSOR_SCALE 2.0f

typedef struct
{
    Texture2D crosshairIdle;
    Texture2D crosshairAttack;
    Texture2D menuIdle;
    Texture2D menuSelect;
    int crosshairFrame;
    int crosshairFrameCount;
    float crosshairTimer;
} CustomCursor;

static CustomCursor cursor = {0};

void InitCustomCursor(void)
{
    cursor.crosshairIdle = LoadTexture("assets/sprites/Mouse/Crosshair_idle-Sheet.png");
    cursor.crosshairAttack = LoadTexture("assets/sprites/Mouse/Crosshair_attack.png");
    cursor.menuIdle = LoadTexture("assets/sprites/Mouse/Mouse_idle.png");
    cursor.menuSelect = LoadTexture("assets/sprites/Mouse/Mouse_select.png");
    cursor.crosshairFrame = 0;
    cursor.crosshairTimer = 0.0f;
    cursor.crosshairFrameCount = cursor.crosshairIdle.height > 0 ? cursor.crosshairIdle.width / cursor.crosshairIdle.height : 1;
}

void UnloadCustomCursor(void)
{
    UnloadTexture(cursor.crosshairIdle);
    UnloadTexture(cursor.crosshairAttack);
    UnloadTexture(cursor.menuIdle);
    UnloadTexture(cursor.menuSelect);
}

void UpdateCustomCursor(float dt)
{
    if (cursor.crosshairFrameCount <= 1)
    {
        return;
    }

    cursor.crosshairTimer += dt;
    if (cursor.crosshairTimer >= CROSSHAIR_FRAME_TIME)
    {
        cursor.crosshairTimer = 0.0f;
        cursor.crosshairFrame++;
        if (cursor.crosshairFrame >= cursor.crosshairFrameCount)
        {
            cursor.crosshairFrame = 0;
        }
    }
}

void DrawGameplayCursor(bool attacking)
{
    Vector2 mouse = GetMousePosition();

    if (attacking)
    {
        Rectangle source =
        {
            0.0f,
            0.0f,
            (float)cursor.crosshairAttack.width,
            (float)cursor.crosshairAttack.height
        };
        Rectangle dest =
        {
            mouse.x - cursor.crosshairAttack.width * GAMEPLAY_CURSOR_SCALE * 0.5f,
            mouse.y - cursor.crosshairAttack.height * GAMEPLAY_CURSOR_SCALE * 0.5f,
            cursor.crosshairAttack.width * GAMEPLAY_CURSOR_SCALE,
            cursor.crosshairAttack.height * GAMEPLAY_CURSOR_SCALE
        };
        DrawTexturePro(cursor.crosshairAttack, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
        return;
    }

    int frameWidth = cursor.crosshairFrameCount > 0 ? cursor.crosshairIdle.width / cursor.crosshairFrameCount : cursor.crosshairIdle.width;
    Rectangle source =
    {
        (float)(cursor.crosshairFrame * frameWidth),
        0.0f,
        (float)frameWidth,
        (float)cursor.crosshairIdle.height
    };
    Rectangle dest =
    {
        mouse.x - frameWidth * GAMEPLAY_CURSOR_SCALE * 0.5f,
        mouse.y - cursor.crosshairIdle.height * GAMEPLAY_CURSOR_SCALE * 0.5f,
        frameWidth * GAMEPLAY_CURSOR_SCALE,
        cursor.crosshairIdle.height * GAMEPLAY_CURSOR_SCALE
    };
    DrawTexturePro(cursor.crosshairIdle, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
}

void DrawMenuCursor(bool hoveringButton)
{
    Texture2D texture = hoveringButton ? cursor.menuSelect : cursor.menuIdle;
    Vector2 mouse = GetMousePosition();
    Rectangle source =
    {
        0.0f,
        0.0f,
        (float)texture.width,
        (float)texture.height
    };
    Rectangle dest =
    {
        mouse.x,
        mouse.y,
        texture.width * MENU_CURSOR_SCALE,
        texture.height * MENU_CURSOR_SCALE
    };
    DrawTexturePro(texture, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
}
