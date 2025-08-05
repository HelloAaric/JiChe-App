/*
*
* file name:    JiChe_APP.c
*  @author  	  X
*  @date        2025.4.15
*  @version     V1.0
*/

/*
*==========================================================================
*                                  Include                                *
*==========================================================================
*/
#include "JiChe_APP.h"


/*
*==========================================================================
*                             Macro & Structures                          *
*==========================================================================
*/
#define JICHE_RESET_THRESHOLD			(10)	//<复位10次报通讯失败
// #define  CurrentSenseResistance_mR      0.25            //电流采样电阻值（单位mR)
#define  CurrentSenseResistance_mR      1            //电流采样电阻值（单位mR)

/*
===========================================================================
*                                 Variables                               *
===========================================================================
*/
static	UINT16 ui16JiCheRunStep = 0;
static	UINT16 ui16JiCheMainStep = 0;
static	UINT16 ui16JiChe_Voltage_CRC_ERR = 0;	
static	UINT16 ui16JiChe_Current_CRC_ERR = 0;
static	UINT16 ui16JiChe_Temperature_CRC_ERR = 0;
static	UINT16 ui16JiChe_ResetCnt = 0;
static	UINT16 ui16JiChe_Cfg_CRC_ERR = 0;

sAFE_Data AFE_Data_ALL;	//<AFE的所有数据汇总接口

//extern TAFERegs g_AfeRegs;	//AFE的寄存器级别的数据接口

//typedef union un_CSC_Status
//{
//    UINT16 uiDaisyStep;
//    struct
//    {
//      UINT16 uiDaisyMainStep:3;
//      UINT16 uiDaisyInitStep:3;
//      UINT16 uiDaisySamplingStep:2;
//      UINT16 uiDaisySaveStep:3;
//      UINT16 uiWakeUpStep:3;    
//    }B;
//}UN_CSC_STA;

//typedef struct st_JiChe
//{
//	UN_CSC_STA uiDaisyStatus;							//菊花链工作状态机
//	UINT16 uiCommStatus;								//通信状态: 0=初始化，1=通信正常，2=通信失败(置离线告警)
//	UINT16 uiResetCnt;      							// 菊花链复位次数
//	UINT16 uiCfgIdErrCntALL;
//	UINT16 uiDaisyWriteCmdErrCntAll;
//	UINT16 uiDaisySaveVoltErrCntAll;					
//	UINT16 uiDaisySaveGpioErrCntAll;
//	UINT16 uiDaisySaveTempErrCntAll;
//	UINT16 uiDaisySaveDiagErrCntAll;
//}ST_JICHE;

/**************************************************************************
功能：		
入参：		
返回值：	空
**************************************************************************/
void JiCheChangeMainStep(eJICHE_MAIN_FUN eJiCheMainStep)
{
   ui16JiCheMainStep = eJiCheMainStep;
}


void JiCheChangeRunStep(eJICHE_RUN_FUN eJiCheRunStep)
{
   ui16JiCheRunStep = eJiCheRunStep;
}

/**
	* @说明	清除所有警报
	* @参数	
	* @返回值	
	* @注	
*/
void CleanError(void){
	g_AfeRegs.R0.cleanflag=0;
	while(!DVC11XX_WriteRegs(AFE_ADDR_R(0),1));
}

//#define CellColse

void OnJiChe_Voltage(void)
{
	UINT16 i = 0;
	INT16 uwVCell;
	UINT32 uwVBAT,uwVPACK,uwVLOAD;
	UINT16 uwV1P8;
	//读取相关寄存器数据到全局结构体变量g_AfeRegs
	//Read IIC from R0~R76, totally 77 registers.
	
#ifdef CellColse
	CellTestClose(0x00004);//R106[7:0]~R108[7:4]
#endif
	if(DVC11XX_ReadRegs(AFE_ADDR_R(0), 110))
	{
		//解析电池组各电芯的电压值			
		for(i = 0; i < AFE_MAX_CELL_CNT; i++)
		{
			uwVCell = DVC11XX_Calc_VCell(i); 

			PRINT_AFE_INFO("Cell#%d  vol=%d mV\r\n", i+1,uwVCell);//将各电芯电压打印到UART1
			
			AFE_Data_ALL.CellVolt[i] = uwVCell;	//将采集转换校准后单体电压存放在AFE的数据集中
		}
		//解析电池包总压
		uwVBAT= DVC11XX_Calc_VBAT();
		PRINT_AFE_INFO("VBAT = %u mV\r\n", uwVBAT);
		AFE_Data_ALL.VBAT = uwVBAT;
		
		//解析PACK电压
		uwVPACK= DVC11XX_Calc_VPACK();
		PRINT_AFE_INFO("VPACK = %u mV\r\n", uwVPACK);
		AFE_Data_ALL.VPACK = uwVPACK;
		
		//解析LOAD电压
		uwVLOAD= DVC11XX_Calc_VLOAD();
		PRINT_AFE_INFO("VLOAD = %u mV\r\n", uwVLOAD);
		AFE_Data_ALL.VLOAD = uwVLOAD;
			
		//解析V1P8电压
		uwV1P8= DVC11XX_Calc_V1P8();
		PRINT_AFE_INFO("V1P8 = %u mV\r\n", uwV1P8);
		AFE_Data_ALL.V1P8 = uwV1P8;
		
		delay_ms(1000);
	}
	else
	{
		ui16JiChe_Voltage_CRC_ERR++;
		AFE_Data_ALL.JiChe_Voltage_CRC_ERR = ui16JiChe_Voltage_CRC_ERR;
	}
}


void OnJiChe_Voltage_Vcell(void)
{
	UINT16 i = 0;
	INT16 uwVCell;
	// UINT32 uwVBAT,uwVPACK,uwVLOAD;
	// UINT16 uwV1P8;
	//读取相关寄存器数据到全局结构体变量g_AfeRegs
	//Read IIC from R0~R76, totally 77 registers.

	if(DVC11XX_ReadRegs(AFE_ADDR_R(29), 48))
	{
		//解析电池组各电芯的电压值			
		for(i = 0; i < AFE_MAX_CELL_CNT; i++)
		{
			uwVCell = DVC11XX_Calc_VCell(i); 

			PRINT_AFE_INFO("Cell#%d  vol=%d mV\r\n", i+1,uwVCell);//将各电芯电压打印到UART1
			
			AFE_Data_ALL.CellVolt[i] = uwVCell;	//将采集转换校准后单体电压存放在AFE的数据集中
		}
	}
	else
	{
		ui16JiChe_Voltage_CRC_ERR++;
		AFE_Data_ALL.JiChe_Voltage_CRC_ERR = ui16JiChe_Voltage_CRC_ERR;
	}
}

