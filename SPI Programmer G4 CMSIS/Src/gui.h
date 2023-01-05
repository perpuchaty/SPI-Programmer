/*
 * gui.h
 *
 *  Created on: 24 sie 2022
 *      Author: Sowa
 */

#ifndef GUI_H_
#define GUI_H_

void gui_draw_main_screen();
void gui_refresh_main_screen_source();
void gui_refresh_main_screen_target();
void gui_draw_status(uint8_t val);
uint8_t check_flash_id(uint8_t flash_id);
void gui_printf_source_size();
void gui_printf_target_size();
void gui_progress_current(uint8_t value);
void gui_progress_total(uint8_t value);

uint8_t chip_id[10];
char vendor[sizeof(chip_id)][10];

#endif /* GUI_H_ */
