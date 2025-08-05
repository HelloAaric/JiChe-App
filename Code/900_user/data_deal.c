#include "data_deal.h"
#include "parameter_save.h"
#include "uart_upper.h"
#include "sys_timer.h"
#include "System_Monitor.h"
#include "sci_com.h"
#include "pub_func.h"
// #include "afe_deal.h"
#include "flash.h"
#include "JiChe_APP.h"
#include "adc_deal.h"

UINT8 u8IICFaultcnt1 = 0;
UINT8 u8WakeCnt1 = 0;
UINT8 u8IICFaultcnt2 = 0;
UINT8 u8WakeCnt2 = 0;

UINT16	g_u16CalibCoefK[KB_NUM];
INT16	g_i16CalibCoefB[KB_NUM];

UINT16 CopperLoss[CompensateNUM];		//u��
UINT16 CopperLoss_Num[CompensateNUM];

UINT32 g_u32CS_Res_AFE = 0;
UINT8 SeriesNum = 24;

#define LENGTH_TBLTEMP_AFE_10K    		((UINT16)56)
const UINT16 iSheldTemp_10K_AFE[LENGTH_TBLTEMP_AFE_10K] = {
    //AD		(Temp+40)*10
    16379	,	100	,	//-30
    12195	,	150	,	//-25
    9172 	,	200	,	//-20
    6966 	,	250	,	//-15
    5338 	,	300	,	//-10
    4122 	,	350	,	//-5
    3204 	,	400	,	//0
    2511 	,	450	,	//5
    1978 	,	500	,	//10
    1567 	,	550	,	//15
    1249 	,	600	,	//20
    1000 	,	650	,	//25
    806  	,	700	,	//30
    652      ,	750	,	//35
    531      ,	800	,	//40
    434      ,	850	,	//45
    357      ,	900	,	//50
    295   	,	950	,	//55
    245  	,	1000,	//60
    205  	,	1050,	//65
    172  	,	1100,	//70
    145  	,	1150,	//75
    122   	,	1200,	//80
    104  	,	1250,	//85
    89   	,	1300,	//90
    76   	,	1350,	//95
    65   	,	1400,	//100
    56   	,	1450,	//105
};

struct OTHER_ELEMENT OtherElement;

//�������кþ��У�����Ҫ���λ��ӳ���>61000Ϊ����
//�������㣬AFE1У׼һ�Σ�Ȼ������У׼һ�ε����ǿ��Եġ�����Ҫȷ��ĳһ��KBֵ��������
//������ȷ����AFE1���Ǳ����KB�Ļ�����������⡣���£�
//������Ҫ����У׼���У�AFE1���У�Ȼ����ĳ���������⣬����ʹ�ñ���KBֵ��Ȼ����KBֵ��Ҫͬ��ǰ��AFE1��KBֵһ�������
//����ֱ�ɵ���ʹ�ñ���KBֵУ׼�����ִ���
void DataLoad_CellVolt(void)
{
	UINT8 	i;
    INT32  t_i32temp = 0;
    UINT32  t_u32temp = 0;

	static UINT16 su16_occur_Tcnt = 0;
	static UINT16 su16_Recover_Cnt = 0;


	for(i = 0; i < SeriesNum; ++i) {
		t_i32temp = (UINT32)AFE_Data_ALL.CellVolt[i];
		if(g_u16CalibCoefK[VOLT_AFE1] != 1024 || g_i16CalibCoefB[VOLT_AFE1] != 0) {
            t_i32temp = ((t_i32temp * g_u16CalibCoefK[VOLT_AFE1]) >> 10) + g_i16CalibCoefB[VOLT_AFE1];
	    }
		t_i32temp = ((t_i32temp * g_u16CalibCoefK[i])>>10) + g_i16CalibCoefB[i];
		t_i32temp = t_i32temp > 0 ? t_i32temp : 0;
		g_stCellInfoReport.u16VCell[i] = (UINT16)t_i32temp;
	}

	if(SeriesNum < 32) { 
		for(i = SeriesNum; i < 32; ++i) {
			g_stCellInfoReport.u16VCell[i] = 61001;
		}
	}

	if(g_stCellInfoReport.u16Ichg > 0) {
		for (i = 0; i < CompensateNUM; ++i) {
			if(CopperLoss_Num[i] == 0) {
				break;
			}
			t_i32temp = (UINT32)CopperLoss[i]*g_stCellInfoReport.u16Ichg;
			g_stCellInfoReport.u16VCell[CopperLoss_Num[i]-1] -= (UINT16)(((t_i32temp>>14)+(t_i32temp>>15)+(t_i32temp>>17))&0xFFFF);
		}
	}
	else if(g_stCellInfoReport.u16IDischg > 0) {
		for (i = 0; i < CompensateNUM; ++i) {
			if(CopperLoss_Num[i] == 0) {
				break;
			}
			t_i32temp = (UINT32)CopperLoss[i]*g_stCellInfoReport.u16IDischg;
			g_stCellInfoReport.u16VCell[CopperLoss_Num[i]-1] += (UINT16)(((t_i32temp>>14)+(t_i32temp>>15)+(t_i32temp>>17))&0xFFFF);
		}
	}

	// һ����
	t_u32temp = (AFE_Data_ALL.BrokenLineDetect.All << (32 - SeriesNum)) ;
	if(AFE_Data_ALL.BrokenLineDetect.All) {
		if(su16_occur_Tcnt++ >= 20*3) {
			su16_occur_Tcnt = 0;
			System_ERROR_UserCallback(ERROR_APP);
		}
		su16_Recover_Cnt = 0;
	} else {
		if(System_ERROR_UserCallback(ERROR_STATUS_APP)) {
			if(++su16_Recover_Cnt >= 20 * 10) {		// �ж�50���Զ���ԭ��ԼΪ200*50=10s
				su16_Recover_Cnt = 0;
				System_ERROR_UserCallback(ERROR_REMOVE_APP);
			}
		}
		su16_occur_Tcnt = 0;
	}
}

