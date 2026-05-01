#ifndef UTILS_H
#define UTILS_H

int GetClickedOption(Rectangle* rects, int count);
void BuildOptionRects(Rectangle* rects, const char** options, int count, int fontSize, int startX, int startY, int spacing);

#endif