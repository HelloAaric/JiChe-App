#include "EEPROM.h"
#include "System_Monitor.h"
#include "sys_timer.h"
#include "wdg.h"
#include "n32h47x_48x_gpio.h"


#define I2C_RW_W	0
#define I2C_RW_R	1



void InitData_E2prom(void);

//����IIC��ʼ�ź�
void IIC_Start_SEE(void) {
	SDA_OUT_SEE();     //sda�����
	IIC_SDA_SEE=1;
	IIC_SCL_SEE=1;
	__delay_us(DELAY_US_IIC_EEPROM);
 	IIC_SDA_SEE=0;//START:when CLK is high,DATA change form high to low 
	__delay_us(DELAY_US_IIC_EEPROM);
	IIC_SCL_SEE=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}


//����IICֹͣ�ź�
void IIC_Stop_SEE(void) {
	SDA_OUT_SEE();		//sda�����
	IIC_SCL_SEE=0;
	IIC_SDA_SEE=0;		//STOP:when CLK is high DATA change form low to high
 	__delay_us(DELAY_US_IIC_EEPROM);
	IIC_SCL_SEE=1;
	__delay_us(DELAY_US_IIC_EEPROM);
	IIC_SDA_SEE=1;		//����I2C���߽����ź�
	__delay_us(DELAY_US_IIC_EEPROM);
}


//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
UINT8 IIC_Wait_Ack_SEE(void) {
	UINT8 ucErrTime=0;
	SDA_IN_SEE();      //SDA����Ϊ����  
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
	IIC_SCL_SEE=0;//ʱ�����0
	__delay_us(DELAY_US_IIC_EEPROM);
	return 0;  
}


//����ACKӦ��
void IIC_Ack_SEE(void) {
	IIC_SCL_SEE=0;
	SDA_OUT_SEE();
	IIC_SDA_SEE=0;
	__delay_us(DELAY_US_IIC_EEPROM);
	IIC_SCL_SEE=1;
	__delay_us(DELAY_US_IIC_EEPROM);
	IIC_SCL_SEE=0;
}


//������ACKӦ��		    
void IIC_NAck_SEE(void) {
	IIC_SCL_SEE=0;
	SDA_OUT_SEE();
	IIC_SDA_SEE=1;
	__delay_us(DELAY_US_IIC_EEPROM);
	IIC_SCL_SEE=1;
	__delay_us(DELAY_US_IIC_EEPROM);
	IIC_SCL_SEE=0;
}


//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte_SEE(UINT8 txd) {                        
    UINT8 t;
	SDA_OUT_SEE();
    IIC_SCL_SEE=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA_SEE=1;
		else
			IIC_SDA_SEE=0;
		txd<<=1;
		__delay_us(DELAY_US_IIC_EEPROM);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL_SEE=1;
		__delay_us(DELAY_US_IIC_EEPROM); 
		IIC_SCL_SEE=0;	
		__delay_us(DELAY_US_IIC_EEPROM);
    }
}


//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK
UINT8 IIC_Read_Byte_SEE(unsigned char ack) {
	unsigned char i,receive=0;
	SDA_IN_SEE();//SDA����Ϊ����
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
        IIC_NAck_SEE();//����nACK
    else
        IIC_Ack_SEE(); //����ACK   
    return receive;
}


//�����뷨ʧ��
UINT8 ReadEEPROM_Byte2(UINT16 addr, UINT8* data) {
	IIC_Start_SEE();
	IIC_Send_Byte_SEE(sEEAddress|I2C_RW_W);	//����д����
	if(1 == IIC_Wait_Ack_SEE()) {
		return 1;							//���Ƿ���ֵ�����⣬ֻ�����������
	}
	
	#ifndef AT24C02
	IIC_Send_Byte_SEE(addr>>8);				//���͸ߵ�ַ
	if(1 == IIC_Wait_Ack_SEE()) {
		return 1;
	}
	#endif
	
	IIC_Send_Byte_SEE(addr%256);  			//���͵͵�ַ
	if(1 == IIC_Wait_Ack_SEE()) {
		return 1;
	}

	IIC_Start_SEE();
	IIC_Send_Byte_SEE(sEEAddress|I2C_RW_R);	//�������ģʽ
	if(1 == IIC_Wait_Ack_SEE()) {
		return 1;
	}

	*data=IIC_Read_Byte_SEE(0);
	IIC_Stop_SEE();							//����һ��ֹͣ����
	return 0;
}

