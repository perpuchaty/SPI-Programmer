#include <stdint.h>
#include "spi_flash.h"
#include "stm32g431xx.h"

    //    DMA1_Channel1-SPI1-TX SOURCE
    //    DMA1_Channel2-SPI1-RX SOURCE

uint16_t spi_flash_read_id()
    {

    DMA1_Channel1->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel2->CCR &= ~ DMA_CCR_EN;

    spi_flash_tx_buff[0] = 0x9F;
    for (uint8_t i = 0; i < 10; i++)
	{
	spi_flash_rx_buff[i] = 0;
	}
    CS_LOW
    ;

    DMA1_Channel1->CPAR = (uint32_t) &(SPI1->DR); /* (5) */
    DMA1_Channel1->CMAR = (uint32_t) &spi_flash_tx_buff; /* (6) */
    DMA1_Channel1->CNDTR = 4; /* Data size */
    DMA1_Channel1->CCR = DMA_CCR_MINC | DMA_CCR_DIR; /* (7) */

    DMA1_Channel2->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel2->CPAR = (uint32_t) &(SPI1->DR); /* (1) */
    DMA1_Channel2->CMAR = (uint32_t) &spi_flash_rx_buff; /* (2) */
    DMA1_Channel2->CNDTR = 4; /* (3) */
    DMA1_Channel2->CCR = DMA_CCR_MINC | DMA_CCR_TCIE | DMA_CCR_EN; /* (4) */
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    while (!(CS_INPUT));
    uint16_t ret = (spi_flash_rx_buff[1] << 8) + spi_flash_rx_buff[3];
    return ret;
    }

void spi_read_sector(uint32_t address, uint32_t size, uint8_t *data)
    {
    spiFlags.read_busy = 1;
    DMA1_Channel1->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel2->CCR &= ~ DMA_CCR_EN;

    spi_flash_tx_buff[0] = 0x03;			//Memory read request
    spi_flash_tx_buff[3] = address;		//Read from address
    spi_flash_tx_buff[2] = address >> 8;
    spi_flash_tx_buff[1] = address >> 16;

    CS_LOW
    ;

    /******Send address********/
    spiFlags.address = 1;
    DMA1_Channel1->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel1->CPAR = (uint32_t) &(SPI1->DR); /* (5) */
    DMA1_Channel1->CMAR = (uint32_t) &spi_flash_tx_buff; /* (6) */
    DMA1_Channel1->CNDTR = 4; /* Data size */
    DMA1_Channel1->CCR = DMA_CCR_MINC | DMA_CCR_DIR; /* (7) */

    DMA1_Channel2->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel2->CPAR = (uint32_t) &(SPI1->DR); /* (1) */
    DMA1_Channel2->CMAR = (uint32_t) data; /* (2) */
    DMA1_Channel2->CNDTR = 4; /* (3) */
    DMA1_Channel2->CCR = DMA_CCR_MINC | DMA_CCR_EN | DMA_CCR_TCIE; /* (4) */
    DMA1_Channel1->CCR |= DMA_CCR_EN;
    /******Send address********/

    while (spiFlags.address); //wait for transfer finish

    DMA1_Channel1->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel2->CCR &= ~ DMA_CCR_EN;
    spi_flash_tx_buff[0] = 0x00;

    spiFlags.read_request = 1;

    /******Send dummy********/
    DMA1_Channel1->CPAR = (uint32_t) &(SPI1->DR); /* (5) */
    DMA1_Channel1->CMAR = (uint32_t) &spi_flash_tx_buff; /* (6) */
    DMA1_Channel1->CNDTR = size; /* Data size */
    DMA1_Channel1->CCR = DMA_CCR_DIR; /* (7) */
    /******Send dummy********/

    /******Recivie data********/

    DMA1_Channel2->CPAR = (uint32_t) &(SPI1->DR); /* (1) */
    DMA1_Channel2->CMAR = (uint32_t) data; /* (2) */
    DMA1_Channel2->CNDTR = size; /* (3) */
    DMA1_Channel2->CCR |= DMA_CCR_MINC | DMA_CCR_TCIE | DMA_CCR_EN; /* (4) */
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    /******Wait for transmition complete********/
//    while (!(CS_INPUT));
    //    spiFlags.read_busy = 0; when reading is complete
    }

