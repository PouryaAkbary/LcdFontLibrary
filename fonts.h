#ifndef FONTS_H
#define FONTS_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

/* Font structure used by st7735 driver (width x height, data points to
   an array organized as: for each character (ASCII 32..126) there are
   'height' 16-bit words representing each row. */
typedef struct {
    uint8_t width;
    uint8_t height;
    const uint16_t *data;
} FontDef;

/* Exposed font */
extern const FontDef Font_7x8;

#endif // FONTS_H