void DataLoad_CellVoltMaxMinFind(void) {
	UINT8	i;
	UINT16	t_u16VcellTemp;
	UINT16	t_u16VcellMaxTemp;
	UINT16	t_u16VcellMinTemp;
	UINT8	t_u8VcellMaxPosition;
	UINT8	t_u8VcellMinPosition;
	UINT32  u32VCellTotle;

	t_u16VcellMaxTemp	= 0;
	t_u16VcellMinTemp	= 0x7FFF;
	t_u8VcellMaxPosition = 0;
	t_u8VcellMinPosition = 0;
	u32VCellTotle = 0;

	for(i = 0; i < SeriesNum; i++) {
		t_u16VcellTemp	=  g_stCellInfoReport.u16VCell[i];
		u32VCellTotle += g_stCellInfoReport.u16VCell[i];
		if(t_u16VcellMaxTemp < t_u16VcellTemp) {
			t_u16VcellMaxTemp = t_u16VcellTemp;
			t_u8VcellMaxPosition = i;
		}
		if(t_u16VcellMinTemp > t_u16VcellTemp) {
			t_u16VcellMinTemp = t_u16VcellTemp;
			t_u8VcellMinPosition = i;
		}
	}
	
	//��Ƭ������ѹ
	//u32VCellTotle = ((g_i32ADCResult[ADC_VBC]*g_u16CalibCoefK[VOLT_VBUS])>>10) + (UINT32)g_i16CalibCoefB[VOLT_VBUS]*1000;
	//AFE����ѹ
	//u32VCellTotle = ((g_stBq769x0_Read_AFE1.u32VBat*g_u16CalibCoefK[VOLT_VBUS])>>10) + (UINT32)g_i16CalibCoefB[VOLT_VBUS]*1000;
	//���е��ڵ�ص�ѹ������
	u32VCellTotle = ((u32VCellTotle*g_u16CalibCoefK[VOLT_VBUS])>>10) + (UINT32)g_i16CalibCoefB[VOLT_VBUS]*1000;

	g_stCellInfoReport.u16VCellpack = AFE_Data_ALL.VPACK / 10;
	g_stCellInfoReport.u16VCellTotle = (UINT16)((u32VCellTotle*1638>>14)&0xFFFF);	// ����10
	g_stCellInfoReport.u16VCellMax = t_u16VcellMaxTemp; 							// max cell voltage
	g_stCellInfoReport.u16VCellMin	= t_u16VcellMinTemp;							// min cell voltage
	g_stCellInfoReport.u16VCellDelta = t_u16VcellMaxTemp - t_u16VcellMinTemp;		// delta cell voltage
	g_stCellInfoReport.u16VCellMaxPosition = t_u8VcellMaxPosition + 1;				// max cell voltage
	g_stCellInfoReport.u16VCellMinPosition	= t_u8VcellMinPosition + 1; 			// min cell voltage
}


