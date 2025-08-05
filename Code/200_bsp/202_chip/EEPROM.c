#include "EEPROM.h"
#include "System_Monitor.h"
#include "sys_timer.h"
#include "wdg.h"
#include "n32h47x_48x_gpio.h"


#define I2C_RW_W	0
#define I2C_RW_R	1



void InitData_E2prom(void);

//产生IIC起始信号
void IIC_Start_SEE(void) {
	SDA_OUT_SEE();     //sda线输出
	IIC_SDA_SEE=1;
	IIC_SCL_SEE=1;
	__delay_us(DELAY_US_IIC_EEPROM);
 	IIC_SDA_SEE=0;//START:when CLK is high,DATA change form high to low 
	__delay_us(DELAY_US_IIC_EEPROM);
	IIC_SCL_SEE=0;//钳住I2C总线，准备发送或接收数据 
}


//产生IIC停止信号
void IIC_Stop_SEE(void) {
	SDA_OUT_SEE();		//sda线输出
	IIC_SCL_SEE=0;
	IIC_SDA_SEE=0;		//STOP:when CLK is high DATA change form low to high
 	__delay_us(DELAY_US_IIC_EEPROM);
	IIC_SCL_SEE=1;
	__delay_us(DELAY_US_IIC_EEPROM);
	IIC_SDA_SEE=1;		//发送I2C总线结束信号
	__delay_us(DELAY_US_IIC_EEPROM);
}


//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
UINT8 IIC_Wait_Ack_SEE(void) {
	UINT8 ucErrTime=0;
	SDA_IN_SEE();      //SDA设置为输入  
	//IIC_SDA_SEE=1;__delay_us(4);
	IIC_SCL_SEE=1;__delay_us(DELAY_US_IIC_EEPROM);
	while(READ_SDA_SEE)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop_SEE();
			return 1;
		}
	}
	IIC_SCL_SEE=0;//时钟输出0
	__delay_us(DELAY_US_IIC_EEPROM);
	return 0;  
}


//产生ACK应答
void IIC_Ack_SEE(void) {
	IIC_SCL_SEE=0;
	SDA_OUT_SEE();
	IIC_SDA_SEE=0;
	__delay_us(DELAY_US_IIC_EEPROM);
	IIC_SCL_SEE=1;
	__delay_us(DELAY_US_IIC_EEPROM);
	IIC_SCL_SEE=0;
}


//不产生ACK应答		    
void IIC_NAck_SEE(void) {
	IIC_SCL_SEE=0;
	SDA_OUT_SEE();
	IIC_SDA_SEE=1;
	__delay_us(DELAY_US_IIC_EEPROM);
	IIC_SCL_SEE=1;
	__delay_us(DELAY_US_IIC_EEPROM);
	IIC_SCL_SEE=0;
}


//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte_SEE(UINT8 txd) {                        
    UINT8 t;
	SDA_OUT_SEE();
    IIC_SCL_SEE=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA_SEE=1;
		else
			IIC_SDA_SEE=0;
		txd<<=1;
		__delay_us(DELAY_US_IIC_EEPROM);   //对TEA5767这三个延时都是必须的
		IIC_SCL_SEE=1;
		__delay_us(DELAY_US_IIC_EEPROM); 
		IIC_SCL_SEE=0;	
		__delay_us(DELAY_US_IIC_EEPROM);
    }
}


//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
UINT8 IIC_Read_Byte_SEE(unsigned char ack) {
	unsigned char i,receive=0;
	SDA_IN_SEE();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL_SEE=0; 
        __delay_us(DELAY_US_IIC_EEPROM);
		IIC_SCL_SEE=1;
        receive<<=1;
        if(READ_SDA_SEE)receive++;   
		__delay_us(DELAY_US_IIC_EEPROM); 
    }					 
    if (!ack)
        IIC_NAck_SEE();//发送nACK
    else
        IIC_Ack_SEE(); //发送ACK   
    return receive;
}


