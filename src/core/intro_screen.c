#include "core/intro_screen.h"
#include "core/cursor.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------
#define INTRO_FRAME_DIR    "assets/sprites/intro_frames"
#define INTRO_FPS          30
#define INTRO_FRAME_TIME   (1.0f / INTRO_FPS)
#define INTRO_FADE_DURATION 0.7f   // seconds for the black fade-out at the end

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

static int CountIntroFrames(void)
{
    int count = 0;
    while (1)
    {
        char path[256];
        snprintf(path, sizeof(path), "%s/frame_%04d.png", INTRO_FRAME_DIR, count + 1);
        if (!FileExists(path)) break;
        count++;
    }
    return count;
}

static void BuildFramePath(char *buf, int bufSize, int frameIndex)
{
    // frameIndex is 0-based; files are 1-based (frame_0001.png …)
    snprintf(buf, bufSize, "%s/frame_%04d.png", INTRO_FRAME_DIR, frameIndex + 1);
}

// ---------------------------------------------------------------------------
// Streaming intro player
//
// Strategy: keep only TWO textures alive at any moment —
//   slot[0] = the frame currently being displayed
//   slot[1] = the next frame (pre-loaded one frame ahead)
//
// After drawing slot[0] we swap slots and load the next frame into the
// now-free slot.  This keeps VRAM usage at roughly 2 × frame size (~16 MB
// for 1920×1080 RGBA), regardless of total frame count.
// ---------------------------------------------------------------------------

GameScreen RunIntro(void)
{
    int frameCount = CountIntroFrames();
    if (frameCount == 0)
    {
        // No frames found — skip intro entirely.
        return SCREEN_START;
    }

    // Two texture slots for double-buffered streaming.
    Texture2D slot[2]   = { {0}, {0} };
    int       slotFrame[2] = { -1, -1 };  // which source frame is in each slot

    // Load frame 0 into slot[0].
    {
        char path[256];
        BuildFramePath(path, sizeof(path), 0);
        slot[0]      = LoadTexture(path);
        slotFrame[0] = 0;
    }

    // Pre-load frame 1 into slot[1] (if it exists).
    if (frameCount > 1)
    {
        char path[256];
        BuildFramePath(path, sizeof(path), 1);
        slot[1]      = LoadTexture(path);
        slotFrame[1] = 1;
    }

    int   displaySlot = 0;     // which slot is currently on screen
    int   curFrame    = 0;     // 0-based logical frame counter
    int   nextToLoad  = 2;     // next source frame index to stream in
    float frameDelta  = 0.0f;
    float fadeAlpha   = 0.0f;
    bool  fading      = false;

    GameScreen result = SCREEN_HISTORY_ANIMATION;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        UpdateCustomCursor(dt);

        // --- Skip input ------------------------------------------------------
        if (!fading && (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)))
        {
            fading = true;
        }

        // --- Frame advance ---------------------------------------------------
        bool frameAdvanced = false;
        if (!fading)
        {
            frameDelta += dt;
            if (frameDelta >= INTRO_FRAME_TIME)
            {
                frameDelta -= INTRO_FRAME_TIME;
                curFrame++;
                if (curFrame >= frameCount)
                {
                    curFrame = frameCount - 1;
                    fading   = true;
                }
                else
                {
                    frameAdvanced = true;
                }
            }
        }

        // --- Stream next frame into the freed slot ---------------------------
        if (frameAdvanced && nextToLoad < frameCount)
        {
            // The slot that was just displaying curFrame-1 is now free.
            // (displaySlot still points to it — we'll swap after loading.)
            int freeSlot = displaySlot;               // about to be overwritten
            displaySlot  = 1 - displaySlot;           // switch display to pre-loaded slot

            // Unload the old texture from the free slot and load the next frame.
            if (slot[freeSlot].id != 0)
                UnloadTexture(slot[freeSlot]);

            char path[256];
            BuildFramePath(path, sizeof(path), nextToLoad);
            slot[freeSlot]      = LoadTexture(path);
            slotFrame[freeSlot] = nextToLoad;
            nextToLoad++;
        }
        else if (frameAdvanced)
        {
            // No more frames to stream; just toggle to the pre-loaded slot.
            displaySlot = 1 - displaySlot;
        }

        // --- Fade-out --------------------------------------------------------
        if (fading)
        {
            fadeAlpha += dt / INTRO_FADE_DURATION;
            if (fadeAlpha >= 1.0f)
            {
                fadeAlpha = 1.0f;
                break;
            }
        }

        // --- Draw ------------------------------------------------------------
        int w = GetScreenWidth();
        int h = GetScreenHeight();

        Texture2D tex = slot[displaySlot];

        BeginDrawing();
            ClearBackground(BLACK);

            if (tex.id != 0)
            {
                float scaleX = (float)w / (float)tex.width;
                float scaleY = (float)h / (float)tex.height;
                float scale  = (scaleX < scaleY) ? scaleX : scaleY;

                float drawW = tex.width  * scale;
                float drawH = tex.height * scale;
                float drawX = (w - drawW) * 0.5f;
                float drawY = (h - drawH) * 0.5f;

                Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
                Rectangle dst = { drawX, drawY, drawW, drawH };
                DrawTexturePro(tex, src, dst, (Vector2){0, 0}, 0.0f, WHITE);
            }

            // Black fade overlay.
            if (fadeAlpha > 0.0f)
            {
                DrawRectangle(0, 0, w, h, Fade(BLACK, fadeAlpha));
            }

            // Skip hint (shown before fade starts).
            if (!fading)
            {
                const char *hint   = "SPACE - pular";
                int         hintSz = h / 40;
                int         hintW  = MeasureText(hint, hintSz);
                DrawText(hint, w - hintW - 12, h - hintSz - 10, hintSz, Fade(LIGHTGRAY, 0.55f));
            }
        EndDrawing();
    }

    // --- Cleanup: unload both slots -----------------------------------------
    for (int i = 0; i < 2; i++)
    {
        if (slot[i].id != 0)
            UnloadTexture(slot[i]);
    }

    SetHistoryAnimationReturnScreen(SCREEN_START);
    return result;
}
