#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

#include <stdint.h>

void setColor(uint32_t text, uint32_t background);
void drawCharOnCursor(int8_t character);
void deleteCharOnCursor();
void drawStringOnCursor(int8_t * string, uint32_t * length);
void drawSquare(uint32_t hexColor,uint64_t x, uint64_t y, uint32_t scale);
void colorScreen(uint32_t hexColor);
void drawCircle(uint32_t hexColor, uint64_t x, uint64_t y, uint32_t length, uint32_t backgroundColor);
uint32_t getFontColor();
void setFontColor(uint32_t color);

#endif
