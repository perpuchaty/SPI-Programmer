/*
 * system.c
 *
 *  Created on: 1 wrz 2022
 *      Author: Sowa
 */

#include <stdint.h>
#include <math.h>
#include <stdio.h>

#include "stm32g431xx.h"
#include "delay.h"
#include "config.h"

#include "ILI9341/config.h"
#include "ILI9341/graph.h"
#include "ILI9341/text.h"
#include "ILI9341/core.h"
#include "ILI9341/fonts/glcdfont.h"

#include "spi_flash.h"
#include "spi_flash_target.h"

#include "gui.h"

#include "system.h"
#include "math.h"
#include "flash.h"
void system_process()
    {
    gui_draw_status(STATUS_CHIP_ERASE);	//Draw status on screen
    progress_flag.erase_in_progress = 1;	//Set flag
    progress_flag.operation_in_progress = 1;	//Set flag
    spi_flash_target_chip_erase();//Send chip erase command (0x06,0x60 - this is good for winbond flash)

    //Pool status register (0x05) and if it is 0, then flash is erased. Register is pooled every 400ms
    while (progress_flag.erase_in_progress)
	{
	if (spi_target_read_status_register() == 0) //chip is erased because status register is 0
	    {
	    progress_flag.erase_in_progress = 0;
	    }
	else
	    {
	    progress_flag.erase_in_progress = 1; //Chip erase is in progress
	    }
	if (progressvar.progress_timeout > flashTarget.number_of_blocks / 2) //progress_timeout is increased every second, so timeout is number of blocks (for 8mbyte is 128) /2.
	    {
	    progress_flag.erase_in_progress = 0;
	    progress_flag.erase_timeout = 1;
	    }
	gui_progress_current(progressvar.progress_timeout * 2); //Progress bar current, when reach 100% then there is timeout
	gui_progress_total((2 * progressvar.progress_timeout) / 3); //Progress bar total
	delayms(400);
	}
    ;
    /*Chip erase*/

    if (progress_flag.erase_timeout == 1)
	{
	gui_draw_status(STATUS_CHIP_ERASE_TIMEOUT);
	}

    else
	{
	gui_progress_current(0);
	gui_progress_total(25);
	gui_draw_status(STATUS_CHIP_PROGRAMMING);

	/*Chip programming*/

	progressvar.counter = 0;
	source_data = 1;

	//Pre-fill first buffer
	spi_read_sector(0, flashTarget.page_size, flashSource.sector1);
	while (spiFlags.read_busy); //Check if SPI is not busy

	progressvar.progress_write = (progressvar.counter * 100) / flashTarget.number_of_pages;

	//We have 2 buffers. When buffer 2 is filling by DMA then buffer 1 is written to the target, and other way around.
	for (progressvar.counter = 0;
		progressvar.counter < flashTarget.number_of_pages;
		progressvar.counter++) //Loop for each sector
	    {
	    if (source_data == 1)		//Buffer 1 is selected
		{
		while (spiFlags.read_busy); //Buffer is not ready yet
		spi_read_sector(flashTarget.page_size * (progressvar.counter + 1), 256, flashSource.sector2); //Read buffer 2
		spi_target_write_cmd(0x06); 		//Write Enable
		spi_target_page_program(flashTarget.page_size * progressvar.counter, 256, flashSource.sector1); //Program from buffer 1
		while (spi_target_read_status_register()); //Pool status register until it is 0
		source_data = 2; //Select buffer 2 as source for next block programming
		}
	    else
		{
		while (spiFlags.read_busy); //Buffer is not ready yet
		spi_read_sector(flashTarget.page_size * (progressvar.counter + 1), 256, flashSource.sector1); //Read buffer 1
		spi_target_write_cmd(0x06); 		//Write Enable
		spi_target_page_program(flashTarget.page_size * progressvar.counter, 256, flashSource.sector2); //Program from buffer 2
		while (spi_target_read_status_register()); //Pool status register until it is 0
		source_data = 1; //Select buffer 1 as source for next block programming
		}
	    if ((progressvar.counter * 100) / flashTarget.number_of_pages > progressvar.progress_write) //Update LCD only when progress changed
		{
		progressvar.progress_write = (progressvar.counter * 100) / flashTarget.number_of_pages; //Update progress with new value
		gui_progress_current(progressvar.progress_write); //Show new current value
		gui_progress_total(33 + (progressvar.progress_write / 3)); //Show new total value
		}

	    }
	/*Chip programming*/

	/*Chip verify*/
	gui_draw_status(STATUS_CHIP_VERIFY);

	progressvar.progress_write = 0;
	if (spi_flash_target_get_crc() == crc32)
	    {
	    gui_draw_status(STATUS_CHIP_VERIFY_PASSED);
	    tmp_cnt_ver_ok++;
	    }
	else
	    {
	    gui_draw_status(STATUS_CHIP_VERIFY_FAILED);
	    tmp_cnt_ver_fail++;

	    }
	}
    progress_flag.start_triggered = 0;

    //Some debug info
    char intbuffer[16];
    itoa((int) total_time, intbuffer, 10);
    LCD_setCursor(200, 225);
    LCD_writeString("Time:", arial_12);
    LCD_writeString(intbuffer, arial_12);
    LCD_writeString(" Fail:", arial_12);
    itoa((int) tmp_cnt_ver_fail, intbuffer, 10);
    LCD_writeString(intbuffer, arial_12);
    LCD_writeString(" Ok:", arial_12);
    itoa((int) tmp_cnt_ver_ok, intbuffer, 10);
    LCD_writeString(intbuffer, arial_12);
    system_clean();
    }