//以下想法失败
UINT8 ReadEEPROM_Byte2(UINT16 addr, UINT8* data) {
	IIC_Start_SEE();
	IIC_Send_Byte_SEE(sEEAddress|I2C_RW_W);	//发送写命令
	if(1 == IIC_Wait_Ack_SEE()) {
		return 1;							//但是返回值的问题，只能在这里加了
	}
	
	#ifndef AT24C02
	IIC_Send_Byte_SEE(addr>>8);				//发送高地址
	if(1 == IIC_Wait_Ack_SEE()) {
		return 1;
	}
	#endif
	
	IIC_Send_Byte_SEE(addr%256);  			//发送低地址
	if(1 == IIC_Wait_Ack_SEE()) {
		return 1;
	}

	IIC_Start_SEE();
	IIC_Send_Byte_SEE(sEEAddress|I2C_RW_R);	//进入接收模式
	if(1 == IIC_Wait_Ack_SEE()) {
		return 1;
	}

	*data=IIC_Read_Byte_SEE(0);
	IIC_Stop_SEE();							//产生一个停止条件
	return 0;
}

UINT8 ReadEEPROM_Word_NoZone2(UINT16 addr, UINT16* data) {
	UINT8  tmp8a,tmp8b,result = 0;	
	result += ReadEEPROM_Byte2(addr, &tmp8a);	//读取低位地址A对应的数据
	result += ReadEEPROM_Byte2(addr+1, &tmp8b);	//读取高位地址A+1对应的数据
	*data = ((UINT16)tmp8b<<8) | tmp8a;		//数据存储，如果通讯失败的话，返回的是0x0000
	return result;
}



//思考失败，有点多余的想法
UINT8 ReadEEPROM_Word_WithZone2(UINT16 addr, UINT16* data) {
	UINT16 tmp16a,tmp16b,tmp16c;
	UINT16 addrB,addrC;
	UINT8 result1 = 0, result2 = 0;
	addrB = addr + BZONE;
	addrC = addr + CZONE;
	result1 += ReadEEPROM_Word_NoZone2(addr, &tmp16a);
	result1 += ReadEEPROM_Word_NoZone2(addrB, &tmp16b);
	result1 += ReadEEPROM_Word_NoZone2(addrC, &tmp16c);
	if(result1 != 0) {
		return ERROR_EEPROM_COM;
	}
	
	if (tmp16a == tmp16b) {                                 //a == b
        if (tmp16a != tmp16c) {                             //a != c
        	result1 += WriteEEPROM_Word_NoZone(addrC, tmp16a);
        }
		*data = tmp16a;
		++result2;
    }
    else {
        if (tmp16b == tmp16c) {                              //b==c  a != b
            result1 += WriteEEPROM_Word_NoZone(addr, tmp16b);
			*data = tmp16b;
			++result2;
        }	
        else {
            if (tmp16a == tmp16c) {                          //a == c, a != b
                result1 += WriteEEPROM_Word_NoZone(addrB, tmp16a);
				*data = tmp16a;
				++result2;
            }
            else {                                           //a != b, b != c, c != a
                result1 += WriteEEPROM_Word_NoZone(addr, tmp16a);
				result1 += WriteEEPROM_Word_NoZone(addrB, tmp16a);
				result1 += WriteEEPROM_Word_NoZone(addrC, tmp16a);
				*data = tmp16a;								//tmp16a,tmp16b,tmp16c返回默认值，返回第一个值？
				++result2;
            }
        }
    }
	if(result1 != 0) {
		return ERROR_EEPROM_COM;
	}
	else if(result2 != 0) {
		return ERROR_EEPROM_COM;
	}
	return 0;
}