void spi_write_cmd(uint8_t cmd)
    {
    spi_flash_tx_buff[0] = cmd;			//Memory read request

    CS_LOW
    ;		//Start transfer

    /******Send write enable CMD********/
    spiFlags.wren = 1;
    DMA1_Channel1->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel1->CPAR = (uint32_t) &(SPI1->DR); /* (5) */
    DMA1_Channel1->CMAR = (uint32_t) &spi_flash_tx_buff; /* (6) */
    DMA1_Channel1->CNDTR = 1; /* Data size */
    DMA1_Channel1->CCR |= DMA_CCR_MINC | DMA_CCR_DIR; /* (7) */

    DMA1_Channel2->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel2->CPAR = (uint32_t) &(SPI1->DR); /* (1) */
    DMA1_Channel2->CMAR = (uint32_t) spi_flash_rx_buff; /* (2) */
    DMA1_Channel2->CNDTR = 1; /* (3) */
    DMA1_Channel2->CCR |= DMA_CCR_MINC | DMA_CCR_TCIE | DMA_CCR_EN; /* (4) */
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    while (!(CS_INPUT));
    }

uint8_t spi_read_status_register()
    {
    spi_flash_tx_buff[0] = 0x05;			//Memory read request

    CS_LOW
    ;		//Start transfer

    /******Send write enable CMD********/
    spiFlags.read_status_register = 1;
    DMA1_Channel1->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel1->CPAR = (uint32_t) &(SPI1->DR); /* (5) */
    DMA1_Channel1->CMAR = (uint32_t) &spi_flash_tx_buff; /* (6) */
    DMA1_Channel1->CNDTR = 2; /* Data size */
    DMA1_Channel1->CCR |= DMA_CCR_MINC | DMA_CCR_DIR; /* (7) */

    DMA1_Channel2->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel2->CPAR = (uint32_t) &(SPI1->DR); /* (1) */
    DMA1_Channel2->CMAR = (uint32_t) spi_flash_rx_buff; /* (2) */
    DMA1_Channel2->CNDTR = 2; /* (3) */
    DMA1_Channel2->CCR |= DMA_CCR_MINC | DMA_CCR_TCIE | DMA_CCR_EN; /* (4) */
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    while (!(CS_INPUT));
    return spi_flash_rx_buff[1];
    }

uint8_t spi_write_status_register(uint8_t value)
    {
    spi_flash_tx_buff[0] = 0x01;			//Memory read request
    spi_flash_tx_buff[1] = value;			//Memory read request

    GPIOA->BSRR |= GPIO_BSRR_BR_8;		//Start transfer

    /******Send write enable CMD********/
    spiFlags.write_status_register = 1;
    DMA1_Channel1->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel1->CPAR = (uint32_t) &(SPI1->DR); /* (5) */
    DMA1_Channel1->CMAR = (uint32_t) &spi_flash_tx_buff; /* (6) */
    DMA1_Channel1->CNDTR = 2; /* Data size */
    DMA1_Channel1->CCR |= DMA_CCR_MINC | DMA_CCR_DIR; /* (7) */

    DMA1_Channel2->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel2->CPAR = (uint32_t) &(SPI1->DR); /* (1) */
    DMA1_Channel2->CMAR = (uint32_t) spi_flash_rx_buff; /* (2) */
    DMA1_Channel2->CNDTR = 2; /* (3) */
    DMA1_Channel2->CCR |= DMA_CCR_MINC | DMA_CCR_TCIE | DMA_CCR_EN; /* (4) */
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    while (!(GPIOA->IDR & GPIO_IDR_IDR_8));
    return spi_flash_rx_buff[1];
    }

void spi_write_byte(uint8_t value, uint32_t address)
    {
    spi_flash_tx_buff[0] = 0x02;			//Byte program
    spi_flash_tx_buff[3] = address;		//Read from address
    spi_flash_tx_buff[2] = address >> 8;
    spi_flash_tx_buff[1] = address >> 16;
    spi_flash_tx_buff[4] = value;

    GPIOA->BSRR |= GPIO_BSRR_BR_8;		//Start transfer

    /******Send write enable CMD********/
    spiFlags.write_byte = 1;
    DMA1_Channel1->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel1->CPAR = (uint32_t) &(SPI1->DR); /* (5) */
    DMA1_Channel1->CMAR = (uint32_t) &spi_flash_tx_buff; /* (6) */
    DMA1_Channel1->CNDTR = 5; /* Data size */
    DMA1_Channel1->CCR |= DMA_CCR_MINC | DMA_CCR_DIR; /* (7) */

    DMA1_Channel2->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel2->CPAR = (uint32_t) &(SPI1->DR); /* (1) */
    DMA1_Channel2->CMAR = (uint32_t) spi_flash_rx_buff; /* (2) */
    DMA1_Channel2->CNDTR = 5; /* (3) */
    DMA1_Channel2->CCR |= DMA_CCR_MINC | DMA_CCR_TCIE | DMA_CCR_EN; /* (4) */
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    while (!(GPIOA->IDR & GPIO_IDR_IDR_8));
    }

