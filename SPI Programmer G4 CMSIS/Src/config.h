/*
 * config.h
 *
 *  Created on: 11 sie 2022
 *      Author: Sowa
 */

#ifndef CONFIG_H_
#define CONFIG_H_

void config_cpu();
void	config_clock();
void	config_gpio();
void	config_uart();
void	config_exti();
void	config_dma();
void	config_spi();

volatile uint16_t progress;

volatile typedef struct {
   uint8_t unknownTargetID;
   uint8_t unknownSourceID;
   uint8_t generalError;
   uint8_t verifyError;
   uint8_t checksumCheck;

} errorFlags;
errorFlags errors;

enum chip_status
{
    STATUS_CHIP_ERASE,
    STATUS_IDLE,
    STATUS_ERROR,
    STATUS_CHIP_ERASE_TIMEOUT,
    STATUS_CHIP_PROGRAMMING,
    STATUS_CHIP_VERIFY,
    STATUS_CHIP_VERIFY_PASSED,
    STATUS_CHIP_VERIFY_FAILED,
    STATUS_LOADING,
    STATUS_TARGET_CRC_FAIL,
    STATUS_FLASH_SIZE_INCORECT

};

volatile typedef struct {
   uint8_t erase_in_progress;
   uint8_t erase_timeout;
   uint8_t operation_in_progress;
   uint8_t uart_mode;
   uint8_t update_main_screen;
   uint8_t button_flag;
   uint8_t start_triggered;
   uint8_t chip_read;
   uint8_t wait_for_checksum;
   uint8_t wait_for_filename;

} progressFlags;
progressFlags progress_flag;

volatile typedef struct {
   uint32_t usb_crc_pass;
   uint32_t usb_crc_fail;
   uint32_t usb_bytes_recivied;

} debug;
debug dbg;

uint8_t colors;

#endif /* CONFIG_H_ */
