

#include "dma.h"


#define DMA_BUF_SIZE 2048
uint16_t dmaBufIndex = 0;
uint16_t dmaBuffer[DMA_BUF_SIZE];

void dmaInit(void) {

}

//<editor-fold desc="Dma init options and start">

inline static void dmaReceive8(uint8_t *data, uint32_t n) {
//    dmaStructure.DMA_MemoryBaseAddr = (uint32_t) data;
//    dmaStructure.DMA_BufferSize     = n;
//
//    dmaStructure.DMA_Mode               = DMA_Mode_Normal;
//    dmaStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
//    dmaStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
//    dmaStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
//    dmaStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//
//    dmaStartRx();
}

inline static void dmaSend8(uint8_t *data, uint32_t n) {
    DMA1_Channel5->CCR =0;
    DMA1_Channel5->CPAR =(uint32_t)&SPI3->DR;
    DMA1_Channel5->CMAR =(uint32_t)data;
    DMA1_Channel5->CNDTR = n;
    DMA1_Channel5->CCR= 0| DMA_CCR_DIR| DMA_CCR_MINC;
    DMA1_Channel5->CCR|=DMA_CCR_EN;


//	DMA2_Stream3->PAR = (uint32_t) &SPI1->DR;       // Peripherial address
//	DMA2_Stream3->M0AR = (uint32_t) data;          // Memory address
//	DMA2_Stream3->NDTR = n;        // Number of data items to transfer
//	DMA2_Stream3->CR = 0 | DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_0  | DMA_SxCR_DIR_0 | DMA_SxCR_MINC;       // Memory to peripherial
//
//	DMA2->LIFCR = DMA_LIFCR_CTCIF3;		// clear flag
//	DMA2->LIFCR = DMA_LIFCR_CHTIF3;		// clear flag
//	DMA2_Stream3->CR |= DMA_SxCR_EN;
}

inline static void dmaSendCircular16(uint16_t *data, uint32_t n) {
    DMA1_Channel5->CCR =0;
    DMA1_Channel5->CPAR =(uint32_t)&SPI3->DR;
    DMA1_Channel5->CMAR =(uint32_t)data;
    DMA1_Channel5->CNDTR = n;
    DMA1_Channel5->CCR= 0| DMA_CCR_DIR | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0;//| DMA_CCR_MINC;
    DMA1_Channel5->CCR|=DMA_CCR_EN;



//	DMA2_Stream3->PAR = (uint32_t) &SPI1->DR;       // Peripherial address
//	DMA2_Stream3->M0AR = (uint32_t) data;          // Memory address
//	DMA2_Stream3->NDTR = n;        // Number of data items to transfer
//	DMA2_Stream3->CR =0 | DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_0  | DMA_SxCR_DIR_0| DMA_SxCR_MSIZE_0 | DMA_SxCR_PSIZE_0;// | DMA_SxCR_MINC;       // Memory to peripherial
//
//	DMA2->LIFCR = DMA_LIFCR_CTCIF3;		// clear flag
//	DMA2->LIFCR = DMA_LIFCR_CHTIF3;		// clear flag
//	DMA2_Stream3->CR |= DMA_SxCR_EN;
}

inline static void dmaSend16(uint16_t *data, uint32_t n) {
    DMA1_Channel5->CCR =0;
    DMA1_Channel5->CPAR =(uint32_t)&SPI3->DR;
    DMA1_Channel5->CMAR =(uint32_t)data;
    DMA1_Channel5->CNDTR = n;
    DMA1_Channel5->CCR= 0| DMA_CCR_DIR | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | DMA_CCR_MINC;
    DMA1_Channel5->CCR|=DMA_CCR_EN;


//	DMA2_Stream3->PAR = (uint32_t) &SPI1->DR;       // Peripherial address
//	DMA2_Stream3->M0AR = (uint32_t) data;          // Memory address
//	DMA2_Stream3->NDTR = n;        // Number of data items to transfer
//	DMA2_Stream3->CR =0 | DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_0  | DMA_SxCR_DIR_0| DMA_SxCR_MSIZE_0 | DMA_SxCR_PSIZE_0  |DMA_SxCR_MINC;       // Memory to peripherial
//
//	DMA2->LIFCR = DMA_LIFCR_CTCIF3;		// clear flag
//	DMA2->LIFCR = DMA_LIFCR_CHTIF3;		// clear flag
//	DMA2_Stream3->CR |= DMA_SxCR_EN;
}

//</editor-fold>

//<editor-fold desc="DMA send receive functions">

inline void dmaSendCmd(uint8_t cmd) {
    TFT_CS_RESET;
    TFT_DC_RESET;
    dmaSend8(&cmd, 1);
    dmaWait();
    TFT_CS_SET;
}

inline void dmaSendCmdCont(uint8_t cmd) {
    TFT_DC_RESET;
    dmaSend8(&cmd, 1);
    dmaWait();
}

inline void dmaReceiveDataCont8(uint8_t *data) {
    uint8_t dummy = 0xFF;
    dmaSend8(&dummy, 1);
    dmaReceive8(data, 1);
    dmaWait();
}

inline void dmaSendData8(uint8_t *data, uint32_t n) {
    TFT_CS_RESET;
    TFT_DC_SET;
    dmaSend8(&data, n);
    dmaWait();
    TFT_CS_SET;
}

inline void dmaSendDataCont8(uint8_t *data, uint32_t n) {
    TFT_DC_SET;
    dmaSend8(data, n);
    dmaWait();
}

inline void dmaSendData16(uint16_t *data, uint32_t n) {

    TFT_CS_RESET;
    TFT_DC_SET;
    dmaSend16(data, n);
    dmaWait();
    TFT_CS_SET;
}

inline void dmaSendDataCont16(uint16_t *data, uint32_t n) {
    TFT_DC_SET;
    dmaSend16(data, n);
    dmaWait();
}

inline void dmaSendDataBuf16() {
    if(dmaBufIndex == 0) return;
    TFT_DC_SET;
    dmaSend16(dmaBuffer, dmaBufIndex);
    dmaBufIndex = 0;
    dmaWait();
}

inline void dmaSendDataContBuf16(uint16_t *data, uint32_t n) {
    while (n--) {
        dmaBuffer[dmaBufIndex] = *data++;
        if (dmaBufIndex == DMA_BUF_SIZE - 1) {
            dmaSendDataBuf16();
        }
        dmaBufIndex++;
    }
}


void dmaSendDataCircular16(uint16_t *data, uint32_t n) {
    TFT_DC_SET;
    dmaSendCircular16(data, n);
    dmaWait();
}

//</editor-fold>

inline void dmaFill16(uint16_t color, uint32_t n) {
    TFT_CS_RESET;
   // dmaSendCmdCont(LCD_GRAM);
    while (n != 0) {
        uint16_t ts = (uint16_t) (n > UINT16_MAX ? UINT16_MAX : n);
        dmaSendDataCircular16(&color, ts);
        n -= ts;
    }
    TFT_CS_SET;
}
void dmaWait()
{
	while((SPI3->SR & SPI_SR_BSY)) ;
}
//<editor-fold desc="IRQ handlers">



//</editor-fold>
