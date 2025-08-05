#ifndef EEPROM_H
#define EEPROM_H

#include "basic_data_type.h"
#include "n32h47x_48x.h"
#include "io_define.h"

//Mini STM32开发板使用的是24c02
//#define AT24C02
#define DELAY_US_IIC_EEPROM		2	//4为100KHz，2为150KHz

#define sEEAddress   0xA0		//E2 = E1 = E0 = 0

#define AZONE				0x0000              // A区: 0x0000~0x0799		//2K一个区间
#define BZONE				0x0800				// B区: 0x0800~0x0999
#define CZONE				0x1000				// C区: 0x1000~0x1800

//IO方向设置
//改括号里面那个数X<<1
// PB13
#define SDA_IN_SEE()  {GPIOB->PMODE&=0xF3FFFFFF;GPIOB->PMODE|=(UINT32)0<<(13<<1);}
#define SDA_OUT_SEE() {GPIOB->PMODE&=0xF3FFFFFF;GPIOB->PMODE|=(UINT32)1<<(13<<1);}

//IO操作函数	
#define IIC_SCL_SEE    MCUO_EEPROM_SCL          //SCL
#define IIC_SDA_SEE    MCUO_EEPROM_SDA          //SDA
#define READ_SDA_SEE   MCUI_EEPROM_SDA          //输入SDA


UINT8 ReadEEPROM_Byte(UINT16 addr);
UINT8 WriteEEPROM_Byte(UINT16 addr, UINT8 val);
UINT16 ReadEEPROM_Word_NoZone(UINT16 addr);
UINT8 WriteEEPROM_Word_NoZone(UINT16 addr, UINT16 data);
UINT16 ReadEEPROM_Word_WithZone(UINT16 addr);						//原则上不返回
void WriteEEPROM_Word_WithZone(UINT16 addr, UINT16 data);

void InitE2PROM(void);


void EEPROM_test(void);

#endif	/* EEPROM_H */

