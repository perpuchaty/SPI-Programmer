#include "core.h"
#include "stm32g431xx.h"
static uint16_t screen_width  = LCD_PIXEL_WIDTH,
    screen_height = LCD_PIXEL_HEIGHT;

//<editor-fold desc="Init commands">

static const uint8_t init_commands[] = {
        // Power control A
        6, LCD_POWERA, 0x39, 0x2C, 0x00, 0x34, 0x02,
        // Power control B
        4, LCD_POWERB, 0x00, 0xC1, 0x30,
        // Driver timing control A
        4, LCD_DTCA, 0x85, 0x00, 0x78,
        // Driver timing control B
        3, LCD_DTCB, 0x00, 0x00,
        // Power on sequence control
        5, LCD_POWER_SEQ, 0x64, 0x03, 0x12, 0x81,
        // Pump ratio control
        2, LCD_PRC, 0x20,
        // Power control 1
        2, LCD_POWER1, 0x23,
        // Power control 2
        2, LCD_POWER2, 0x10,
        // VCOM control 1
        3, LCD_VCOM1, 0x3E, 0x28,
        // VCOM cotnrol 2
        2, LCD_VCOM2, 0x86,
        // Memory access control
        2, LCD_MAC, 0x48,
        // Pixel format set
        2, LCD_PIXEL_FORMAT, 0x55,
        // Frame rate control
        3, LCD_FRMCTR1, 0x00, 0x18,
        // Display function control
        4, LCD_DFC, 0x08, 0x82, 0x27,
        // 3Gamma function disable
        2, LCD_3GAMMA_EN, 0x00,
        // Gamma curve selected
        2, LCD_GAMMA, 0x01,
        // Set positive gamma
        16, LCD_PGAMMA, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
        16, LCD_NGAMMA, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
        0
};

//</editor-fold>

//<editor-fold desc="LCD initialization functions">

static void LCD_pinsInit() {

//	  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; //Enabling clock for SPI peripheral
//
//	  RCC->AHBENR|= RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOEEN|RCC_AHBENR_DMA1EN;
//
//	  GPIOB->MODER |= GPIO_MODER_MODER10_0 | GPIO_MODER_MODER11_0 |GPIO_MODER_MODER12_0;
//
//	  //GPIOA->MODER |= 0x0000A800; //Setting PA5-PA6-PA7 on alternate function (Check pin description on datasheet)
//	  GPIOE->MODER |= GPIO_MODER_MODER13_1 | GPIO_MODER_MODER14_1 |GPIO_MODER_MODER15_1;
//	  GPIOE->AFR[1]|= 1<<(7*4) | 1<<(6*4) | 1<<(5*4);
//	  //GPIOA->OSPEEDR |= 0x00005400; //10MHz for port A
//
//	 // SPI1->CR1 |= (1 << 4); //SPI_CR1_BR_1 010 , The gyroscope goes at 10MHz. I can't have a Bode rate > 10MHz. So I select divided by 72/8 = 9MHz < 10MHz.
//	  SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR; // Software driven and master mode.
//	  SPI1->CR2 |= SPI_CR2_TXDMAEN | SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2;	//8BIT SPI
//	  SPI1->CR1 |= SPI_CR1_SPE; //Enabling
}

void LCD_reset() {
    TFT_RST_RESET;
    delayms(10);
    TFT_RST_SET;
    delayms(50);
}

void LCD_exitStandby() {
    dmaSendCmd(LCD_SLEEP_OUT);
    delayms(150);
    dmaSendCmd(LCD_DISPLAY_ON);
}

static void LCD_configure() {
    uint8_t count;
    uint8_t *address = (uint8_t *) init_commands;

    TFT_CS_RESET;
    while (1) {
        count = *(address++);
        if (count-- == 0) break;
        dmaSendCmdCont(*(address++));
        dmaSendDataCont8(address, count);
        address += count;
    }
    TFT_CS_SET;

    LCD_setOrientation(0);
}

void LCD_init() {
    LCD_pinsInit();
    TFT_CS_SET;
    dmaInit();
    LCD_setSpi8();
    LCD_reset();
    LCD_exitStandby();
    LCD_configure();
	LCD_fillScreen(BLACK);
	LCD_setTextBgColor(BLACK);
	LCD_setTextColor(WHITE);
	LCD_setOrientation(0xE8);
	LCD_setCursor(0,0);
	LCD_setSpace(1);

}

//</editor-fold>

//<editor-fold desc="LCD common operations">

void LCD_setOrientation(uint8_t o) {
    if (o == ORIENTATION_LANDSCAPE || o == ORIENTATION_LANDSCAPE_MIRROR) {
        screen_height = LCD_PIXEL_WIDTH;
        screen_width  = LCD_PIXEL_HEIGHT;
    } else {
        screen_height = LCD_PIXEL_HEIGHT;
        screen_width  = LCD_PIXEL_WIDTH;
    }
    TFT_CS_RESET;
    dmaSendCmdCont(LCD_MAC);
    dmaSendDataCont8(&o, 1);
    TFT_CS_SET;
}

inline void LCD_setAddressWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    uint16_t pointData[2];

    TFT_CS_RESET;
    dmaSendCmdCont(LCD_COLUMN_ADDR);
    pointData[0] = x1;
    pointData[1] = x2;
    LCD_setSpi16();
    dmaSendDataCont16(pointData, 2);
    LCD_setSpi8();

    dmaSendCmdCont(LCD_PAGE_ADDR);
    pointData[0] = y1;
    pointData[1] = y2;
    LCD_setSpi16();
    dmaSendDataCont16(pointData, 2);
    LCD_setSpi8();
    TFT_CS_SET;
}

inline uint16_t LCD_getWidth() {
    return screen_width;
}

inline uint16_t LCD_getHeight() {
    return screen_height;
}

//</editor-fold>

//<editor-fold desc="SPI functions">

inline void LCD_setSpi8(void) {
    SPI_MASTER->CR1 &= ~SPI_CR1_SPE; // DISABLE SPI
    SPI_MASTER->CR2 &= ~SPI_CR2_DS_3; // SPI 8
    SPI_MASTER->CR1 |= SPI_CR1_SPE;  // ENABLE SPI
}

inline void LCD_setSpi16(void) {
    SPI_MASTER->CR1 &= ~SPI_CR1_SPE; // DISABLE SPI
    SPI_MASTER->CR2 |= SPI_CR2_DS_3;  // SPI 16
    SPI_MASTER->CR1 |= SPI_CR1_SPE;  // ENABLE SPI
}

// </editor-fold>
