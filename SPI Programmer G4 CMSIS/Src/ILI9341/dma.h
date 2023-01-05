#ifndef TEST1_DMA_H
#define TEST1_DMA_H

#include "control.h"




void dmaInit(void);

void dmaSendCmd(uint8_t cmd);
void dmaSendCmdCont(uint8_t cmd);

void dmaReceiveDataCont8(uint8_t *data);

void dmaSendData8(uint8_t *data, uint32_t n);
void dmaSendData16(uint16_t *data, uint32_t n);

void dmaSendDataCont8(uint8_t *data, uint32_t n);
void dmaSendDataCont16(uint16_t *data, uint32_t n);

void dmaSendDataBuf16();
void dmaSendDataContBuf16(uint16_t *data, uint32_t n);

void dmaFill16(uint16_t color, uint32_t n);

#endif //TEST1_DMA_H