UINT8 ReadEEPROM_Word_NoZone2(UINT16 addr, UINT16* data) {
	UINT8  tmp8a,tmp8b,result = 0;	
	result += ReadEEPROM_Byte2(addr, &tmp8a);	//��ȡ��λ��ַA��Ӧ������
	result += ReadEEPROM_Byte2(addr+1, &tmp8b);	//��ȡ��λ��ַA+1��Ӧ������
	*data = ((UINT16)tmp8b<<8) | tmp8a;		//���ݴ洢�����ͨѶʧ�ܵĻ������ص���0x0000
	return result;
}



//˼��ʧ�ܣ��е������뷨
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
				*data = tmp16a;								//tmp16a,tmp16b,tmp16c����Ĭ��ֵ�����ص�һ��ֵ��
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



//����ά����Ա��ֹʹ���������
UINT8 WriteEEPROM_Byte(UINT16 addr, UINT8 val) {
	Feed_IWatchDog;
	MCUO_E2PR_WP = 0;
	
    IIC_Start_SEE();  
	IIC_Send_Byte_SEE(sEEAddress|I2C_RW_W);	//����д����
	if(1 == IIC_Wait_Ack_SEE()) {
		return 1;
	}
	
	#ifndef AT24C02
	IIC_Send_Byte_SEE(addr>>8);				//���͸ߵ�ַ
	if(1 == IIC_Wait_Ack_SEE()) {
		return 1;
	}

	#endif
	
    IIC_Send_Byte_SEE(addr%256);   			//���͵͵�ַ
	if(1 == IIC_Wait_Ack_SEE()) {
		return 1;
	}	 										  		   
	IIC_Send_Byte_SEE(val);     			//�����ֽ�
	if(1 == IIC_Wait_Ack_SEE()) {
		return 1;
	}
    IIC_Stop_SEE();							//����һ��ֹͣ����
	__delay_ms(5);	 						//EEPROM���ԣ���Ҫ5ms��֤д��
	
	MCUO_E2PR_WP = 1;
	Feed_IWatchDog;
	return 0;
}


UINT8 ReadEEPROM_Byte(UINT16 addr) {
	UINT8 temp=0;
	Feed_IWatchDog;
	IIC_Start_SEE();
	IIC_Send_Byte_SEE(sEEAddress|I2C_RW_W);	//����д����
	if(1 == IIC_Wait_Ack_SEE()) {
		System_ERROR_UserCallback(ERROR_EEPROM_COM);	//�������������һ��ŵ�����������������̷��
		return 0;							//���Ƿ���ֵ�����⣬ֻ�����������
	}

	
	#ifndef AT24C02
	IIC_Send_Byte_SEE(addr>>8);				//���͸ߵ�ַ
	if(1 == IIC_Wait_Ack_SEE()) {
		System_ERROR_UserCallback(ERROR_EEPROM_COM);
		return 0;
	}

	#endif
	
	IIC_Send_Byte_SEE(addr%256);  			//���͵͵�ַ
	if(1 == IIC_Wait_Ack_SEE()) {
		System_ERROR_UserCallback(ERROR_EEPROM_COM);
		return 0;
	}

	IIC_Start_SEE();
	IIC_Send_Byte_SEE(sEEAddress|I2C_RW_R);	//�������ģʽ
	if(1 == IIC_Wait_Ack_SEE()) {
		System_ERROR_UserCallback(ERROR_EEPROM_COM);
		return 0;
	}

	temp=IIC_Read_Byte_SEE(0);
	IIC_Stop_SEE();							//����һ��ֹͣ����
	Feed_IWatchDog;
	return temp;
}