void OnJiChe_Voltage_other(void)
{
	UINT32 uwVBAT,uwVPACK,uwVLOAD;
	UINT16 uwV1P8;
	//读取相关寄存器数据到全局结构体变量g_AfeRegs
	//Read IIC from R0~R76, totally 77 registers.
	
	if(DVC11XX_ReadRegs(AFE_ADDR_R(0), 17))
	{
		//解析电池包总压
		uwVBAT= DVC11XX_Calc_VBAT();
		PRINT_AFE_INFO("VBAT = %u mV\r\n", uwVBAT);
		AFE_Data_ALL.VBAT = uwVBAT;
		
		//解析PACK电压
		uwVPACK= DVC11XX_Calc_VPACK();
		PRINT_AFE_INFO("VPACK = %u mV\r\n", uwVPACK);
		AFE_Data_ALL.VPACK = uwVPACK;
		
		//解析LOAD电压
		uwVLOAD= DVC11XX_Calc_VLOAD();
		PRINT_AFE_INFO("VLOAD = %u mV\r\n", uwVLOAD);
		AFE_Data_ALL.VLOAD = uwVLOAD;
			
		//解析V1P8电压
		uwV1P8= DVC11XX_Calc_V1P8();
		PRINT_AFE_INFO("V1P8 = %u mV\r\n", uwV1P8);
		AFE_Data_ALL.V1P8 = uwV1P8;
	}
	else
	{
		ui16JiChe_Voltage_CRC_ERR++;
		AFE_Data_ALL.JiChe_Voltage_CRC_ERR = ui16JiChe_Voltage_CRC_ERR;
	}
}


void OnJiChe_Current(void)
{
		float fCurrent;
		//从IIC接口读取AFE寄存器值（R2~R6）到全局结构体变量g_AfeRegs中，用于后续操作计算电流值。
		if(DVC11XX_ReadRegs(AFE_ADDR_R(2), 6))
		{
			//读取相关寄存器数据到全局结构体变量g_AfeRegs
			//通过过滤器CC1的采样值来计算电流 
			fCurrent = DVC11XX_Calc_CurrentWithCC1(CurrentSenseResistance_mR); 
			// PRINT_AFE_INFO("Current with CC1=%.3f A\r\n", fCurrent);//将电流值打印到UART1
			AFE_Data_ALL.I_CC1 = fCurrent;
			
			//通过过滤器CC2的采样值来计算电流
			fCurrent = DVC11XX_Calc_CurrentWithCC2(CurrentSenseResistance_mR); 
			// PRINT_AFE_INFO("Current with CC2=%.3f A\r\n", fCurrent);//将电流值打印到UART1 
			AFE_Data_ALL.I_CC2 = fCurrent;
		}
		else
		{
			ui16JiChe_Current_CRC_ERR++;
			AFE_Data_ALL.JiChe_Current_CRC_ERR = ui16JiChe_Current_CRC_ERR;
		}
		delay_ms(3000);
}

u8 EVENT_alert_req =0;

void OnJiChe_AlertInterruptMask(void)
{
	#if 0
	InterruptShield_Config(0x1F);//使能各警报中断
	if(DVC11XX_ReadRegs(AFE_ADDR_R(0), 1))
	{		
		//查询中断报警信息
		if(g_AfeRegs.R0.bitmap.SCD==1)	//打印对应报警
		{
				PRINT_AFE_INFO("SCD warning! \r\n");
		}	
				
		if(g_AfeRegs.R0.bitmap.OCC2==1)
		{
				PRINT_AFE_INFO("OCC2 warning! \r\n");
		}

		if(g_AfeRegs.R0.bitmap.OCD2==1)
		{
				PRINT_AFE_INFO("OCD2 warning! \r\n");
		}

		if(g_AfeRegs.R0.bitmap.OCC1==1)
		{
				PRINT_AFE_INFO("OCC1 warning! \r\n");
		}

		if(g_AfeRegs.R0.bitmap.OCD1==1)
		{
				PRINT_AFE_INFO("OCD1 warning! \r\n");
		}

		if(g_AfeRegs.R0.bitmap.CUV==1)
		{
				PRINT_AFE_INFO("UV warning! \r\n");
		}

		if(g_AfeRegs.R0.bitmap.COV==1)
		{
				PRINT_AFE_INFO("OV warning! \r\n");
		}

		if(g_AfeRegs.R0.bitmap.IWF==1)
		{
				PRINT_AFE_INFO("WatchDogs OverFlow! \r\n");
		}
	}
	CleanError();//擦除报警
	#endif

	if(DVC11XX_ReadRegs(AFE_ADDR_R(0), 1)) {		
		//查询中断报警信息
		AFE_Data_ALL.Warning.sBit.SCD = g_AfeRegs.R0.bitmap.SCD;
		AFE_Data_ALL.Warning.sBit.OCC2 = g_AfeRegs.R0.bitmap.OCC2;
		AFE_Data_ALL.Warning.sBit.OCD2 = g_AfeRegs.R0.bitmap.OCD2;
		AFE_Data_ALL.Warning.sBit.OCC1 = g_AfeRegs.R0.bitmap.OCC1;
		AFE_Data_ALL.Warning.sBit.OCD1 = g_AfeRegs.R0.bitmap.OCD1;
		AFE_Data_ALL.Warning.sBit.CUV = g_AfeRegs.R0.bitmap.CUV;
		AFE_Data_ALL.Warning.sBit.COV = g_AfeRegs.R0.bitmap.COV;
		AFE_Data_ALL.Warning.sBit.IWF = g_AfeRegs.R0.bitmap.IWF;
	}
}

