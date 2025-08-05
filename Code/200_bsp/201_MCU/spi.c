#include "spi.h"
#include "n32h47x_48x.h"
#include "n32h47x_48x_rcc.h"
#include "io_define.h"
#include "n32h47x_48x_gpio.h"

//This function sends and receives size number of bytes
uint8_t SPI1_TransmitReceive(uint8_t *TxData, uint8_t *RxData, uint8_t size, uint16_t Timeout)
{
	uint8_t txcount=size, rxcount=size;
	uint8_t txallowed=1;
	uint16_t i;
	
    // 如果SPI为禁止的话，则开启SPI
	if (!(SPI1->CTRL2 & 0x01)) {
		SPI1->CTRL2 |= (1<<6);
	}

	while((txcount > 0) | (rxcount > 0)) {
		i++;
        /* check TXE flag */
        // 如果发送缓存区为空
        if((txcount > 0) && txallowed && (SPI1->STS & 0x01)) {
            SPI1->DAT = (*TxData++);
            /* Next Data is a reception (Rx). Tx not allowed */
            txcount--;				
            txallowed = 0;
        }

        /* Wait until RXNE flag is reset */
        // 如果接收缓存区非空
        if(SPI1->STS & 0x02) {
            (*(uint8_t *)RxData++) = SPI1->DAT;
            rxcount--;
            /* Next Data is a Transmission (Tx). Tx is allowed */ 
            txallowed = 1;
        }

        if (i >= Timeout) {
            return 1;
        }
    }

	return 0;
}

//This function sends and receives size number of bytes
uint8_t SPI2_TransmitReceive(uint8_t *TxData, uint8_t *RxData, uint8_t size, uint16_t Timeout)
{
	uint8_t txcount=size, rxcount=size;
	uint8_t txallowed=1;
	uint16_t i;
	
    // 如果SPI为禁止的话，则开启SPI
	if (!(SPI2->CTRL2 & 0x01)) {
		SPI2->CTRL2 |= (1<<6);
	}

	while((txcount > 0) | (rxcount > 0)) {
		i++;
        /* check TXE flag */
        // 如果发送缓存区为空
        if((txcount > 0) && txallowed && (SPI2->STS & 0x01)) {
            SPI2->DAT = (*TxData++);
            /* Next Data is a reception (Rx). Tx not allowed */
            txcount--;				
            txallowed = 0;
        }

        /* Wait until RXNE flag is reset */
        // 如果接收缓存区非空
        if(SPI2->STS & 0x02) {
            (*(uint8_t *)RxData++) = SPI2->DAT;
            rxcount--;
            /* Next Data is a Transmission (Tx). Tx is allowed */ 
            txallowed = 1;
        }

        if (i >= Timeout) {
            return 1;
        }
    }

	return 0;
}

extern UINT8 gu8_AFE_Select;
uint8_t SPI_TransmitReceive(SPI_Module* SPIx, uint8_t *TxData, uint8_t *RxData, uint8_t size, uint16_t Timeout)
{
    uint8_t result = 0;

    if(SPIx == SPI1) {
        if(gu8_AFE_Select == 1) {
            MCUO_SPI_NSS_AFE2 = 1;
            MCUO_SPI_NSS_AFE1 = 0;
            result = SPI1_TransmitReceive(TxData, RxData, size, Timeout);
            MCUO_SPI_NSS_AFE1 = 1;
        } else if(gu8_AFE_Select == 2) {
            MCUO_SPI_NSS_AFE1 = 1;
            MCUO_SPI_NSS_AFE2 = 0;
            result = SPI1_TransmitReceive(TxData, RxData, size, Timeout);
            MCUO_SPI_NSS_AFE2 = 1;
        }
    } else if(SPIx == SPI2) {
        // MCUO_SPI_NSS_AFE2 = 0;
        result = SPI2_TransmitReceive(TxData, RxData, size, Timeout);
        // MCUO_SPI_NSS_AFE2 = 1;
    }

    return result;
}

void Init_SPI1(void)
{
    GPIO_InitType GPIO_InitStructure;
    SPI_InitType SPI_InitStructure;
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_SPI1, ENABLE);

    // SCK,MISO,MOSI
    GPIO_InitStructure.Pin          = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStructure.GPIO_Pull    = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF1;
    GPIO_InitStructure.GPIO_Mode    = GPIO_MODE_AF_PP;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

    // CS-AFE1
    GPIO_InitStructure.Pin          = GPIO_PIN_4;
    GPIO_InitStructure.GPIO_Pull    = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Alternate = GPIO_NO_AF;
    GPIO_InitStructure.GPIO_Mode    = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Current = GPIO_DS_8mA;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

    // CS-AFE2
    GPIO_InitStructure.Pin          = GPIO_PIN_5;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);

    /* SPI_Master Config -------------------------------------------------------------*/
    SPI_InitStructure.DataDirection = SPI_DIR_DOUBLELINE_FULLDUPLEX;
    SPI_InitStructure.SpiMode       = SPI_MODE_MASTER;
    SPI_InitStructure.DataLen       = SPI_DATA_SIZE_8BITS;
    SPI_InitStructure.CLKPOL        = SPI_CLKPOL_HIGH;
    SPI_InitStructure.CLKPHA        = SPI_CLKPHA_SECOND_EDGE;
    SPI_InitStructure.NSS           = SPI_NSS_SOFT;
    SPI_InitStructure.BaudRatePres  = SPI_BR_PRESCALER_128;
    SPI_InitStructure.FirstBit      = SPI_FB_MSB;
    SPI_InitStructure.CRCPoly       = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    /* Enable SPI_Master */
    SPI_Enable(SPI1, ENABLE);
}


void Init_SPI2(void)
{
    GPIO_InitType GPIO_InitStructure;
    SPI_InitType SPI_InitStructure;
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_SPI2, ENABLE);

    // SCK,MISO,MOSI
    GPIO_InitStructure.Pin          = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitStructure.GPIO_Pull    = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF1;
    GPIO_InitStructure.GPIO_Mode    = GPIO_MODE_AF_PP;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);

    // CS
    GPIO_InitStructure.Pin          = GPIO_PIN_12;
    GPIO_InitStructure.GPIO_Pull    = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Alternate = GPIO_NO_AF;
    GPIO_InitStructure.GPIO_Mode    = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Current = GPIO_DS_8mA;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);


    /* SPI_Master Config -------------------------------------------------------------*/
    SPI_InitStructure.DataDirection = SPI_DIR_DOUBLELINE_FULLDUPLEX;
    SPI_InitStructure.SpiMode       = SPI_MODE_MASTER;
    SPI_InitStructure.DataLen       = SPI_DATA_SIZE_8BITS;
    SPI_InitStructure.CLKPOL        = SPI_CLKPOL_LOW;
    SPI_InitStructure.CLKPHA        = SPI_CLKPHA_FIRST_EDGE;
    SPI_InitStructure.NSS           = SPI_NSS_SOFT;
    SPI_InitStructure.BaudRatePres  = SPI_BR_PRESCALER_8;
    SPI_InitStructure.FirstBit      = SPI_FB_LSB;
    SPI_InitStructure.CRCPoly       = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    /* Enable SPI_Master */
    SPI_Enable(SPI1, ENABLE);
}

void InitSPI(void)
{
    Init_SPI1();
    // Init_SPI1_hardware();
    // Init_SPI2();
}
