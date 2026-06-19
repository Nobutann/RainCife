#include "core/screens.h"
#include "core/cursor.h"
#include "utils.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------
#define HISTORY_FRAME_DIR      "assets/sprites/history_frames"
#define HISTORY_DURATION       24.0f  // total natural duration of the video in seconds
#define HISTORY_FADE_DURATION  0.7f   // seconds for the black fade-out

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

static int CountHistoryFrames(void)
{
    int count = 0;
    while (1)
    {
        char path[256];
        snprintf(path, sizeof(path), "%s/frame_%04d.png", HISTORY_FRAME_DIR, count + 1);
        if (!FileExists(path)) break;
        count++;
    }
    return count;
}

static void BuildHistoryFramePath(char *buf, int bufSize, int fileIndex)
{
    snprintf(buf, bufSize, "%s/frame_%04d.png", HISTORY_FRAME_DIR, fileIndex);
}

// ---------------------------------------------------------------------------
// Streaming history animation player
// ---------------------------------------------------------------------------

// Controls which screen is returned after the animation finishes or is skipped.
// Default: SCREEN_START (safe fallback). Set by callers before returning
// SCREEN_HISTORY_ANIMATION.
static GameScreen historyReturnScreen = SCREEN_START;

void SetHistoryAnimationReturnScreen(GameScreen returnScreen)
{
    historyReturnScreen = returnScreen;
}

