#include "st7735.h"

extern SPI_HandleTypeDef hspi1;
extern uint8_t hour , minute , second ,f;

// پین‌ها (مطابق با main.c)
#define CS_LOW()   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)
#define CS_HIGH()  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)
#define DC_LOW()   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET)
#define DC_HIGH()  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET)
#define RESET_LOW()   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET)
#define RESET_HIGH()  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET)

static void ST7735_SendCommand(uint8_t cmd)
{
    DC_LOW();
    CS_LOW();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    CS_HIGH();
}

static void ST7735_SendData(uint8_t *data, uint16_t size)
{
    DC_HIGH();
    CS_LOW();
    HAL_SPI_Transmit(&hspi1, data, size, HAL_MAX_DELAY);
    CS_HIGH();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

static void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    uint8_t data[4];

    ST7735_SendCommand(ST7735_CASET); // Column addr set
    data[0] = 0x00;
    data[1] = x0 + 2;   // ← این دو offset برای مدل 1.44 مهم هستند
    data[2] = 0x00;
    data[3] = x1 + 2;
    ST7735_SendData(data, 4);

    ST7735_SendCommand(ST7735_RASET); // Row addr set
    data[0] = 0x00;
    data[1] = y0 + 1;   // ← offset ردیف
    data[2] = 0x00;
    data[3] = y1 + 1;
    ST7735_SendData(data, 4);

    ST7735_SendCommand(ST7735_RAMWR); // Write to RAM
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void ST7735_Init(void)
{
    RESET_LOW();
    HAL_Delay(50);
    RESET_HIGH();
    HAL_Delay(50);

    ST7735_SendCommand(ST7735_SWRESET);
    HAL_Delay(150);
    ST7735_SendCommand(ST7735_SLPOUT);
    HAL_Delay(500);

    uint8_t data = 0x05;
    ST7735_SendCommand(ST7735_COLMOD);
    ST7735_SendData(&data, 1);
		
		// this  function for set color mode from BGR to RGB
		uint8_t madctl_data = 0xC0; 
		ST7735_SendCommand(ST7735_MADCTL);
		ST7735_SendData(&madctl_data, 1);
		//---------------------------------------------------
	

    ST7735_SendCommand(ST7735_DISPON);
    HAL_Delay(100);
}

void ST7735_FillScreen(uint16_t color)
{
    uint8_t data[4];

    ST7735_SendCommand(ST7735_CASET);
    data[0] = 0; data[1] = 0;
    data[2] = 0; data[3] = ST7735_WIDTH - 1;
    ST7735_SendData(data, 4);

    ST7735_SendCommand(ST7735_RASET);
    data[0] = 0; data[1] = 0;
    data[2] = 0; data[3] = ST7735_HEIGHT - 1;
    ST7735_SendData(data, 4);

    ST7735_SendCommand(ST7735_RAMWR);

    uint8_t colorData[2] = { color >> 8, color & 0xFF };
    DC_HIGH();
    CS_LOW();
    for (uint16_t i = 0; i < ST7735_WIDTH * ST7735_HEIGHT; i++) {
        HAL_SPI_Transmit(&hspi1, colorData, 2, HAL_MAX_DELAY);
    }
    CS_HIGH();
}

void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT) return;

		ST7735_SetAddressWindow(x ,y ,x ,y );
	  uint8_t data[2] = {color >> 8, color & 0xFF};
		ST7735_SendData(data ,2);
	/*
    uint8_t data[4];
    ST7735_SendCommand(ST7735_CASET);
    data[0] = 0; data[1] = x;
    data[2] = 0; data[3] = x;
    ST7735_SendData(data, 4);

    ST7735_SendCommand(ST7735_RASET);
    data[0] = 0; data[1] = y;
    data[2] = 0; data[3] = y;
    ST7735_SendData(data, 4);

    ST7735_SendCommand(ST7735_RAMWR);
    data[0] = color >> 8;
    data[1] = color & 0xFF;
    ST7735_SendData(data, 2);
	*/
}

