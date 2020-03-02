#ifndef KEYPAD_H_
#define KEYPAD_H_
#include "stm32l0xx.h"
#include <stdint.h>


void GPIO_Keypad_Enable(void);
void detect_keypress(void);

#endif /* KEYPAD_H_ */

