/*
*
* file name:    JiChe_APP.h
* author:       X
* date:         2025.4.15
* version:      V1.0
*/
#ifndef		JICHE_APP_H
#define		JICHE_APP_H
/*
===================================================================
*                          Include Files                          *
===================================================================
*/
//#include "base_data_type.h"
#include "basic_data_type.h"
#include "DVC11XX.h"
#include "FETControl.h"
#include "GPn.h"
#include "Reset.h"
#include "Temperature.h"
#include "Voltage.h"
#include "Current.h"
#include "LoadDetect.h"
#include "Protection.h"
#include "Reset.h"
#include "Shutdown.h"
#include "Sleep.h"
#include "AlertInterruptMask.h"
#include "ChargerDetect.h"
#include "BrokenLineDetect.h"
#include "CellBalance.h"
#include "timer.h"
/*
===================================================================
*                    Defines/Macros/Structures                    *
===================================================================
*/
//<集澈功能主状态
#if 0
 typedef enum eJiChe_MainFun
{
	eJiChe_MainFun_Voltage 								= 0,
	eJiChe_MainFun_Current								= 1,
	eJiChe_MainFun_AlertInterruptMask	    			= 2,
	eJiChe_MainFun_Protection 							= 3,
	eJiChe_MainFun_ChargerDetect 						= 4,
	eJiChe_MainFun_LoadDetect 							= 5,
	eJiChe_MainFun_BrokenLineDetect						= 6,
	eJiChe_MainFun_Sleep								= 7,
	eJiChe_MainFun_Shutdown								= 8,	
	eJiChe_MainFun_CellBalance							= 9,	
	eJiChe_MainFun_FETControl					 		= 10,	
	eJiChe_MainFun_Temperature					 		= 11,	
	eJiChe_MainFun_GPn					 				= 12,	
	eJiChe_MainFun_Reset					 			= 13,	
	eJiChe_MainFun_Max,
}eJICHE_MAIN_FUN;
#endif

 typedef enum eJiChe_MainFun
{
	eJiChe_MainFun_Voltage,
	eJiChe_MainFun_Voltage_other,
	eJiChe_MainFun_Current,
	eJiChe_MainFun_AlertInterruptMask,
	eJiChe_MainFun_Protection,
	eJiChe_MainFun_ChargerDetect,
	eJiChe_MainFun_LoadDetect,
	eJiChe_MainFun_BrokenLineDetect,
	eJiChe_MainFun_Sleep,
	eJiChe_MainFun_Shutdown,	
	eJiChe_MainFun_CellBalance,	
	eJiChe_MainFun_FETControl,	
	eJiChe_MainFun_Temperature,	
	eJiChe_MainFun_GPn,	
	eJiChe_MainFun_Reset,	
	eJiChe_MainFun_Max,
}eJICHE_MAIN_FUN;




//<集澈运行主状态
 typedef enum eJiChe_RunFun
{
	eJiChe_RunFun_Init 										= 0,
	eJiChe_RunFun_Work										= 1,
	eJiChe_RunFun_ERR	   									= 2,
	eJiChe_RunFun_Max,
}eJICHE_RUN_FUN;


typedef union 
{
	uint32_t All;
	struct
	{
		uint32_t cell_01:1; 
		uint32_t cell_02:1; 
		uint32_t cell_03:1;
		uint32_t cell_04:1;
		uint32_t cell_05:1; 
		uint32_t cell_06:1; 
		uint32_t cell_07:1;
		uint32_t cell_08:1;
		uint32_t cell_09:1; 
		uint32_t cell_10:1; 
		uint32_t cell_11:1;
		uint32_t cell_12:1;
		uint32_t cell_13:1;
		uint32_t cell_14:1;
		uint32_t cell_15:1;
		uint32_t cell_16:1;
		uint32_t cell_17:1;
		uint32_t cell_18:1;
		uint32_t cell_19:1;
		uint32_t cell_20:1;
		uint32_t cell_21:1;
		uint32_t cell_22:1;
		uint32_t cell_23:1;
		uint32_t cell_24:1;
		u8 RES:8;	
	}sBit;
}uBalance_Status;

