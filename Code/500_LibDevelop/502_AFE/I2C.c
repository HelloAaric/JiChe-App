/******************************************************************************
;  *   	@MCU				 N32H474
;  *   	@Create Date         2025.04.10
;  *    @Official 		 
;  *----------------------Abstract Description---------------------------------
;  *			          IIC驱动函数                              		
******************************************************************************/
#include "DVC11XX.h"
#include "I2C.h"

//---------------------------------------------------------------------------
//#define I2C_LONG_TIMEOUT       ((u32)0x10000)	//<超时时间太长，时间测试约10ms-30ms
#define I2C_LONG_TIMEOUT       ((u32)0x1000)
static  void I2C_Configure(void);
int i2c_master_init(void);

/**
	* @说明	IIC初始化
	* @参数	
	* @返回值	
	* @注	
*/
void IIC_Init(void){ 
	i2c_master_init();
}
/**
	* @说明	IIC唤醒
	* @参数	
	* @返回值	
	* @注	
*/
void IIC_Wakeup(void)
{
//	//<初始化为GPIO，发送唤醒信号后再初始化为IIC
//	GPIO_InitType  GPIO_InitStructure; 
//	RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOB, ENABLE);
//	GPIO_InitStructure.Pin =  GPIO_PIN_6 | GPIO_PIN_7;// PB6 -- SCL; PB7 -- SDA
//	GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
//	GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;//推挽输出
//	GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);

//	GPIO_SetBits(GPIOB,GPIO_PIN_6);//唤醒信号：SCL>SDA 2V
//	GPIO_ResetBits(GPIOB,GPIO_PIN_7);//
//	delay_ms(5);
	//<初始化为GPIO，发送唤醒信号后再初始化为IIC
	GPIO_InitType  GPIO_InitStructure; 
	
	RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA, ENABLE);
	RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOC, ENABLE);
	
	GPIO_InitStructure.Pin =  GPIO_PIN_8 ;// PA8 -- SCL; 
	GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
	GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;//推挽输出
	GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.Pin =  GPIO_PIN_9 ;// PC9 -- SDA
	GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
	GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;//推挽输出
	GPIO_InitPeripheral(GPIOC, &GPIO_InitStructure);

	GPIO_SetBits(GPIOA,GPIO_PIN_8);//唤醒信号：SCL>SDA 2V
	GPIO_ResetBits(GPIOC,GPIO_PIN_9);//
//	delay_ms(5);
	delay_us(5000);
}
//<
#if 1
//#define I2Cx I2C1
//#define I2Cx_RCC         RCC_APB1_PERIPH_I2C1

//#define I2Cx_SCL_PIN GPIO_PIN_6
//#define I2Cx_SDA_PIN GPIO_PIN_7
//#define I2Cx_SCL_GPIO      GPIOB
//#define I2Cx_SDA_GPIO      GPIOB
//#define I2Cx_SCL_AF        GPIO_AF2
//#define I2Cx_SDA_AF        GPIO_AF2

//#define I2Cx_clk_en        RCC_AHB_PERIPHEN_GPIOB

//#define I2Cx I2C3
//#define I2Cx_RCC         RCC_APB1_PERIPH_I2C3

//#define I2Cx_SCL_PIN GPIO_PIN_0
//#define I2Cx_SDA_PIN GPIO_PIN_5
//#define I2Cx_SCL_GPIO      GPIOC
//#define I2Cx_SDA_GPIO      GPIOB
//#define I2Cx_SCL_AF        GPIO_AF8
//#define I2Cx_SDA_AF        GPIO_AF11

//#define I2Cx_clk_en        RCC_AHB_PERIPHEN_GPIOB|RCC_AHB_PERIPHEN_GPIOC

#define I2Cx I2C3
#define I2Cx_RCC         RCC_APB1_PERIPH_I2C3

#define I2Cx_SCL_PIN GPIO_PIN_8
#define I2Cx_SDA_PIN GPIO_PIN_9
#define I2Cx_SCL_GPIO      GPIOA
#define I2Cx_SDA_GPIO      GPIOC
#define I2Cx_SCL_AF        GPIO_AF7
#define I2Cx_SDA_AF        GPIO_AF7

#define I2Cx_clk_en        RCC_AHB_PERIPHEN_GPIOA|RCC_AHB_PERIPHEN_GPIOC

