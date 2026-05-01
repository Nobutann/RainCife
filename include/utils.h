#ifndef UTILS_H
#define UTILS_H

#include <raylib.h>

int GetClickedOption(Rectangle* rects, int count);
void BuildOptionRects(Rectangle* rects, const char** options, int count, int fontSize, int startX, int startY, int spacing);
void DrawSlider(Rectangle bounds, float value);
float UpdateSlider(Rectangle bounds, float value);
#endif