//后续维护人员禁止使用这个函数
UINT8 WriteEEPROM_Byte(UINT16 addr, UINT8 val) {
	Feed_IWatchDog;
	MCUO_E2PR_WP = 0;
	
    IIC_Start_SEE();  
	IIC_Send_Byte_SEE(sEEAddress|I2C_RW_W);	//发送写命令
	if(1 == IIC_Wait_Ack_SEE()) {
		return 1;
	}
	
	#ifndef AT24C02
	IIC_Send_Byte_SEE(addr>>8);				//发送高地址
	if(1 == IIC_Wait_Ack_SEE()) {
		return 1;
	}

	#endif
	
    IIC_Send_Byte_SEE(addr%256);   			//发送低地址
	if(1 == IIC_Wait_Ack_SEE()) {
		return 1;
	}	 										  		   
	IIC_Send_Byte_SEE(val);     			//发送字节
	if(1 == IIC_Wait_Ack_SEE()) {
		return 1;
	}
    IIC_Stop_SEE();							//产生一个停止条件
	__delay_ms(5);	 						//EEPROM特性，需要5ms保证写完
	
	MCUO_E2PR_WP = 1;
	Feed_IWatchDog;
	return 0;
}


UINT8 ReadEEPROM_Byte(UINT16 addr) {
	UINT8 temp=0;
	Feed_IWatchDog;
	IIC_Start_SEE();
	IIC_Send_Byte_SEE(sEEAddress|I2C_RW_W);	//发送写命令
	if(1 == IIC_Wait_Ack_SEE()) {
		System_ERROR_UserCallback(ERROR_EEPROM_COM);	//本来打算在最后一层才调用这个函数，这个编程风格
		return 0;							//但是返回值的问题，只能在这里加了
	}

	
	#ifndef AT24C02
	IIC_Send_Byte_SEE(addr>>8);				//发送高地址
	if(1 == IIC_Wait_Ack_SEE()) {
		System_ERROR_UserCallback(ERROR_EEPROM_COM);
		return 0;
	}

	#endif
	
	IIC_Send_Byte_SEE(addr%256);  			//发送低地址
	if(1 == IIC_Wait_Ack_SEE()) {
		System_ERROR_UserCallback(ERROR_EEPROM_COM);
		return 0;
	}

	IIC_Start_SEE();
	IIC_Send_Byte_SEE(sEEAddress|I2C_RW_R);	//进入接收模式
	if(1 == IIC_Wait_Ack_SEE()) {
		System_ERROR_UserCallback(ERROR_EEPROM_COM);
		return 0;
	}

	temp=IIC_Read_Byte_SEE(0);
	IIC_Stop_SEE();							//产生一个停止条件
	Feed_IWatchDog;
	return temp;
}


UINT16 ReadEEPROM_Word_NoZone(UINT16 addr) {
	UINT16 tmp16a;
	UINT8  tmp8a,tmp8b;	
	tmp8a = ReadEEPROM_Byte(addr);		                        //读取低位地址A对应的数据
	tmp8b = ReadEEPROM_Byte(addr+1);	                        //读取高位地址A+1对应的数据
	tmp16a = tmp8b;
	tmp16a = (tmp16a<<8) | tmp8a;	                        //数据存储

	return tmp16a;
}


//主要调这个，加了几句话
UINT8 WriteEEPROM_Word_NoZone(UINT16            addr, UINT16 data) {
    UINT8  tmp8a, tmp8b, WriteCounter = 0, result = 0;
    UINT16 tmp_addr,tmp16;;

	tmp_addr = addr;						//移植忘了这句话
	WriteCounter = 0;
	do {
		result += WriteEEPROM_Byte(tmp_addr, data&0xff);		//数据的低8位写入EEPROM
		result += WriteEEPROM_Byte(tmp_addr+1, data>>8);		//数据的高8位写入EEPROM
		tmp8a = ReadEEPROM_Byte(tmp_addr);						//获取刚存入EEPROM的低8位数据
		tmp8b = ReadEEPROM_Byte(tmp_addr+1);					//获取刚存入EEPROM的高8位数据
		tmp16 = (tmp8b<<8) |tmp8a ; 						//存储读到的数据于变量tmp16
		
		WriteCounter++;
		if(WriteCounter > 2 || result != 0) {				/*判断tmp16 != data的计数*/
			++result;										//要跳出来执行写保护置位
			break;
		}
	}while(tmp16 != data);
	return result;
}

