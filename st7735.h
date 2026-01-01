#ifndef ST7735_H
#define ST7735_H

#include "main.h"
#include "fonts.h"
#include "stdlib.h"
#include "math.h"

// دستورات ST7735
#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_SLPOUT  0x11
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_MADCTL  0x36
#define ST7735_COLMOD  0x3A
#define st7735_col     0x05

// رنگ‌ها (RGB565)
#define ST7735_BLACK   0x0000
#define ST7735_BLUE    0x001F
#define ST7735_RED     0xF800
#define ST7735_GREEN   0x07E0
#define ST7735_CYAN    0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0xFFE0
#define ST7735_WHITE   0xFFFF

// اندازه صفحه
#define ST7735_WIDTH   128
#define ST7735_HEIGHT  128



#define CENTER_X 64
#define CENTER_Y 64
#define RADIUS   55
#define M_PI     3.14159265358979323846

void ST7735_Init(void);
void ST7735_FillScreen(uint16_t color);
void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ST7735_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor);
void ST7735_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor);
void ST7735_DrawCircle(uint16_t x0 , uint16_t y0 , uint16_t radius , uint16_t color);
void ST7735_DrawLine(int x0, int y0, int x1, int y1, uint16_t color);
void DrawHands(void);
void DrawClockFace(void);
void ST7735_SetRotation(uint8_t m);
static void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

#endif

