#include "Keypad.h"
#include "LCD_SPI.h"

uint16_t keyRelease = 0;
uint8_t textX = 10;
uint16_t debounce = 0;
uint8_t prevKey = 33;
uint8_t currentKey = 33;
uint8_t keySent = 0;
uint8_t numCount = 0;
uint32_t enteredNumber = 0;
uint8_t currentState = MAIN_MENU;
uint8_t menu_select = 0;

char keys[4][5] = {
	{'1', '2', '3', 'F', CLEAR}, //R = CLEAR
	{'4', '5', '6', 'E', ENTER}, //T = ENTER
	{'7', '8', '9', 'D', UP}, //U = UP
	{'0', 'A', 'B', 'C', DOWN}	//J = DOWN
};


/********************************************************************************
GPIO Keypad Enable

Sets column of multiplexed keypad to outputs and rows as inputs

Then enables external interrupts for the rows and choose rising edge trigger
********************************************************************************/
void GPIO_Keypad_Enable(void){
	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	//Set-up Rows as Inputs (PINS: A9,A10,A11,A12)
	GPIOA->MODER &= ~(GPIO_MODER_MODE9 | GPIO_MODER_MODE10 | GPIO_MODER_MODE11 | GPIO_MODER_MODE12);
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEED9_0 | GPIO_OSPEEDER_OSPEED10_0 | GPIO_OSPEEDER_OSPEED11_0 | GPIO_OSPEEDER_OSPEED12_0 | GPIO_OSPEEDER_OSPEED15_0);
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD9 | GPIO_PUPDR_PUPD10 | GPIO_PUPDR_PUPD11 | GPIO_PUPDR_PUPD12);
	GPIOA->PUPDR |= (GPIO_PUPDR_PUPD9_1 | GPIO_PUPDR_PUPD10_1 | GPIO_PUPDR_PUPD11_1 | GPIO_PUPDR_PUPD12_1);
	
	
	//Set-up Columns as Outputs (PINS: B3,B4,B5,B6, B7)
	GPIOB->MODER &= ~(GPIO_MODER_MODE3 | GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
	GPIOB->MODER |= (GPIO_MODER_MODE3_0 | GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0 | GPIO_MODER_MODE6_0 | GPIO_MODER_MODE7_0);
	GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEED3_0 | GPIO_OSPEEDER_OSPEED4_0 | GPIO_OSPEEDER_OSPEED5_0 | GPIO_OSPEEDER_OSPEED6_0 | GPIO_OSPEEDER_OSPEED7_0);
	GPIOB->ODR |= (1<<7);
	
	//Set-up interrupts for the rows
	EXTI->IMR |= (EXTI_IMR_IM9 | EXTI_IMR_IM10 | EXTI_IMR_IM11 | EXTI_IMR_IM12);
	EXTI->RTSR |= (EXTI_RTSR_RT9 | EXTI_RTSR_RT10 | EXTI_RTSR_RT11 | EXTI_RTSR_RT12);
	
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
	GPIOB->ODR &= ~((1<<3)|(1<<4)|(1<<5)|(1<<6)|(1<<7));
	
		//Check column 5
	GPIOB->ODR |= (1<<7);
	LCD_Delay(20);
	if(GPIOA->IDR & (1<<9)){
		col = 4; row = 0;
	} else if (GPIOA->IDR & (1<<10)){
		col = 4; row = 1;
	} else if (GPIOA->IDR & (1<<11)){
		col = 4; row = 2;
	} else if (GPIOA->IDR & (1<<12)){
		col = 4; row = 3;
	}
	GPIOB->ODR &= ~(1<<7);
	
		//Check column 4
	GPIOB->ODR |= (1<<6);
	if(GPIOA->IDR & (1<<9)){
		col = 3; row = 0;
	} else if (GPIOA->IDR & (1<<10)){
		col = 3; row = 1;
	} else if (GPIOA->IDR & (1<<11)){
		col = 3; row = 2;
	} else if (GPIOA->IDR & (1<<12)){
		col = 3; row = 3;
	}
	GPIOB->ODR &= ~(1<<6);

		//Check column 3
	GPIOB->ODR |= (1<<5);
	if(GPIOA->IDR & (1<<9)){
		col = 2; row = 0;
	} else if (GPIOA->IDR & (1<<10)){
		col = 2; row = 1;
	} else if (GPIOA->IDR & (1<<11)){
		col = 2; row = 2;
	} else if (GPIOA->IDR & (1<<12)){
		col = 2; row = 3;
	}
	GPIOB->ODR &= ~(1<<5);
	
		//Check column 2
	GPIOB->ODR |= (1<<4);
	if(GPIOA->IDR & (1<<9)){
		col = 1; row = 0;
	} else if (GPIOA->IDR & (1<<10)){
		col = 1; row = 1;
	} else if (GPIOA->IDR & (1<<11)){
		col = 1; row = 2;
	} else if (GPIOA->IDR & (1<<12)){
		col = 1; row = 3;
	}
	GPIOB->ODR &= ~(1<<4);
	
	//Check column 1
	GPIOB->ODR |= (1<<3);
	if(GPIOA->IDR & (1<<9)){
		col = 0; row = 0;
	} else if (GPIOA->IDR & (1<<10)){
		col = 0; row = 1;
	} else if (GPIOA->IDR & (1<<11)){
		col = 0; row = 2;
	} else if (GPIOA->IDR & (1<<12)){
		col = 0; row = 3;
	}
	GPIOB->ODR &= ~(1<<3);


	GPIOB->ODR |= (1<<3)|(1<<4)|(1<<5)|(1<<6)|(1<<7);
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
		switch(currentState){
/*****************************HEX START*************************************/			
			case HEX_MODE: 
				if(keys[row][col] < 71){
					moveCursor(textX, hex_y);
					enteredNumber |= (convert_to_integer(keys[row][col]) << (28 - numCount*4));
					drawChar(keys[row][col], GREEN);
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
						currentState = MAIN_MENU;
					}
				} else if (keys[row][col] == CLEAR){
					erase_Hex();
					enteredNumber = 0;
					textX = 44;
					numCount = 0;
				}
				break;
