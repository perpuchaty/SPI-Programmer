/*
 * gui.c
 *
 *  Created on: 24 sie 2022
 *      Author: Sowa
 */

#include "ILI9341/config.h"
#include "ILI9341/graph.h"
#include "ILI9341/text.h"
#include "ILI9341/core.h"
#include "ILI9341/fonts/glcdfont.h"
#include "config.h"
#include "spi_flash.h"
#include "flash.h"
#include "gui.h"
#include "system.h"
#include <stdio.h>
uint8_t chip_id[] =
    {
    0xEF, 0x00, 0xFF
    };
char vendor[sizeof(chip_id)][10] =
    {
    "Winbond", "0x00", "0xFF",

    };

void gui_progress_current(uint8_t value) //105
    {

    char intbuffer[4];
    itoa((int) value, intbuffer, 10);

    LCD_fillRect(102 + (value * 2), 107, (200 - (value * 2)) - 2, 21, BLACK);
    if (value > 98)
	value = 98;
    LCD_fillRect(102, 107, value * 2, 21, GREEN);

    LCD_setCursor(185, 109);
    LCD_writeString(intbuffer, ArialBlack18);
    LCD_writeString("%", ArialBlack18);

    }

void gui_progress_total(uint8_t value) //140
    {

    char intbuffer[4];
    itoa((int) value, intbuffer, 10);

    LCD_fillRect(102 + (value * 2), 142, (200 - (value * 2)) - 2, 21, BLACK);
    if (value > 98)
	value = 98;
    LCD_fillRect(102, 142, value * 2, 21, GREEN);

    LCD_setCursor(185, 144);
    LCD_writeString(intbuffer, ArialBlack18);
    LCD_writeString("%", ArialBlack18);

    }

void gui_draw_main_screen()
    {
    LCD_drawFastHLine(0, 30, 320, ILINE);
    LCD_setCursor(0, 0);

    if (errors.unknownSourceID == 1)
	LCD_setTextColor(RED);
    else
	LCD_setTextColor(WHITE);
    LCD_writeString("Source:", HelveticaNeue25);
    LCD_setTextColor(WHITE);
    LCD_setCursor(0, 35);
    LCD_writeString("MFR:", HelveticaNeue25);
    LCD_setCursor(0, 70);
    LCD_writeString("Size:", HelveticaNeue25);

    LCD_setCursor(160, 0);
    if (errors.unknownTargetID == 1)
	LCD_setTextColor(RED);
    else
	LCD_setTextColor(WHITE);
    LCD_writeString("Target:", HelveticaNeue25);
    LCD_setTextColor(WHITE);
    LCD_setCursor(160, 35);
    LCD_writeString("MFR:", HelveticaNeue25);
    LCD_setCursor(160, 70);
    LCD_writeString("Size:", HelveticaNeue25);

    LCD_drawFastHLine(0, 100, 320, ILINE);

    LCD_setCursor(0, 105);
    LCD_writeString("Current:", HelveticaNeue25);
    LCD_drawRect(100, 105, 201, 25, WHITE);

    LCD_setCursor(0, 140);
    LCD_writeString("Total:", HelveticaNeue25);
    LCD_drawRect(100, 140, 201, 25, WHITE);

    LCD_drawRect(10, 170, 300, 50, WHITE);

    LCD_setCursor(0, 225);
    LCD_writeString("Fileame: ", arial_12);

    uint8_t *ptr = (uint8_t*)FLASH_FILE_NAME_ADDRESS; // cast the memory address to a pointer to uint8_t
     for (uint8_t i=0;i<20;i++)
         {
         LCD_write(*ptr, arial_12);
  	   ptr++;
  	   if(*ptr==0)
  	       {
  	       break;
  	       }
  	}

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

    }

void gui_refresh_main_screen_source()
    {
    //Erase old data
    LCD_fillRect(60, 38, 90, 60, BLACK);

    //Show vendor ID, if unknown go to else
    if (errors.unknownSourceID == 0)
	{
	for (uint8_t i = 0; i < sizeof(chip_id); i++)
	    {
	    if (flashSource.id >> 8 == chip_id[i])
		{
		LCD_setCursor(60, 38);
		LCD_writeString(vendor[i], veribest20_b);
		i = sizeof(chip_id);
		}
	    }
	}
    else
	{
	LCD_setCursor(60, 38);
	LCD_writeString("Unknown", veribest20_b);
	}
    //Show chip size
    gui_printf_source_size();
    }

void gui_refresh_main_screen_target()
    {
    //Erase old data
    LCD_fillRect(220, 38, 90, 60, BLACK);

    //Show vendor ID, if unknown go to else
    if (errors.unknownTargetID == 0)
	{
	for (uint8_t i = 0; i < sizeof(chip_id); i++)
	    {
	    if (flashTarget.id >> 8 == chip_id[i])
		{
		LCD_setCursor(220, 38);
		LCD_writeString(vendor[i], veribest20_b);
		i = sizeof(chip_id);
		}
	    }
	}
    else
	{
	LCD_setCursor(220, 38);
	LCD_writeString("Unknown", veribest20_b);
	}
    //Show chip size
    gui_printf_target_size();
    }

