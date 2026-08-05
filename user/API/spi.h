#ifndef __SPI_H_
#define __SPI_H_

#include "stm32f10x.h"

void SPI_Config(void);
uint8_t SPI_Send_Rec_Byte(uint8_t Byte);

#endif
