/*
 * flash.c
 *
 *  Created on: 6 gru 2022
 *      Author: Sowa
 */

#include "stm32g431xx.h"
#include "flash.h"

uint8_t flash_erase_sector(uint8_t sector_number)
    {

    while (FLASH->SR & FLASH_SR_BSY);
    FLASH->CR &= ~FLASH_CR_PG;		 //Disable programming
    FLASH->SR = 0xFF; 			//Erase all errors
    FLASH->CR |= sector_number << FLASH_CR_PNB_Pos | FLASH_CR_PER; //Enable page erase, set page number
    FLASH->CR |= FLASH_CR_STRT;	//Start erase
    while (FLASH->SR & FLASH_SR_BSY);	//Wait until ready
    return 0;
    }

void flash_unlock()
    {
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xCDEF89AB;
    }

void flash_lock()
    {
    FLASH->CR |= FLASH_CR_LOCK;
    }

void flash_program(uint32_t Address, uint64_t Data)
    {
    FLASH->CR &= ~FLASH_CR_PER; //Disable page erase
    FLASH->CR |= FLASH_CR_PG | FLASH_CR_EOPIE; //Flash programming, interrupt enable
    /* Program first word */
    *(uint32_t*) Address = (uint32_t) Data;
    /* Barrier to ensure programming is performed in 2 steps, in right order
     (independently of compiler optimization behavior) */
    __ISB();
    /* Program second word */
    *(uint32_t*) (Address + 4U) = (uint32_t) (Data >> 32U);
    while(FLASH->SR & FLASH_SR_BSY);//Wait for operation finish
    FLASH->SR|=FLASH_SR_EOP; //Erase end of operation flag
    }