/*
=================以下进入第二层应用阶段=================
*/

UINT16 ReadEEPROM_Word_WithZone(UINT16 addr) {

	UINT16 tmp16a,tmp16b,tmp16c;
	UINT8  tmp8a,tmp8b;	
	UINT16 addrB,addrC;
	
	addrB = addr + BZONE;
	addrC = addr + CZONE;

	tmp8a = ReadEEPROM_Byte(addr);		                        //读取低位地址A对应的数据
	tmp8b = ReadEEPROM_Byte(addr+1);	                        //读取高位地址A+1对应的数据
	tmp16a = tmp8b;
	tmp16a = (tmp16a<<8) | tmp8a;	                        //数据存储
	
	tmp8a = ReadEEPROM_Byte(addrB);                            //读取低位地址B对应的数据
	tmp8b = ReadEEPROM_Byte(addrB+1);	                        //读取高位地址B+1对应的数据
	tmp16b = tmp8b;
	tmp16b = (tmp16b<<8) | tmp8a;	                        //数据存储
	
	tmp8a = ReadEEPROM_Byte(addrC);                            //读取低位地址C对应的数据
	tmp8b = ReadEEPROM_Byte(addrC+1);	                        //读取高位地址C+1对应的数据
	tmp16c = tmp8b;
	tmp16c = (tmp16c<<8) | tmp8a;	                        //数据存储
	
	if (tmp16a == tmp16b) {                                 //a == b
        if (tmp16a != tmp16c) {                             //a != c      
            WriteEEPROM_Word_NoZone(addrC, tmp16a);
        }
        return tmp16a;
    }
    else {
        if (tmp16b == tmp16c) {                              //b==c  a != b
            WriteEEPROM_Word_NoZone(addr, tmp16b);
            return tmp16b;
        }	
        else {
            if (tmp16a == tmp16c) {                          //a == c, a != b
                WriteEEPROM_Word_NoZone(addrB, tmp16a);
                return tmp16a;
            }
            else {                                            //a != b, b != c, c != a
                WriteEEPROM_Word_NoZone(addr, tmp16a);
				WriteEEPROM_Word_NoZone(addrB, tmp16a);
				WriteEEPROM_Word_NoZone(addrC, tmp16a);
                return tmp16a;                        //tmp16a,tmp16b,tmp16c返回默认值，返回第一个值？
            }
        }
    }
}



//主要调这个，加了几句话
void WriteEEPROM_Word_WithZone(UINT16 addr, UINT16 data) {
	UINT8 result = 0;
	result += WriteEEPROM_Word_NoZone(addr, data);
	result += WriteEEPROM_Word_NoZone(addr + BZONE, data);
	result += WriteEEPROM_Word_NoZone(addr + CZONE, data);
	if(result != 0) {
		System_ERROR_UserCallback(ERROR_EEPROM_COM);
	}
}


//初始化IIC
void InitE2PROM(void)
{
	GPIO_InitType InitStruct;

    // 推挽输出类型
    InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    InitStruct.GPIO_Mode      = GPIO_MODE_OUTPUT_PP;
    InitStruct.GPIO_Alternate = GPIO_NO_AF;
    InitStruct.GPIO_Pull      = GPIO_NO_PULL;
    InitStruct.GPIO_Current   = GPIO_DS_8mA;

    InitStruct.Pin = EEPROM_SDA_PIN;
    GPIO_InitPeripheral(EEPROM_SDA_PORT, &InitStruct);
    InitStruct.Pin = EEPROM_SCL_PIN;
    GPIO_InitPeripheral(EEPROM_SCL_PORT, &InitStruct);
    InitStruct.Pin = E2PR_WP_PIN;
    GPIO_InitPeripheral(E2PR_WP_PORT, &InitStruct);

	// InitData_E2prom();
}



