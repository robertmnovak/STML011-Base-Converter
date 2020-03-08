#include "delay.h"
#include "stm32l0xx.h"

void delay_ticks(uint32_t ticks){
	SysTick->LOAD = ticks;
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
	
	while((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0 );
	
	SysTick->CTRL = 0;
}

void delay_us(uint32_t us){
	delay_ticks((us * (SYS_CLOCK_HZ / 8)) / 1000000);
}

void delay_ms(uint32_t ms){
	delay_ticks((ms * (SYS_CLOCK_HZ / 8)) / 1000);
}

