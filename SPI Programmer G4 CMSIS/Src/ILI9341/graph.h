#ifndef TEST1_ILI9341_H
#define TEST1_ILI9341_H

#include "core.h"
#include "dma.h"

#ifndef _int16_swap
#define _int16_swap(a, b) { int16_t t = a; a = b; b = t; }
#endif

#ifndef abs
#define abs(a) ((a)<0?-(a):a)
#endif

#define ILI9341_COLOR(r, g, b)\
     ((((uint16_t)b) >> 3) |\
            ((((uint16_t)g) << 3) & 0x07E0) |\
            ((((uint16_t)r) << 8) & 0xf800))

void LCD_readPixels(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *buf);

void LCD_fillRect(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t color);
void LCD_fillScreen(uint16_t color);

void LCD_drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

void LCD_putPixel(uint16_t x, uint16_t y, uint16_t color);
void LCD_drawFastHLine(uint16_t x0, uint16_t y0, uint16_t w, uint16_t color);
void LCD_drawFastVLine(uint16_t x0, uint16_t y0, uint16_t h, uint16_t color);
void LCD_drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

void LCD_drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
void LCD_fillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);

void LCD_setVerticalScrolling(uint16_t startY, uint16_t endY);
void LCD_scroll(uint16_t v);
void LCD_DrawImage(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t *picture);

const uint16_t praxis_logo[];
const uint16_t usb[];

#endif //TEST1_ILI9341_H
