/*
 * delay.h
 *
 *  Created on: 3 gru 2020
 *      Author: bga-service
 */

#ifndef DELAY_H_
#define DELAY_H_

#include "stm32g431xx.h"

uint32_t delayvalue;

void delayms(uint32_t n);
void delay();

#endif /* DELAY_H_ */
