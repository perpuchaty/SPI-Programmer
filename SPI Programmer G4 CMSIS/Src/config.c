/*
 * config.c
 *
 *  Created on: 11 sie 2022
 *      Author: Sowa
 */
#include <stdint.h>
#include "stm32g431xx.h"
#include "config.h"

void config_cpu()
    {
    config_clock();
    config_gpio();
    config_uart();
    config_exti();
    config_dma();
    config_spi();
    config_dma();

    SysTick_Config(160000000 / 1000);
    }

void config_gpio()
    {
    //BUTTON
    GPIOA->MODER &= ~GPIO_MODER_MODE0; //BUTTON
    GPIOA->PUPDR |= GPIO_PUPDR_PUPD0_0;	//BUTTON
    while(!(GPIOA->IDR & GPIO_IDR_ID0)); //Stay here until button is relased, for safety after reset
    //LED
    GPIOA->MODER &= ~GPIO_MODER_MODE1_1; //LED

    //LCD
    GPIOB->MODER &= ~GPIO_MODER_MODE0_1; //DC

    GPIOB->MODER &= ~GPIO_MODER_MODE4_1; //CS
    GPIOB->MODER |= GPIO_MODER_MODE4_0; //CS
    GPIOB->PUPDR|=GPIO_PUPDR_PUPD4; //CS

    GPIOB->MODER &= ~GPIO_MODER_MODE6_1; //RST
    GPIOF->MODER &= ~GPIO_MODER_MODE0_1; //Backlight
    GPIOB->MODER &= ~GPIO_MODER_MODE3_0 & //SCK
    ~GPIO_MODER_MODE5_0; //MOSI
    GPIOB->AFR[0] |= 6 << GPIO_AFRL_AFSEL3_Pos | 6 << GPIO_AFRL_AFSEL5_Pos; //AF5- SPI1
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR4 | GPIO_OSPEEDER_OSPEEDR0 | GPIO_OSPEEDER_OSPEEDR6| GPIO_OSPEEDER_OSPEEDR3 | GPIO_OSPEEDER_OSPEEDR5;


    //UART
    GPIOA->MODER &= ~GPIO_MODER_MODE2_0 & //UART TX
    ~GPIO_MODER_MODE3_0;	//UART RX
    GPIOA->AFR[0] |= 7 << GPIO_AFRL_AFSEL2_Pos | 7 << GPIO_AFRL_AFSEL3_Pos; //AF7- UART2
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR2 | GPIO_OSPEEDER_OSPEEDR3;

    //SPI1 - Source
    GPIOA->MODER &= ~GPIO_MODER_MODE5_0 & //SCK
    ~GPIO_MODER_MODE6_0 & //MISO
    ~GPIO_MODER_MODE7_0; //MOSI
    GPIOA->AFR[0] |= 5 << GPIO_AFRL_AFSEL5_Pos | 5 << GPIO_AFRL_AFSEL6_Pos | 5 << GPIO_AFRL_AFSEL7_Pos; //AF5- SPI1
    GPIOA->MODER &= ~GPIO_MODER_MODE4_1;		 //CS
    GPIOA->BSRR |= GPIO_BSRR_BS4;

    //SPI2 - Target
    GPIOA->MODER &= ~GPIO_MODER_MODE10_0 & //MISO
    ~GPIO_MODER_MODE11_0; //MOSI
    GPIOF->MODER &= ~GPIO_MODER_MODE1_0; //SCK
    GPIOA->AFR[1] |= 5 << GPIO_AFRH_AFSEL10_Pos | 5 << GPIO_AFRH_AFSEL11_Pos;
    GPIOF->AFR[0] |=  5 << GPIO_AFRL_AFSEL1_Pos;	//SCK
    GPIOA->MODER &= ~GPIO_MODER_MODE12_1;		 //CS
    GPIOA->BSRR |= GPIO_BSRR_BS12;

    //START STATE
    GPIOA->BSRR |= GPIO_BSRR_BS1;				//Set LED


    }

void config_clock()
    {
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOFEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_SPI2EN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_SPI3EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_DMAMUX1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;

    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2)); // Switch on FPU.

    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_4WS;

    //Switch on HSI and PLL
    RCC->CR |= RCC_CR_HSION;
    while ((RCC->CR & RCC_CR_HSIRDY) == 0);
    RCC->CR &= ~RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) == RCC_CR_PLLRDY);
    // Select clock source to PLL, configure PLL to 160MHz, why not.
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI;
    RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLN) | 20U << RCC_PLLCFGR_PLLN_Pos;
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLR;
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;
    RCC->CR |= RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) == 0);
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
    }