GameScreen RunHistoryAnimation(void)
{
    GameScreen result = historyReturnScreen;
    historyReturnScreen = SCREEN_START;

    int totalFiles = CountHistoryFrames();
    if (totalFiles == 0)
    {
        // No frames found — skip history animation entirely.
        return result;
    }

    // Allocate array to hold the mapped file indices of the frames we want to play
    int *fileFrames = malloc(sizeof(int) * totalFiles);
    if (fileFrames == NULL)
    {
        return result;
    }

    int frameCount = 0;
    for (int f = 1; f <= totalFiles; )
    {
        fileFrames[frameCount++] = f;
        int step = 2; // Default step to skip every other frame (50% reduction)
        f += step;
    }

    // Calculate individual frame weights.
    // 2. "when it reaches frame 551 make the frames run just slower than how the rest before was running"
    float totalWeight = 0.0f;
    for (int i = 0; i < frameCount; i++)
    {
        if (fileFrames[i] >= 551)
        {
            totalWeight += 1.25f; // Slower, but faster than 1.6f (changed from 1.6f to 1.25f)
        }
        else
        {
            totalWeight += 1.0f; // Standard duration
        }
    }

    float timePerWeight = HISTORY_DURATION / totalWeight;

    // Two texture slots for double-buffered streaming.
    Texture2D slot[2]   = { {0}, {0} };
    int       slotFrame[2] = { -1, -1 };  // which logical frame is in each slot

    // Load frame 0 into slot[0].
    {
        char path[256];
        BuildHistoryFramePath(path, sizeof(path), fileFrames[0]);
        slot[0]      = LoadTexture(path);
        slotFrame[0] = 0;
    }

    // Pre-load frame 1 into slot[1] (if it exists).
    if (frameCount > 1)
    {
        char path[256];
        BuildHistoryFramePath(path, sizeof(path), fileFrames[1]);
        slot[1]      = LoadTexture(path);
        slotFrame[1] = 1;
    }

    int   displaySlot = 0;     // which slot is currently on screen
    int   curFrame    = 0;     // 0-based logical frame counter
    int   nextToLoad  = 2;     // next logical frame index to stream in
    float frameDelta  = 0.0f;
    float fadeAlpha   = 0.0f;
    bool  fading      = false;

    // acceptSkip: wait for the triggering key to be fully released before
    // accepting SPACE/ENTER as a skip command, so the same keypress that
    // launched the screen doesn't immediately skip the animation.
    bool acceptSkip = false;

    while (!WindowShouldClose())
    {
        AtualizarPreferenciaEspaco();
        float dt = GetFrameTime();
        UpdateCustomCursor(dt);
        bool finishedNaturally = false;

        // Gate: don't accept skip until both keys are released.
        if (!acceptSkip &&
            !IsKeyDown(KEY_SPACE) &&
            !IsKeyDown(KEY_ENTER) &&
            !IsKeyPressed(KEY_SPACE) &&
            !IsKeyPressed(KEY_ENTER))
        {
            acceptSkip = true;
        }

        // --- Skip input (Space triggers a slow blackout) ---------------------
        if (acceptSkip && !fading && (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)))
        {
            fading = true;
        }

        // --- Frame advance ---------------------------------------------------
        bool frameAdvanced = false;
        int framesToAdvance = 0;
        if (!fading)
        {
            frameDelta += dt;
            while (1)
            {
                float currentFrameDuration = timePerWeight * (fileFrames[curFrame] >= 551 ? 1.25f : 1.0f);
                if (frameDelta >= currentFrameDuration)
                {
                    frameDelta -= currentFrameDuration;
                    framesToAdvance++;
                    curFrame++;
                    if (curFrame >= frameCount)
                    {
                        curFrame = frameCount - 1;
                        finishedNaturally = true;
                        break;
                    }
                }
                else
                {
                    break;
                }
            }

            if (framesToAdvance > 0)
            {
                frameAdvanced = true;
            }
        }

        if (finishedNaturally)
        {
            break;
        }

        // --- Stream next frame or reload if skipped ---------------------------
        if (frameAdvanced)
        {
            int expectedDisplayFrame = curFrame;
            int expectedPreloadFrame = curFrame + 1;

            if (framesToAdvance == 1)
            {
                // Normal sequential advance
                int freeSlot = displaySlot;
                displaySlot  = 1 - displaySlot;           // switch display to pre-loaded slot

                // Unload the old texture from the free slot and load the next frame.
                if (slot[freeSlot].id != 0)
                    UnloadTexture(slot[freeSlot]);

                // Verify that slot[displaySlot] has the expected frame. If not, reload it.
                if (slotFrame[displaySlot] != expectedDisplayFrame)
                {
                    if (slot[displaySlot].id != 0) UnloadTexture(slot[displaySlot]);
                    char path[256];
                    BuildHistoryFramePath(path, sizeof(path), fileFrames[expectedDisplayFrame]);
                    slot[displaySlot] = LoadTexture(path);
                    slotFrame[displaySlot] = expectedDisplayFrame;
                }

                // Preload the next frame into the free slot
                if (expectedPreloadFrame < frameCount)
                {
                    char path[256];
                    BuildHistoryFramePath(path, sizeof(path), fileFrames[expectedPreloadFrame]);
                    slot[freeSlot]      = LoadTexture(path);
                    slotFrame[freeSlot] = expectedPreloadFrame;
                    nextToLoad          = expectedPreloadFrame + 1;
                }
                else
                {
                    slotFrame[freeSlot] = -1;
                }
            }
            else
            {
                // We skipped frames! Reload display slot and preload slot from scratch for the new position.
                if (slot[displaySlot].id != 0) UnloadTexture(slot[displaySlot]);
                char path[256];
                BuildHistoryFramePath(path, sizeof(path), fileFrames[expectedDisplayFrame]);
                slot[displaySlot] = LoadTexture(path);
                slotFrame[displaySlot] = expectedDisplayFrame;

                int otherSlot = 1 - displaySlot;
                if (slot[otherSlot].id != 0) UnloadTexture(slot[otherSlot]);
                if (expectedPreloadFrame < frameCount)
                {
                    BuildHistoryFramePath(path, sizeof(path), fileFrames[expectedPreloadFrame]);
                    slot[otherSlot] = LoadTexture(path);
                    slotFrame[otherSlot] = expectedPreloadFrame;
                    nextToLoad = expectedPreloadFrame + 1;
                }
                else
                {
                    slotFrame[otherSlot] = -1;
                }
            }
        }

        // --- Fade-out --------------------------------------------------------
        if (fading)
        {
            fadeAlpha += dt / HISTORY_FADE_DURATION;
            if (fadeAlpha >= 1.0f)
            {
                fadeAlpha = 1.0f;
                break;
            }
        }

        // --- Draw ------------------------------------------------------------
        int w = GetRenderWidth();
        int h = GetRenderHeight();

        Texture2D tex = slot[displaySlot];

        BeginDrawing();
            ClearBackground(BLACK);

            if (tex.id != 0)
            {
                DrawFullscreenTexture(tex, w, h);
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

    free(fileFrames);

    return result;
}
