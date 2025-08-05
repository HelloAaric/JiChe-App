#ifndef _SPI_H
#define _SPI_H

#include "n32h47x_48x_spi.h"

uint8_t SPI_TransmitReceive(SPI_Module* SPIx, uint8_t *TxData, uint8_t *RxData, uint8_t size, uint16_t Timeout);
void InitSPI(void);

#endif	/* _SPI_H */