/**
	* @说明	保护功能参数配置
	* @参数	
	* @返回值	
	* @注	
*/
void Protection_Config(void)
{
//	if(OCD1_Config(250,16))//0.25mv 16ms
//	{
//		PRINT_AFE_INFO("OCD1 Config SUCCESS \r\n");
//	}

//	if(OCC1_Config(250,16))//0.25mv 16ms
//	{
//		PRINT_AFE_INFO("OCC1 Config SUCCESS \r\n");
//	}

//	if(OV_Config(2500,200,ENABLE))//2500mv 200ms
//	{
//		PRINT_AFE_INFO("OV Config SUCCESS \r\n");
//	}

//	if(UV_Config(500,200,ENABLE))//500mv 200ms
//	{
//		PRINT_AFE_INFO("UV Config SUCCESS \r\n");
//	}

//	if(SCD_Config(10,0,ENABLE)) //10mv 0ms
//	{
//		PRINT_AFE_INFO("SCD Config SUCCESS \r\n");
//	}

//	if(OCD2_Config(4,8,ENABLE))//4mv 8ms
//	{
//		PRINT_AFE_INFO("OCD2 Config SUCCESS \r\n");
//	}

//	if(OCC2_Config(4,8,ENABLE))//4mv 8ms
//	{
//		PRINT_AFE_INFO("OCC2 Config SUCCESS \r\n");
//	}

	if(OCD1_Config(AFE_Data_ALL.ProtectionSet.OCD1_TH, AFE_Data_ALL.ProtectionSet.OCD1_DLY))//0.25mv 16ms
	{
		PRINT_AFE_INFO("OCD1 Config SUCCESS \r\n");
		PRINT_AFE_INFO("OCD1 = %u uV\r\n", AFE_Data_ALL.ProtectionSet.OCD1_TH);
		PRINT_AFE_INFO("OCD1 = %u ms\r\n", AFE_Data_ALL.ProtectionSet.OCD1_DLY);
	}

	if(OCC1_Config(AFE_Data_ALL.ProtectionSet.OCC1_TH, AFE_Data_ALL.ProtectionSet.OCC1_DLY))//0.25mv 16ms
	{
		PRINT_AFE_INFO("OCC1 Config SUCCESS \r\n");
		PRINT_AFE_INFO("OCC1 = %u uV\r\n", AFE_Data_ALL.ProtectionSet.OCC1_TH);
		PRINT_AFE_INFO("OCC1 = %u ms\r\n", AFE_Data_ALL.ProtectionSet.OCC1_DLY);
	}

	if(OV_Config(AFE_Data_ALL.ProtectionSet.OV_TH, AFE_Data_ALL.ProtectionSet.OV_DLY, AFE_Data_ALL.ProtectionSet.OV_EN))//2500mv 200ms
	{
		PRINT_AFE_INFO("OV Config SUCCESS \r\n");
		PRINT_AFE_INFO("OV = %u uV\r\n", AFE_Data_ALL.ProtectionSet.OV_TH);
		PRINT_AFE_INFO("OV = %u ms\r\n", AFE_Data_ALL.ProtectionSet.OV_DLY);
		PRINT_AFE_INFO("OV = %u \r\n", AFE_Data_ALL.ProtectionSet.OV_EN);
	}

	if(UV_Config(AFE_Data_ALL.ProtectionSet.UV_TH, AFE_Data_ALL.ProtectionSet.UV_DLY, AFE_Data_ALL.ProtectionSet.UV_EN))//500mv 200ms
	{
		PRINT_AFE_INFO("UV Config SUCCESS \r\n");
		PRINT_AFE_INFO("UV = %u uV\r\n", AFE_Data_ALL.ProtectionSet.UV_TH);
		PRINT_AFE_INFO("UV = %u ms\r\n", AFE_Data_ALL.ProtectionSet.UV_DLY);
		PRINT_AFE_INFO("UV = %u \r\n", AFE_Data_ALL.ProtectionSet.UV_EN);
	}

	if(SCD_Config(AFE_Data_ALL.ProtectionSet.SCD_TH, AFE_Data_ALL.ProtectionSet.SCD_DLY, AFE_Data_ALL.ProtectionSet.SCD_EN)) //10mv 0ms
	{
		PRINT_AFE_INFO("SCD Config SUCCESS \r\n");
		PRINT_AFE_INFO("SCD = %u uV\r\n", AFE_Data_ALL.ProtectionSet.SCD_TH);
		PRINT_AFE_INFO("SCD = %u ms\r\n", AFE_Data_ALL.ProtectionSet.SCD_DLY);
		PRINT_AFE_INFO("SCD = %u \r\n", AFE_Data_ALL.ProtectionSet.SCD_EN);
	}

	if(OCD2_Config(AFE_Data_ALL.ProtectionSet.OCD2_TH, AFE_Data_ALL.ProtectionSet.OCD2_DLY, AFE_Data_ALL.ProtectionSet.OCD2_EN))//4mv 8ms
	{
		PRINT_AFE_INFO("OCD2 Config SUCCESS \r\n");
		PRINT_AFE_INFO("OCD2 = %u uV\r\n", AFE_Data_ALL.ProtectionSet.OCD2_TH);
		PRINT_AFE_INFO("OCD2 = %u ms\r\n", AFE_Data_ALL.ProtectionSet.OCD2_DLY);
		PRINT_AFE_INFO("OCD2 = %u \r\n", AFE_Data_ALL.ProtectionSet.OCD2_EN);
	}

	if(OCC2_Config(AFE_Data_ALL.ProtectionSet.OCC2_TH, AFE_Data_ALL.ProtectionSet.OCC2_DLY, AFE_Data_ALL.ProtectionSet.OCC2_EN))//4mv 8ms
	{
		PRINT_AFE_INFO("OCC2 Config SUCCESS \r\n");
		PRINT_AFE_INFO("OCD2 = %u uV\r\n", AFE_Data_ALL.ProtectionSet.OCC2_TH);
		PRINT_AFE_INFO("OCD2 = %u ms\r\n", AFE_Data_ALL.ProtectionSet.OCC2_DLY);
		PRINT_AFE_INFO("OCD2 = %u \r\n", AFE_Data_ALL.ProtectionSet.OCC2_EN);
	}

	CleanError();
}

/**
	* @说明	保护功能参处理
	* @参数	
	* @返回值	
	* @注	
*/
void Protection_handle(void)
{
	if(DVC11XX_ReadRegs(AFE_ADDR_R(0), 1))	//查询报警信息
	{
		//打印对应报警
		if(g_AfeRegs.R0.bitmap.SCD==1)
		{
			PRINT_AFE_INFO("SCD warning! \r\n");
			AFE_Data_ALL.Warning.sBit.SCD = g_AfeRegs.R0.bitmap.SCD;
		}

		if(g_AfeRegs.R0.bitmap.OCC2==1)
		{
			PRINT_AFE_INFO("OCC2 warning! \r\n");
			AFE_Data_ALL.Warning.sBit.OCC2 = g_AfeRegs.R0.bitmap.OCC2;
		}

		if(g_AfeRegs.R0.bitmap.OCD2==1)
		{
			PRINT_AFE_INFO("OCD2 warning! \r\n");
			AFE_Data_ALL.Warning.sBit.OCD2 = g_AfeRegs.R0.bitmap.OCD2;
		}

		if(g_AfeRegs.R0.bitmap.OCC1==1)
		{
			PRINT_AFE_INFO("OCC1 warning! \r\n");
			AFE_Data_ALL.Warning.sBit.OCC1 = g_AfeRegs.R0.bitmap.OCC1;
		}

		if(g_AfeRegs.R0.bitmap.OCD1==1)
		{
			PRINT_AFE_INFO("OCD1 warning! \r\n");
			AFE_Data_ALL.Warning.sBit.OCD1 = g_AfeRegs.R0.bitmap.OCD1;
		}

		if(g_AfeRegs.R0.bitmap.CUV==1)
		{
			PRINT_AFE_INFO("UV warning! \r\n");
			AFE_Data_ALL.Warning.sBit.CUV = g_AfeRegs.R0.bitmap.CUV;
		}

		if(g_AfeRegs.R0.bitmap.COV==1)
		{
			PRINT_AFE_INFO("OV warning! \r\n");
			AFE_Data_ALL.Warning.sBit.COV = g_AfeRegs.R0.bitmap.COV;
		}
	}
	CleanError();//擦除报警
}

void OnJiChe_Protection(void)
{
	Protection_Config();//保护功能全部打开且配置为较低数值；
	//需要加延时，等待诊断运行完成后读取结果
	Protection_handle();
	delay_ms(3000);
}

