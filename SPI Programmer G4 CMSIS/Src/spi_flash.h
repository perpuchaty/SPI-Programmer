/*
 * spi_flash.h
 *
 *  Created on: 31 lip 2022
 *      Author: Sowa
 */
#include "stm32g431xx.h"


#ifndef SPI_FLASH_H_
#define SPI_FLASH_H_



#define SPI_CMD_WREN 0x06
#define SPI_CMD_WRDI 0x04
#define SPI_CMD_EWSR 0x50
#define SPI_CMD_CHIP_ERASE 0x60

#define CS_LOW     GPIOA->BSRR|=GPIO_BSRR_BR4;
#define CS_HIGH   GPIOA->BSRR|=GPIO_BSRR_BS4;
#define CS_INPUT GPIOA->IDR & GPIO_IDR_IDR_4

uint16_t spi_flash_read_id();
void spi_read_sector(uint32_t address, uint32_t size, uint8_t *data);
void spi_write_cmd(uint8_t cmd);
uint8_t spi_read_status_register();
uint8_t spi_write_status_register(uint8_t value);
void spi_write_byte(uint8_t value, uint32_t address);
void spi_write_aai(uint32_t address, uint8_t *data, uint8_t size);
void spi_flash_irq_handler();
uint32_t spi_flash_get_crc();
void spi_flash_page_program(uint32_t address, uint32_t size, uint8_t *data);
void spi_flash_source_chip_erase();




uint8_t spi_flash_tx_buff[16], spi_flash_rx_buff[16];
uint8_t buffer_tst[1024];

typedef struct {
   uint8_t read_request;
   uint8_t address;
   uint8_t wren;
   uint8_t read_status_register;
   uint8_t write_status_register;
   uint8_t write_byte;
   uint8_t write_aai_address;
   uint8_t write_aai_data;
   uint8_t adc_read_data;
   uint8_t read_busy;
   uint8_t page_program;

} spiFlag;

spiFlag spiFlags;

typedef struct {
uint16_t id;
uint16_t id_tmp;
uint8_t status_register;
uint8_t sector1[0x1000];
uint8_t sector2[0x1000];
uint32_t page_size;
uint32_t sector_size;
uint32_t number_of_blocks;
uint32_t number_of_sectors;
uint32_t number_of_sectors_per_block;
uint32_t number_of_pages;


} spiChip;

spiChip flashSource, flashTarget;

#endif /* SPI_FLASH_H_ */
