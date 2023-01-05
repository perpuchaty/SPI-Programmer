#ifndef TEST2_CONTROL_H
#define TEST2_CONTROL_H

#include "config.h"
#include "commands.h"
#include "stm32g431xx.h"

// <editor-fold desc="Functions">
#define TFT_CS_RESET GPIOB->BSRR|=GPIO_BSRR_BR_4
#define TFT_CS_SET GPIOB->BSRR|=GPIO_BSRR_BS_4

#define TFT_DC_RESET GPIOB->BSRR|=GPIO_BSRR_BR_0
#define TFT_DC_SET GPIOB->BSRR|=GPIO_BSRR_BS_0

#define TFT_RST_RESET GPIOB->BSRR|=GPIO_BSRR_BR_6
#define TFT_RST_SET GPIOB->BSRR|=GPIO_BSRR_BS_6
// </editor-fold>

#endif //TEST2_CONTROL_H