void OnJiChe_ChargerDetect(void)
{
	#if 0
	if(DVC11XX_ReadRegs(AFE_ADDR_R(1), 1))	//查询充电器接入
	{
//		ChargerDetect();//充电器监测
		if(g_AfeRegs.R1.PD==1)//打印对应报警
		{		
			PRINT_AFE_INFO("Charger Connected! \r\n");
			AFE_Data_ALL.ChargeDetect = g_AfeRegs.R1.PD;
		}
	}
	else
	{
		PRINT_AFE_INFO("Charger Disconnected! \r\n");
		AFE_Data_ALL.ChargeDetect = 0;
	}
	delay_ms(3000);
	#endif

	if(DVC11XX_ReadRegs(AFE_ADDR_R(1), 1)) {	//查询充电器接入
		AFE_Data_ALL.ChargeDetect = g_AfeRegs.R1.PD;
	}

}

void OnJiChe_LoadDetect(void)
{
	if(DVC11XX_ReadRegs(AFE_ADDR_R(11),72))
	{//读取数据

		if(g_AfeRegs.R81.LDPU==0)//负载未上拉
		{
			LoadPullUp();//使能负载上拉
		}
		else
		{
//			LoadDetect();//负载检测
			u16 Vload;	
	
			Vload=DVC11XX_Calc_VLOAD();//计算Vload电压
			PRINT_AFE_INFO("Vload = %u mV\r\n", Vload);

			if(Vload >= LoadDetect_Threshold)//比较阈值2V
			{
				PRINT_AFE_INFO("Load Disconnect \r\n");
				AFE_Data_ALL.LoadDetect = 0;
			}
			else
			{
				PRINT_AFE_INFO("Load Connecting \r\n");
				AFE_Data_ALL.LoadDetect = 1;
			}
		}
	}
	delay_ms(3000);
}

void OnJiChe_BrokenLineDetect(void)
{
	#if 0
	if(DVC11XX_ReadRegs(AFE_ADDR_R(109), 1))
	{//查询断线检测使能位

		if(g_AfeRegs.R109.COW==0)
		{//断线检测使能已复位

			COW_enable();//使能断线检测

			if(DVC11XX_ReadRegs(AFE_ADDR_R(29), 47))//读取电池各串电压
			{
//				BrokenLineDetect();//断线检测结果输出
				u8 i;
				int uw_cellvotage[AFE_MAX_CELL_CNT];
	
				for(i=0;i<AFE_MAX_CELL_CNT;i++)
				{
					uw_cellvotage[i]=DVC11XX_Calc_VCell(i);//计算各串电压

					if(uw_cellvotage[i]==0)//断线串打印输出
					{
						PRINT_AFE_INFO("CELL %d Broken!\r\n",i+1);
						AFE_Data_ALL.BrokenLineDetect.All |= (1<<i);
					}
				}
			}
			else
			{
				PRINT_AFE_INFO("No CELL Broken!\r\n");
				AFE_Data_ALL.BrokenLineDetect.All = 0;
			}
		}
	}
	delay_ms(3000);
	#endif

	u8 i = 0;
	static u8 su8_startUp = 0;

	switch(su8_startUp) {
		case 0:
			if(DVC11XX_ReadRegs(AFE_ADDR_R(109), 1)) {
				if(g_AfeRegs.R109.COW == 0) {									// 断线检测使能已复位
					// 使能断线检测
					COW_enable();
				}
			}
			su8_startUp = 1;
			break;

		case 1:
			// 查询断线检测使能位
			if(DVC11XX_ReadRegs(AFE_ADDR_R(109), 1)) {
				if(g_AfeRegs.R109.COW == 0) {									// 断线检测使能已复位
					for(i = 0; i < AFE_MAX_CELL_CNT; i++) {						// 断线检测结果输出
						if(AFE_Data_ALL.CellVolt[i] == 0) {					// 断线串打印输出
							PRINT_AFE_INFO("CELL %d Broken!\r\n",i+1);
							AFE_Data_ALL.BrokenLineDetect.All |= (1<<i);
						}
					}
					// 使能断线检测
					COW_enable();
				}
			}
			break;
			
		default:
			break;
	}
}

bool	wakeup_flag=0;	//唤醒按键标志
bool 	sleep_flag=0;		//睡眠按键标志
bool  INT_flag=0;			//GP中断标志
/**
	* @说明	睡眠处理函数
	* @参数	
	* @返回值	
	* @注	
*/
void Sleep_Handle(void){

	if(wakeup_flag)
	{//按下唤醒按键

		if(DVC11XX_ReadRegs(AFE_ADDR_R(0), 1))
		{//IIC通信唤醒
			wakeup_flag=0;//唤醒标志复位
		}
	}

	if(sleep_flag)
	{//按下睡眠按键

		g_AfeRegs.R1.CST=0xE;//配置睡眠
		if(DVC11XX_WriteRegs(AFE_ADDR_R(1), 1))
		{//写入寄存器
			PRINT_AFE_INFO("AFE Going To Sleep!\r\n");
			sleep_flag=0;//睡眠标志复位
		}
	}
		
	if(INT_flag==1)
	{//收到AFE唤醒中断
		INT_flag=0;
		if(DVC11XX_ReadRegs(AFE_ADDR_R(1), 1))
		{
			AFE_Data_ALL.WakeUpSourceDetect = g_AfeRegs.R1.CST;
			switch(g_AfeRegs.R1.CST)
			{
				case 0:PRINT_AFE_INFO("AFE Wake Up by Power\r\n");
						break;//关断模式唤醒
				case 1:PRINT_AFE_INFO("AFE Wake Up by IIC\r\n");
						break;//IIC唤醒
				case 2:PRINT_AFE_INFO("AFE Wake Up by Timed\r\n");	
						break;//定时唤醒				
				case 3:PRINT_AFE_INFO("AFE Wake Up by Discharge\r\n");
						break;//放电唤醒
				case 4:PRINT_AFE_INFO("AFE Wake Up by Charge\r\n");
						break;//充电唤醒
				case 5:PRINT_AFE_INFO("AFE Wake Up by OCD2\r\n");
						break;//二级放电过流唤醒
				case 6:PRINT_AFE_INFO("AFE Wake Up by OCC2\r\n");
						break;//二级充电过流唤醒
				case 7:PRINT_AFE_INFO("AFE Wake Up by SCD\r\n");
						break;//放电短路唤醒
				case 8:PRINT_AFE_INFO("AFE Wake Up by Charger\r\n");
						break;//充电器唤醒
				default :PRINT_AFE_INFO("Interrupt Trigger Up by Jitter\r\n");
						break;//中断源抖动
			}
		}
	}
}

void OnJiChe_Sleep(void)
{
	Sleep_config();//睡眠模式初始化
	Sleep_Handle();//关断处理
	delay_ms(1000);
}

/**
	* @说明	AFE初始化配置写入
	* @参数	寄存器自定义参数默认值
	* @返回值	成功/失败
	* @注	
*/
static bool DVC11XX_InitRegs(void){
	memcpy((char *)&g_AfeRegs+81,DVC11XX_PresetConfigRegData_R81To121,sizeof(DVC11XX_PresetConfigRegData_R81To121));
	return DVC11XX_WriteRegs(AFE_ADDR_R(81),sizeof(DVC11XX_PresetConfigRegData_R81To121));
}

/**
	* @说明	AFE芯片重新初始化
	* @参数	
	* @返回值	成功/失败
	* @注	
*/
bool DVC11XX_InitRegs_Twice(void)
{
	return DVC11XX_InitRegs();
}

