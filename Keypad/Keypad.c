#include "Keypad.h"
#include "LCD_SPI.h"

uint8_t debounce = 0;
uint8_t prevKey = 33;
uint8_t currentKey = 33;
uint8_t keySent = 0;

char keys[2][2] = {
	{'A', 'B'},
	{'C', 'D'}
};

/********************************************************************************
GPIO Keypad Enable

Sets column of multiplexed keypad to outputs and rows as inputs

Then enables external interrupts for the rows and choose rising edge trigger
********************************************************************************/
void GPIO_Keypad_Enable(void){
	GPIOA->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE9 | GPIO_MODER_MODE10);
	GPIOA->MODER |= (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0);
	GPIOA->OSPEEDR |= (GPIO_MODER_MODE0_1 | GPIO_MODER_MODE1_1);
	GPIOA->PUPDR |= (GPIO_PUPDR_PUPD9_1 | GPIO_PUPDR_PUPD10_1);
	
	GPIOA->ODR |= (1<<0)|(1<<1);

	EXTI->IMR |= (EXTI_IMR_IM9 | EXTI_IMR_IM10);
	EXTI->RTSR |= (EXTI_RTSR_RT9 | EXTI_RTSR_RT10);
	
	NVIC_EnableIRQ(EXTI4_15_IRQn);
	NVIC_SetPriority(EXTI4_15_IRQn, 1);
	
}

/********************************************************************************
Detect Keypress

Turns each column on and see if it sets the input as high to detect keypress

The bottom part contains a simple debouncing loop
********************************************************************************/
void detect_keypress(void){
	uint8_t row = 0, col = 0;
	GPIOA->ODR &= ~((1<<0)|(1<<1));
	GPIOA->ODR |= (1<<0);
	if(GPIOA->IDR & (1<<9)){
		
			moveCursor(10, 205);
			col = 0;
			row = 0;
			//drawChar('A');
		
	} else if (GPIOA->IDR & (1<<10)){
		
			moveCursor(10,180);\
			col = 0;
			row = 1;
			//drawString("2222 2222 2222");
		
	}
	GPIOA->ODR &= ~(1<<0);
	GPIOA->ODR |= (1<<1);
	if(GPIOA->IDR & (1<<9)){

		moveCursor(10, 105);
		col = 1;
		row = 0;
		//drawString("4038378528");
		
	} else if (GPIOA->IDR & (1<<10)){
		
		moveCursor(10, 55);
		col = 1;
		row = 1;
		//drawString("0xF0B4C420");
	
	}
	GPIOA->ODR |= (1<<0);
	
	currentKey = (uint8_t)keys[row][col];
	
//Debouncing
	if(currentKey == prevKey && debounce < 240){
		debounce++;
	} else if(currentKey != prevKey){
		keySent = 0;
		prevKey = currentKey;
		debounce = 0;
	}
	if(debounce > 239 && keySent == 0){
		drawChar(keys[row][col]);
		keySent = 1;
	}
}

/********************************************************************************
GPIO External Interrupt

Checks to see which row the button was pressed in, then clears the interrupt
and calls the detect keypress function
********************************************************************************/
void EXTI4_15_IRQHandler(void){
	if(EXTI->PR & EXTI_PR_PR10){
		EXTI->PR |= EXTI_PR_PR10;
	} else if(EXTI->PR & EXTI_PR_PR9){
		EXTI->PR |= EXTI_PR_PR9;
	}
	detect_keypress();
}
