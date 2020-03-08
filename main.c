#include "stm32l0xx.h"
#include "SPI_STM32l011.h"
#include "LCD_SPI.h"
#include "Keypad.h"
#include "delay.h"

void delay(int x);
void GPIOA_Enable(void);
void system_clock_init(void);
void setup_lcd_background(void);
void sleepMode(void);

int main(void){
	system_clock_init();
	GPIOA_Enable();
	SPI_Enable();
	init_lcd();
	GPIO_Keypad_Enable();
	setup_lcd_background();
	
	
	while(1){
	}

}

void setup_lcd_background(void){
	fill_background();
	moveCursor(10, 230);
	drawString("Binary", RED);
	moveCursor(10, 205);
	drawString("0000 0000 0000", GREEN);
	moveCursor(10,180);
	drawString("0000 0000 0000", GREEN);
	moveCursor(10,155);
	drawString("0000 0000", GREEN);
	moveCursor(10, 130);
	drawString("Decimal", GREEN);
	moveCursor(10, 105);
	drawString("0", GREEN);
	moveCursor(10, 80);
	drawString("Hex", GREEN);
	moveCursor(10, 55);
	drawString("0x00000000", GREEN);
}

void system_clock_init(void){
  RCC->CR    |=  (RCC_CR_HSION);
  while (!(RCC->CR & RCC_CR_HSIRDY));
  RCC->CFGR  &= ~(RCC_CFGR_SW);
  RCC->CFGR  |=  (RCC_CFGR_SW_HSI);
  while (!(RCC->CFGR & RCC_CFGR_SWS_HSI));
}

/********************************************************************************
GPIO Enable Function

Enables PA1(LCD LED), PA2(LCD DC), PA3(LCD RESET), PA4(LCD CS)
********************************************************************************/
void GPIOA_Enable(void){
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
	
	GPIOA->ODR &= ~(1<<10);
	GPIOA->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3 | GPIO_MODER_MODE4);
	
	GPIOA->MODER |= (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0 | GPIO_MODER_MODE4_0);
	GPIOA->OSPEEDR &= ~((1<<20)|(1<<21));
	GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEED2_0 | GPIO_OSPEEDER_OSPEED3_0 | GPIO_OSPEEDER_OSPEED4_0);
	GPIOA->PUPDR &= ~((1<<20)|(1<<21));
	GPIOA->ODR |= CS;
}

