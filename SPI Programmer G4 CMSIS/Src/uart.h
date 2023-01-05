/*
 * uart.h
 *
 *  Created on: 3 gru 2020
 *      Author: bga-service
 */

#ifndef UART_H_
#define UART_H_

#include "stm32g431xx.h"

#define DMA_CHx DMA1_Channel6
#define USARTx USART2
#define DMA_TRANSFER_COMPLETE_FLAG DMA_ISR_TCIF6

volatile char uart1_rxbuffer[1024];
uint16_t uart1_rx_buffer_position;
uint32_t uart1_tx_buffer;

void uart_config();
void send_char(char c);
void send_string(const char* s);
void send_string_dma(uint8_t *buffer, uint16_t size, uint8_t mode);

enum{
    BLOCK,
    NON_BLOCK
};
#endif /* UART_H_ */
