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
    InitDevice();          // 初始化外设，这两个函数的位置需要斟酌一下，现在换回去先
    InitVar();             // 初始化变量

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
		 	1. 时序问题
			2. 导致按钮很难进去休眠
			3. L0的优化不能采集到数据，这个条件下屏蔽一些函数减少flash就行，原因不明
			4. 是否建议用模拟I2C？
		*/
		TASK_JiChe_APP();
		App_BalanceProcess();
		App_AFE_DataDeal();
		App_ParaSave_Flash();

		App_WarnCtrl();
		App_ADC();
		App_MOS_Relay_Ctrl();
		App_SOC(); 
		// App_SleepDeal();		//放在App_MOS_Relay_Control()后面
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

//这个初始化函数很容易出问题
void InitDevice(void)
{
	SystemInit();           // RCC相关，这个可以屏蔽，开机执行 Reset_Handler 会执行这个函数。
							// N32GH474CEL7 最大200MHz
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

	// IsSleepStartUp();	// 这个函数成为历史
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
	// InitShortCur();			// 放到AFE函数里面

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