UINT16 ReadEEPROM_Word_NoZone(UINT16 addr) {
	UINT16 tmp16a;
	UINT8  tmp8a,tmp8b;	
	tmp8a = ReadEEPROM_Byte(addr);		                        //��ȡ��λ��ַA��Ӧ������
	tmp8b = ReadEEPROM_Byte(addr+1);	                        //��ȡ��λ��ַA+1��Ӧ������
	tmp16a = tmp8b;
	tmp16a = (tmp16a<<8) | tmp8a;	                        //���ݴ洢

	return tmp16a;
}


//��Ҫ����������˼��仰
UINT8 WriteEEPROM_Word_NoZone(UINT16            addr, UINT16 data) {
    UINT8  tmp8a, tmp8b, WriteCounter = 0, result = 0;
    UINT16 tmp_addr,tmp16;;

	tmp_addr = addr;						//��ֲ������仰
	WriteCounter = 0;
	do {
		result += WriteEEPROM_Byte(tmp_addr, data&0xff);		//���ݵĵ�8λд��EEPROM
		result += WriteEEPROM_Byte(tmp_addr+1, data>>8);		//���ݵĸ�8λд��EEPROM
		tmp8a = ReadEEPROM_Byte(tmp_addr);						//��ȡ�մ���EEPROM�ĵ�8λ����
		tmp8b = ReadEEPROM_Byte(tmp_addr+1);					//��ȡ�մ���EEPROM�ĸ�8λ����
		tmp16 = (tmp8b<<8) |tmp8a ; 						//�洢�����������ڱ���tmp16
		
		WriteCounter++;
		if(WriteCounter > 2 || result != 0) {				/*�ж�tmp16 != data�ļ���*/
			++result;										//Ҫ������ִ��д������λ
			break;
		}
	}while(tmp16 != data);
	return result;
}

/*
=================���½���ڶ���Ӧ�ý׶�=================
*/

UINT16 ReadEEPROM_Word_WithZone(UINT16 addr) {

	UINT16 tmp16a,tmp16b,tmp16c;
	UINT8  tmp8a,tmp8b;	
	UINT16 addrB,addrC;
	
	addrB = addr + BZONE;
	addrC = addr + CZONE;

	tmp8a = ReadEEPROM_Byte(addr);		                        //��ȡ��λ��ַA��Ӧ������
	tmp8b = ReadEEPROM_Byte(addr+1);	                        //��ȡ��λ��ַA+1��Ӧ������
	tmp16a = tmp8b;
	tmp16a = (tmp16a<<8) | tmp8a;	                        //���ݴ洢
	
	tmp8a = ReadEEPROM_Byte(addrB);                            //��ȡ��λ��ַB��Ӧ������
	tmp8b = ReadEEPROM_Byte(addrB+1);	                        //��ȡ��λ��ַB+1��Ӧ������
	tmp16b = tmp8b;
	tmp16b = (tmp16b<<8) | tmp8a;	                        //���ݴ洢
	
	tmp8a = ReadEEPROM_Byte(addrC);                            //��ȡ��λ��ַC��Ӧ������
	tmp8b = ReadEEPROM_Byte(addrC+1);	                        //��ȡ��λ��ַC+1��Ӧ������
	tmp16c = tmp8b;
	tmp16c = (tmp16c<<8) | tmp8a;	                        //���ݴ洢
	
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
                return tmp16a;                        //tmp16a,tmp16b,tmp16c����Ĭ��ֵ�����ص�һ��ֵ��
            }
        }
    }
}



//��Ҫ����������˼��仰
void WriteEEPROM_Word_WithZone(UINT16 addr, UINT16 data) {
	UINT8 result = 0;
	result += WriteEEPROM_Word_NoZone(addr, data);
	result += WriteEEPROM_Word_NoZone(addr + BZONE, data);
	result += WriteEEPROM_Word_NoZone(addr + CZONE, data);
	if(result != 0) {
		System_ERROR_UserCallback(ERROR_EEPROM_COM);
	}
}


//��ʼ��IIC
void InitE2PROM(void)
{
	GPIO_InitType InitStruct;

    // �����������
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



