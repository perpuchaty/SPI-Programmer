/*
 * spi_flash_target.c
 *
 *  Created on: 17 sie 2022
 *      Author: Sowa
 */
#include <stdint.h>
#include "spi_flash_target.h"
#include "config.h"
#include "spi_flash.h"
#include "stm32g431xx.h"
#include "system.h"
#include "gui.h"

uint16_t spi_flash_target_read_id()
    {

    DMA1_Channel4->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel3->CCR &= ~ DMA_CCR_EN;

    for (uint8_t i = 0; i < 10; i++)
	{
	spi_flash_tx_buff_target[i] = 0;
	}
    spi_flash_tx_buff_target[0] = 0x9F;

    CS_LOW_TARGET
    ;

    DMA1_Channel3->CPAR = (uint32_t) &(SPI2->DR); /* (5) */
    DMA1_Channel3->CMAR = (uint32_t) &spi_flash_tx_buff_target; /* (6) */
    DMA1_Channel3->CNDTR = 4; /* Data size */
    DMA1_Channel3->CCR = DMA_CCR_MINC | DMA_CCR_DIR; /* (7) */

    DMA1_Channel4->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel4->CPAR = (uint32_t) &(SPI2->DR); /* (1) */
    DMA1_Channel4->CMAR = (uint32_t) &spi_flash_rx_buff_target; /* (2) */
    DMA1_Channel4->CNDTR = 4; /* (3) */
    DMA1_Channel4->CCR = DMA_CCR_MINC | DMA_CCR_TCIE | DMA_CCR_EN; /* (4) */
    DMA1_Channel3->CCR |= DMA_CCR_EN;

    while (!(CS_INPUT_TARGET));
    uint16_t ret = (spi_flash_rx_buff_target[1] << 8) + spi_flash_rx_buff_target[3];
    return ret;
    }

void spi_target_write_cmd(uint8_t cmd)
    {
    DMA1_Channel4->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel3->CCR &= ~ DMA_CCR_EN;
    spi_flash_tx_buff_target[0] = cmd;			//Memory read request

    CS_LOW_TARGET
    ;		//Start transfer

    /******Send write enable CMD********/
    spiTargetFlags.wren = 1;
    DMA1_Channel3->CPAR = (uint32_t) &(SPI2->DR); /* (5) */
    DMA1_Channel3->CMAR = (uint32_t) &spi_flash_tx_buff_target; /* (6) */
    DMA1_Channel3->CNDTR = 1; /* Data size */
    DMA1_Channel3->CCR = DMA_CCR_MINC | DMA_CCR_DIR; /* (7) */

    DMA1_Channel4->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel4->CPAR = (uint32_t) &(SPI2->DR); /* (1) */
    DMA1_Channel4->CMAR = (uint32_t) &spi_flash_rx_buff_target; /* (2) */
    DMA1_Channel4->CNDTR = 1; /* (3) */
    DMA1_Channel4->CCR = DMA_CCR_MINC | DMA_CCR_TCIE | DMA_CCR_EN; /* (4) */
    DMA1_Channel3->CCR |= DMA_CCR_EN;

    while (!(CS_INPUT_TARGET));
    }

uint8_t spi_target_read_status_register()
    {
    DMA1_Channel4->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel3->CCR &= ~ DMA_CCR_EN;

    spi_flash_tx_buff_target[0] = 0x05;			//Memory read request

    CS_LOW_TARGET
    ;		//Start transfer

    /******Send write enable CMD********/
    spiTargetFlags.read_status_register = 1;
    DMA1_Channel3->CPAR = (uint32_t) &(SPI2->DR); /* (5) */
    DMA1_Channel3->CMAR = (uint32_t) &spi_flash_tx_buff_target; /* (6) */
    DMA1_Channel3->CNDTR = 2; /* Data size */
    DMA1_Channel3->CCR = DMA_CCR_MINC | DMA_CCR_DIR; /* (7) */

    DMA1_Channel4->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel4->CPAR = (uint32_t) &(SPI2->DR); /* (1) */
    DMA1_Channel4->CMAR = (uint32_t) &spi_flash_rx_buff_target; /* (2) */
    DMA1_Channel4->CNDTR = 2; /* (3) */
    DMA1_Channel4->CCR = DMA_CCR_MINC | DMA_CCR_TCIE | DMA_CCR_EN; /* (4) */
    DMA1_Channel3->CCR |= DMA_CCR_EN;

    while (!(CS_INPUT_TARGET));
    return spi_flash_rx_buff_target[1];
    }