#define I2C_MASTER_ADDR   0xD0
//#define I2C_SLAVE_ADDR    0x10
//typedef enum
//{
//    C_READY = 0,
//    C_START_BIT,
//    C_STOP_BIT
//}CommCtrl_t;

//static uint32_t Mutex_Flag = 0;
//static CommCtrl_t Comm_Flag = C_READY;

//typedef enum
//{
//    MASTER_OK = 0,
//    MASTER_BUSY,
//    MASTER_MODE,
//    MASTER_TXMODE,
//    MASTER_RXMODE,
//    MASTER_SENDING,
//    MASTER_SENDED,
//    MASTER_RECVD,
//    MASTER_BYTEF,
//    MASTER_BUSERR,
//    MASTER_UNKNOW,
//    SLAVE_OK = 20,
//    SLAVE_BUSY,
//    SLAVE_MODE,
//    SLAVE_BUSERR,
//    SLAVE_UNKNOW,

//}ErrCode_t;
//#define MODULE_SELF_RESET       1

//void IIC_RCCReset(void)
//{
//       
//    RCC_EnableAPB1PeriphReset(I2Cx_RCC);
//    
//    
////    IIC_RestoreSlaveByClock();
//    
//    i2c_master_init();
//    log_info("***** IIC module by RCC reset! *****\r\n");
//    
//}

///**
//*\*\name    CommTimeOut_CallBack.
//*\*\fun     Callback function.
//*\*\param   none
//*\*\return  none 
//**/
//void CommTimeOut_CallBack(ErrCode_t errcode)
//{
//    log_info("...ErrCode:%d\r\n", errcode);
//	  Mutex_Flag = 0;
//    Comm_Flag = C_READY;
//    
//#if (COMM_RECOVER_MODE == MODULE_SELF_RESET)
//    IIC_SWReset();
//#elif (COMM_RECOVER_MODE == MODULE_RCC_RESET)
//    IIC_RCCReset();
//#elif (COMM_RECOVER_MODE == SYSTEM_NVIC_RESET)
//    SystemNVICReset();
//#endif
//}


//int i2c_master_init(void)
//{
//    I2C_InitType i2cx_master;
//	  GPIO_InitType GPIO_InitStructure;
//	
//	  RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_I2C1, ENABLE);
////    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
////    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOB, ENABLE);
//	
//    GPIO_InitStructure.Pin        = GPIO_PIN_6 ;
//    GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
//    GPIO_InitStructure.GPIO_Alternate = GPIO_AF2;
//    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_OD;
//    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
//    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
//	
//		//<PB7 SDA AF2
//    GPIO_InitStructure.Pin        = GPIO_PIN_7;
//    GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
//    GPIO_InitStructure.GPIO_Alternate = GPIO_AF2;
//    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_OD;
//    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
//    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);

//    I2C_DeInit(I2C1);
//    I2C_InitStruct(&i2cx_master); 
//    i2cx_master.BusMode     = I2C_BUSMODE_I2C;
//    i2cx_master.FmDutyCycle = I2C_FMDUTYCYCLE_2;
//    i2cx_master.OwnAddr1    = I2C_MASTER_ADDR;
//    i2cx_master.AckEnable   = I2C_ACKEN;
//    i2cx_master.AddrMode    = I2C_ADDR_MODE_7BIT;
//    i2cx_master.ClkSpeed    = 100000; // 100K

//    I2C_Init(I2C1, &i2cx_master);
//    I2C_Enable(I2C1, ENABLE);
//		
//    return 0;
//}

