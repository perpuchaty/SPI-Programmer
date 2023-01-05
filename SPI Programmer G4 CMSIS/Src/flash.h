/*
 * flash.h
 *
 *  Created on: 6 gru 2022
 *      Author: Sowa
 */
#include "stm32g431xx.h"

#ifndef FLASH_H_
#define FLASH_H_

uint64_t flash_buffer;

#define FLASH_BASE_ADDRESS 0x0801F800
#define FLASH_FILE_NAME_ADDRESS 0x0801F808
#define FLASH_SECTOR 63

uint8_t flash_erase_sector(uint8_t sector_number);
void flash_unlock();
void flash_lock();
void flash_program(uint32_t Address, uint64_t Data);

#endif /* FLASH_H_ */