void spi_target_read_sector(uint32_t address, uint32_t size, uint8_t *data)
    {

    DMA1_Channel4->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel3->CCR &= ~ DMA_CCR_EN;

    spi_flash_tx_buff_target[0] = 0x0B;			//Fast read 0b
    spi_flash_tx_buff_target[3] = address;		//Read from address
    spi_flash_tx_buff_target[2] = address >> 8;
    spi_flash_tx_buff_target[1] = address >> 16;
    spi_flash_tx_buff_target[4] = 0;		//Dummy byte

    CS_LOW_TARGET
    ;

    /******Send address********/
    spiTargetFlags.address = 1;
    DMA1_Channel3->CPAR = (uint32_t) &(SPI2->DR); /* (5) */
    DMA1_Channel3->CMAR = (uint32_t) &spi_flash_tx_buff_target; /* (6) */
    DMA1_Channel3->CNDTR = 5; /* Data size */
    DMA1_Channel3->CCR = DMA_CCR_MINC | DMA_CCR_DIR; /* (7) */

    DMA1_Channel4->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel4->CPAR = (uint32_t) &(SPI2->DR); /* (1) */
    DMA1_Channel4->CMAR = (uint32_t) &spi_flash_rx_buff_target; /* (2) */
    DMA1_Channel4->CNDTR = 5; /* (3) */
    DMA1_Channel4->CCR = DMA_CCR_MINC | DMA_CCR_TCIE | DMA_CCR_EN; /* (4) */
    DMA1_Channel3->CCR |= DMA_CCR_EN;
    /******Send address********/

    while (spiTargetFlags.address); //wait for transfer finish

    DMA1_Channel4->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel3->CCR &= ~ DMA_CCR_EN;
    spi_flash_tx_buff_target[0] = 0x00;

    spiTargetFlags.read_request = 1;

    DMA1_Channel3->CPAR = (uint32_t) &(SPI2->DR); /* (5) */
    DMA1_Channel3->CMAR = (uint32_t) &spi_flash_tx_buff_target; /* (6) */
    DMA1_Channel3->CNDTR = size; /* Data size */
    DMA1_Channel3->CCR = DMA_CCR_DIR; /* (7) */

    DMA1_Channel4->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel4->CPAR = (uint32_t) &(SPI2->DR); /* (1) */
    DMA1_Channel4->CMAR = (uint32_t) data; /* (2) */
    DMA1_Channel4->CNDTR = size; /* (3) */
    DMA1_Channel4->CCR = DMA_CCR_MINC | DMA_CCR_TCIE | DMA_CCR_EN; /* (4) */
    DMA1_Channel3->CCR |= DMA_CCR_EN;

    /******Wait for transmition complete********/
    while (!(CS_INPUT_TARGET));
    }

