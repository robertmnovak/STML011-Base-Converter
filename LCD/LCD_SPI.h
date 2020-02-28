#ifndef SPI_LCD_SPI_H_
#define SPI_LCD_SPI_H_
#include "stm32l0xx.h"
#include <stdint.h>
#include "SPI_STM32l011.h"

//Data pin for LCD
#define DC (1<<2)
//Reset pin for LCD
#define RS (1<<3)
//Led Pin for LCD
#define LED (1<<1)
//CS pin for LCD
#define CS (1<<4)


// Color definitions
#define GREEN 0x07E0
#define BLACK 0x0000

void clearChar(int16_t y, int16_t x, int16_t h, int16_t w, uint16_t color);
void fillRectTest(int16_t y, int16_t x, int16_t h, int16_t w, uint16_t color);
void moveCursor(uint8_t x , uint8_t y);
void drawString(char *str);
void drawPixel(int16_t x, int16_t y, uint16_t color);
void drawChar(char character);
uint8_t reverseBits(uint8_t num);
void LCD_Delay(int x);
void init_lcd(void);
void writedata(uint8_t d);
void writecommand(uint8_t d);
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void hspi_cmd(uint8_t cmd);
#endif /* SPI_LCD_SPI_H_ */
