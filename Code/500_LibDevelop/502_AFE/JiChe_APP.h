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
//<����������״̬
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




//<����������״̬
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
	u8 DSGM:1;	//DSG Output MODE, 0����ɱã�1����Դ
	u8 CPM:4;	//��ɱ�ģʽ��0���رյ�ɱã�1����ɱ�6V��2����ɱ�7V��3����ɱ�8V��4����ɱ�9V��5����ɱ�10V��6����ɱ�11V��7����ɱ�12V
	u8 RES:3;	//����
	u8 DPC;		//�߱�DSG����ǿ������, ǿ�ȵȼ� 0~30
	u16 TH;	//����������ֵ���� ������ֵ ��λ uV ������ֵ=BDPT*40uV
	bool HSFM;	//�߱�������������  0��/1��
}sFET_Config;

typedef struct 
{
	u8 DSG:2;	//0��δ���壻1���ر�FET������2���ر�FET�������������ڵ�������������ֵ����FET������3���Ͽ�FET����
	u8 CHG:2;	//0��δ���壻1���ر�FET������2���ر�FET�������������ڵ�������������ֵ����FET������3���Ͽ�FET����
	u8 PCHG:1;	//0������1����
	u8 PDSG:1;	//0������1����
	u8 RES:2;	//����
}sFET_Control;

typedef struct
{
	u8 CHGF:1; //CHG_FLAG, CHG���������ʶλ;0, CHG��������ѹر�;1, CHG��������ѿ���
    u8 DSGF:1; //DSG_FLAG, DSG���������ʶλ;0, DSG��������ѹر�;1, DSG��������ѿ���
    u8 PCHGF:1;//PCHG_FLAG, PCHG���������ʶλ;0, PCHG��������ѹر�;1, PCHG��������ѿ���
    u8 PDSGF:1;//PDSG_FLAG, PDSG���������ʶλ;0, PDSG��������ѹر�;1, PDSG��������ѿ���
	u8 RES:4;	//����
}sFET_Status;

typedef struct
{
	u16 OCD1_TH;	//��ֵ 250-63750uv ���� 250uv	//һ���ŵ��������
	u16 OCD1_DLY;	//��ʱ��8-2040ms ���� 8ms
	
	u16 OCC1_TH;	//��ֵ 250-63750uv ���� 250uv	//һ������������
	u16 OCC1_DLY;	//��ʱ�� 8-2040ms ���� 8ms
	
	u16 OCD2_TH;	//��ֵ 4-256mv ����4mv	//�����ŵ��������
	u16 OCD2_DLY;	//��ʱ��4-1020ms ����4ms
	bool OCD2_EN;	//ʹ�ܣ�0��ʹ�ܣ�1ʹ��
	
	u16 OCC2_TH;	//��ֵ 4-128mv ����4mv	//��������������
	u16 OCC2_DLY;	//��ʱ��4-1020ms ����4ms
	bool OCC2_EN;	//ʹ�ܣ�0��ʹ�ܣ�1ʹ��
	
	u16 UV_TH;		//��ֵ��1-4095mv ����1mv		//Ƿѹ����
	u16 UV_DLY;		//��ʱ��200-8000ms 200-1000����100, 1000-8000����1000
	bool UV_EN;		//ʹ�ܣ�0��ʹ�ܣ�1ʹ��
	
	u16 OV_TH;		//��ֵ��500-4595mv ����1mv	//��ѹ����
	u16 OV_DLY;		//��ʱ��200-8000ms 200-1000����100, 1000-8000����1000
	bool OV_EN;		//ʹ�ܣ�0��ʹ�ܣ�1ʹ��
	
	u16 SCD_TH;		//��ֵ 10-640mv ���� 10mv	//�ŵ��·����
	u16 SCD_DLY;	//��ʱ 0-1992us ���� 7.81us	
	u16 SCD_EN;		//ʹ�ܣ�0��ʹ�ܣ�1ʹ��
	
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
		u8 SCD:1;   //SCD_FLAG, �ŵ��·��ʶλ;0��δ�����ŵ��·;1���ѷ����ŵ��·������bit��0�������
		u8 OCC2:1;  //OCIC2_FLAG, 2����������ʶλ;0��δ����2��������;1���ѷ���2��������������bit��0�������
		u8 OCD2:1;  //OCID2_FLAG, 2���ŵ������ʶλ;0��δ����2���ŵ����;1���ѷ���2���ŵ����������bit��0�������
		u8 OCC1:1;  //OCIC1_FLAG, 1����������ʶλ;0��δ����1��������;1���ѷ���1��������������bit��0�������
		u8 OCD1:1;  //OCID1_FLAG, 1���ŵ������ʶλ;0��δ����1���ŵ����;1���ѷ���1���ŵ����������bit��0�������
		u8 CUV:1;   //CUV_FLAG, ���Ƿѹ����;0��δ�������Ƿѹ;1���ѷ������Ƿѹ������bit��0�������
		u8 COV:1;   //COV_FLAG, ��ع�ѹ��ʶλ;0��δ������ع�ѹ;1���ѷ�����ع�ѹ������bit��0�������
		u8 IWF:1;   //I2C_WDTO_FLAG, I2C���Ź������ʶλ;0, δ����I2C���Ź����;1, �ѷ���I2C���Ź����������bit��0�������
	}sBit;
}uWarningInfo;