void config_exti()
    {

    //Button
    EXTI->IMR1 |= EXTI_IMR1_IM0;
    EXTI->FTSR1 |= EXTI_FTSR1_FT0;
    EXTI->RTSR1 |= EXTI_RTSR1_RT0;
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA;
    NVIC_EnableIRQ(EXTI0_IRQn);
    }

void config_uart()
    {

    USART2->BRR = 160000000 / 115200;
    USART2->CR1 = USART_CR1_UE |               // Enable
    USART_CR1_TE |
    USART_CR1_RE | USART_CR1_RXNEIE;
    USART2->CR3 |= USART_CR3_DMAT;
    NVIC_EnableIRQ(USART2_IRQn);
    NVIC_SetPriority(USART2_IRQn, 1);
    }

void config_spi()
    {
    //SPI1 - Source
    SPI1->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI |  SPI_CR1_BR_1;
    SPI1->CR2 |= SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN  | SPI_CR2_FRXTH;
    SPI1->CR1 |= SPI_CR1_SPE;

    //SPI2 - Target
    SPI2->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI |  SPI_CR1_BR_1;
    SPI2->CR2 |=SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN  | SPI_CR2_FRXTH;
    SPI2->CR1 |= SPI_CR1_SPE;

    //SPI3 - LCD
    SPI3->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI   | SPI_CR1_BR_0;
    SPI3->CR2 |= SPI_CR2_TXDMAEN | SPI_CR2_FRXTH;
    SPI3->CR1 |= SPI_CR1_SPE;
    }

void config_dma()
    {
    //    DMA1_Channel1-SPI1-TX SOURCE
    //    DMA1_Channel2-SPI1-RX SOURCE
    //    DMA1_Channel3-SPI2-TX TARGET
    //    DMA1_Channel4-SPI2-RX TARGET
    //    DMA1_Channel5-SPI3-TX LCD - DMA config in LCD config function
    //    DMA1_Channel6-USART2-TX

//    DMA1_Channel1->CPAR = (uint32_t) &(SPI1->DR);
//    DMA1_Channel1->CMAR = (uint32_t) &colors;
//    DMA1_Channel1->CNDTR = 1;
//    DMA1_Channel1->CCR |= DMA_CCR_DIR | DMA_CCR_TCIE;

    DMAMUX1_Channel0->CCR &= ~( DMAMUX_CxCR_DMAREQ_ID);
    DMAMUX1_Channel0->CCR |= (11 << DMAMUX_CxCR_DMAREQ_ID_Pos);
    DMAMUX1_Channel1->CCR &= ~( DMAMUX_CxCR_DMAREQ_ID);
    DMAMUX1_Channel1->CCR |= (10 << DMAMUX_CxCR_DMAREQ_ID_Pos);
    DMAMUX1_Channel2->CCR &= ~( DMAMUX_CxCR_DMAREQ_ID);
    DMAMUX1_Channel2->CCR |= (13 << DMAMUX_CxCR_DMAREQ_ID_Pos);
    DMAMUX1_Channel3->CCR &= ~( DMAMUX_CxCR_DMAREQ_ID);
    DMAMUX1_Channel3->CCR |= (12 << DMAMUX_CxCR_DMAREQ_ID_Pos);
    DMAMUX1_Channel4->CCR &= ~( DMAMUX_CxCR_DMAREQ_ID);
    DMAMUX1_Channel4->CCR |= (15 << DMAMUX_CxCR_DMAREQ_ID_Pos);
    DMAMUX1_Channel5->CCR &= ~( DMAMUX_CxCR_DMAREQ_ID);
    DMAMUX1_Channel5->CCR |= (27 << DMAMUX_CxCR_DMAREQ_ID_Pos);

  //  NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    NVIC_EnableIRQ(DMA1_Channel2_IRQn);
 //   NVIC_EnableIRQ(DMA1_Channel3_IRQn);
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);
   // NVIC_EnableIRQ(DMA1_Channel5_IRQn);

    //DMA1_Channel1->CCR |= DMA_CCR_EN;

    }