void spi_write_aai(uint32_t address, uint8_t *data, uint8_t size)
    {
    spi_flash_tx_buff[0] = 0xAF;
    spi_flash_tx_buff[3] = address;
    spi_flash_tx_buff[2] = address >> 8;
    spi_flash_tx_buff[1] = address >> 16;
    spi_flash_tx_buff[4] = *data++;

    GPIOA->BSRR |= GPIO_BSRR_BR_8;		//Start transfer

    /******Send write enable CMD********/
    spiFlags.write_aai_address = 1;
    DMA1_Channel1->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel1->CPAR = (uint32_t) &(SPI1->DR); /* (5) */
    DMA1_Channel1->CMAR = (uint32_t) &spi_flash_tx_buff; /* (6) */
    DMA1_Channel1->CNDTR = 5; /* Data size */
    DMA1_Channel1->CCR |= DMA_CCR_MINC | DMA_CCR_DIR; /* (7) */

    DMA1_Channel2->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel2->CPAR = (uint32_t) &(SPI1->DR); /* (1) */
    DMA1_Channel2->CMAR = (uint32_t) spi_flash_rx_buff; /* (2) */
    DMA1_Channel2->CNDTR = 5; /* (3) */
    DMA1_Channel2->CCR |= DMA_CCR_MINC | DMA_CCR_TCIE | DMA_CCR_EN; /* (4) */
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    while (!(GPIOA->IDR & GPIO_IDR_IDR_8));
    for (uint16_t i = 0; i < size - 1; i++)
	{
	while (spi_read_status_register() & 1);

	spiFlags.write_aai_data = 1;
	spi_flash_tx_buff[0] = 0xAF;
	spi_flash_tx_buff[1] = *data++;
	GPIOA->BSRR |= GPIO_BSRR_BR_8;		//Start transfer

	DMA1_Channel1->CCR &= ~ DMA_CCR_EN;
	DMA1_Channel1->CPAR = (uint32_t) &(SPI1->DR); /* (5) */
	DMA1_Channel1->CMAR = (uint32_t) &spi_flash_tx_buff; /* (6) */
	DMA1_Channel1->CNDTR = 2; /* Data size */
	DMA1_Channel1->CCR |= DMA_CCR_MINC | DMA_CCR_DIR; /* (7) */

	DMA1_Channel2->CCR &= ~ DMA_CCR_EN;
	DMA1_Channel2->CPAR = (uint32_t) &(SPI1->DR); /* (1) */
	DMA1_Channel2->CMAR = (uint32_t) spi_flash_rx_buff; /* (2) */
	DMA1_Channel2->CNDTR = 2; /* (3) */
	DMA1_Channel2->CCR |= DMA_CCR_MINC | DMA_CCR_TCIE | DMA_CCR_EN; /* (4) */
	DMA1_Channel1->CCR |= DMA_CCR_EN;

	while (!(GPIOA->IDR & GPIO_IDR_IDR_8));
	}
    spi_write_cmd(0x04);
    }