/*****************************HEX END*************************************/	
				
/*****************************BINARY START*************************************/				
			case BINARY_MODE: 
				if(keys[row][col] == '0' || keys[row][col] == '1'){
					if(numCount < 12){
						moveCursor(textX, binary_y_1);
					} else if(numCount < 24){
						moveCursor(textX, binary_y_2);
					} else {
						moveCursor(textX, binary_y_3);
					}
					enteredNumber |= (convert_to_integer(keys[row][col]) << (31-numCount));
					drawChar(keys[row][col], GREEN);
					keySent = 1;
					numCount++;
					textX += get_charWidth(keys[row][col]);
					if(numCount == 32){
						print_decimal();
						print_hex();
						enteredNumber = 0;
						numCount = 0;
						textX = 10;
						currentState = MAIN_MENU;
					} else if(numCount == 4 | numCount == 8 | numCount == 16 | numCount == 20 | numCount == 28){
						drawChar(' ', GREEN);
						textX += get_charWidth(' ');
					} else if(numCount == 12 | numCount == 24){
						textX = 10;
					}
				} else if (keys[row][col] == CLEAR){
					erase_Binary();
					textX = 10;
					numCount = 0;
					enteredNumber = 0;
				}
				break;
/*****************************BINARY END*************************************/	
				
/*****************************DECIMAL START*************************************/			
			case DECIMAL_MODE:
				if(keys[row][col] == ENTER){
					print_binary();
					print_hex();
					enteredNumber = 0;
					numCount = 0;
					textX = 10;
					currentState = MAIN_MENU;
					break;
				} else if(keys[row][col] == CLEAR){
					erase_Decimal();
					enteredNumber = 0;
					numCount = 0;
					textX = 10;
				} else if(keys[row][col] < 58){
					moveCursor(textX, decimal_y);
					keySent = 1;
					drawChar(keys[row][col], GREEN);
					textX += get_charWidth(keys[row][col]);
					numCount++;
					enteredNumber *= 10;
					enteredNumber += convert_to_integer(keys[row][col]);
					
					if(numCount == 10){
						print_binary();
						print_hex();
						enteredNumber = 0;
						numCount = 0;
						textX = 10;
						currentState = MAIN_MENU;
					}
				}
/*****************************DECIMAL END*************************************/		
				
/*****************************MAIN MENU START*************************************/			
			case MAIN_MENU:
				if(keys[row][col] == ENTER){
					erase_Binary();
					erase_Hex();
					erase_Decimal();
					currentState = menu_select;
					if(currentState == HEX_MODE) textX = 44;
					if(currentState == DECIMAL_MODE | currentState == BINARY_MODE) textX = 10;
					
				} else if(keys[row][col] == UP){
					if(menu_select == 0){
						menu_select = 2;
					} else {
						menu_select--;
					}
					updateMenu();
				} else if(keys[row][col] == DOWN){
					if(menu_select == 2){
						menu_select = 0;
					} else {
						menu_select++;
					}
					updateMenu();
				}
				keySent = 1;
			break;
/*****************************MAIN MENU END*************************************/		
			default:
			
				break;
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


void erase_Binary(void){
	set_fill(45000);
	fillRect(10, 130, 232, 75, BLACK);
	moveCursor(10, 205);
	drawString("0000 0000 0000", GREEN);
	moveCursor(10,180);
	drawString("0000 0000 0000", GREEN);
	moveCursor(10,155);
	drawString("0000 0000", GREEN);
}

void erase_Hex(void){
	set_fill(50000);
	fillRect(44, 30, 131, 25, BLACK);
	moveCursor(44,55);
	drawString("00000000", GREEN);
}

void erase_Decimal(void){
	set_fill(50000);
	fillRect(10, 80, 175, 25, BLACK);
	moveCursor(10, 105);
	drawString("0", GREEN);
}

