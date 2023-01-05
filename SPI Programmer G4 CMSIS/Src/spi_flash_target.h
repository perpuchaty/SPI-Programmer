/*
 * spi_flash_target.h
 *
 *  Created on: 17 sie 2022
 *      Author: Sowa
 */
#include "stm32g431xx.h"

#ifndef SPI_FLASH_TARGET_H_
#define SPI_FLASH_TARGET_H_

#define CS_LOW_TARGET     GPIOA->BSRR |= GPIO_BSRR_BR_12;
#define CS_HIGH_TARGET    GPIOA->BSRR |= GPIO_BSRR_BS_12;
#define CS_INPUT_TARGET GPIOA->IDR & GPIO_IDR_IDR_12

void spi_flash_target_chip_erase();
void spi_flash_target_irq_handler();
void spi_target_page_program(uint32_t address, uint32_t size, uint8_t *data);
void spi_target_read_sector(uint32_t address, uint32_t size, uint8_t *data);
uint8_t spi_target_read_status_register();
void spi_target_write_cmd(uint8_t cmd);
uint16_t spi_flash_target_read_id();

uint8_t spi_flash_tx_buff_target[16], spi_flash_rx_buff_target[16];

volatile typedef struct {
   uint8_t read_request;
   uint8_t address;
   uint8_t wren;
   uint8_t read_status_register;
   uint8_t write_status_register;
   uint8_t write_byte;
   uint8_t write_aai_address;
   uint8_t write_aai_data;
   uint8_t adc_read_data;
   uint8_t page_program;
} spiTargetFlag;

volatile spiTargetFlag spiTargetFlags;



#endif /* SPI_FLASH_TARGET_H_ */
