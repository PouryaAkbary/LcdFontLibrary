#ifndef EMOJI_H
#define EMOJI_H

#include "st7735.h"
#include <stdint.h>

// تعریف سایز ایموجی
#define EMOJI_WIDTH  100
#define EMOJI_HEIGHT 100

extern const uint16_t smile_emoji_64[100*100];
extern const uint16_t heart_emoji[128 * 128];

// تابع نمایش ایموجی
void ST7735_drawemoji(uint16_t i ,uint16_t j, const uint16_t *emoji);

#endif