/*�����������������⣬�����>>��������ȼ��ͱ�ķ������ȼ�������
  ��������ȼ�̫���ҵ����������������
   (UINT16)(t_i32temp/100) ��
	(UINT16)(t_i32temp)/100��һ��
*/
void DataLoad_Temperature(void) {
	UINT8 i;
	INT32 t_i32temp;
	UINT8 Select;
	UINT16 ntc_temp2 = 0;


	Select = 4;

	for(i = 0; i < Select; i++) {
		ntc_temp2 = AFE_Data_ALL.CellTemp[i] + 400;
		// t_i32temp = GetEndValue(iSheldTemp_10K_AFE, (UINT16)LENGTH_TBLTEMP_AFE_10K, ntc_temp2);
		t_i32temp = (INT32)ntc_temp2/10 - 40;
		t_i32temp = ((t_i32temp * g_u16CalibCoefK[MDL_TEMP4+i]) + g_i16CalibCoefB[MDL_TEMP4+i])>>10;
		g_stCellInfoReport.u16Temperature[i] = (UINT16)(t_i32temp*10 + 400);
		
		Monitor_TempBreak(&g_stCellInfoReport.u16Temperature[i]);
	}

	// ���������¶ȷ���env��
	t_i32temp = gi32_ADCResult1[ADC_TEMP_EV1];
	t_i32temp = t_i32temp / 10 - 40;
	t_i32temp = ((t_i32temp * g_u16CalibCoefK[MDL_TEMP_ENV1]) + g_i16CalibCoefB[MDL_TEMP_ENV1])>>10;
	g_stCellInfoReport.u16Temperature[ENV_TEMP1] = (UINT16)(t_i32temp*10 + 400);
	Monitor_TempBreak(&g_stCellInfoReport.u16Temperature[ENV_TEMP1]);

	t_i32temp = gi32_ADCResult1[ADC_TEMP_EV2];
	t_i32temp = t_i32temp / 10 - 40;
	t_i32temp = ((t_i32temp * g_u16CalibCoefK[MDL_TEMP_ENV2]) + g_i16CalibCoefB[MDL_TEMP_ENV2])>>10;
	g_stCellInfoReport.u16Temperature[ENV_TEMP2] = (UINT16)(t_i32temp*10 + 400);
	Monitor_TempBreak(&g_stCellInfoReport.u16Temperature[ENV_TEMP2]);
}


UINT8 gu8_TcellMaxPosition = 0;
UINT8 gu8_TcellMinPosition = 0;
void DataLoad_TemperatureMaxMinFind(void)
{
	UINT8	i;
	UINT16	t_u16VcellTemp;
	UINT16	t_u16VcellMaxTemp;
	UINT16	t_u16VcellMinTemp;
	t_u16VcellMaxTemp = 0;
	t_u16VcellMinTemp = 0x7FFF;

	UINT8 t_u8TcellMaxPosition = 0;
	UINT8 t_u8TcellMinPosition = 0;


	//��������������¶ȣ����Ϊ9���
	for(i = 0; i < 8; i++) {		//Mos��������
		if(g_stCellInfoReport.u16Temperature[i] == 0) {
			continue;
		}
		t_u16VcellTemp	=  g_stCellInfoReport.u16Temperature[i];
		if(t_u16VcellMaxTemp < t_u16VcellTemp) {
            t_u16VcellMaxTemp = t_u16VcellTemp;
			t_u8TcellMaxPosition = i;
        }
		if(t_u16VcellMinTemp > t_u16VcellTemp) {
            t_u16VcellMinTemp = t_u16VcellTemp;
			t_u8TcellMinPosition = i;
        }
	}
	g_stCellInfoReport.u16TempMax = t_u16VcellMaxTemp;				// max temp
	g_stCellInfoReport.u16TempMin = t_u16VcellMinTemp;				// min temp

	gu8_TcellMaxPosition = t_u8TcellMaxPosition;
	gu8_TcellMinPosition = t_u8TcellMinPosition;
}

