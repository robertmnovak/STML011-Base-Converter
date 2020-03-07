#ifndef KEYPAD_H_
#define KEYPAD_H_
#include "stm32l0xx.h"
#include <stdint.h>

#define binary_y_1 205
#define binary_y_2 180
#define binary_y_3 155
#define decimal_y 105
#define hex_y 55

#define HEX_MODE 2
#define BINARY_MODE 0
#define DECIMAL_MODE 1
#define MAIN_MENU 3

#define CLEAR 'R'
#define ENTER 'T'
#define UP 'U'
#define DOWN 'J'
	
uint32_t convert_to_integer(char character);
void GPIO_Keypad_Enable(void);
void detect_keypress(void);
void print_decimal(void);
void print_binary(void);
void print_hex(void);
void keypad_Delay(int x);
void updateMenu(void);
#endif /* KEYPAD_H_ */