//todo two functions bellow can be mixed into one, just set X cursor in correct position deppend if source or targethas to be updated
void gui_printf_source_size()
    {
    LCD_setCursor(60, 73);
    switch ((uint8_t) flashSource.id)
	{
    case 23:
	LCD_writeString("8Mbyte", veribest20_b);
	break;
    case 24:
	LCD_writeString("16Mbyte", veribest20_b);
	break;

    case 25:
	LCD_writeString("32Mbyte", veribest20_b);
	break;

    case 26:
	LCD_writeString("64Mbyte", veribest20_b);
	break;
    case 27:
	LCD_writeString("128Mbyte", veribest20_b);
	break;
    default:
	LCD_writeString("Unknown", veribest20_b);
	break;
	}
    }

void gui_printf_target_size()
    {
    LCD_setCursor(220, 73);
    switch ((uint8_t) flashTarget.id)
	{
    case 23:
	LCD_writeString("8Mbyte", veribest20_b);
	break;
    case 24:
	LCD_writeString("16Mbyte", veribest20_b);
	break;

    case 25:
	LCD_writeString("32Mbyte", veribest20_b);
	break;

    case 26:
	LCD_writeString("64Mbyte", veribest20_b);
	break;
    case 27:
	LCD_writeString("128Mbyte", veribest20_b);
	break;
    default:
	LCD_writeString("Unknown", veribest20_b);
	break;
	}
    }

uint8_t check_flash_id(uint8_t flash_id)
    {
    uint8_t ret = 1;
    if (flash_id != 0xFF && flash_id != 0x00) //0x00 and 0xFF mean there is no flash in socket, return error
	{
	for (uint8_t i = 0; i < sizeof(chip_id); i++)
	    {
	    if (flash_id == chip_id[i])
		{
		ret = 0;
		}
	    }
	}
    return ret;
    }

void gui_draw_status(uint8_t val)
    {
    //1. Chip erase
    //2. Idle
    LCD_setCursor(140, 170+25-12);
    switch (val)
	{
    case STATUS_CHIP_ERASE:
    LCD_setTextBgColor(YELLOW);
    LCD_setTextColor(BLACK);
    LCD_fillRect(12, 172, 296, 46, YELLOW);
    LCD_writeStringCenter("CHIP ERASE..", ArialBlack18 , 160, 12);
	break;

    case 2: //Idle
    LCD_setTextBgColor(YELLOW);
    LCD_setTextColor(BLACK);
    LCD_fillRect(12, 172, 296, 46, YELLOW);
    LCD_writeStringCenter("Idle", ArialBlack18 , 160, 4);
	break;

    case STATUS_CHIP_ERASE_TIMEOUT:
    LCD_setTextBgColor(RED);
    LCD_setTextColor(BLACK);
    LCD_fillRect(12, 172, 296, 46, RED);
    LCD_writeStringCenter("ERASE TIMEOUT!", ArialBlack18 , 160, 15);
	break;

    case STATUS_CHIP_PROGRAMMING:
    LCD_setTextBgColor(YELLOW);
    LCD_setTextColor(BLACK);
    LCD_fillRect(12, 172, 296, 46, YELLOW);
    LCD_writeStringCenter("Programming", ArialBlack18 , 160, 11);
	break;

    case STATUS_CHIP_VERIFY:
    LCD_setTextBgColor(YELLOW);
    LCD_setTextColor(BLACK);
    LCD_fillRect(12, 172, 296, 46, YELLOW);
    LCD_writeStringCenter("VERYFING", ArialBlack18 , 160, 8);
	break;

    case STATUS_CHIP_VERIFY_PASSED:
    LCD_setTextBgColor(GREEN);
    LCD_setTextColor(BLACK);
    LCD_fillRect(12, 172, 296, 46, GREEN);
    LCD_writeStringCenter("PASSED!", ArialBlack18 , 160, 6);
	break;

    case STATUS_CHIP_VERIFY_FAILED:
    LCD_setTextBgColor(RED);
    LCD_setTextColor(BLACK);
    LCD_fillRect(12, 172, 296, 46, RED);
    LCD_writeStringCenter("VERYFING FAILED", ArialBlack18 , 160, 15);
	break;

    case STATUS_LOADING:
    LCD_setTextBgColor(WHITE);
    LCD_setTextColor(BLACK);
    LCD_fillRect(12, 172, 296, 46, WHITE);
    LCD_writeStringCenter("LOADING", ArialBlack18 , 160, 7);
	break;

    case STATUS_TARGET_CRC_FAIL:
    LCD_setTextBgColor(RED);
    LCD_setTextColor(BLACK);
    LCD_fillRect(12, 172, 296, 46, RED);
    LCD_writeStringCenter("CHECKSUM FAIL", ArialBlack18 , 160, 13);
	break;

    case STATUS_FLASH_SIZE_INCORECT:
    LCD_setTextBgColor(RED);
    LCD_setTextColor(BLACK);
    LCD_fillRect(12, 172, 296, 46, RED);
    LCD_writeStringCenter("FLASH SIZE ERROR", ArialBlack18 , 160, 25);
	break;
	}
 //   LCD_drawRect(10, 170, 300, 50, WHITE);
    LCD_setTextBgColor(BLACK);
    LCD_setTextColor(WHITE);
    }