//��AFE1����AD����������˵

extern UINT16 gu16_Rs_uO;
UINT16 tempAD_AD5;
void DataLoad_Current(void)
{
	INT32 i32_CaliCurAD = 0;

	// i32_CaliCurAD = AFE1_DataTemp.PackCurrent;	// uV
	// ��ֵΪ�ŵ磬��ֵΪ��硣
	// gu16_Rs_uO
	i32_CaliCurAD = AFE_Data_ALL.I_CC1;				// uV

	if(i32_CaliCurAD < 0) {
		tempAD_AD5 = (UINT16)(-i32_CaliCurAD);
	} else {
		tempAD_AD5 = i32_CaliCurAD;
	}
	// i32_CaliCurAD = i32_CaliCurAD * OtherElement.u16Sys_CS_Res_Num / OtherElement.u16Sys_CS_Res;	// mA
	// i32_CaliCurAD = i32_CaliCurAD / (gu16_Rs_uO/1000);	// A
	// i32_CaliCurAD = i32_CaliCurAD * 1000 / (gu16_Rs_uO/1000);	// mA
	i32_CaliCurAD = i32_CaliCurAD * 1000 / gu16_Rs_uO;	// mA

	if(i32_CaliCurAD < 0) {
		/* ��������2aУ׼��bֵ */
		if(i32_CaliCurAD <= -2000) {
			i32_CaliCurAD = ((i32_CaliCurAD * g_u16CalibCoefK[MDL_IDSG])) + (INT32)g_i16CalibCoefB[MDL_IDSG]*1000;
		} else {
			/* ����С��2aУ׼��bֵ */
			i32_CaliCurAD = ((i32_CaliCurAD * 1024));
		}
	} else {
		/* ��������2aУ׼��bֵ */
		if(i32_CaliCurAD >= 2000) {
			i32_CaliCurAD = ((i32_CaliCurAD * g_u16CalibCoefK[MDL_ICHG])) + (INT32)g_i16CalibCoefB[MDL_ICHG]*1000;
		} else {
			/* ����С��2aУ׼��bֵ */
			i32_CaliCurAD = ((i32_CaliCurAD * 1024));
		}
	}
	i32_CaliCurAD = i32_CaliCurAD >> 10;

	if(i32_CaliCurAD > 0) {
		g_stCellInfoReport.u16Ichg = (UINT16)(i32_CaliCurAD/100);
		g_stCellInfoReport.u16IDischg = 0;
	}
	else {
		g_stCellInfoReport.u16Ichg = 0;
		//��������з��������޷��ŵ�ת�����ͷ�Ϊ���ࣺ
		//һ����ǿ��ת�����ͣ�����ֵ����(������洢��ֵ��-2�洢Ϊ0xFFFE)���Ӹ����ű��,����0xFFFE��ǿ��ת��Ϊ�з���������-2����ת����65534��
		//�������������ⲿ��������������;�Ϊ�޷������ͣ�Ȼ����ǿ��ת��������
		//�ǵô洢�ı����ĵ�λ������Ҫ�ĵ�λ����Ϊ������и���ʽת������a(UINT16)=-1�������Ȼ����65535
		//һ���Ǽ��㣬ȡ����ֵ֮��ģ�ֱ�Ӱ���������ѧ�߼������ɣ�����-2����Ҫȡ����ֵ��ֱ��-(-2)���
		g_stCellInfoReport.u16IDischg = (UINT16)((-i32_CaliCurAD)/100);
	}

	if(gu16_Small_DsgCur > 0 && gu16_Small_DsgCur <= 3000) {
		// g_stCellInfoReport.u16IDischg = gu16_Small_DsgCur/100;
	}
}

UINT16 a1 = 0;
UINT16 a2 = 0;
UINT16 a3 = 0;
UINT16 a4 = 0;
void FlashTest(void)
{
	static UINT16 su16_Tcnt = 0;
	// static UINT16 su16_Num = 0;


	if(++su16_Tcnt < 20) {
		return;
	}

	su16_Tcnt = 0;
}

