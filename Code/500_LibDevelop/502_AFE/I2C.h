#ifndef  __I2C__H__
#define  __I2C__H__
#include "DVC11XX.h"
//#include "bms_hardware.h"
#include "n32h47x_48x_i2c.h"	//IICÍ¨ÐÅ
#include "n32h47x_48x_gpio.h"
/////////////////////////////////////////////////////////////////////////////////////
void IIC_Init(void); 
void IIC_Wakeup(void);
bool IIC_TransferDataRaw(u8 *writeData,u16 writeLen,u8 *readData,u16 readLen);



/////////////////////////////////////////////////////////////////////////////////////
#endif
