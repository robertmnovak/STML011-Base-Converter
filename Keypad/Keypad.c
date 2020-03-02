#include "Keypad.h"
#include "LCD_SPI.h"

uint16_t keyRelease = 0;
uint8_t textX = 44;
uint16_t debounce = 0;
uint8_t prevKey = 33;
uint8_t currentKey = 33;
uint8_t keySent = 0;
uint8_t numCount = 0;
uint32_t enteredNumber = 0;

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
			col = 0; row = 0;
		
	} else if (GPIOA->IDR & (1<<10)){
			col = 0; row = 1;
		
	}
	GPIOA->ODR &= ~(1<<0);
	GPIOA->ODR |= (1<<1);
	if(GPIOA->IDR & (1<<9)){
		col = 1; row = 0;
		
	} else if (GPIOA->IDR & (1<<10)){
		col = 1; row = 1;
	
	}
	GPIOA->ODR |= (1<<0);
	
	currentKey = (uint8_t)keys[row][col];
	
//Debouncing
	if(currentKey == prevKey && debounce < 500){
		debounce++;
	} else if(currentKey != prevKey){
		keySent = 0;
		prevKey = currentKey;
		debounce = 0;
	} else if(debounce > 499 && keySent == 0){
		keyRelease = 0;
		moveCursor(textX, hex_y);
		enteredNumber |= (convert_to_integer(keys[row][col]) << (28 - numCount*4));
		drawChar(keys[row][col]);
		keySent = 1;
		numCount++;
		textX += get_charWidth(keys[row][col]);
		if(numCount == 8){
			print_decimal();
			print_binary();
			enteredNumber = 0;
			numCount = 0;
			textX = 44;
			moveCursor(textX, 55);
		}
	} else {
		if(keyRelease > 23000){
			debounce = 0;
			prevKey = 33;
			keySent = 0;
		}
		keyRelease++;
	}
}


/********************************************************************************
Print Binary

Takes the number that was inputted by the user and converts it to binary and prints
it onto the LCD screen
********************************************************************************/
void print_binary(void){
	//3rd row start = 138
	//1st + 2nd row start = 215
	uint32_t tempNum = enteredNumber;
	uint8_t tempX = 138;
	uint8_t temp_numCount = 0;
	while(temp_numCount < 8){
		moveCursor(tempX, binary_y_3);
		if(tempNum % 2){
			drawChar('1');
			tempX -= get_charWidth('1');
		} else {
			drawChar('0');
			tempX -= get_charWidth('0');
		}
		tempNum /= 2;
		temp_numCount++;
		
		if(temp_numCount == 4){
			moveCursor(tempX, binary_y_3);
			tempX -= get_charWidth(' ');
			drawChar(' ');
		}
	}
	
	tempX = 215;
	while(temp_numCount < 20){
		moveCursor(tempX, binary_y_2);
		if(tempNum % 2){
			drawChar('1');
			tempX -= get_charWidth('1');
		} else {
			drawChar('0');
			tempX -= get_charWidth('0');
		}
		tempNum /= 2;
		temp_numCount++;
		if(temp_numCount == 12 | temp_numCount == 16){
			moveCursor(tempX, binary_y_2);
			tempX -= get_charWidth(' ');
			drawChar(' ');
		}
	}
	
	tempX = 215;
	while(temp_numCount < 32){
		moveCursor(tempX, binary_y_1);
		if(tempNum % 2){
			drawChar('1');
			tempX -= get_charWidth('1');
		} else {
			drawChar('0');
			tempX -= get_charWidth('0');
		}
		tempNum /= 2;
		temp_numCount++;
		if(temp_numCount == 24 | temp_numCount == 28){
			moveCursor(tempX, binary_y_1);
			tempX -= get_charWidth(' ');
			drawChar(' ');
		}
	}
}

/********************************************************************************
Print Decimal

Takes the number that was inputted by the user and prints it onto the LCD screen
********************************************************************************/
void print_decimal(void){
	uint32_t tempNum = enteredNumber;
	uint8_t num;
	uint8_t tempX = 10;
	uint32_t divisor = 1000000000;
	for(uint8_t i = 0; i < 10; i++){
		moveCursor(tempX, 105);
		num = (uint8_t)(tempNum/divisor);
		drawChar(num + '0');
		tempNum %= divisor;
		tempX += get_charWidth(num + '0');
		divisor /= 10;
	}
}

/********************************************************************************
Convert To Integer

Takes the incoming char from the user input and returns a integer value that
is used to calculate the entered decimal value
********************************************************************************/
uint32_t convert_to_integer(char character){
	uint32_t result;
	switch(character){
		case '0':
			result = 0;
			break;
		case '1':
			result = 1;
			break;
		case '2':
			result = 2;
			break;
		case '3':
			result = 3;
			break;
		case '4':
			result = 4;
			break;
		case '5':
			result = 5;
			break;
		case '6':
			result = 6;
			break;
		case '7':
			result = 7;
			break;
		case '8':
			result = 8;
			break;
		case '9':
			result = 9;
			break;
		case 'A':
			result = 10;
			break;
		case 'B':
			result = 11;
			break;
		case 'C':
			result = 12;
			break;
		case 'D':
			result = 13;
			break;
		case 'E':
			result = 14;
			break;
		case 'F':
			result = 15;
			break;
		default:
			result = 0;
			break;
	}
	return result;
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