void system_chips_id()
//This function read device id from both chips. If DevID changed from last read, it will set update progress_flag.update_main_screen.
    {
    flashSource.id = spi_flash_read_id();
    flashTarget.id = spi_flash_target_read_id();
    if (flashSource.id != flashSource.id_tmp) //Source flash ID changed, new flash detected
	{
	progress_flag.update_main_screen = 1;   //Main screen update request
	flashSource.id_tmp = flashSource.id;
	flashSource.page_size = 256;
	flashSource.number_of_sectors_per_block = 16;
	flashSource.sector_size = 0x1000;
	uint8_t tmp = flashSource.id;
	flashSource.number_of_blocks = (pow(2, tmp)) / (flashSource.sector_size * flashSource.number_of_sectors_per_block);
	flashSource.number_of_sectors = flashSource.number_of_blocks * flashSource.number_of_sectors_per_block;
	flashSource.number_of_pages = (((flashSource.sector_size / flashSource.page_size) * flashSource.number_of_sectors_per_block)) * flashSource.number_of_blocks;

	}
    if (flashTarget.id != flashTarget.id_tmp) //Source flash ID changed, new flash detected
	{
	progress_flag.update_main_screen = 1;   //Main screen update request
	flashTarget.id_tmp = flashTarget.id;
	flashTarget.page_size = 256;
	flashTarget.number_of_sectors_per_block = 16;
	flashTarget.sector_size = 0x1000;
	uint8_t tmp = flashTarget.id;
	flashTarget.number_of_blocks = (pow(2, tmp)) / (flashTarget.sector_size * flashTarget.number_of_sectors_per_block);
	flashTarget.number_of_sectors = flashTarget.number_of_blocks * flashTarget.number_of_sectors_per_block;
	flashTarget.number_of_pages = (((flashTarget.sector_size / flashTarget.page_size) * flashTarget.number_of_sectors_per_block)) * flashTarget.number_of_blocks;
	}

    if (check_flash_id(flashSource.id >> 8))
	{
	errors.unknownSourceID = 1;
	}
    else
	{
	errors.unknownSourceID = 0;
	}
    if (check_flash_id(flashTarget.id >> 8))
	{
	errors.unknownTargetID = 1;
	}
    else
	{
	errors.unknownTargetID = 0;
	}
    }

void system_clean()
    {

    //Clean all vars so we can program chip again
    errors.verifyError = 0;
    progressvar.progress_timeout = 0;
    progressvar.progress_timeout_cnt = 0;
    progressvar.counter = 0;
    progressvar.progress_write = 0;
    total_time = 0;

    }

void system_set_checksum(uint32_t crc, const uint8_t *filename)
    {
    flash_unlock();
    flash_erase_sector(FLASH_SECTOR);
    flash_program(FLASH_BASE_ADDRESS, (uint64_t) crc); //Program CRC32
    /* Program filename */
    uint64_t nametoflash[2] =
   	{
   	0, 0
   	};
       for (uint8_t i = 0; i < 2; i++)
   	{
   	for (uint8_t j = 0; j < 8; j++)
   	    {
   	    nametoflash[i] = nametoflash[i] + ((uint64_t) *filename << j * 8);
   	    filename++;
   	    }
   	}
       for (uint8_t i = 0; i < 2; i++)
   	{
   	flash_program(FLASH_FILE_NAME_ADDRESS + (8 * i), nametoflash[i]);
   	}
    flash_lock();
    }

void system_set_filename(const uint8_t *s)
    {
    uint64_t nametoflash[2] =
	{
	0, 0
	};
    for (uint8_t i = 0; i < 2; i++)
	{
	for (uint8_t j = 0; j < 8; j++)
	    {
	    nametoflash[i] = nametoflash[i] + ((uint64_t) *s << j * 8);
	    s++;
	    }
	}
    flash_unlock();
    flash_erase_sector(63);
    for (uint8_t i = 0; i < 2; i++)
	{
	flash_program(FLASH_FILE_NAME_ADDRESS + (8 * i), nametoflash[i]);
	}
    flash_lock();
    }

uint32_t system_get_checksum()
    {
    return *(uint32_t*) FLASH_BASE_ADDRESS;
    }