typedef union 
{
	uint32_t All;
	struct
	{
		uint32_t cell_01:1; 
		uint32_t cell_02:1; 
		uint32_t cell_03:1;
		uint32_t cell_04:1;
		uint32_t cell_05:1; 
		uint32_t cell_06:1; 
		uint32_t cell_07:1;
		uint32_t cell_08:1;
		uint32_t cell_09:1; 
		uint32_t cell_10:1; 
		uint32_t cell_11:1;
		uint32_t cell_12:1;
		uint32_t cell_13:1;
		uint32_t cell_14:1;
		uint32_t cell_15:1;
		uint32_t cell_16:1;
		uint32_t cell_17:1;
		uint32_t cell_18:1;
		uint32_t cell_19:1;
		uint32_t cell_20:1;
		uint32_t cell_21:1;
		uint32_t cell_22:1;
		uint32_t cell_23:1;
		uint32_t cell_24:1;
		u8 RES:8;	
	}sBit;
}uBalance_Cmd;

typedef struct 
{
	u8 DSGM:1;	//DSG Output MODE, 0：电荷泵；1：随源
	u8 CPM:4;	//电荷泵模式：0：关闭电荷泵，1：电荷泵6V，2：电荷泵7V，3：电荷泵8V，4：电荷泵9V，5：电荷泵10V，6：电荷泵11V，7：电荷泵12V
	u8 RES:3;	//保留
	u8 DPC;		//高边DSG下拉强度设置, 强度等级 0~30
	u16 TH;	//续流保护阈值设置 保护阈值 单位 uV 保护阈值=BDPT*40uV
	bool HSFM;	//高边驱动屏蔽设置  0开/1关
}sFET_Config;

typedef struct 
{
	u8 DSG:2;	//0：未定义；1：关闭FET驱动；2：关闭FET驱动，但允许在电流大于续流阈值开启FET驱动；3：断开FET驱动
	u8 CHG:2;	//0：未定义；1：关闭FET驱动；2：关闭FET驱动，但允许在电流大于续流阈值开启FET驱动；3：断开FET驱动
	u8 PCHG:1;	//0：开，1：关
	u8 PDSG:1;	//0：开，1：关
	u8 RES:2;	//保留
}sFET_Control;

typedef struct
{
	u8 CHGF:1; //CHG_FLAG, CHG驱动输出标识位;0, CHG驱动输出已关闭;1, CHG驱动输出已开启
    u8 DSGF:1; //DSG_FLAG, DSG驱动输出标识位;0, DSG驱动输出已关闭;1, DSG驱动输出已开启
    u8 PCHGF:1;//PCHG_FLAG, PCHG驱动输出标识位;0, PCHG驱动输出已关闭;1, PCHG驱动输出已开启
    u8 PDSGF:1;//PDSG_FLAG, PDSG驱动输出标识位;0, PDSG驱动输出已关闭;1, PDSG驱动输出已开启
	u8 RES:4;	//保留
}sFET_Status;

typedef struct
{
	u16 OCD1_TH;	//阈值 250-63750uv 步进 250uv	//一级放电过流配置
	u16 OCD1_DLY;	//延时：8-2040ms 步进 8ms
	
	u16 OCC1_TH;	//阈值 250-63750uv 步进 250uv	//一级充电过流配置
	u16 OCC1_DLY;	//延时： 8-2040ms 步进 8ms
	
	u16 OCD2_TH;	//阈值 4-256mv 步进4mv	//二级放电过流配置
	u16 OCD2_DLY;	//延时：4-1020ms 步进4ms
	bool OCD2_EN;	//使能：0不使能，1使能
	
	u16 OCC2_TH;	//阈值 4-128mv 步进4mv	//二级充电过流配置
	u16 OCC2_DLY;	//延时：4-1020ms 步进4ms
	bool OCC2_EN;	//使能：0不使能，1使能
	
	u16 UV_TH;		//阈值：1-4095mv 步进1mv		//欠压配置
	u16 UV_DLY;		//延时：200-8000ms 200-1000步进100, 1000-8000步进1000
	bool UV_EN;		//使能：0不使能，1使能
	
	u16 OV_TH;		//阈值：500-4595mv 步进1mv	//过压配置
	u16 OV_DLY;		//延时：200-8000ms 200-1000步进100, 1000-8000步进1000
	bool OV_EN;		//使能：0不使能，1使能
	
	u16 SCD_TH;		//阈值 10-640mv 步进 10mv	//放电短路配置
	u16 SCD_DLY;	//延时 0-1992us 步进 7.81us	
	u16 SCD_EN;		//使能：0不使能，1使能
	
}sProtection_Set;