//AFE�ɼ������⼰�����Ϣ�洢�ṹ�壨�������ݽӿڣ�
typedef struct
{
	uint16_t 			CellVolt[AFE_MAX_CELL_CNT];		//��λ: mV�����ڵ�ص�ѹֵ
	int16_t 			CellTemp[AFE_MAX_GP_CNT];		//��λ: 0.1��,�¶�ֵ
	float				ChipIntTemp;					//��λ: 0.1��,�¶�ֵ AFEоƬ�ڲ��¶�
	float 				I_CC1;							//ͨ��������CC1�Ĳ���ֵ������������ŵ縺�����������λuV
	float 				I_CC2;							//ͨ��������CC2�Ĳ���ֵ���������
	uint32_t    		VBAT; 							//��λ:mV, ��ذ���ѹ
	uint32_t 			VPACK;							//��λ:mV, PACK��ѹ
	uint32_t 			VLOAD;							//��λ:mV, LOAD��ѹ
	uint16_t 			V1P8;							//��λ:mV, V1.8��׼��ѹ
//	int32_t 			CellCurrent[AFE_MAX_CELL_CNT];	// ���ڵ�ص���ֵ������
	uBalance_Cmd		CellsBalancCmd;					//��������ͨ������ָ��
	uBalance_Status 	CellsBalancStatus;				//��������ͨ��״̬
	uint16_t 			CellBalanceMode;				//������ⷽʽ��0���Զ����⣬1���ֶ�����
	sFET_Config			FET_Config;						//����AFE��FET����
	sFET_Control		FET_Control;					//���Ƴ�ŵ�MOS
	sFET_Status			FET_Status;						//�ض�AFE�ĳ�ŵ�MOS״̬
	sProtection_Set 	ProtectionSet;					//���������ã��������1����������Ƿѹ���ŵ��·����
	bool 				ChargeDetect;					//��������,0��ʾδ��⵽�������1��ʾ��⵽�����
	bool 				LoadDetect;						//���ؼ�⣬0��ʾδ��⵽��1��ʾ��⵽����
	uint16_t			WakeUpSourceDetect;				//����Դ��⣺оƬ״̬��ʶλ;0000, оƬ�ӹػ�״̬������;0001, оƬ������״̬��I2Cͨ�Ż���;0010, оƬ������״̬����ʱ����;0011, оƬ������״̬���ŵ��������;0100, оƬ������״̬������������;0101, оƬ������״̬��2���ŵ��������;0110, оƬ������״̬��2������������;0111, оƬ������״̬���ŵ��·����;1000, оƬ������״̬�����������;1110, ʹоƬ��������״̬;1111, ʹоƬ����ض�״̬
	uBrokenLine_Status	BrokenLineDetect;				//���߼�⣺0��ʾ������1��ʾ�쳣�������ߣ�
	uWarningInfo 		Warning;						//�澯��Ϣ
	uint16_t			JiChe_Cfg_CRC_ERR;				//AFE��ʼ���������ֵ
	uint16_t 			JiCheRunStep;					//�������к���״̬����0����ʼ����1������״̬��2������״̬
	uint16_t 			JiCheMainStep;					//���������ܺ���״̬��
	uint16_t 			JiChe_Voltage_CRC_ERR;			//��ѹ�ɼ�ͨ�Ŵ������ֵ
	uint16_t			JiChe_Current_CRC_ERR;			//�����ɼ�ͨ�Ŵ������ֵ
	uint16_t			JiChe_Temperature_CRC_ERR;		//�¶Ȳɼ�ͨ�Ŵ������ֵ
	uint16_t			JiChe_ResetCnt;					//AFE��������
	uint16_t			JiChe_StartUp;					// AFE��ʼ���ɹ���־λ��1���ɹ�
}sAFE_Data;


extern sAFE_Data AFE_Data_ALL;	//<AFE���������ݻ��ܽӿ�

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
