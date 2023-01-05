/*
 * uart.c
 *
 *  Created on: 3 gru 2020
 *      Author: bga-service
 */
#include "stm32g431xx.h"
#include "uart.h"



void send_char(char c)
{
    while (!(USART2->ISR & USART_ISR_TXE));
    USART2->TDR=c;
}


void send_string(const char* s)
{
    while (*s)
        send_char(*s++);

}

int __io_putchar(int ch) {
  while (!(USART2->ISR & USART_ISR_TXE));
  USART2->TDR=ch;
  return 0;
}

void send_string_dma(uint8_t *buffer, uint16_t size, uint8_t mode)
    {
    if(mode==BLOCK)
  	{
	DMA1->IFCR |=DMA_TRANSFER_COMPLETE_FLAG;
  	}
    DMA_CHx->CCR &= ~ DMA_CCR_EN;
    DMA_CHx->CPAR = (uint32_t) &(USARTx->TDR); /* (5) */
    DMA_CHx->CMAR = (uint32_t) buffer; /* (6) */
    DMA_CHx->CNDTR = size; /* Data size */
    DMA_CHx->CCR = DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_EN; /* (7) */
    if(mode==BLOCK)
	{
	while(!(DMA1->ISR & DMA_TRANSFER_COMPLETE_FLAG));
	DMA1->IFCR |=DMA_TRANSFER_COMPLETE_FLAG;
	}
    }