bool shutdown_flag=0;
/**
	* @说明	关断处理函数
	* @参数	
	* @返回值	
	* @注	
*/
void Shutdown_Handle(void)
{

	if(wakeup_flag)
	{//按下唤醒按键
		IIC_Wakeup();//发送IIC唤醒信号
		IIC_Init();//重启IIC通信
		if(DVC11XX_ReadRegs(AFE_ADDR_R(1), 1))
		{//读取到AFE状态			
			DVC11XX_InitRegs_Twice();//重新初始化AFE
			CleanError();//清除警报
			wakeup_flag=0;//唤醒标志复位
			PRINT_AFE_INFO("AFE Wake Up by IIC\r\n");	
		}
	}
		
	if(shutdown_flag)
	{//按下关断按键
		Shutdown();
		PRINT_AFE_INFO("AFE Shutdown!\r\n");
		shutdown_flag=0;//关断标志复位
	}
}

void OnJiChe_Shutdown(void)
{
	Shutdown_Handle();//关断处理
	delay_ms(1000);
}

void OnJiChe_CellBalance(void)
{
	#if 0
	if(DVC11XX_ReadRegs(AFE_ADDR_R(13),98))
	{//读取数据至R108.CVS

		OverTempProtect(100);//过温保护 阈值100摄氏度

		CalcuVolMaxMin();//电压最大最小值计算
		
		if(0 == AFE_Data_ALL.CellBalanceMode)
		{
			BalanceProcess();//自动均衡处理
		}
		else
		{
			Balance_Contrl(AFE_Data_ALL.CellsBalancCmd.All);	//<手动控制均衡
		}
		
		AFE_Data_ALL.CellsBalancStatus.All = (g_AfeRegs.R103_R105.CB[0]<<16)+(g_AfeRegs.R103_R105.CB[1]<<8)+g_AfeRegs.R103_R105.CB[2];//刷新当前均衡位
	}
	delay_ms(3000);
	#endif

	if(DVC11XX_ReadRegs(AFE_ADDR_R(103), 3)) {		//读取数据至R108.CVS
		AFE_Data_ALL.CellsBalancStatus.All = (g_AfeRegs.R103_R105.CB[0]<<16)+(g_AfeRegs.R103_R105.CB[1]<<8)+g_AfeRegs.R103_R105.CB[2];//刷新当前均衡位
	}
}

/**
	* @说明	高边驱动屏蔽设置
	* @参数	
	* @返回值	
	* @注	保护阈值=BDPT*40uV
*/
void FET_Config(void){
		
//	DSGM_Control(DSGM_CP);//随电荷泵
//	DPC_Config(16);//下拉强度16
//	BDPT_Config(80);//续流阈值40uV
//	ChargePump_Control(CP_10V);//10V
//		
//	HSFM_Control(DISABLE);//允许高边驱动
//		
//	CHG_FETControl(FET_OPEN);//打开充电
//	DSG_FETControl(FET_Close_BFCO);
	
	DSGM_Control(AFE_Data_ALL.FET_Config.DSGM);//随电荷泵
	
	if(0 == AFE_Data_ALL.FET_Config.DPC)	//<如果未设置则默认参数
	{
		DPC_Config(16);//下拉强度16
	}
	else
	{
		DPC_Config(AFE_Data_ALL.FET_Config.DPC);
	}

	if(0 == AFE_Data_ALL.FET_Config.TH)	//<如果未设置则默认参数
	{
		BDPT_Config(80);//续流阈值40uV
	}	
	else
	{
	BDPT_Config(AFE_Data_ALL.FET_Config.TH);//续流阈值40uV
	}
	
	if(0 == AFE_Data_ALL.FET_Config.CPM)
	{
		ChargePump_Control(CP_10V);//10V
	}
	else
	{
	ChargePump_Control(AFE_Data_ALL.FET_Config.CPM);//10V
	}
		
	HSFM_Control(AFE_Data_ALL.FET_Config.HSFM);//允许高边驱动
		
//	CHG_FETControl(AFE_Data_ALL.FET_Control.CHG);//打开充电
//	DSG_FETControl(AFE_Data_ALL.FET_Control.DSG);
}

//<更新AFE的充放电引脚状态
void OnJiChe_FETControl(void)
{
	#if 0
	FET_Config();//FET配置
	if(DVC11XX_ReadRegs(AFE_ADDR_R(4),3))
	{
		if(g_AfeRegs.R4_6.PDSGF)
		{
			PRINT_AFE_INFO("PDSG OPEN!\r\n");
			AFE_Data_ALL.FET_Status.PDSGF = g_AfeRegs.R4_6.PDSGF;
		}

		if(g_AfeRegs.R4_6.PCHGF)
		{
			PRINT_AFE_INFO("PCHG OPEN!\r\n");
			AFE_Data_ALL.FET_Status.PCHGF = g_AfeRegs.R4_6.PCHGF;
		}

		if(g_AfeRegs.R4_6.DSGF)
		{
			PRINT_AFE_INFO("DSG OPEN!\r\n");
			AFE_Data_ALL.FET_Status.DSGF = g_AfeRegs.R4_6.DSGF;
		}

		if(g_AfeRegs.R4_6.CHGF)
		{
			PRINT_AFE_INFO("CHG OPEN!\r\n");
			AFE_Data_ALL.FET_Status.CHGF = g_AfeRegs.R4_6.CHGF;
		}
	}
	//	delay_ms(1000);
	#endif

	if(DVC11XX_ReadRegs(AFE_ADDR_R(4),3)) {
		AFE_Data_ALL.FET_Status.PDSGF = g_AfeRegs.R4_6.PDSGF;
		AFE_Data_ALL.FET_Status.PCHGF = g_AfeRegs.R4_6.PCHGF;
		AFE_Data_ALL.FET_Status.DSGF = g_AfeRegs.R4_6.DSGF;
		AFE_Data_ALL.FET_Status.CHGF = g_AfeRegs.R4_6.CHGF;
	}
}

