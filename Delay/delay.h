#ifndef DELAY_H_
#define DELAY_H_
#define SYS_CLOCK_HZ 16000000U
#include <stdint.h>

void delay_enable(uint32_t ticks);
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);


#endif /* DELAY_H_ */

