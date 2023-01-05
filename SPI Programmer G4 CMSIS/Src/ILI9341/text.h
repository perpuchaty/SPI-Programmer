#ifndef TEST1_TEXT_H
#define TEST1_TEXT_H

#include "graph.h"
#include "fonts/glcdfont.h"

#define TRANSPARENT_COLOR CYAN

#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))

uint8_t LCD_drawChar(uint16_t x0, uint16_t y0, unsigned char c, uint16_t color, uint16_t bg, uint8_t * font);
void LCD_drawChar_tst(uint16_t x0, uint16_t y0, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
uint8_t calc_string_pixels(unsigned char c, uint8_t * font);

void LCD_write(unsigned char c, uint8_t * font);
void LCD_writeStringCenter(unsigned char s[], uint8_t * font,uint8_t x,uint16_t lenght);
void LCD_writeString(char *s, uint8_t * font);
void LCD_writeString2(unsigned char *string);
void LCD_setSpace(uint8_t space);

void LCD_setCursor(uint16_t x, uint16_t y);
void LCD_setTextSize(uint8_t size);
void LCD_setTextColor(uint16_t color);
void LCD_setTextBgColor(uint16_t color);

uint16_t LCD_getCursorX();
uint16_t LCD_getCursorY();

void drawChar2(int16_t x, int16_t y, unsigned char c,
			    uint16_t fgcolor, uint16_t bgcolor, uint8_t size);

uint16_t textColor;

#endif //TEST1_TEXT_H