void OnJiChe_Temperature(void)
{
	float scChipTemp,scBatTemp;
//	GPM_Temperature();//GP1管脚模式配置
	
	Read_NFRT();//读取电阻修调值
		
//	delay_ms(200);//等待GP1采样
	
	if(DVC11XX_ReadRegs(AFE_ADDR_R(13), 16))
	{//读取相关寄存器数据到全局结构体变量g_AfeRegs			
			
		//计算芯片温度值			
		scChipTemp = DVC11XX_Calc_ChipTemp();
		PRINT_AFE_INFO("Chip temperature = %.1f \r\n", scChipTemp);//打印芯片温度
		AFE_Data_ALL.ChipIntTemp = 	scChipTemp*10;
		
		//计算电池温度值	
		scBatTemp = DVC11XX_Calc_BatTemp(GP1);
		PRINT_AFE_INFO("Battery temperature1 = %.1f \r\n", scBatTemp);//打印电池温度
		AFE_Data_ALL.CellTemp[0] = 	(int16_t)scBatTemp*10;

		//计算电池温度值	
		scChipTemp = DVC11XX_Calc_BatTemp(GP2);
		PRINT_AFE_INFO("Battery temperature2 = %.1f \r\n", scChipTemp);//打印电池温度
		AFE_Data_ALL.CellTemp[1] = 	(int16_t)scChipTemp*10;
		
		//计算电池温度值	
		scChipTemp = DVC11XX_Calc_BatTemp(GP3);
		PRINT_AFE_INFO("Battery temperature3 = %.1f \r\n", scChipTemp);//打印电池温度
		AFE_Data_ALL.CellTemp[2] = 	(int16_t)scChipTemp*10;
		
		//计算电池温度值	
		scChipTemp = DVC11XX_Calc_BatTemp(GP4);
		PRINT_AFE_INFO("Battery temperature4 = %.1f \r\n", scChipTemp);//打印电池温度
		AFE_Data_ALL.CellTemp[3] = 	(int16_t)scChipTemp*10;
		
		//计算电池温度值	
		scChipTemp = DVC11XX_Calc_BatTemp(GP5);
		PRINT_AFE_INFO("Battery temperature5 = %.1f \r\n", scChipTemp);//打印电池温度
		AFE_Data_ALL.CellTemp[4] = 	scChipTemp*10;
		
		//计算电池温度值	
		scChipTemp = DVC11XX_Calc_BatTemp(GP6);
		PRINT_AFE_INFO("Battery temperature6 = %.1f \r\n", scChipTemp);//打印电池温度
		AFE_Data_ALL.CellTemp[5] = 	scChipTemp*10;
	}
	else
	{
		ui16JiChe_Temperature_CRC_ERR++;
		AFE_Data_ALL.JiChe_Temperature_CRC_ERR = ui16JiChe_Temperature_CRC_ERR;
	}
//	delay_ms(3000);
}

/**
	* @说明	GP脚复用配置
	* @参数	bool 模式
	* @返回值	
	* @注	
*/
void GPn_ModeConfig(bool mode){

//	GPIO_Ini_Config();//GPIO初始化

	memset((u8 *)&g_AfeRegs+116,0,2);//GPn数据位 初始化为0

	if(!mode)
	{//模式A
//			EXTI4_initial();//MCU外部中断
//			GP1_ModeConfig(0x03);//GP1 CHG硬线控制
//			GP2_ModeConfig(0x06);//GP2 INT
//			GP3_ModeConfig(0x02);//GP3 模拟输入
//			GP4_ModeConfig(0x03);//GP4 DSG硬线控制
//			GP5_ModeConfig(0x07);//GP5 CHG_LS
//			GP6_ModeConfig(0x07);//GP6 DSG_LS
//			INT_Ctrl(0xE0);//GP中断触发选择
		
			GP1_ModeConfig(0x01);
			GP2_ModeConfig(0x01);
			GP3_ModeConfig(0x01);
			GP4_ModeConfig(0x01);
			GP5_ModeConfig(0x01);
			GP6_ModeConfig(0x01);
	}
	else
	{//模式B
			GP1_ModeConfig(0x01);
			GP2_ModeConfig(0x01);
			GP3_ModeConfig(0x01);
			GP4_ModeConfig(0x01);
			GP5_ModeConfig(0x07);
			GP6_ModeConfig(0x07);
	}	
	FET_Open();//打开充放电
}


void OnJiChe_GPn(void)
{
	GPn_ModeConfig(MODE);//GP复用相关配置
	//读取到FET状态
	if(DVC11XX_ReadRegs(AFE_ADDR_R(6), 1))
	{
		GPnDemo_Cal();
	}

	delay_ms(1000);
}


//static unsigned char Read_Back_R81To121[41];
bool	rst_flag=0;	//复位按键标志

void OnJiChe_Reset(void)
{
//	int i,uwVCell;
//	u32 uwVBAT,uwVPACK,uwVLOAD;
//	u16 uwV1P8;
//	
//		if(rst_flag==1)
//		{	//按下复位按键
//			if(AFE_RST())
//			{
//				PRINT_AFE_INFO("Reset Complete \r\n");
//			}

//			if(DVC11XX_ReadRegs(AFE_ADDR_R(81), 41))
//			{//回读配置寄存器
//				memcpy(Read_Back_R81To121,(char *)&g_AfeRegs+81,41);//回读转存
//			
//				for(i=0;i<41;i++)
//				{//打印初始化数值、回读数值
//					PRINT_AFE_INFO("Reg Number %x\r\n",i+81);
//					PRINT_AFE_INFO("		Init Array Value  = %x\r\n",DVC11XX_PresetConfigRegData_R81To121[i]);
//					PRINT_AFE_INFO("		Reset Array Value = %x\r\n",Read_Back_R81To121[i]);
//				}
//			}
//				
//			rst_flag=0;//复位状态清0
//			delay_ms(1000);
//		}
//		else if(DVC11XX_ReadRegs(AFE_ADDR_R(0), 110))
//		{
//			//解析电池组各电芯的电压值			
//			for(i=0;i<AFE_MAX_CELL_CNT;i++)
//			{
//				uwVCell = DVC11XX_Calc_VCell(i); 

//				PRINT_AFE_INFO("Cell#%d  vol=%d mV\r\n", i+1,uwVCell);//将各电芯电压打印到UART1

//			}
//			//解析电池包总压
//			uwVBAT= DVC11XX_Calc_VBAT();
//			PRINT_AFE_INFO("VBAT = %u mV\r\n", uwVBAT);
//					
//			//解析PACK电压
//			uwVPACK= DVC11XX_Calc_VPACK();
//			PRINT_AFE_INFO("VPACK = %u mV\r\n", uwVPACK);
//					
//			//解析LOAD电压
//			uwVLOAD= DVC11XX_Calc_VLOAD();
//			PRINT_AFE_INFO("VLOAD = %u mV\r\n", uwVLOAD);
//					
//			//解析V1P8电压
//			uwV1P8= DVC11XX_Calc_V1P8();
//			PRINT_AFE_INFO("V1P8 = %u mV\r\n", uwV1P8);
//						
//			delay_ms(1000);
//		}
			}

/**
	* @说明	系统初始化
	* @参数	
	* @返回值	
	* @注	
*/

void TASK_JiChe_InitFun(void)
{	
	// IIC_Wakeup();				//5.唤醒AFE芯片
	IIC_Init();       				//6. 初始化IIC通信接口(MCU跟AFE的通信接口）
	AFE_RST();						//7.复位AFE
	delay_ms(300);    				//8. 延时256ms（确保AFE芯片至少完成一个周期的电压和电流值的测量）
	#if ADDRESS>=0xC0
	GPIO_INTPUT();
	#elif ADDRESS==0x40
	IIC_SLAVE_ADDRESS=ADDRESS>>1;
	#endif	
	if(DVC11XX_InitRegs()) 			//<AFE芯片寄存器初始化（写入用户自定义的配置数据）
	{
		Protection_Config();		//保护功能全部打开且配置为较低数值；
		// FET_Config();			//FET配置
		GPn_ModeConfig(MODE);		//GP复用相关配置，配置为热偶探头
		
		CleanError();				//写入默认值后清除警报	//<此函数内部有死循环，需注意

		JiCheChangeRunStep(eJiChe_RunFun_Work);
		PRINT_AFE_INFO("Initial Completed !\r\n");

		AFE_Data_ALL.JiChe_StartUp = 1;
	}
	else
	{
		ui16JiChe_Cfg_CRC_ERR++;
		PRINT_AFE_INFO("Initial ERR !\r\n");
	}
		
	// JiCheChangeRunStep(eJiChe_RunFun_Work);
}