/********************************************************************************
Menu Update

Changes current selected mode to RED color and all others to GREEN
********************************************************************************/
void updateMenu(void){
	if(menu_select == BINARY_MODE){
		moveCursor(10, 230);
		drawString("Binary", RED);
		moveCursor(10, 130);
		drawString("Decimal", GREEN);
		moveCursor(10, 80);
		drawString("Hex", GREEN);
	} else if (menu_select == DECIMAL_MODE){
		moveCursor(10, 230);
		drawString("Binary", GREEN);
		moveCursor(10, 130);
		drawString("Decimal", RED);
		moveCursor(10, 80);
		drawString("Hex", GREEN);
	} else if (menu_select == HEX_MODE){
		moveCursor(10, 230);
		drawString("Binary", GREEN);
		moveCursor(10, 130);
		drawString("Decimal", GREEN);
		moveCursor(10, 80);
		drawString("Hex", RED);
	}
}

/********************************************************************************
Print Hex

Takes the number that was inputted by the user and converts it to Hex and prints
it onto the LCD screen
********************************************************************************/
void print_hex(void){
	uint32_t tempNum = enteredNumber;
	uint8_t tempX = 163;
	uint8_t temp_numCount = 0;
	while(temp_numCount < 8){
		moveCursor(tempX, hex_y);
		switch(tempNum%16){
			case 0:
				drawChar('0', GREEN);
					tempX -= get_charWidth('0');
				break;
			case 1:
				drawChar('1', GREEN);
				tempX -= get_charWidth('1');
				break;
			case 2:
				drawChar('2', GREEN);
				tempX -= get_charWidth('2');
				break;
			case 3:
				drawChar('3', GREEN);
				tempX -= get_charWidth('3');
				break;
			case 4:
				drawChar('4', GREEN);
				tempX -= get_charWidth('4');
				break;
			case 5:
				drawChar('5', GREEN);
				tempX -= get_charWidth('5');
				break;
			case 6:
				drawChar('6', GREEN);
				tempX -= get_charWidth('6');
				break;
			case 7:
				drawChar('7', GREEN);
				tempX -= get_charWidth('7');
				break;
			case 8:
				drawChar('8', GREEN);
				tempX -= get_charWidth('8');
				break;
			case 9:
				drawChar('9', GREEN);
				tempX -= get_charWidth('9');
				break;
			case 10:
				drawChar('A', GREEN);
				tempX -= get_charWidth('A');
				break;
			case 11:
				drawChar('B', GREEN);
				tempX -= get_charWidth('B');
				break;
			case 12:
				drawChar('C', GREEN);
				tempX -= get_charWidth('C');
				break;
			case 13:
				drawChar('D', GREEN);
				tempX -= get_charWidth('D');
				break;
			case 14:
				drawChar('E', GREEN);
				tempX -= get_charWidth('E');
				break;
			case 15:
				drawChar('F', GREEN);
				tempX -= get_charWidth('F');
				break;
			default:
				
				break;
				
		}
		tempNum /= 16;
		temp_numCount++;
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
			drawChar('1', GREEN);
			tempX -= get_charWidth('1');
		} else {
			drawChar('0', GREEN);
			tempX -= get_charWidth('0');
		}
		tempNum /= 2;
		temp_numCount++;
		
		if(temp_numCount == 4){
			moveCursor(tempX, binary_y_3);
			tempX -= get_charWidth(' ');
			drawChar(' ', GREEN);
		}
	}
	
	tempX = 215;
	while(temp_numCount < 20){
		moveCursor(tempX, binary_y_2);
		if(tempNum % 2){
			drawChar('1', GREEN);
			tempX -= get_charWidth('1');
		} else {
			drawChar('0', GREEN);
			tempX -= get_charWidth('0');
		}
		tempNum /= 2;
		temp_numCount++;
		if(temp_numCount == 12 | temp_numCount == 16){
			moveCursor(tempX, binary_y_2);
			tempX -= get_charWidth(' ');
			drawChar(' ', GREEN);
		}
	}
	
	tempX = 215;
	while(temp_numCount < 32){
		moveCursor(tempX, binary_y_1);
		if(tempNum % 2){
			drawChar('1', GREEN);
			tempX -= get_charWidth('1');
		} else {
			drawChar('0', GREEN);
			tempX -= get_charWidth('0');
		}
		tempNum /= 2;
		temp_numCount++;
		if(temp_numCount == 24 | temp_numCount == 28){
			moveCursor(tempX, binary_y_1);
			tempX -= get_charWidth(' ');
			drawChar(' ', GREEN);
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
		drawChar(num + '0', GREEN);
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
	if(EXTI->PR & (1<<11)){
		EXTI->PR |= (1<<11);
	}
	if(EXTI->PR & (1<<9)){
		EXTI->PR |= (1<<9);
	}
	if(EXTI->PR & (1<<10)){
		EXTI->PR |= (1<<10);
	}
	if(EXTI->PR & (1<<12)){
		EXTI->PR |= (1<<12);
	}
	detect_keypress();
}
