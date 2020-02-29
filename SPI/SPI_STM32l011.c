#include "SPI_STM32l011.h"

void spi_delay(int x){
	while(x>0){
		x--;
	}
}

/********************************************************************************
DMA Enable function

Activates the DMA clock, and interrupts to be used with SPI1
********************************************************************************/
void DMA_Enable(void){
	//Enable DMA1 clock
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	
	DMA1_Channel3->CNDTR = 2;
	DMA1_Channel3->CCR |= DMA_CCR_DIR | DMA_CCR_TCIE | DMA_CCR_CIRC;
	DMA1_Channel3->CPAR = (uint32_t)&SPI1->DR;
	DMA1_CSELR->CSELR |= (1<<8);

	NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0);
	NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
}


/********************************************************************************
SPI Enable function

This function enables pins PA5(SCLK), PA6(MISO), and, PA7(MOSI) to be used as alternate 
functions. Then enables the settings used for the SPI peripheral. In this chase
it is enabled as half duplex.
********************************************************************************/

void SPI_Enable(void){
	DMA_Enable();
	//SPI1 CLOCK ENABLE
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	//Set PA5 AND PA7 to alternative functions SCLK AND MOSI
	GPIOA->MODER &= ~(GPIO_MODER_MODE5 | GPIO_MODER_MODE7 | GPIO_MODER_MODE6);
	GPIOA->MODER |= (GPIO_MODER_MODE5_1 | GPIO_MODER_MODE7_1 | GPIO_MODER_MODE6_1);
	GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEED5_0 | GPIO_OSPEEDER_OSPEED7_0 | GPIO_OSPEEDER_OSPEED6_0);
	
	SPI1->CR1 |= (SPI_CR1_MSTR | SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_BR_0);
	SPI1->CR2 |= SPI_CR2_SSOE | SPI_CR2_TXDMAEN;
	SPI1->CR1 |= SPI_CR1_SPE;
}



/********************************************************************************
SPI send data function

This function waits for the transmit buffer to be empty before sending the data
********************************************************************************/
void SPI_Send(uint8_t data) {
	while (!(SPI1->SR & SPI_SR_TXE)) {};
		SPI1->DR = data;
		spi_delay(20);
}

/********************************************************************************
SPI transmit function

This function is what is primarly used in the rest of the project. It allows
for multiple bytes of data to be sent at once.
********************************************************************************/
void SPI_Transmit(uint8_t *dataIn, uint16_t size){
	uint8_t count = 0;
	while(count < size){
		SPI_Send(*dataIn);
		dataIn++;
		count++;
	}
}