void TASK_JiChe_MainFun_OK_old(void)
{
	switch(ui16JiCheMainStep)
	{
		//<电压采样
		case eJiChe_MainFun_Voltage:
			OnJiChe_Voltage();
			JiCheChangeMainStep(eJiChe_MainFun_Current);
		break;
		//<电流采样
		case eJiChe_MainFun_Current:
			OnJiChe_Current();
//			JiCheChangeMainStep(eJiChe_MainFun_AlertInterruptMask);
			JiCheChangeMainStep(eJiChe_MainFun_Protection);	
		break;
		//<中断查询告警//<目前做的是轮询查看告警信息
		case eJiChe_MainFun_AlertInterruptMask:
//			OnJiChe_AlertInterruptMask();
			JiCheChangeMainStep(eJiChe_MainFun_Protection);			
		break;
		//<保护参数设置//事件型
		case eJiChe_MainFun_Protection:
			OnJiChe_Protection();
			JiCheChangeMainStep(eJiChe_MainFun_ChargerDetect);		
		break;
		//<充电检测//周期性
		case eJiChe_MainFun_ChargerDetect:
			OnJiChe_ChargerDetect();
			JiCheChangeMainStep(eJiChe_MainFun_LoadDetect);	
		break;
		//<负载检测//周期性
		case eJiChe_MainFun_LoadDetect:
			OnJiChe_LoadDetect();
			JiCheChangeMainStep(eJiChe_MainFun_BrokenLineDetect);	
		break;
		//<断线检测//周期性
		case eJiChe_MainFun_BrokenLineDetect:
			OnJiChe_BrokenLineDetect();
			JiCheChangeMainStep(eJiChe_MainFun_CellBalance);
//			JiCheChangeMainStep(eJiChe_MainFun_Sleep);	
		break;
		//<休眠，待调试//事件型
		case eJiChe_MainFun_Sleep:
//			OnJiChe_Sleep();
//			JiCheChangeMainStep(eJiChe_MainFun_Shutdown);	
		break;
		//<关机//事件型
		case eJiChe_MainFun_Shutdown:
//			OnJiChe_Shutdown();
//			JiCheChangeMainStep(eJiChe_MainFun_CellBalance);
		break;
		//<单体均衡	//周期性
		case eJiChe_MainFun_CellBalance:
			OnJiChe_CellBalance();	
			JiCheChangeMainStep(eJiChe_MainFun_FETControl);			
		break;
		//<MOS管状态更新//周期性
		case eJiChe_MainFun_FETControl:
			OnJiChe_FETControl();
			JiCheChangeMainStep(eJiChe_MainFun_Temperature);	
			break;
		//<温度采集	//周期性
		case eJiChe_MainFun_Temperature:
			OnJiChe_Temperature();
			JiCheChangeMainStep(eJiChe_MainFun_Voltage);
//			JiCheChangeMainStep(eJiChe_MainFun_GPn);				
		break;
		//<GPIO控制	//周期性
		case eJiChe_MainFun_GPn:
//			OnJiChe_GPn();
//			JiCheChangeMainStep(eJiChe_MainFun_Reset);	
		break;
		//<复位	//事件型
		case eJiChe_MainFun_Reset:
//			OnJiChe_Reset();
//			JiCheChangeMainStep(eJiChe_MainFun_Voltage);	
		break;
		default :
			JiCheChangeMainStep(eJiChe_MainFun_Voltage);
		break;
	}
}


void TASK_JiChe_MainFun_ok2(void)
{
	switch(ui16JiCheMainStep)
	{
		//<电压采样
		case eJiChe_MainFun_Voltage:
			OnJiChe_Voltage_Vcell();
			JiCheChangeMainStep(eJiChe_MainFun_Voltage_other);
			break;

		case eJiChe_MainFun_Voltage_other:
			OnJiChe_Voltage_other();
			JiCheChangeMainStep(eJiChe_MainFun_Current);
			break;

		//<电流采样
		case eJiChe_MainFun_Current:
			OnJiChe_Current();
//			JiCheChangeMainStep(eJiChe_MainFun_AlertInterruptMask);
			JiCheChangeMainStep(eJiChe_MainFun_Protection);	
			break;
		//<中断查询告警//<目前做的是轮询查看告警信息
		case eJiChe_MainFun_AlertInterruptMask:
//			OnJiChe_AlertInterruptMask();
			JiCheChangeMainStep(eJiChe_MainFun_Protection);			
			break;
		//<保护参数设置//事件型
		case eJiChe_MainFun_Protection:
			// OnJiChe_Protection();
			JiCheChangeMainStep(eJiChe_MainFun_ChargerDetect);		
			break;
		//<充电检测//周期性
		case eJiChe_MainFun_ChargerDetect:
			OnJiChe_ChargerDetect();
			JiCheChangeMainStep(eJiChe_MainFun_LoadDetect);	
			break;
		//<负载检测//周期性
		case eJiChe_MainFun_LoadDetect:
			// OnJiChe_LoadDetect();
			JiCheChangeMainStep(eJiChe_MainFun_BrokenLineDetect);	
			break;
		//<断线检测//周期性
		case eJiChe_MainFun_BrokenLineDetect:
			OnJiChe_BrokenLineDetect();
			JiCheChangeMainStep(eJiChe_MainFun_CellBalance);
//			JiCheChangeMainStep(eJiChe_MainFun_Sleep);	
			break;
		//<休眠，待调试//事件型
		case eJiChe_MainFun_Sleep:
//			OnJiChe_Sleep();
//			JiCheChangeMainStep(eJiChe_MainFun_Shutdown);	
			break;
		//<关机//事件型
		case eJiChe_MainFun_Shutdown:
//			OnJiChe_Shutdown();
//			JiCheChangeMainStep(eJiChe_MainFun_CellBalance);
			break;
		//<单体均衡	//周期性
		case eJiChe_MainFun_CellBalance:
			OnJiChe_CellBalance();	
			JiCheChangeMainStep(eJiChe_MainFun_FETControl);			
			break;
		//<MOS管状态更新//周期性
		case eJiChe_MainFun_FETControl:
			// OnJiChe_FETControl();
			JiCheChangeMainStep(eJiChe_MainFun_Temperature);	
			break;
		//<温度采集	//周期性
		case eJiChe_MainFun_Temperature:
			OnJiChe_Temperature();
			JiCheChangeMainStep(eJiChe_MainFun_Voltage);
//			JiCheChangeMainStep(eJiChe_MainFun_GPn);				
			break;
		//<GPIO控制	//周期性
		case eJiChe_MainFun_GPn:
//			OnJiChe_GPn();
//			JiCheChangeMainStep(eJiChe_MainFun_Reset);	
			break;
		//<复位	//事件型
		case eJiChe_MainFun_Reset:
//			OnJiChe_Reset();
//			JiCheChangeMainStep(eJiChe_MainFun_Voltage);	
			break;
		default :
			JiCheChangeMainStep(eJiChe_MainFun_Voltage);
			break;
	}
}