//typedef union 
//{
//	u8 All;
//	struct
//	{
//		u8 WakeUp_ByPower:1; 
//		u8 WakeUp_ByIIC:1; 
//		u8 WakeUp_ByTime:1; 

//		u8 RES:8;	
//	}sBit;
//}uWakeUpSourceDetect;

typedef union 
{
	uint32_t All;
	struct
	{
		uint32_t cell_01:1; 
		uint32_t cell_02:1; 
		uint32_t cell_03:1;
		uint32_t cell_04:1;
		uint32_t cell_05:1; 
		uint32_t cell_06:1; 
		uint32_t cell_07:1;
		uint32_t cell_08:1;
		uint32_t cell_09:1; 
		uint32_t cell_10:1; 
		uint32_t cell_11:1;
		uint32_t cell_12:1;
		uint32_t cell_13:1;
		uint32_t cell_14:1;
		uint32_t cell_15:1;
		uint32_t cell_16:1;
		uint32_t cell_17:1;
		uint32_t cell_18:1;
		uint32_t cell_19:1;
		uint32_t cell_20:1;
		uint32_t cell_21:1;
		uint32_t cell_22:1;
		uint32_t cell_23:1;
		uint32_t cell_24:1;
		u8 RES:8;	
	}sBit;
}uBrokenLine_Status;

typedef union 
{
	u8 All;
	struct
	{
		u8 SCD:1;   //SCD_FLAG, 放电短路标识位;0，未发生放电短路;1，已发生放电短路，将该bit置0即可清除
		u8 OCC2:1;  //OCIC2_FLAG, 2级充电过流标识位;0，未发生2级充电过流;1，已发生2级充电过流，将该bit置0即可清除
		u8 OCD2:1;  //OCID2_FLAG, 2级放电过流标识位;0，未发生2级放电过流;1，已发生2级放电过流，将该bit置0即可清除
		u8 OCC1:1;  //OCIC1_FLAG, 1级充电过流标识位;0，未发生1级充电过流;1，已发生1级充电过流，将该bit置0即可清除
		u8 OCD1:1;  //OCID1_FLAG, 1级放电过流标识位;0，未发生1级放电过流;1，已发生1级放电过流，将该bit置0即可清除
		u8 CUV:1;   //CUV_FLAG, 电池欠压警告;0，未发生电池欠压;1，已发生电池欠压，将该bit置0即可清除
		u8 COV:1;   //COV_FLAG, 电池过压标识位;0，未发生电池过压;1，已发生电池过压，将该bit置0即可清除
		u8 IWF:1;   //I2C_WDTO_FLAG, I2C看门狗溢出标识位;0, 未发生I2C看门狗溢出;1, 已发生I2C看门狗溢出，将该bit置0即可清除
	}sBit;
}uWarningInfo;