void ST7735_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor)
{
    uint32_t i, b, j;
    for (i = 0; i < font.height; i++) {
        b = font.data[(ch - 32) * font.height + i];
        for (j = 0; j < font.width; j++) {
            if ((b << j) & 0x8000) {
                ST7735_DrawPixel(x + j, y + i, color);
            } else {
                ST7735_DrawPixel(x + j, y + i, bgcolor);
            }
        }
    }
}

void ST7735_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor)
{
    while (*str) {
        if (x + font.width >= ST7735_WIDTH) {
            x = 0;
            y += font.height;
            if (y + font.height >= ST7735_HEIGHT) break;
        }
        ST7735_WriteChar(x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }
}

void ST7735_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    ST7735_DrawPixel(x0, y0 + r, color);
    ST7735_DrawPixel(x0, y0 - r, color);
    ST7735_DrawPixel(x0 + r, y0, color);
    ST7735_DrawPixel(x0 - r, y0, color);

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        ST7735_DrawPixel(x0 + x, y0 + y, color);
        ST7735_DrawPixel(x0 - x, y0 + y, color);
        ST7735_DrawPixel(x0 + x, y0 - y, color);
        ST7735_DrawPixel(x0 - x, y0 - y, color);
        ST7735_DrawPixel(x0 + y, y0 + x, color);
        ST7735_DrawPixel(x0 - y, y0 + x, color);
        ST7735_DrawPixel(x0 + y, y0 - x, color);
        ST7735_DrawPixel(x0 - y, y0 - x, color);
    }
}



void ST7735_DrawLine(int x0, int y0, int x1, int y1, uint16_t color)
{
    int dx = abs(x1 - x0);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (1)
    {
        ST7735_DrawPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

	
void DrawClockFace(void)
{
    ST7735_DrawCircle(CENTER_X, CENTER_Y, RADIUS, ST7735_WHITE);

    for (int i = 0; i < 12; i++)
    {
        float angle = (i * 30.0f - 90.0f) * (M_PI / 180.0f);

        int x1 = CENTER_X + (int)(cos(angle) * (RADIUS - 8));
        int y1 = CENTER_Y + (int)(sin(angle) * (RADIUS - 8));
        int x2 = CENTER_X + (int)(cos(angle) * (RADIUS - 2));
        int y2 = CENTER_Y + (int)(sin(angle) * (RADIUS - 2));

        ST7735_DrawLine(x1, y1, x2, y2, ST7735_WHITE);
    }
}


void DrawHands(void) {
    float angle_s = (second * 6 - 90) * M_PI / 180;
    float angle_m = (minute * 6 + second * 0.1 - 90) * M_PI / 180;
    float angle_h = ((hour % 12) * 30 + minute * 0.5 - 90) * M_PI / 180;

    int sx = CENTER_X + cos(angle_s) * (RADIUS * 0.9);
    int sy = CENTER_Y + sin(angle_s) * (RADIUS * 0.9);
    int mx = CENTER_X + cos(angle_m) * (RADIUS * 0.75);
    int my = CENTER_Y + sin(angle_m) * (RADIUS * 0.75);
    int hx = CENTER_X + cos(angle_h) * (RADIUS * 0.5);
    int hy = CENTER_Y + sin(angle_h) * (RADIUS * 0.5);

    ST7735_DrawLine(CENTER_X, CENTER_Y, hx, hy, ST7735_GREEN);
    ST7735_DrawLine(CENTER_X, CENTER_Y, mx, my, ST7735_BLUE);
    ST7735_DrawLine(CENTER_X, CENTER_Y, sx, sy, ST7735_RED);
}

void ST7735_SetRotation(uint8_t m)
{
    uint8_t rotation = m % 4;
    uint8_t madctl = 0;

    ST7735_SendCommand(0x36);  // MADCTL

    switch (rotation)
    {
        case 0: // Portrait (پیش‌فرض)
            madctl = 0x00; // MX=0, MY=0
            break;
        case 1: // Landscape (چرخش 90 درجه)
            madctl = 0x60; // MV=1, MX=0, MY=1
            break;
        case 2: // Portrait inverted
            madctl = 0xC0; // MX=1, MY=1
            break;
        case 3: // Landscape inverted (چرخش 270 درجه)
            madctl = 0xA0; // MV=1, MX=1, MY=0
            break;
    }

    ST7735_SendData(&madctl, 1);
}