// 一个循环250ms
void TASK_JiChe_MainFun(void)
{
	switch(ui16JiCheMainStep) {
		//<电压采样
		case eJiChe_MainFun_Voltage:
			OnJiChe_Voltage_Vcell();
			JiCheChangeMainStep(eJiChe_MainFun_Voltage_other);
			break;

		case eJiChe_MainFun_Voltage_other:
			OnJiChe_Voltage_other();
			JiCheChangeMainStep(eJiChe_MainFun_Current);
			break;

		//<电流采样
		//<温度采集	//周期性
		case eJiChe_MainFun_Current:
			OnJiChe_Current();
			OnJiChe_Temperature();
			JiCheChangeMainStep(eJiChe_MainFun_AlertInterruptMask);	
			break;
		//<中断查询告警//<目前做的是轮询查看告警信息
		//<MOS管状态更新//周期性
		//<充电检测//周期性
		//<负载检测//周期性，先不要
		// 那些状态全部放这里
		case eJiChe_MainFun_AlertInterruptMask:
			OnJiChe_AlertInterruptMask();
			OnJiChe_FETControl();
			OnJiChe_ChargerDetect();
			JiCheChangeMainStep(eJiChe_MainFun_CellBalance);			
			break;
		//<断线检测//周期性
		//<单体均衡	//周期性
		case eJiChe_MainFun_CellBalance:
			OnJiChe_BrokenLineDetect();
			OnJiChe_CellBalance();	
			JiCheChangeMainStep(eJiChe_MainFun_Voltage);			
			break;
		default :
			JiCheChangeMainStep(eJiChe_MainFun_Voltage);
			break;
	}
}


void TASK_JiChe_ERRFun(void)
{
	//<显示错误类型及计数值
	 PRINT_AFE_INFO("JiChe_Voltage_CRC_ERR=%x \r\n",ui16JiChe_Voltage_CRC_ERR);  
     PRINT_AFE_INFO("JiChe_Current_CRC_ERR=%x \r\n",ui16JiChe_Current_CRC_ERR);  
     PRINT_AFE_INFO("JiChe_Temperature_CRC_ERR=%x \r\n",ui16JiChe_Temperature_CRC_ERR);  
     PRINT_AFE_INFO("DaisyResetCnt=%x \r\n",ui16JiChe_ResetCnt); 
	//<将相关数据置位成默认值

}

/**************************************************************************
功能：		故障强制切换状态机，重启，发送默认值
入参：		空
返回值：	空
**************************************************************************/

void JiChe_FaultDispose(void)
{
    //<更新通信错误计数值
     PRINT_AFE_INFO("ui16JiCheRunStep=%d \r\n",ui16JiCheRunStep); 
	 PRINT_AFE_INFO("ui16JiCheMainStep=%d \r\n",ui16JiCheMainStep); 
     PRINT_AFE_INFO("JiChe_Voltage_CRC_ERR=%d \r\n",ui16JiChe_Voltage_CRC_ERR);  
     PRINT_AFE_INFO("JiChe_Current_CRC_ERR=%d \r\n",ui16JiChe_Current_CRC_ERR);  
     PRINT_AFE_INFO("JiChe_Temperature_CRC_ERR=%d \r\n",ui16JiChe_Temperature_CRC_ERR);  
	 PRINT_AFE_INFO("JiChe_Cfg_CRC_ERR=%d \r\n",ui16JiChe_Cfg_CRC_ERR);  
     PRINT_AFE_INFO("JiChe_ResetCnt=%d \r\n",ui16JiChe_ResetCnt); 
	
    AFE_Data_ALL.JiCheRunStep = ui16JiCheRunStep;
	AFE_Data_ALL.JiCheMainStep = ui16JiCheMainStep;
	AFE_Data_ALL.JiChe_Voltage_CRC_ERR = ui16JiChe_Voltage_CRC_ERR;
	AFE_Data_ALL.JiChe_Current_CRC_ERR = ui16JiChe_Current_CRC_ERR;
	AFE_Data_ALL.JiChe_Temperature_CRC_ERR = ui16JiChe_Temperature_CRC_ERR;
	AFE_Data_ALL.JiChe_Cfg_CRC_ERR = ui16JiChe_Cfg_CRC_ERR;
	AFE_Data_ALL.JiChe_ResetCnt =ui16JiChe_ResetCnt;
	
	
    //如果连续出现100次写入采集指令失败则重新初始化
    if((ui16JiChe_Voltage_CRC_ERR> 100)
       || (ui16JiChe_Current_CRC_ERR> 100)
       || (ui16JiChe_Temperature_CRC_ERR > 100)
	   ||(ui16JiChe_Cfg_CRC_ERR > 100)
		)
    {
        JiCheChangeRunStep(eJiChe_RunFun_Init);
        ui16JiChe_ResetCnt++;

        //故障计数器清零
		ui16JiChe_Voltage_CRC_ERR = 0;
		ui16JiChe_Current_CRC_ERR = 0;
		ui16JiChe_Temperature_CRC_ERR = 0;
		ui16JiChe_Cfg_CRC_ERR = 0;
    }
       
    //初始化超过十次且超过10s
//    if((ui16JiChe_ResetCnt >= JICHE_RESET_THRESHOLD) && (TMR_GetRunSecond() > 10))
	//初始化超过十次
	if((ui16JiChe_ResetCnt >= JICHE_RESET_THRESHOLD))
    {
        ui16JiChe_ResetCnt = JICHE_RESET_THRESHOLD;
		JiCheChangeRunStep(eJiChe_RunFun_ERR);
    }
	
	

}

#include "sys_timer.h"
#include "uart_upper.h"
#include "io_define.h"
#include "short_current.h"

void TASK_JiChe_APP(void)
{
	if(0 == g_st_SysTimeFlag.bits.b1Sys10msFlag2 || 1 == gu8_TxEnable_SCI1 || 1 == gu8_TxEnable_SCI2\
		|| 1 == gu8_TxEnable_SCI3) {
		return;
	}
	
	JiChe_FaultDispose();

	switch(ui16JiCheRunStep)
	{
		//<初始状态
		case eJiChe_RunFun_Init:
			TASK_JiChe_InitFun();
			break;
		//<运行状态
		case eJiChe_RunFun_Work:
			//JiCheChangeRunStep(eJiChe_RunFun_Work);
			TASK_JiChe_MainFun();
			break;
		//<EER状态,进入ERR状态死等
		case eJiChe_RunFun_ERR:
			TASK_JiChe_ERRFun();
			JiCheChangeRunStep(eJiChe_RunFun_ERR);
			break;
		default :
			JiCheChangeRunStep(eJiChe_RunFun_Init);
		break;
	}


}