void spi_target_page_program(uint32_t address, uint32_t size, uint8_t *data)
    {

    DMA1_Channel4->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel3->CCR &= ~ DMA_CCR_EN;

    spi_flash_tx_buff_target[0] = 0x02;			//Fast read
    spi_flash_tx_buff_target[3] = address;		//Read from address
    spi_flash_tx_buff_target[2] = address >> 8;
    spi_flash_tx_buff_target[1] = address >> 16;

    CS_LOW_TARGET
    ;

    /******Send address********/
    spiTargetFlags.address = 1;
    DMA1_Channel3->CPAR = (uint32_t) &(SPI2->DR); /* (5) */
    DMA1_Channel3->CMAR = (uint32_t) &spi_flash_tx_buff_target; /* (6) */
    DMA1_Channel3->CNDTR = 4; /* Data size */
    DMA1_Channel3->CCR = DMA_CCR_MINC | DMA_CCR_DIR; /* (7) */

    DMA1_Channel4->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel4->CPAR = (uint32_t) &(SPI2->DR); /* (1) */
    DMA1_Channel4->CMAR = (uint32_t) &spi_flash_rx_buff_target; /* (2) */
    DMA1_Channel4->CNDTR = 4; /* (3) */
    DMA1_Channel4->CCR = DMA_CCR_MINC | DMA_CCR_TCIE | DMA_CCR_EN; /* (4) */
    DMA1_Channel3->CCR |= DMA_CCR_EN;
    /******Send address********/

    while (spiTargetFlags.address); //wait for transfer finish

    DMA1_Channel4->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel3->CCR &= ~ DMA_CCR_EN;
    //spi_flash_tx_buff_target[0] = 0x00;

    spiTargetFlags.page_program = 1;

    DMA1_Channel3->CPAR = (uint32_t) &(SPI2->DR); /* (5) */
    DMA1_Channel3->CMAR = (uint32_t) data; /* (6) */
    DMA1_Channel3->CNDTR = size; /* Data size */
    DMA1_Channel3->CCR = DMA_CCR_MINC | DMA_CCR_DIR; /* (7) */

    DMA1_Channel4->CPAR = (uint32_t) &(SPI2->DR); /* (1) */
    DMA1_Channel4->CMAR = (uint32_t) spi_flash_tx_buff_target; /* (2) */
    DMA1_Channel4->CNDTR = size; /* (3) */

    DMA1_Channel4->CCR = DMA_CCR_TCIE | DMA_CCR_EN; /* (4) */
    DMA1_Channel3->CCR |= DMA_CCR_EN;

    /******Wait for transmition complete********/
    while (!(CS_INPUT_TARGET));
    }

void spi_flash_target_irq_handler()
    {
    if (DMA1->ISR & DMA_ISR_TCIF4)
	{
	DMA1->IFCR |= DMA_IFCR_CTCIF4; /* Clear TC flag */
	if (spiTargetFlags.page_program == 1)
	    {
	    spiTargetFlags.page_program = 0;
	    CS_HIGH_TARGET
	    ;
	    }
	if (spi_flash_tx_buff_target[0] == 0x9f)
	    {
	    spi_flash_tx_buff_target[0] = 0x0;
	    CS_HIGH_TARGET
	    ;
	    }
	if (spiTargetFlags.wren == 1)
	    {
	    spiTargetFlags.wren = 0;
	    CS_HIGH_TARGET
	    ;
	    }
	if (spiTargetFlags.read_status_register == 1)
	    {
	    spiTargetFlags.read_status_register = 0;
	    CS_HIGH_TARGET
	    ;
	    }
	if (spiTargetFlags.address == 1)
	    {
	    spiTargetFlags.address = 0;
	    }
	if (spiTargetFlags.read_request == 1)
	    {
	    spiTargetFlags.read_request = 0;
	    CS_HIGH_TARGET
	    ;
	    }

	}
    }

void spi_flash_target_chip_erase()
    {
    spi_target_write_cmd(0x06); //Write Enable
    spi_target_write_cmd(0x60); //Chip erase
    }

uint32_t spi_flash_target_get_crc()
    {
    //This function will calculate CRC only for size of the source!
    uint32_t crc32;
    for (uint16_t i = 0; i < flashSource.number_of_sectors; i++)
	{
	spi_target_read_sector(i * flashTarget.sector_size, flashTarget.sector_size, flashTarget.sector1);
	for (uint16_t j = 0; j < flashTarget.sector_size / 4; j++)
	    {
	    CRC->DR = __REV(*(uint32_t*) (uintptr_t) &flashTarget.sector1[j * 4]);
	    }
	if ((i * 100) / flashSource.number_of_sectors > progressvar.progress_write)//If progress changed, refresh screen
	    {
	    progressvar.progress_write = (i * 100) / flashSource.number_of_sectors;
	    gui_progress_current(progressvar.progress_write);
	    gui_progress_total(66 + (progressvar.progress_write / 3));
	    }
	}
    crc32 = CRC->DR;
    CRC->CR |= CRC_CR_RESET;
    return crc32;
    }

