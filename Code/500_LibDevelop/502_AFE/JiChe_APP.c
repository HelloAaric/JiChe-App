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
#define JICHE_RESET_THRESHOLD			(10)	//<��λ10�α�ͨѶʧ��
// #define  CurrentSenseResistance_mR      0.25            //������������ֵ����λmR)
#define  CurrentSenseResistance_mR      1            //������������ֵ����λmR)

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

sAFE_Data AFE_Data_ALL;	//<AFE���������ݻ��ܽӿ�

//extern TAFERegs g_AfeRegs;	//AFE�ļĴ�����������ݽӿ�

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
//	UN_CSC_STA uiDaisyStatus;							//�ջ�������״̬��
//	UINT16 uiCommStatus;								//ͨ��״̬: 0=��ʼ����1=ͨ��������2=ͨ��ʧ��(�����߸澯)
//	UINT16 uiResetCnt;      							// �ջ�����λ����
//	UINT16 uiCfgIdErrCntALL;
//	UINT16 uiDaisyWriteCmdErrCntAll;
//	UINT16 uiDaisySaveVoltErrCntAll;					
//	UINT16 uiDaisySaveGpioErrCntAll;
//	UINT16 uiDaisySaveTempErrCntAll;
//	UINT16 uiDaisySaveDiagErrCntAll;
//}ST_JICHE;