//AFE采集、均衡及诊断信息存储结构体（用于数据接口）
typedef struct
{
	uint16_t 			CellVolt[AFE_MAX_CELL_CNT];		//单位: mV，单节电池电压值
	int16_t 			CellTemp[AFE_MAX_GP_CNT];		//单位: 0.1℃,温度值
	float				ChipIntTemp;					//单位: 0.1℃,温度值 AFE芯片内部温度
	float 				I_CC1;							//通过过滤器CC1的采样值来计算电流，放电负，充电正，单位uV
	float 				I_CC2;							//通过过滤器CC2的采样值来计算电流
	uint32_t    		VBAT; 							//单位:mV, 电池包总压
	uint32_t 			VPACK;							//单位:mV, PACK电压
	uint32_t 			VLOAD;							//单位:mV, LOAD电压
	uint16_t 			V1P8;							//单位:mV, V1.8基准电压
//	int32_t 			CellCurrent[AFE_MAX_CELL_CNT];	// 单节电池电流值，不管
	uBalance_Cmd		CellsBalancCmd;					//单个均衡通道均衡指令
	uBalance_Status 	CellsBalancStatus;				//单个均衡通道状态
	uint16_t 			CellBalanceMode;				//单体均衡方式：0：自动均衡，1：手动均衡
	sFET_Config			FET_Config;						//配置AFE的FET设置
	sFET_Control		FET_Control;					//控制充放电MOS
	sFET_Status			FET_Status;						//回读AFE的充放电MOS状态
	sProtection_Set 	ProtectionSet;					//保护点设置，过充过放1级二级，过欠压，放电短路保护
	bool 				ChargeDetect;					//充电器检测,0表示未检测到充电器，1表示检测到充电器
	bool 				LoadDetect;						//负载检测，0表示未检测到，1表示检测到负载
	uint16_t			WakeUpSourceDetect;				//唤醒源检测：芯片状态标识位;0000, 芯片从关机状态被唤醒;0001, 芯片从休眠状态被I2C通信唤醒;0010, 芯片从休眠状态被定时唤醒;0011, 芯片从休眠状态被放电电流唤醒;0100, 芯片从休眠状态被充电电流唤醒;0101, 芯片从休眠状态被2级放电过流唤醒;0110, 芯片从休眠状态被2级充电过流唤醒;0111, 芯片从休眠状态被放电短路唤醒;1000, 芯片从休眠状态被充电器唤醒;1110, 使芯片进入休眠状态;1111, 使芯片进入关断状态
	uBrokenLine_Status	BrokenLineDetect;				//断线检测：0表示正常，1表示异常（即断线）
	uWarningInfo 		Warning;						//告警信息
	uint16_t			JiChe_Cfg_CRC_ERR;				//AFE初始化错误计数值
	uint16_t 			JiCheRunStep;					//集澈运行函数状态机：0、初始化，1、运行状态，2、错误状态
	uint16_t 			JiCheMainStep;					//集测主功能函数状态机
	uint16_t 			JiChe_Voltage_CRC_ERR;			//电压采集通信错误计数值
	uint16_t			JiChe_Current_CRC_ERR;			//电流采集通信错误计数值
	uint16_t			JiChe_Temperature_CRC_ERR;		//温度采集通信错误计数值
	uint16_t			JiChe_ResetCnt;					//AFE重启次数
	uint16_t			JiChe_StartUp;					// AFE初始化成功标志位，1，成功
}sAFE_Data;


extern sAFE_Data AFE_Data_ALL;	//<AFE的所有数据汇总接口

void JiCheChangeRunStep(eJICHE_RUN_FUN eJiCheRunStep);
void JiCheChangeMainStep(eJICHE_MAIN_FUN eJiCheMainStep);

void CleanError(void);
void OnJiChe_Voltage(void);
void OnJiChe_Current(void);
void OnJiChe_AlertInterruptMask(void);
void Protection_Config(void);
void Protection_handle(void);
void OnJiChe_Protection(void);
void OnJiChe_ChargerDetect(void);
void OnJiChe_LoadDetect(void);
void OnJiChe_BrokenLineDetect(void);
void Sleep_Handle(void);
void OnJiChe_Sleep(void);

static bool DVC11XX_InitRegs(void);

bool DVC11XX_InitRegs_Twice(void);
void Shutdown_Handle(void);
void OnJiChe_Shutdown(void);
void OnJiChe_CellBalance(void);
void FET_Config(void);
void OnJiChe_FETControl(void);
void OnJiChe_Temperature(void);
void GPn_ModeConfig(bool mode);
void OnJiChe_GPn(void);
void OnJiChe_Reset(void);
void TASK_JiChe_InitFun(void);
void TASK_JiChe_ERRFun(void);
void JiChe_FaultDispose(void);

void TASK_JiChe_APP(void);

#endif
