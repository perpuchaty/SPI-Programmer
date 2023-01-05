/*
 * system.h
 *
 *  Created on: 1 wrz 2022
 *      Author: Sowa
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

uint8_t source_data;
uint16_t total_time_div, total_time;
uint16_t progress_write_cnt;
uint32_t checksum_usb;
uint32_t crc32;

uint8_t filename[16];
//uint8_t filenamelenght;
volatile typedef struct {
   uint16_t  progress_timeout;
   uint16_t  progress_timeout_cnt;
   uint16_t  progress_write;
   uint32_t programming_time;
   uint32_t counter;

} progressVars;
progressVars progressvar;

uint32_t loop_counter, tmp_cnt_ver_fail, tmp_cnt_ver_ok;

uint32_t chk;
uint32_t checksum, checksum2;

void system_process();
void system_clean();
void system_chips_id();
uint32_t system_get_checksum();
void system_set_checksum(uint32_t crc, const uint8_t *filename);


#endif /* SYSTEM_H_ */