/**************************************************************************
���ܣ�		
��Σ�		
����ֵ��	��
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
	* @˵��	������о���
	* @����	
	* @����ֵ	
	* @ע	
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
	//��ȡ��ؼĴ������ݵ�ȫ�ֽṹ�����g_AfeRegs
	//Read IIC from R0~R76, totally 77 registers.
	
#ifdef CellColse
	CellTestClose(0x00004);//R106[7:0]~R108[7:4]
#endif
	if(DVC11XX_ReadRegs(AFE_ADDR_R(0), 110))
	{
		//������������о�ĵ�ѹֵ			
		for(i = 0; i < AFE_MAX_CELL_CNT; i++)
		{
			uwVCell = DVC11XX_Calc_VCell(i); 

			PRINT_AFE_INFO("Cell#%d  vol=%d mV\r\n", i+1,uwVCell);//������о��ѹ��ӡ��UART1
			
			AFE_Data_ALL.CellVolt[i] = uwVCell;	//���ɼ�ת��У׼�����ѹ�����AFE�����ݼ���
		}
		//������ذ���ѹ
		uwVBAT= DVC11XX_Calc_VBAT();
		PRINT_AFE_INFO("VBAT = %u mV\r\n", uwVBAT);
		AFE_Data_ALL.VBAT = uwVBAT;
		
		//����PACK��ѹ
		uwVPACK= DVC11XX_Calc_VPACK();
		PRINT_AFE_INFO("VPACK = %u mV\r\n", uwVPACK);
		AFE_Data_ALL.VPACK = uwVPACK;
		
		//����LOAD��ѹ
		uwVLOAD= DVC11XX_Calc_VLOAD();
		PRINT_AFE_INFO("VLOAD = %u mV\r\n", uwVLOAD);
		AFE_Data_ALL.VLOAD = uwVLOAD;
			
		//����V1P8��ѹ
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
	//��ȡ��ؼĴ������ݵ�ȫ�ֽṹ�����g_AfeRegs
	//Read IIC from R0~R76, totally 77 registers.

	if(DVC11XX_ReadRegs(AFE_ADDR_R(29), 48))
	{
		//������������о�ĵ�ѹֵ			
		for(i = 0; i < AFE_MAX_CELL_CNT; i++)
		{
			uwVCell = DVC11XX_Calc_VCell(i); 

			PRINT_AFE_INFO("Cell#%d  vol=%d mV\r\n", i+1,uwVCell);//������о��ѹ��ӡ��UART1
			
			AFE_Data_ALL.CellVolt[i] = uwVCell;	//���ɼ�ת��У׼�����ѹ�����AFE�����ݼ���
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
	//��ȡ��ؼĴ������ݵ�ȫ�ֽṹ�����g_AfeRegs
	//Read IIC from R0~R76, totally 77 registers.
	
	if(DVC11XX_ReadRegs(AFE_ADDR_R(0), 17))
	{
		//������ذ���ѹ
		uwVBAT= DVC11XX_Calc_VBAT();
		PRINT_AFE_INFO("VBAT = %u mV\r\n", uwVBAT);
		AFE_Data_ALL.VBAT = uwVBAT;
		
		//����PACK��ѹ
		uwVPACK= DVC11XX_Calc_VPACK();
		PRINT_AFE_INFO("VPACK = %u mV\r\n", uwVPACK);
		AFE_Data_ALL.VPACK = uwVPACK;
		
		//����LOAD��ѹ
		uwVLOAD= DVC11XX_Calc_VLOAD();
		PRINT_AFE_INFO("VLOAD = %u mV\r\n", uwVLOAD);
		AFE_Data_ALL.VLOAD = uwVLOAD;
			
		//����V1P8��ѹ
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
		//��IIC�ӿڶ�ȡAFE�Ĵ���ֵ��R2~R6����ȫ�ֽṹ�����g_AfeRegs�У����ں��������������ֵ��
		if(DVC11XX_ReadRegs(AFE_ADDR_R(2), 6))
		{
			//��ȡ��ؼĴ������ݵ�ȫ�ֽṹ�����g_AfeRegs
			//ͨ��������CC1�Ĳ���ֵ��������� 
			fCurrent = DVC11XX_Calc_CurrentWithCC1(CurrentSenseResistance_mR); 
			// PRINT_AFE_INFO("Current with CC1=%.3f A\r\n", fCurrent);//������ֵ��ӡ��UART1
			AFE_Data_ALL.I_CC1 = fCurrent;
			
			//ͨ��������CC2�Ĳ���ֵ���������
			fCurrent = DVC11XX_Calc_CurrentWithCC2(CurrentSenseResistance_mR); 
			// PRINT_AFE_INFO("Current with CC2=%.3f A\r\n", fCurrent);//������ֵ��ӡ��UART1 
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
	InterruptShield_Config(0x1F);//ʹ�ܸ������ж�
	if(DVC11XX_ReadRegs(AFE_ADDR_R(0), 1))
	{		
		//��ѯ�жϱ�����Ϣ
		if(g_AfeRegs.R0.bitmap.SCD==1)	//��ӡ��Ӧ����
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
	CleanError();//��������
	#endif

	if(DVC11XX_ReadRegs(AFE_ADDR_R(0), 1)) {		
		//��ѯ�жϱ�����Ϣ
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
	* @˵��	�������ܲ�������
	* @����	
	* @����ֵ	
	* @ע	
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
	* @˵��	�������ܲδ���
	* @����	
	* @����ֵ	
	* @ע	
*/
void Protection_handle(void)
{
	if(DVC11XX_ReadRegs(AFE_ADDR_R(0), 1))	//��ѯ������Ϣ
	{
		//��ӡ��Ӧ����
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
	CleanError();//��������
}

void OnJiChe_Protection(void)
{
	Protection_Config();//��������ȫ����������Ϊ�ϵ���ֵ��
	//��Ҫ����ʱ���ȴ����������ɺ��ȡ���
	Protection_handle();
	delay_ms(3000);
}

void OnJiChe_ChargerDetect(void)
{
	#if 0
	if(DVC11XX_ReadRegs(AFE_ADDR_R(1), 1))	//��ѯ���������
	{
//		ChargerDetect();//��������
		if(g_AfeRegs.R1.PD==1)//��ӡ��Ӧ����
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

	if(DVC11XX_ReadRegs(AFE_ADDR_R(1), 1)) {	//��ѯ���������
		AFE_Data_ALL.ChargeDetect = g_AfeRegs.R1.PD;
	}

}

void OnJiChe_LoadDetect(void)
{
	if(DVC11XX_ReadRegs(AFE_ADDR_R(11),72))
	{//��ȡ����

		if(g_AfeRegs.R81.LDPU==0)//����δ����
		{
			LoadPullUp();//ʹ�ܸ�������
		}
		else
		{
//			LoadDetect();//���ؼ��
			u16 Vload;	
	
			Vload=DVC11XX_Calc_VLOAD();//����Vload��ѹ
			PRINT_AFE_INFO("Vload = %u mV\r\n", Vload);

			if(Vload >= LoadDetect_Threshold)//�Ƚ���ֵ2V
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
	{//��ѯ���߼��ʹ��λ

		if(g_AfeRegs.R109.COW==0)
		{//���߼��ʹ���Ѹ�λ

			COW_enable();//ʹ�ܶ��߼��

			if(DVC11XX_ReadRegs(AFE_ADDR_R(29), 47))//��ȡ��ظ�����ѹ
			{
//				BrokenLineDetect();//���߼�������
				u8 i;
				int uw_cellvotage[AFE_MAX_CELL_CNT];
	
				for(i=0;i<AFE_MAX_CELL_CNT;i++)
				{
					uw_cellvotage[i]=DVC11XX_Calc_VCell(i);//���������ѹ

					if(uw_cellvotage[i]==0)//���ߴ���ӡ���
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
				if(g_AfeRegs.R109.COW == 0) {									// ���߼��ʹ���Ѹ�λ
					// ʹ�ܶ��߼��
					COW_enable();
				}
			}
			su8_startUp = 1;
			break;

		case 1:
			// ��ѯ���߼��ʹ��λ
			if(DVC11XX_ReadRegs(AFE_ADDR_R(109), 1)) {
				if(g_AfeRegs.R109.COW == 0) {									// ���߼��ʹ���Ѹ�λ
					for(i = 0; i < AFE_MAX_CELL_CNT; i++) {						// ���߼�������
						if(AFE_Data_ALL.CellVolt[i] == 0) {					// ���ߴ���ӡ���
							PRINT_AFE_INFO("CELL %d Broken!\r\n",i+1);
							AFE_Data_ALL.BrokenLineDetect.All |= (1<<i);
						}
					}
					// ʹ�ܶ��߼��
					COW_enable();
				}
			}
			break;
			
		default:
			break;
	}
}

bool	wakeup_flag=0;	//���Ѱ�����־
bool 	sleep_flag=0;		//˯�߰�����־
bool  INT_flag=0;			//GP�жϱ�־
/**
	* @˵��	˯�ߴ�����
	* @����	
	* @����ֵ	
	* @ע	
*/
void Sleep_Handle(void){

	if(wakeup_flag)
	{//���»��Ѱ���

		if(DVC11XX_ReadRegs(AFE_ADDR_R(0), 1))
		{//IICͨ�Ż���
			wakeup_flag=0;//���ѱ�־��λ
		}
	}

	if(sleep_flag)
	{//����˯�߰���

		g_AfeRegs.R1.CST=0xE;//����˯��
		if(DVC11XX_WriteRegs(AFE_ADDR_R(1), 1))
		{//д��Ĵ���
			PRINT_AFE_INFO("AFE Going To Sleep!\r\n");
			sleep_flag=0;//˯�߱�־��λ
		}
	}
		
	if(INT_flag==1)
	{//�յ�AFE�����ж�
		INT_flag=0;
		if(DVC11XX_ReadRegs(AFE_ADDR_R(1), 1))
		{
			AFE_Data_ALL.WakeUpSourceDetect = g_AfeRegs.R1.CST;
			switch(g_AfeRegs.R1.CST)
			{
				case 0:PRINT_AFE_INFO("AFE Wake Up by Power\r\n");
						break;//�ض�ģʽ����
				case 1:PRINT_AFE_INFO("AFE Wake Up by IIC\r\n");
						break;//IIC����
				case 2:PRINT_AFE_INFO("AFE Wake Up by Timed\r\n");	
						break;//��ʱ����				
				case 3:PRINT_AFE_INFO("AFE Wake Up by Discharge\r\n");
						break;//�ŵ绽��
				case 4:PRINT_AFE_INFO("AFE Wake Up by Charge\r\n");
						break;//��绽��
				case 5:PRINT_AFE_INFO("AFE Wake Up by OCD2\r\n");
						break;//�����ŵ��������
				case 6:PRINT_AFE_INFO("AFE Wake Up by OCC2\r\n");
						break;//��������������
				case 7:PRINT_AFE_INFO("AFE Wake Up by SCD\r\n");
						break;//�ŵ��·����
				case 8:PRINT_AFE_INFO("AFE Wake Up by Charger\r\n");
						break;//���������
				default :PRINT_AFE_INFO("Interrupt Trigger Up by Jitter\r\n");
						break;//�ж�Դ����
			}
		}
	}
}

void OnJiChe_Sleep(void)
{
	Sleep_config();//˯��ģʽ��ʼ��
	Sleep_Handle();//�ضϴ���
	delay_ms(1000);
}

/**
	* @˵��	AFE��ʼ������д��
	* @����	�Ĵ����Զ������Ĭ��ֵ
	* @����ֵ	�ɹ�/ʧ��
	* @ע	
*/
static bool DVC11XX_InitRegs(void){
	memcpy((char *)&g_AfeRegs+81,DVC11XX_PresetConfigRegData_R81To121,sizeof(DVC11XX_PresetConfigRegData_R81To121));
	return DVC11XX_WriteRegs(AFE_ADDR_R(81),sizeof(DVC11XX_PresetConfigRegData_R81To121));
}

/**
	* @˵��	AFEоƬ���³�ʼ��
	* @����	
	* @����ֵ	�ɹ�/ʧ��
	* @ע	
*/
bool DVC11XX_InitRegs_Twice(void)
{
	return DVC11XX_InitRegs();
}

bool shutdown_flag=0;
/**
	* @˵��	�ضϴ�����
	* @����	
	* @����ֵ	
	* @ע	
*/
void Shutdown_Handle(void)
{

	if(wakeup_flag)
	{//���»��Ѱ���
		IIC_Wakeup();//����IIC�����ź�
		IIC_Init();//����IICͨ��
		if(DVC11XX_ReadRegs(AFE_ADDR_R(1), 1))
		{//��ȡ��AFE״̬			
			DVC11XX_InitRegs_Twice();//���³�ʼ��AFE
			CleanError();//�������
			wakeup_flag=0;//���ѱ�־��λ
			PRINT_AFE_INFO("AFE Wake Up by IIC\r\n");	
		}
	}
		
	if(shutdown_flag)
	{//���¹ضϰ���
		Shutdown();
		PRINT_AFE_INFO("AFE Shutdown!\r\n");
		shutdown_flag=0;//�ضϱ�־��λ
	}
}

void OnJiChe_Shutdown(void)
{
	Shutdown_Handle();//�ضϴ���
	delay_ms(1000);
}

void OnJiChe_CellBalance(void)
{
	#if 0
	if(DVC11XX_ReadRegs(AFE_ADDR_R(13),98))
	{//��ȡ������R108.CVS

		OverTempProtect(100);//���±��� ��ֵ100���϶�

		CalcuVolMaxMin();//��ѹ�����Сֵ����
		
		if(0 == AFE_Data_ALL.CellBalanceMode)
		{
			BalanceProcess();//�Զ����⴦��
		}
		else
		{
			Balance_Contrl(AFE_Data_ALL.CellsBalancCmd.All);	//<�ֶ����ƾ���
		}
		
		AFE_Data_ALL.CellsBalancStatus.All = (g_AfeRegs.R103_R105.CB[0]<<16)+(g_AfeRegs.R103_R105.CB[1]<<8)+g_AfeRegs.R103_R105.CB[2];//ˢ�µ�ǰ����λ
	}
	delay_ms(3000);
	#endif

	if(DVC11XX_ReadRegs(AFE_ADDR_R(103), 3)) {		//��ȡ������R108.CVS
		AFE_Data_ALL.CellsBalancStatus.All = (g_AfeRegs.R103_R105.CB[0]<<16)+(g_AfeRegs.R103_R105.CB[1]<<8)+g_AfeRegs.R103_R105.CB[2];//ˢ�µ�ǰ����λ
	}
}

/**
	* @˵��	�߱�������������
	* @����	
	* @����ֵ	
	* @ע	������ֵ=BDPT*40uV
*/
void FET_Config(void){
		
//	DSGM_Control(DSGM_CP);//���ɱ�
//	DPC_Config(16);//����ǿ��16
//	BDPT_Config(80);//������ֵ40uV
//	ChargePump_Control(CP_10V);//10V
//		
//	HSFM_Control(DISABLE);//����߱�����
//		
//	CHG_FETControl(FET_OPEN);//�򿪳��
//	DSG_FETControl(FET_Close_BFCO);
	
	DSGM_Control(AFE_Data_ALL.FET_Config.DSGM);//���ɱ�
	
	if(0 == AFE_Data_ALL.FET_Config.DPC)	//<���δ������Ĭ�ϲ���
	{
		DPC_Config(16);//����ǿ��16
	}
	else
	{
		DPC_Config(AFE_Data_ALL.FET_Config.DPC);
	}

	if(0 == AFE_Data_ALL.FET_Config.TH)	//<���δ������Ĭ�ϲ���
	{
		BDPT_Config(80);//������ֵ40uV
	}	
	else
	{
	BDPT_Config(AFE_Data_ALL.FET_Config.TH);//������ֵ40uV
	}
	
	if(0 == AFE_Data_ALL.FET_Config.CPM)
	{
		ChargePump_Control(CP_10V);//10V
	}
	else
	{
	ChargePump_Control(AFE_Data_ALL.FET_Config.CPM);//10V
	}
		
	HSFM_Control(AFE_Data_ALL.FET_Config.HSFM);//����߱�����
		
//	CHG_FETControl(AFE_Data_ALL.FET_Control.CHG);//�򿪳��
//	DSG_FETControl(AFE_Data_ALL.FET_Control.DSG);
}

//<����AFE�ĳ�ŵ�����״̬
void OnJiChe_FETControl(void)
{
	#if 0
	FET_Config();//FET����
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
//	GPM_Temperature();//GP1�ܽ�ģʽ����
	
	Read_NFRT();//��ȡ�����޵�ֵ
		
//	delay_ms(200);//�ȴ�GP1����
	
	if(DVC11XX_ReadRegs(AFE_ADDR_R(13), 16))
	{//��ȡ��ؼĴ������ݵ�ȫ�ֽṹ�����g_AfeRegs			
			
		//����оƬ�¶�ֵ			
		scChipTemp = DVC11XX_Calc_ChipTemp();
		PRINT_AFE_INFO("Chip temperature = %.1f \r\n", scChipTemp);//��ӡоƬ�¶�
		AFE_Data_ALL.ChipIntTemp = 	scChipTemp*10;
		
		//�������¶�ֵ	
		scBatTemp = DVC11XX_Calc_BatTemp(GP1);
		PRINT_AFE_INFO("Battery temperature1 = %.1f \r\n", scBatTemp);//��ӡ����¶�
		AFE_Data_ALL.CellTemp[0] = 	(int16_t)scBatTemp*10;

		//�������¶�ֵ	
		scChipTemp = DVC11XX_Calc_BatTemp(GP2);
		PRINT_AFE_INFO("Battery temperature2 = %.1f \r\n", scChipTemp);//��ӡ����¶�
		AFE_Data_ALL.CellTemp[1] = 	(int16_t)scChipTemp*10;
		
		//�������¶�ֵ	
		scChipTemp = DVC11XX_Calc_BatTemp(GP3);
		PRINT_AFE_INFO("Battery temperature3 = %.1f \r\n", scChipTemp);//��ӡ����¶�
		AFE_Data_ALL.CellTemp[2] = 	(int16_t)scChipTemp*10;
		
		//�������¶�ֵ	
		scChipTemp = DVC11XX_Calc_BatTemp(GP4);
		PRINT_AFE_INFO("Battery temperature4 = %.1f \r\n", scChipTemp);//��ӡ����¶�
		AFE_Data_ALL.CellTemp[3] = 	(int16_t)scChipTemp*10;
		
		//�������¶�ֵ	
		scChipTemp = DVC11XX_Calc_BatTemp(GP5);
		PRINT_AFE_INFO("Battery temperature5 = %.1f \r\n", scChipTemp);//��ӡ����¶�
		AFE_Data_ALL.CellTemp[4] = 	scChipTemp*10;
		
		//�������¶�ֵ	
		scChipTemp = DVC11XX_Calc_BatTemp(GP6);
		PRINT_AFE_INFO("Battery temperature6 = %.1f \r\n", scChipTemp);//��ӡ����¶�
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
	* @˵��	GP�Ÿ�������
	* @����	bool ģʽ
	* @����ֵ	
	* @ע	
*/
void GPn_ModeConfig(bool mode){

//	GPIO_Ini_Config();//GPIO��ʼ��

	memset((u8 *)&g_AfeRegs+116,0,2);//GPn����λ ��ʼ��Ϊ0

	if(!mode)
	{//ģʽA
//			EXTI4_initial();//MCU�ⲿ�ж�
//			GP1_ModeConfig(0x03);//GP1 CHGӲ�߿���
//			GP2_ModeConfig(0x06);//GP2 INT
//			GP3_ModeConfig(0x02);//GP3 ģ������
//			GP4_ModeConfig(0x03);//GP4 DSGӲ�߿���
//			GP5_ModeConfig(0x07);//GP5 CHG_LS
//			GP6_ModeConfig(0x07);//GP6 DSG_LS
//			INT_Ctrl(0xE0);//GP�жϴ���ѡ��
		
			GP1_ModeConfig(0x01);
			GP2_ModeConfig(0x01);
			GP3_ModeConfig(0x01);
			GP4_ModeConfig(0x01);
			GP5_ModeConfig(0x01);
			GP6_ModeConfig(0x01);
	}
	else
	{//ģʽB
			GP1_ModeConfig(0x01);
			GP2_ModeConfig(0x01);
			GP3_ModeConfig(0x01);
			GP4_ModeConfig(0x01);
			GP5_ModeConfig(0x07);
			GP6_ModeConfig(0x07);
	}	
	FET_Open();//�򿪳�ŵ�
}


void OnJiChe_GPn(void)
{
	GPn_ModeConfig(MODE);//GP�����������
	//��ȡ��FET״̬
	if(DVC11XX_ReadRegs(AFE_ADDR_R(6), 1))
	{
		GPnDemo_Cal();
	}

	delay_ms(1000);
}


//static unsigned char Read_Back_R81To121[41];
bool	rst_flag=0;	//��λ������־

void OnJiChe_Reset(void)
{
//	int i,uwVCell;
//	u32 uwVBAT,uwVPACK,uwVLOAD;
//	u16 uwV1P8;
//	
//		if(rst_flag==1)
//		{	//���¸�λ����
//			if(AFE_RST())
//			{
//				PRINT_AFE_INFO("Reset Complete \r\n");
//			}

//			if(DVC11XX_ReadRegs(AFE_ADDR_R(81), 41))
//			{//�ض����üĴ���
//				memcpy(Read_Back_R81To121,(char *)&g_AfeRegs+81,41);//�ض�ת��
//			
//				for(i=0;i<41;i++)
//				{//��ӡ��ʼ����ֵ���ض���ֵ
//					PRINT_AFE_INFO("Reg Number %x\r\n",i+81);
//					PRINT_AFE_INFO("		Init Array Value  = %x\r\n",DVC11XX_PresetConfigRegData_R81To121[i]);
//					PRINT_AFE_INFO("		Reset Array Value = %x\r\n",Read_Back_R81To121[i]);
//				}
//			}
//				
//			rst_flag=0;//��λ״̬��0
//			delay_ms(1000);
//		}
//		else if(DVC11XX_ReadRegs(AFE_ADDR_R(0), 110))
//		{
//			//������������о�ĵ�ѹֵ			
//			for(i=0;i<AFE_MAX_CELL_CNT;i++)
//			{
//				uwVCell = DVC11XX_Calc_VCell(i); 

//				PRINT_AFE_INFO("Cell#%d  vol=%d mV\r\n", i+1,uwVCell);//������о��ѹ��ӡ��UART1

//			}
//			//������ذ���ѹ
//			uwVBAT= DVC11XX_Calc_VBAT();
//			PRINT_AFE_INFO("VBAT = %u mV\r\n", uwVBAT);
//					
//			//����PACK��ѹ
//			uwVPACK= DVC11XX_Calc_VPACK();
//			PRINT_AFE_INFO("VPACK = %u mV\r\n", uwVPACK);
//					
//			//����LOAD��ѹ
//			uwVLOAD= DVC11XX_Calc_VLOAD();
//			PRINT_AFE_INFO("VLOAD = %u mV\r\n", uwVLOAD);
//					
//			//����V1P8��ѹ
//			uwV1P8= DVC11XX_Calc_V1P8();
//			PRINT_AFE_INFO("V1P8 = %u mV\r\n", uwV1P8);
//						
//			delay_ms(1000);
//		}
			}

/**
	* @˵��	ϵͳ��ʼ��
	* @����	
	* @����ֵ	
	* @ע	
*/

void TASK_JiChe_InitFun(void)
{	
	// IIC_Wakeup();				//5.����AFEоƬ
	IIC_Init();       				//6. ��ʼ��IICͨ�Žӿ�(MCU��AFE��ͨ�Žӿڣ�
	AFE_RST();						//7.��λAFE
	delay_ms(300);    				//8. ��ʱ256ms��ȷ��AFEоƬ�������һ�����ڵĵ�ѹ�͵���ֵ�Ĳ�����
	#if ADDRESS>=0xC0
	GPIO_INTPUT();
	#elif ADDRESS==0x40
	IIC_SLAVE_ADDRESS=ADDRESS>>1;
	#endif	
	if(DVC11XX_InitRegs()) 			//<AFEоƬ�Ĵ�����ʼ����д���û��Զ�����������ݣ�
	{
		Protection_Config();		//��������ȫ����������Ϊ�ϵ���ֵ��
		// FET_Config();			//FET����
		GPn_ModeConfig(MODE);		//GP����������ã�����Ϊ��ż̽ͷ
		
		CleanError();				//д��Ĭ��ֵ���������	//<�˺����ڲ�����ѭ������ע��

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
		//<��ѹ����
		case eJiChe_MainFun_Voltage:
			OnJiChe_Voltage();
			JiCheChangeMainStep(eJiChe_MainFun_Current);
		break;
		//<��������
		case eJiChe_MainFun_Current:
			OnJiChe_Current();
//			JiCheChangeMainStep(eJiChe_MainFun_AlertInterruptMask);
			JiCheChangeMainStep(eJiChe_MainFun_Protection);	
		break;
		//<�жϲ�ѯ�澯//<Ŀǰ��������ѯ�鿴�澯��Ϣ
		case eJiChe_MainFun_AlertInterruptMask:
//			OnJiChe_AlertInterruptMask();
			JiCheChangeMainStep(eJiChe_MainFun_Protection);			
		break;
		//<������������//�¼���
		case eJiChe_MainFun_Protection:
			OnJiChe_Protection();
			JiCheChangeMainStep(eJiChe_MainFun_ChargerDetect);		
		break;
		//<�����//������
		case eJiChe_MainFun_ChargerDetect:
			OnJiChe_ChargerDetect();
			JiCheChangeMainStep(eJiChe_MainFun_LoadDetect);	
		break;
		//<���ؼ��//������
		case eJiChe_MainFun_LoadDetect:
			OnJiChe_LoadDetect();
			JiCheChangeMainStep(eJiChe_MainFun_BrokenLineDetect);	
		break;
		//<���߼��//������
		case eJiChe_MainFun_BrokenLineDetect:
			OnJiChe_BrokenLineDetect();
			JiCheChangeMainStep(eJiChe_MainFun_CellBalance);
//			JiCheChangeMainStep(eJiChe_MainFun_Sleep);	
		break;
		//<���ߣ�������//�¼���
		case eJiChe_MainFun_Sleep:
//			OnJiChe_Sleep();
//			JiCheChangeMainStep(eJiChe_MainFun_Shutdown);	
		break;
		//<�ػ�//�¼���
		case eJiChe_MainFun_Shutdown:
//			OnJiChe_Shutdown();
//			JiCheChangeMainStep(eJiChe_MainFun_CellBalance);
		break;
		//<�������	//������
		case eJiChe_MainFun_CellBalance:
			OnJiChe_CellBalance();	
			JiCheChangeMainStep(eJiChe_MainFun_FETControl);			
		break;
		//<MOS��״̬����//������
		case eJiChe_MainFun_FETControl:
			OnJiChe_FETControl();
			JiCheChangeMainStep(eJiChe_MainFun_Temperature);	
			break;
		//<�¶Ȳɼ�	//������
		case eJiChe_MainFun_Temperature:
			OnJiChe_Temperature();
			JiCheChangeMainStep(eJiChe_MainFun_Voltage);
//			JiCheChangeMainStep(eJiChe_MainFun_GPn);				
		break;
		//<GPIO����	//������
		case eJiChe_MainFun_GPn:
//			OnJiChe_GPn();
//			JiCheChangeMainStep(eJiChe_MainFun_Reset);	
		break;
		//<��λ	//�¼���
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
		//<��ѹ����
		case eJiChe_MainFun_Voltage:
			OnJiChe_Voltage_Vcell();
			JiCheChangeMainStep(eJiChe_MainFun_Voltage_other);
			break;

		case eJiChe_MainFun_Voltage_other:
			OnJiChe_Voltage_other();
			JiCheChangeMainStep(eJiChe_MainFun_Current);
			break;

		//<��������
		case eJiChe_MainFun_Current:
			OnJiChe_Current();
//			JiCheChangeMainStep(eJiChe_MainFun_AlertInterruptMask);
			JiCheChangeMainStep(eJiChe_MainFun_Protection);	
			break;
		//<�жϲ�ѯ�澯//<Ŀǰ��������ѯ�鿴�澯��Ϣ
		case eJiChe_MainFun_AlertInterruptMask:
//			OnJiChe_AlertInterruptMask();
			JiCheChangeMainStep(eJiChe_MainFun_Protection);			
			break;
		//<������������//�¼���
		case eJiChe_MainFun_Protection:
			// OnJiChe_Protection();
			JiCheChangeMainStep(eJiChe_MainFun_ChargerDetect);		
			break;
		//<�����//������
		case eJiChe_MainFun_ChargerDetect:
			OnJiChe_ChargerDetect();
			JiCheChangeMainStep(eJiChe_MainFun_LoadDetect);	
			break;
		//<���ؼ��//������
		case eJiChe_MainFun_LoadDetect:
			// OnJiChe_LoadDetect();
			JiCheChangeMainStep(eJiChe_MainFun_BrokenLineDetect);	
			break;
		//<���߼��//������
		case eJiChe_MainFun_BrokenLineDetect:
			OnJiChe_BrokenLineDetect();
			JiCheChangeMainStep(eJiChe_MainFun_CellBalance);
//			JiCheChangeMainStep(eJiChe_MainFun_Sleep);	
			break;
		//<���ߣ�������//�¼���
		case eJiChe_MainFun_Sleep:
//			OnJiChe_Sleep();
//			JiCheChangeMainStep(eJiChe_MainFun_Shutdown);	
			break;
		//<�ػ�//�¼���
		case eJiChe_MainFun_Shutdown:
//			OnJiChe_Shutdown();
//			JiCheChangeMainStep(eJiChe_MainFun_CellBalance);
			break;
		//<�������	//������
		case eJiChe_MainFun_CellBalance:
			OnJiChe_CellBalance();	
			JiCheChangeMainStep(eJiChe_MainFun_FETControl);			
			break;
		//<MOS��״̬����//������
		case eJiChe_MainFun_FETControl:
			// OnJiChe_FETControl();
			JiCheChangeMainStep(eJiChe_MainFun_Temperature);	
			break;
		//<�¶Ȳɼ�	//������
		case eJiChe_MainFun_Temperature:
			OnJiChe_Temperature();
			JiCheChangeMainStep(eJiChe_MainFun_Voltage);
//			JiCheChangeMainStep(eJiChe_MainFun_GPn);				
			break;
		//<GPIO����	//������
		case eJiChe_MainFun_GPn:
//			OnJiChe_GPn();
//			JiCheChangeMainStep(eJiChe_MainFun_Reset);	
			break;
		//<��λ	//�¼���
		case eJiChe_MainFun_Reset:
//			OnJiChe_Reset();
//			JiCheChangeMainStep(eJiChe_MainFun_Voltage);	
			break;
		default :
			JiCheChangeMainStep(eJiChe_MainFun_Voltage);
			break;
	}
}


// һ��ѭ��250ms
void TASK_JiChe_MainFun(void)
{
	switch(ui16JiCheMainStep) {
		//<��ѹ����
		case eJiChe_MainFun_Voltage:
			OnJiChe_Voltage_Vcell();
			JiCheChangeMainStep(eJiChe_MainFun_Voltage_other);
			break;

		case eJiChe_MainFun_Voltage_other:
			OnJiChe_Voltage_other();
			JiCheChangeMainStep(eJiChe_MainFun_Current);
			break;

		//<��������
		//<�¶Ȳɼ�	//������
		case eJiChe_MainFun_Current:
			OnJiChe_Current();
			OnJiChe_Temperature();
			JiCheChangeMainStep(eJiChe_MainFun_AlertInterruptMask);	
			break;
		//<�жϲ�ѯ�澯//<Ŀǰ��������ѯ�鿴�澯��Ϣ
		//<MOS��״̬����//������
		//<�����//������
		//<���ؼ��//�����ԣ��Ȳ�Ҫ
		// ��Щ״̬ȫ��������
		case eJiChe_MainFun_AlertInterruptMask:
			OnJiChe_AlertInterruptMask();
			OnJiChe_FETControl();
			OnJiChe_ChargerDetect();
			JiCheChangeMainStep(eJiChe_MainFun_CellBalance);			
			break;
		//<���߼��//������
		//<�������	//������
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
	//<��ʾ�������ͼ�����ֵ
	 PRINT_AFE_INFO("JiChe_Voltage_CRC_ERR=%x \r\n",ui16JiChe_Voltage_CRC_ERR);  
     PRINT_AFE_INFO("JiChe_Current_CRC_ERR=%x \r\n",ui16JiChe_Current_CRC_ERR);  
     PRINT_AFE_INFO("JiChe_Temperature_CRC_ERR=%x \r\n",ui16JiChe_Temperature_CRC_ERR);  
     PRINT_AFE_INFO("DaisyResetCnt=%x \r\n",ui16JiChe_ResetCnt); 
	//<�����������λ��Ĭ��ֵ

}

/**************************************************************************
���ܣ�		����ǿ���л�״̬��������������Ĭ��ֵ
��Σ�		��
����ֵ��	��
**************************************************************************/

void JiChe_FaultDispose(void)
{
    //<����ͨ�Ŵ������ֵ
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
	
	
    //�����������100��д��ɼ�ָ��ʧ�������³�ʼ��
    if((ui16JiChe_Voltage_CRC_ERR> 100)
       || (ui16JiChe_Current_CRC_ERR> 100)
       || (ui16JiChe_Temperature_CRC_ERR > 100)
	   ||(ui16JiChe_Cfg_CRC_ERR > 100)
		)
    {
        JiCheChangeRunStep(eJiChe_RunFun_Init);
        ui16JiChe_ResetCnt++;

        //���ϼ���������
		ui16JiChe_Voltage_CRC_ERR = 0;
		ui16JiChe_Current_CRC_ERR = 0;
		ui16JiChe_Temperature_CRC_ERR = 0;
		ui16JiChe_Cfg_CRC_ERR = 0;
    }
       
    //��ʼ������ʮ���ҳ���10s
//    if((ui16JiChe_ResetCnt >= JICHE_RESET_THRESHOLD) && (TMR_GetRunSecond() > 10))
	//��ʼ������ʮ��
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
		//<��ʼ״̬
		case eJiChe_RunFun_Init:
			TASK_JiChe_InitFun();
			break;
		//<����״̬
		case eJiChe_RunFun_Work:
			//JiCheChangeRunStep(eJiChe_RunFun_Work);
			TASK_JiChe_MainFun();
			break;
		//<EER״̬,����ERR״̬����
		case eJiChe_RunFun_ERR:
			TASK_JiChe_ERRFun();
			JiCheChangeRunStep(eJiChe_RunFun_ERR);
			break;
		default :
			JiCheChangeRunStep(eJiChe_RunFun_Init);
		break;
	}


}
