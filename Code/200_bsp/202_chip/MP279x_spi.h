#ifndef _MP279X_SPI_H
#define _MP279X_SPI_H

#include "MP279x.h"
#include "n32h47x_48x.h"

typedef enum {
	CFG_OK,
	CFG_ERR
}CFG_TypeDef;

typedef enum {
	Disable,
	Enable
}FUNC_EN;

typedef enum {
	AFE_OK,
	AFE_ERR
}AFE_TypeDef;


#define Addr_MP279x_1			    1
#define Addr_MP279x_2			    1

#define SPI_MP2797x_1               SPI1
#define SPI_MP2797x_2               SPI2

extern uint8_t gu8_AFE_Select;

uint8_t SPI_MP279x_Write(SPI_Module* SPIx, uint8_t Address, uint8_t Register, uint8_t DataL, uint8_t DataH, uint8_t usecrc);
uint8_t SPI_MP279x_Read(SPI_Module* SPIx, uint8_t Address, uint8_t Register, uint8_t* DataL, uint8_t* DataH, uint8_t usecrc);
void MP279x_WakeUpAFE(void);
void MP279x_Init(SPI_Module* SPIx, uint8_t Address, uint8_t cellnums);
uint8_t MP279x_GetDataOnce(SPI_Module* SPIx, uint8_t Address, Meas_REG* Measurements);

#endif	/* _MP279X_SPI_H */
