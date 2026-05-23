#ifndef UTILS_H
#define UTILS_H

#include <raylib.h>

int GetClickedOption(Rectangle* rects, int count);
void BuildOptionRects(Rectangle* rects, const char** options, int count, int fontSize, int startX, int startY, int spacing);
Rectangle ScaleUiRect(float x, float y, float width, float height, int screenWidth, int screenHeight);
void DrawFullscreenTexture(Texture2D texture, int screenWidth, int screenHeight);
void DrawFullscreenTextureOffset(Texture2D texture, float baseOffsetX, float baseOffsetY, int screenWidth, int screenHeight);
void DrawFullscreenTextureClipped(Texture2D texture, Rectangle baseClip, int screenWidth, int screenHeight);
void DrawSlider(Rectangle bounds, float value);
float UpdateSlider(Rectangle bounds, float value, bool *isDragging);
#endif
