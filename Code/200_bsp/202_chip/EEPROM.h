#ifndef EEPROM_H
#define EEPROM_H

#include "basic_data_type.h"
#include "n32h47x_48x.h"
#include "io_define.h"

//Mini STM32������ʹ�õ���24c02
//#define AT24C02
#define DELAY_US_IIC_EEPROM		2	//4Ϊ100KHz��2Ϊ150KHz

#define sEEAddress   0xA0		//E2 = E1 = E0 = 0

#define AZONE				0x0000              // A��: 0x0000~0x0799		//2Kһ������
#define BZONE				0x0800				// B��: 0x0800~0x0999
#define CZONE				0x1000				// C��: 0x1000~0x1800

//IO��������
//�����������Ǹ���X<<1
// PB13
#define SDA_IN_SEE()  {GPIOB->PMODE&=0xF3FFFFFF;GPIOB->PMODE|=(UINT32)0<<(13<<1);}
#define SDA_OUT_SEE() {GPIOB->PMODE&=0xF3FFFFFF;GPIOB->PMODE|=(UINT32)1<<(13<<1);}

//IO��������	
#define IIC_SCL_SEE    MCUO_EEPROM_SCL          //SCL
#define IIC_SDA_SEE    MCUO_EEPROM_SDA          //SDA
#define READ_SDA_SEE   MCUI_EEPROM_SDA          //����SDA


UINT8 ReadEEPROM_Byte(UINT16 addr);
UINT8 WriteEEPROM_Byte(UINT16 addr, UINT8 val);
UINT16 ReadEEPROM_Word_NoZone(UINT16 addr);
UINT8 WriteEEPROM_Word_NoZone(UINT16 addr, UINT16 data);
UINT16 ReadEEPROM_Word_WithZone(UINT16 addr);						//ԭ���ϲ�����
void WriteEEPROM_Word_WithZone(UINT16 addr, UINT16 data);

void InitE2PROM(void);


void EEPROM_test(void);

#endif	/* EEPROM_H */

