#ifndef SPI_STM32l011_H_
#define SPI_STM32l011_H_
#include "stm32l0xx.h"
#include <stdint.h>

void spi_delay(int x);
void DMA_Enable(void);
void SPI_Enable(void);
void SPI_Transmit(uint8_t *dataIn, uint16_t size);
void SPI_Send(uint8_t data);
#endif /* SPI_STM32l011_H_ */

