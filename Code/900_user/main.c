#include "main.h"
#include "io_ctrl.h"
#include "sys_timer.h"
#include "wdg.h"
#include "spi.h"
#include "sci_com.h"
#include "sys_pwr.h"
#include "JiChe_APP.h"
#include "parameter_save.h"
#include "drivers_ctrl.h"
#include "soc_user.h"
#include "Fault.h"
#include "log_record.h"
#include "ProductionID.h"
#include "charger_loader_func.h"
#include "sleep_user.h"
#include "heat_cool.h"
#include "CBC.h"
#include "can_com.h"
#include "System_Monitor.h"

#include "parameter_save_flash.h"
#include "pwm.h"
#include "adc_deal.h"
#include "balance.h"
#include "short_current.h"

void InitVar(void);
void InitDevice(void);

UINT8 NOR_VALUE = 0;
UINT16 gu16_Rs_uO = 0;


int main(void)
{
    InitDevice();          // ��ʼ�����裬������������λ����Ҫ����һ�£����ڻ���ȥ��
    InitVar();             // ��ʼ������

    while(1) {
		#if (defined _DEBUG_CODE)
		App_SysTime();
		App_SysPower();
		// App_Sci();
		// App_ParaSave_Flash();
		// // TASK_JiChe_APP();
		// App_AFE_DataDeal();
		// App_ADC();

		// App_CanCom();
		// App_SleepDeal();
		// App_Sys_FlashUpdate();
		Feed_IWatchDog;

		#else

		App_SysTime();
		App_Sci();
		/*
		 	1. ʱ������
			2. ���°�ť���ѽ�ȥ����
			3. L0���Ż����ܲɼ������ݣ��������������һЩ��������flash���У�ԭ����
			4. �Ƿ�����ģ��I2C��
		*/
		TASK_JiChe_APP();
		App_BalanceProcess();
		App_AFE_DataDeal();
		App_ParaSave_Flash();

		App_WarnCtrl();
		App_ADC();
		App_MOS_Relay_Ctrl();
		App_SOC(); 
		// App_SleepDeal();		//����App_MOS_Relay_Control()����
		// App_LogRecord();
		 
		App_ChargerLoad_Det();
		App_Heat_Cool_Ctrl();
		App_CanCom();
		App_SysPower();
		App_CBC();
		App_ShortCurDeal_AFE();

		App_Sys_FlashUpdate();
		//App_NormalSleepTest();
		Feed_IWatchDog;
		#endif
    }
}

//�����ʼ�����������׳�����
void InitDevice(void)
{
	SystemInit();           // RCC��أ�����������Σ�����ִ�� Reset_Handler ��ִ�����������
							// N32GH474CEL7 ���200MHz
							// HCLK = AHB = SYSCLK = 200MHz
							// PCLK2 = PCLK1 = APB1 = APB2 = 100MHz

	#if (defined _DEBUG_CODE)
	InitIO();
	InitSys_Power();
	InitTimer();

	// InitPara_Flash();
	// InitSci();
	// InitData_SleepFunc();
	// InitPWM();
	// InitADC();
	// Init_IWDG();
	// Init_CanCom();
	#else

	// IsSleepStartUp();	// ���������Ϊ��ʷ
	InitIO();
	InitSys_Power();
	InitPara_Flash();

	InitTimer();
	InitADC();
	InitPWM();
	// InitSPI();
	InitSci();
	InitData_SOC();
	InitHeat_Cool();
	InitMosRelay_DOx();
	Init_ChargerLoad_Det();
	Init_CanCom();
	InitData_SleepFunc();
	
	// TASK_JiChe_InitFun();
	// InitShortCur();			// �ŵ�AFE��������

	// Init_IWDG();
	#endif
}

void InitVar(void)
{
	// UINT16 i;
	InitSystemMonitorData_EEPROM();

	SystemStatus.bits.b1StartUpBMS = 0;

	SeriesNum = OtherElement.u16Sys_SeriesNum;
	// gu16_Rs_uO = OtherElement.u16Sys_CS_Res*1000 / OtherElement.u16Sys_CS_Res_Num;
}


void InitSystemWakeUp(void)
{


}
