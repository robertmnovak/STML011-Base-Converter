#include "stm32l0xx.h"
#include "SPI_STM32l011.h"
#include "LCD_SPI.h"

void delay(int x);
void GPIOA_Enable(void);
int main(void){
	GPIOA_Enable();
	SPI_Enable();
	
	init_lcd();
	moveCursor(10, 230);
	drawString("Binary");
	moveCursor(10, 205);
	drawString("1010 1010 1010");
	moveCursor(10,180);
	drawString("1010 1010 1010");
	moveCursor(10,155);
	drawString("1010 1010");
	moveCursor(10, 130);
	drawString("Decimal");
	moveCursor(10, 105);
	drawString("2863311530");
	moveCursor(10, 80);
	drawString("Hex");
	moveCursor(10, 55);
	drawString("0xAAAAAAAA");
	delay(500000);
	moveCursor(10, 205);
	drawString("1111 0000 1011");
	moveCursor(10,180);
	drawString("0100 1100 0100");
	moveCursor(10,155);
	drawString("0010 0000");
	moveCursor(10, 105);
	drawString("4038378528");
	moveCursor(10, 55);
	drawString("0xF0B4C420");
	while(1){
		GPIOA->ODR ^= (1<<10);
		delay(100000);
	}
}

void delay(int x){
	while(x>0){
		x--;
	}
}

/********************************************************************************
GPIO Enable Function

Enables PA1(LCD LED), PA2(LCD DC), PA3(LCD RESET), PA4(LCD CS), PA10(External LED)
********************************************************************************/
void GPIOA_Enable(void){
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
	
	GPIOA->ODR &= ~(1<<10);
	GPIOA->MODER &= ~(GPIO_MODER_MODE10 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3 | GPIO_MODER_MODE4);
	GPIOA->MODER |= (GPIO_MODER_MODE10_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0 | GPIO_MODER_MODE4_0);
	GPIOA->OSPEEDR &= ~((1<<20)|(1<<21));
	GPIOA->PUPDR &= ~((1<<20)|(1<<21));
	GPIOA->ODR |= CS;
}