void spi_flash_irq_handler()
    {
    if (DMA1->ISR & DMA_ISR_TCIF2)
	{
	DMA1->IFCR |= DMA_IFCR_CTCIF2; /* Clear TC flag */
	if (spi_flash_tx_buff[0] == 0x9f)
	    {
	    spi_flash_tx_buff[0] = 0x0;
	    CS_HIGH
	    ;
	    }
	if (spiFlags.address == 1)
	    {
	    spiFlags.address = 0;
	    }
	if (spiFlags.page_program == 1)
	    {
	    spiFlags.page_program = 0;
	    CS_HIGH;
	    }
	if (spiFlags.read_request == 1)
	    {
	    CS_HIGH
	    ;
	    spiFlags.read_request = 0;
	    spiFlags.read_busy = 0;
	    }
	if (spiFlags.wren == 1)
	    {
	    spiFlags.wren = 0;
	    CS_HIGH
	    ;
	    }
	if (spiFlags.read_status_register == 1)
	    {
	    spiFlags.read_status_register = 0;
	    CS_HIGH
	    ;
	    }
	if (spiFlags.write_status_register == 1)
	    {
	    spiFlags.write_status_register = 0;
	    CS_HIGH
	    ;
	    }
	if (spiFlags.write_byte == 1)
	    {
	    spiFlags.write_byte = 0;
	    CS_HIGH
	    ;
	    }
	if (spiFlags.write_aai_address == 1)
	    {
	    spiFlags.write_aai_address = 0;
	    CS_HIGH
	    ;

	    }
	if (spiFlags.write_aai_data == 1)
	    {
	    spiFlags.write_aai_data = 0;
	    CS_HIGH
	    ;

	    }
	if (spiFlags.write_aai_data == 1)
	    {
	    spiFlags.adc_read_data = 0;
	    CS_HIGH
	    ;

	    }

	}
    }

uint32_t spi_flash_get_crc()
    {
    while (spiFlags.read_busy); //Check if SPI is not busy
    uint32_t crc32;
    for (uint16_t i = 0; i < flashSource.number_of_sectors; i++)
	{
	spi_read_sector(i * flashSource.sector_size, flashSource.sector_size, flashSource.sector1);
	while (spiFlags.read_busy);	//Wait for spi transfer
	for (uint16_t j = 0; j < flashSource.sector_size / 4; j++)
	    {
	    CRC->DR = __REV(*(uint32_t*) (uintptr_t) &flashSource.sector1[j * 4]);
	    }
	}
    crc32 = CRC->DR;
    CRC->CR |= CRC_CR_RESET;
    return crc32;
    }

void spi_flash_source_chip_erase()
    {
    spi_write_cmd(0x06); //Write Enable
    spi_write_cmd(0x60); //Chip erase
    }

void spi_flash_page_program(uint32_t address, uint32_t size, uint8_t *data)
    {

    DMA1_Channel1->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel2->CCR &= ~ DMA_CCR_EN;

    spi_flash_tx_buff[0] = 0x02;			//Fast read
    spi_flash_tx_buff[3] = address;		//Read from address
    spi_flash_tx_buff[2] = address >> 8;
    spi_flash_tx_buff[1] = address >> 16;

    CS_LOW
    ;

    /******Send address********/
    spiFlags.address = 1;
    DMA1_Channel1->CPAR = (uint32_t) &(SPI1-> DR); /* (5) */
    DMA1_Channel1->CMAR = (uint32_t) &spi_flash_tx_buff; /* (6) */
    DMA1_Channel1->CNDTR = 4; /* Data size */
    DMA1_Channel1->CCR = DMA_CCR_MINC | DMA_CCR_DIR; /* (7) */

    DMA1_Channel2->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel2->CPAR = (uint32_t) &(SPI1->DR); /* (1) */
    DMA1_Channel2->CMAR = (uint32_t) &spi_flash_rx_buff; /* (2) */
    DMA1_Channel2->CNDTR = 4; /* (3) */
    DMA1_Channel2->CCR = DMA_CCR_MINC | DMA_CCR_TCIE | DMA_CCR_EN; /* (4) */
    DMA1_Channel1->CCR |= DMA_CCR_EN;
    /******Send address********/

    while (spiFlags.address); //wait for transfer finish

    DMA1_Channel2->CCR &= ~ DMA_CCR_EN;
    DMA1_Channel1->CCR &= ~ DMA_CCR_EN;
    //spi_flash_tx_buff_target[0] = 0x00;

    spiFlags.page_program = 1;

    DMA1_Channel1->CPAR = (uint32_t) &(SPI1->DR); /* (5) */
    DMA1_Channel1->CMAR = (uint32_t) data; /* (6) */
    DMA1_Channel1->CNDTR = size; /* Data size */
    DMA1_Channel1->CCR = DMA_CCR_MINC | DMA_CCR_DIR; /* (7) */

    DMA1_Channel2->CPAR = (uint32_t) &(SPI1->DR); /* (1) */
    DMA1_Channel2->CMAR = (uint32_t) spi_flash_tx_buff; /* (2) */
    DMA1_Channel2->CNDTR = size; /* (3) */

    DMA1_Channel2->CCR = DMA_CCR_TCIE | DMA_CCR_EN; /* (4) */
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    /******Wait for transmition complete********/
    while (!(CS_INPUT));
    }

