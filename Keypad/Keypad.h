#ifndef KEYPAD_H_
#define KEYPAD_H_
#include "stm32l0xx.h"
#include <stdint.h>

#define binary_y_1 205
#define binary_y_2 180
#define binary_y_3 155
#define decimal_y 105
#define hex_y 55


uint32_t convert_to_integer(char character);
void GPIO_Keypad_Enable(void);
void detect_keypress(void);
void print_decimal(void);
void print_binary(void);
#endif /* KEYPAD_H_ */