// #include "adc_deal.h"
extern UINT16 tempAD_AD1;
extern UINT16 tempAD_AD2;
extern UINT16 tempAD_AD3;
extern UINT16 tempAD_AD4;
extern UINT16 CCR2_Val;
// extern __IO UINT16	gu16_ADCValFilter1[ADC_NUM];
// extern UINT16 test_sleep;
extern UINT16 test_sleep[4];
void DataLoad_Test(void)
{
	#if 0
	int i = 0;
	for(i = 0; i < SeriesNum; ++i) {
		g_stCellInfoReport.u16VCell[i] = 1280;
	}
	g_stCellInfoReport.u16Temperature[0] = 700;
	g_stCellInfoReport.u16Temperature[1] = 700;
	g_stCellInfoReport.u16Temperature[2] = 700;

	// g_stCellInfoReport.u16Ichg = 80;
	// g_stCellInfoReport.u16IDischg = 400;
	#endif
	// g_stCellInfoReport.u16VCell[28] = test_sleep[0];
	// g_stCellInfoReport.u16VCell[29] = test_sleep[1];
	// g_stCellInfoReport.u16VCell[30] = test_sleep[2];
	// g_stCellInfoReport.u16VCell[31] = test_sleep[3];

	g_stCellInfoReport.u16Temperature[0] = 700;
	g_stCellInfoReport.u16Temperature[1] = 700;
	g_stCellInfoReport.u16Temperature[2] = 700;
	g_stCellInfoReport.u16Temperature[3] = 700;
	g_stCellInfoReport.u16Temperature[4] = 700;
	g_stCellInfoReport.u16Temperature[5] = 700;
	g_stCellInfoReport.u16Temperature[6] = 700;
	g_stCellInfoReport.u16Temperature[7] = 700;

	#if 1
	g_stCellInfoReport.u16VCell[24] = AFE_Data_ALL.BrokenLineDetect.All;
	g_stCellInfoReport.u16VCell[25] = AFE_Data_ALL.BrokenLineDetect.All>>16;
	// g_stCellInfoReport.u16VCell[25] = CCR2_Val;
	g_stCellInfoReport.u16VCell[26] = tempAD_AD1;
	g_stCellInfoReport.u16VCell[27] = tempAD_AD2;
	g_stCellInfoReport.u16VCell[28] = tempAD_AD3;
	g_stCellInfoReport.u16VCell[29] = tempAD_AD4;
	g_stCellInfoReport.u16VCell[30] = tempAD_AD5;
	g_stCellInfoReport.u16VCell[31] = gu16_Small_DsgCur;
	#endif

	g_stCellInfoReport.u16VCell[29] = AFE_Data_ALL.FET_Status.PDSGF;
	g_stCellInfoReport.u16VCell[30] = AFE_Data_ALL.FET_Status.CHGF;
	g_stCellInfoReport.u16VCell[31] = AFE_Data_ALL.FET_Status.DSGF;

	// FlashTest();

	// g_stCellInfoReport.u16Ichg = 0;
	// g_stCellInfoReport.u16IDischg = 0;
}

void App_AFE_DataDeal(void)
{
	if(0 == g_st_SysTimeFlag.bits.b1Sys50msFlag || 1 == gu8_TxEnable_SCI1 || 1 == gu8_TxEnable_SCI2\
		|| 1 == gu8_TxEnable_SCI3) {
		return;
	}
	
	if(u32E2P_Pro_VolCur_WriteFlag!=0 || u32E2P_Pro_Temp_WriteFlag!=0 || u32E2P_Pro_Other_WriteFlag!=0\
		|| u32E2P_OtherElement1_WriteFlag!=0 || u32E2P_RTC_Element_WriteFlag!=0 || u8E2P_SocTable_WriteFlag!=0\
		|| u8E2P_CopperLoss_WriteFlag!=0 || u8E2P_KB_WriteFlag!=0) {
		return;
	}

    DataLoad_CellVolt();
    DataLoad_CellVoltMaxMinFind();
    // DataLoad_Temperature();
    DataLoad_TemperatureMaxMinFind();
	DataLoad_Current();
	
	#if 0
	DataLoad_Test();
	DataLoad_CellVoltMaxMinFind();
	DataLoad_TemperatureMaxMinFind();
	#endif
	DataLoad_Test();
    DataLoad_TemperatureMaxMinFind();

}