/**
*\*\name    i2c_master_init.
*\*\fun     master gpio/rcc/i2c initializes.
*\*\param   none
*\*\return  result 
**/
int i2c_master_init(void)
{
    I2C_InitType i2cx_master;
    GPIO_InitType i2cx_gpio;
    RCC_EnableAPB1PeriphClk(I2Cx_RCC, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
    RCC_EnableAHB1PeriphClk(I2Cx_clk_en, ENABLE);
    I2Cx_SCL_GPIO->POD |= (I2Cx_SCL_PIN );//pull up
    I2Cx_SDA_GPIO->POD |= (I2Cx_SDA_PIN);//pull up

	/* Initialize GPIO_InitStructure */
    GPIO_InitStruct(&i2cx_gpio);
	
    i2cx_gpio.Pin        = I2Cx_SCL_PIN ;
    i2cx_gpio.GPIO_Pull = GPIO_PULL_UP;
    i2cx_gpio.GPIO_Alternate = I2Cx_SCL_AF;
    i2cx_gpio.GPIO_Mode  = GPIO_MODE_AF_OD;
    i2cx_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    GPIO_InitPeripheral(I2Cx_SCL_GPIO, &i2cx_gpio);
	
    i2cx_gpio.Pin        = I2Cx_SDA_PIN;
    i2cx_gpio.GPIO_Pull = GPIO_PULL_UP;
    i2cx_gpio.GPIO_Alternate = I2Cx_SDA_AF;
    i2cx_gpio.GPIO_Mode  = GPIO_MODE_AF_OD;
    i2cx_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    GPIO_InitPeripheral(I2Cx_SDA_GPIO, &i2cx_gpio);

    I2C_DeInit(I2Cx);
    I2C_InitStruct(&i2cx_master); 
    i2cx_master.BusMode     = I2C_BUSMODE_I2C;
    i2cx_master.FmDutyCycle = I2C_FMDUTYCYCLE_2;
    i2cx_master.OwnAddr1    = I2C_MASTER_ADDR;
    i2cx_master.AckEnable   = I2C_ACKEN;
    i2cx_master.AddrMode    = I2C_ADDR_MODE_7BIT;
    i2cx_master.ClkSpeed    = 100000; // 100K

    I2C_Init(I2Cx, &i2cx_master);
    I2C_Enable(I2Cx, ENABLE);
		
    return 0;
}

//static __IO uint32_t I2CTimeout;
//#define I2CT_FLAG_TIMEOUT ((uint32_t)0x8000)
//#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))
///**
//*\*\name    i2c_master_send.
//*\*\fun     master send data.
//*\*\param   data-data to send
//*\*\param   len-length of data to send
//*\*\return  send result 
//**/
//int i2c_master_send(uint8_t* data, int len)
//{
//    uint8_t* sendBufferPtr = data;
//    
//#ifdef NON_REENTRANT
//    if (Mutex_Flag)
//        return -1;
//    else
//        Mutex_Flag = 1;
//#endif
//    
//    I2CTimeout             = I2CT_LONG_TIMEOUT;
//    while (I2C_GetFlag(I2Cx, I2C_FLAG_BUSY))
//    {
//        if ((I2CTimeout--) == 0)
//        {
//            CommTimeOut_CallBack(MASTER_BUSY);
//					  return 1;
//        }
//    }
//    
//    if (Comm_Flag == C_READY)
//    {
//        Comm_Flag = C_START_BIT;
//        I2C_GenerateStart(I2Cx, ENABLE);
//    }
//    
//    I2CTimeout = I2CT_LONG_TIMEOUT;
//    while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_MODE_FLAG)) // EV5
//    {
//        if ((I2CTimeout--) == 0)
//        {
//            CommTimeOut_CallBack(MASTER_MODE);
//					  return 1;
//        }
//    }
//    
//    I2C_SendAddr7bit(I2Cx, I2C_SLAVE_ADDR, I2C_DIRECTION_SEND);   
//    I2CTimeout = I2CT_LONG_TIMEOUT;
//    while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_TXMODE_FLAG)) // EV6
//    {
//        if ((I2CTimeout--) == 0)
//        {
//            CommTimeOut_CallBack(MASTER_TXMODE);
//					  return 1;
//        }
//    }
//    Comm_Flag = C_READY;
//    
//    // send data
//    while (len-- > 0)
//    {
//        I2C_SendData(I2Cx, *sendBufferPtr++);
//        I2CTimeout = I2CT_LONG_TIMEOUT;
//        while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_SENDING)) // EV8
//        {
//            if ((I2CTimeout--) == 0)
//            {
//                CommTimeOut_CallBack(MASTER_SENDING);
//							  return 1;
//            }
//        }
//    }

//    I2CTimeout = I2CT_LONG_TIMEOUT;
//    while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_SENDED)) // EV8-2
//    {
//        if ((I2CTimeout--) == 0)
//        {
//            CommTimeOut_CallBack(MASTER_SENDED);
//					  return 1;
//        }
//    }
//    
//    if (Comm_Flag == C_READY)
//    {
//        Comm_Flag = C_STOP_BIT;
//        I2C_GenerateStop(I2Cx, ENABLE);
//    }
//    
//    while (I2C_GetFlag(I2Cx, I2C_FLAG_BUSY))
//    {
//        if ((I2CTimeout--) == 0)
//        {
//            CommTimeOut_CallBack(MASTER_BUSY);
//					  return 1;
//        }
//    }
//    Comm_Flag = C_READY;

//#ifdef NON_REENTRANT
//    if (Mutex_Flag)
//        Mutex_Flag = 0;
//    else
//        return -2;
//#endif
//    
//    return 0;
//}
///**
//*\*\name    i2c_master_recv.
//*\*\fun     master receive data.
//*\*\param   data-data to receive
//*\*\param   len-length of data to receive
//*\*\return  receive result 
//**/
//int i2c_master_recv(uint8_t* data, int len)
//{
//    uint8_t* recvBufferPtr = data;
//	//uint32_t state;

//#ifdef NON_REENTRANT
//    if (Mutex_Flag)
//        return -1;
//    else
//        Mutex_Flag = 1;
//#endif
//    
//    I2CTimeout             = I2CT_LONG_TIMEOUT;
//    while (I2C_GetFlag(I2Cx, I2C_FLAG_BUSY))
//    {
//        if ((I2CTimeout--) == 0)
//        {
//            CommTimeOut_CallBack(MASTER_BUSY);
//					  return 1;
//        }
//    }
//    I2C_ConfigAck(I2Cx, ENABLE);

//    // send start
//    if (Comm_Flag == C_READY)
//    {
//        Comm_Flag = C_START_BIT;
//        I2C_GenerateStart(I2Cx, ENABLE);
//    }
//    
//    I2CTimeout = I2CT_LONG_TIMEOUT;
//    while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_MODE_FLAG)) // EV5
//    {
//        if ((I2CTimeout--) == 0)
//        {
//            CommTimeOut_CallBack(MASTER_MODE);
//					  return 1;
//        }
//    }
//    // send addr
//    I2C_SendAddr7bit(I2Cx, I2C_SLAVE_ADDR, I2C_DIRECTION_RECV);
//    I2CTimeout = I2CT_LONG_TIMEOUT;
//    while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_RXMODE_FLAG)) // EV6
//    {
//        if ((I2CTimeout--) == 0)
//        {
//            CommTimeOut_CallBack(MASTER_RXMODE);
//					  return 1;
//        }
//    }
//    Comm_Flag = C_READY;
//    
//    if (len == 1)
//    {
//        I2C_ConfigAck(I2Cx, DISABLE);
//        (void)(I2Cx->STS1); /// clear ADDR
//        (void)(I2Cx->STS2);
//        if (Comm_Flag == C_READY)
//        {
//            Comm_Flag = C_STOP_BIT;
//            I2C_GenerateStop(I2Cx, ENABLE);
//        }
//        
//        I2CTimeout = I2CT_LONG_TIMEOUT;
//        while (!I2C_GetFlag(I2Cx, I2C_FLAG_RXDATNE))
//        {
//            if ((I2CTimeout--) == 0)
//            {
//                CommTimeOut_CallBack(MASTER_RECVD);
//							  return 1;
//            }
//        }
//        *recvBufferPtr++ = I2C_RecvData(I2Cx);
//        len--;
//    }
//    else if (len == 2)
//    {
//        I2Cx->CTRL1 |= 0x0400; /// set ACKPOS
//        (void)(I2Cx->STS1);
//        (void)(I2Cx->STS2);
//        I2C_ConfigAck(I2Cx, DISABLE);
//        
//        I2CTimeout = I2CT_LONG_TIMEOUT;
//        while (!I2C_GetFlag(I2Cx, I2C_FLAG_BSF))
//        {
//            if ((I2CTimeout--) == 0)
//            {
//                CommTimeOut_CallBack(MASTER_BYTEF);
//							  return 1;
//            }
//        }
//        
//        if (Comm_Flag == C_READY)
//        {
//            Comm_Flag = C_STOP_BIT;
//            I2C_GenerateStop(I2Cx, ENABLE);
//        }
//        
//        *recvBufferPtr++ = I2C_RecvData(I2Cx);
//        len--;
//        *recvBufferPtr++ = I2C_RecvData(I2Cx);
//        len--;
//    }
//    else
//    {
//        I2C_ConfigAck(I2Cx, ENABLE);
//        (void)(I2Cx->STS1);
//        (void)(I2Cx->STS2);
//        
//        while (len)
//        {
//            if (len == 3)
//            {
//                I2CTimeout = I2CT_LONG_TIMEOUT;
//                while (!I2C_GetFlag(I2Cx, I2C_FLAG_BSF))
//                {
//                    if ((I2CTimeout--) == 0)
//                    {
//                        CommTimeOut_CallBack(MASTER_BYTEF);
//											  return 1;
//                    }
//                }
//                I2C_ConfigAck(I2Cx, DISABLE);
//                *recvBufferPtr++ = I2C_RecvData(I2Cx);
//                len--;
//                
//                I2CTimeout = I2CT_LONG_TIMEOUT;
//                while (!I2C_GetFlag(I2Cx, I2C_FLAG_BSF))
//                {
//                    if ((I2CTimeout--) == 0)
//                    {
//                        CommTimeOut_CallBack(MASTER_BYTEF);
//											  return 1;
//                    }
//                }
//                
//                if (Comm_Flag == C_READY)
//                {
//                    Comm_Flag = C_STOP_BIT;
//                    I2C_GenerateStop(I2Cx, ENABLE);
//                }
//        
//                *recvBufferPtr++ = I2C_RecvData(I2Cx);
//                len--;
//                *recvBufferPtr++ = I2C_RecvData(I2Cx);
//                len--;
//                
//                break;
//            }
//            
//            I2CTimeout = I2CT_LONG_TIMEOUT;
//						
//			I2C_GetLastEvent(I2Cx);
//						
//            while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_RECVD_FLAG)) // EV7
//            {
//                if ((I2CTimeout--) == 0)
//                {
//                    CommTimeOut_CallBack(MASTER_RECVD);
//									  return 1;
//                }
//            }
//            *recvBufferPtr++ = I2C_RecvData(I2Cx);
//            len--;
//        }
//    }
//    
//    I2CTimeout = I2CT_LONG_TIMEOUT;
//    while (I2C_GetFlag(I2Cx, I2C_FLAG_BUSY))
//    {
//        if ((I2CTimeout--) == 0)
//        {
//            CommTimeOut_CallBack(MASTER_BUSY);
//					  return 1;
//        }
//    }
//    Comm_Flag = C_READY;
//    
//#ifdef NON_REENTRANT
//    if (Mutex_Flag)
//        Mutex_Flag = 0;
//    else
//        return -2;
//#endif
//    
//    return 0;
//}

#endif

//---------------------------------------------------------------------------
/**
	* @说明	读写寄存器数据存入缓存
	* @参数	地址、缓存数组、长度
	* @返回值	成功/失败
	* @注	
*/

u32 gu32_AFE_errorTcnt =0;
bool IIC_TransferDataRaw(u8 *writeData,u16 writeLen,u8 *readData,u16 readLen){
	u32 I2C_Wait;
	u8 slaveRdAddr=writeData[0];
//	i2c_master_send((u8 *)writeLen,writeLen);
//	i2c_master_recv(readData,readLen);
	if(writeLen<2) return ERROR;

  //超时等待	
	for(I2C_Wait=I2C_LONG_TIMEOUT<<1;I2C_GetFlag(I2Cx,I2C_FLAG_BUSY);I2C_Wait--)//检测IIC控制器是否忙(如果不存在总线多机通信可以省略）
	{
		if(I2C_Wait==0)
		{
			I2C_DeInit(I2Cx);
      I2C_Configure();//重新初始化IIC，以解除IIC死锁状态
			return ERROR;
		}	
	}
	
	I2C_GenerateStart(I2Cx, ENABLE);//发送START信号
	
	//超时等待
	for(I2C_Wait=I2C_LONG_TIMEOUT;!I2C_CheckEvent(I2Cx,I2C_EVT_MASTER_MODE_FLAG);I2C_Wait--)
	{/*EV5,主模式*/
		if(I2C_Wait==0)
		{
			label_timeout:
			I2C_GenerateStop(I2Cx, ENABLE);//退出时确保关闭I2Cx总线
			++gu32_AFE_errorTcnt;
			return ERROR;
		}
	}

	I2C_SendAddr7bit(I2Cx, slaveRdAddr, I2C_DIRECTION_SEND);//Send EEPROM address for write
	
  //超时等待
  for(I2C_Wait=I2C_LONG_TIMEOUT;!I2C_CheckEvent(I2Cx,I2C_EVT_MASTER_TXMODE_FLAG);I2C_Wait--)
	{//Test on EV6 and clear it
		if(I2C_Wait==0)
		{
				goto label_timeout;
		}
	}
	
	//I2C_Cmd( I2Cx,ENABLE );//Clear EV6 by setting again the PE bit
  while(--writeLen)
	{
	  I2C_SendData(I2Cx, *++writeData);//寄存器地址
		
	  //超时等待
		for(I2C_Wait=I2C_LONG_TIMEOUT;!I2C_CheckEvent(I2Cx,I2C_EVT_MASTER_DATA_SENDING);I2C_Wait--)
		{
			if(I2C_Wait==0)
			{
					goto label_timeout;
			}
	  }
	}	
	
	if(readLen>0)
	{
	  I2C_GenerateStart(I2Cx,ENABLE);//二次发送起始符
		
		//超时等待
	  for(I2C_Wait=I2C_LONG_TIMEOUT;!I2C_CheckEvent(I2Cx,I2C_EVT_MASTER_MODE_FLAG);I2C_Wait--)
		{//检查EV5并清除
				if(I2C_Wait==0) 
				{
						goto label_timeout;
				}

	  }
	
	  I2C_SendAddr7bit(I2Cx,slaveRdAddr,I2C_DIRECTION_RECV);//发送设备地址(8位以及读)
		
	  I2C_Wait = I2C_LONG_TIMEOUT;//等待时间复位
		
	  for(I2C_Wait=I2C_LONG_TIMEOUT;!I2C_CheckEvent(I2Cx,I2C_EVT_MASTER_RXMODE_FLAG);I2C_Wait--)
		{//检查EV6(读)并清除
				if(I2C_Wait==0) 
				{
					goto label_timeout;
				}
	  }
	
		if(readLen>1)
		{
				I2C_ConfigAck(I2Cx,ENABLE);//使能读应答
			
				while(--readLen!=0)
				{
						for(I2C_Wait=I2C_LONG_TIMEOUT;!I2C_CheckEvent(I2Cx,I2C_EVT_MASTER_DATA_RECVD_FLAG);I2C_Wait--)
						{
								if(I2C_Wait==0) 
								{
										goto label_timeout;
								}
						} 	
						*readData++=I2C_RecvData(I2Cx);
				}
		}
		
		I2C_ConfigAck(I2Cx, DISABLE);//读最后一个字节（要关闭应答）
		
    I2C_GenerateStop(I2Cx, ENABLE);//关闭I2Cx总线
		//超时等待
    for(I2C_Wait=I2C_LONG_TIMEOUT;I2C_GetFlag(I2Cx,I2C_FLAG_RXDATNE)==RESET;I2C_Wait--)
		{
				if(I2C_Wait==0)
				{
					goto label_timeout;
				}
    }
		*readData++=I2C_RecvData(I2Cx);
	}	
	else
	{
	  I2C_GenerateStop(I2Cx, ENABLE);//关闭I2Cx总线
	}

	gu32_AFE_errorTcnt = 0;
  return SUCCESS;	
}

/**
	* @说明	IIC重启
	* @参数	
	* @返回值	
	* @注	
*/
void I2C_Configure(void){
	
	I2C_InitType  I2C_InitStructure;
  I2C_InitStructure.BusMode = I2C_BUSMODE_I2C;
	I2C_InitStructure.FmDutyCycle = I2C_FMDUTYCYCLE_2;
	I2C_InitStructure.OwnAddr1 = 0xD0;//主机的I2C地址,用不到则随便写，无影响
	I2C_InitStructure.AckEnable = I2C_ACKEN;
	I2C_InitStructure.AddrMode = I2C_ADDR_MODE_7BIT;
	I2C_InitStructure.ClkSpeed = 100000;//100K，根据自己所用芯片手册查看支持的速度。
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Enable(I2C1, ENABLE);

}
