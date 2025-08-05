#include "sleep_user.h"
#include "lib_sleepFunc.h"
#include "io_ctrl.h"
#include "sci_com.h"
#include "sys_timer.h"
#include "log_record.h"
#include "System_Monitor.h"
#include "EEPROM.h"

#include "io_define.h"
#include "misc.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_exti.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_pwr.h"
#include "n32h47x_48x_iwdg.h"
#include "n32h47x_48x_adc.h"

void SleepTest(void);

UINT8 RTC_ExtComCnt;


 
//延时n毫秒,48MHz
void delay_ms_sleep(unsigned int n)
{
    unsigned int i=0,j=0;
    // for(i=0;i<n*4;i++)
    for(i=0;i<n*100;i++)
        for(j=0;j<128;j++);
}
//通讯唤醒对深度休眠不起效果。不能再Base加入通讯唤醒。
void InitWakeUp_Base(void)
{
	GPIO_InitType InitStruct;
    EXTI_InitType EXTI_Struct;
    NVIC_InitType NVIC_Struct;

    InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    InitStruct.GPIO_Mode      = GPIO_MODE_INPUT;
    InitStruct.GPIO_Alternate = GPIO_NO_AF;
    InitStruct.GPIO_Pull      = GPIO_NO_PULL;
    InitStruct.GPIO_Current   = GPIO_DS_8mA;

	// DI1
    InitStruct.Pin            = DI1_PIN;
    GPIO_InitPeripheral(DI1_PORT, &InitStruct);
    EXTI_Struct.EXTI_Line    = DI1_EXTI_LINE;
    EXTI_Struct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_Struct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_Struct.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_Struct);
    NVIC_Struct.NVIC_IRQChannel = DI1_IRQN;
    NVIC_Struct.NVIC_IRQChannelSubPriority = NVIC_SUB_PRIORITY_0;
    NVIC_Struct.NVIC_IRQChannelPreemptionPriority = NVIC_PRE_PRIORITY_0;
    NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_Struct);
	GPIO_ConfigEXTILine(DI1_EXTI_LINE_SOURCE, DI1_EXTI_PORT, DI1_EXTI_PIN);

	// DI2
    InitStruct.Pin            = DI2_PIN;
    GPIO_InitPeripheral(DI2_PORT, &InitStruct);
    EXTI_Struct.EXTI_Line    = DI2_EXTI_LINE;
    EXTI_Struct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_Struct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_Struct.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_Struct);
    NVIC_Struct.NVIC_IRQChannel = DI2_IRQN;
    NVIC_Struct.NVIC_IRQChannelSubPriority = NVIC_SUB_PRIORITY_0;
    NVIC_Struct.NVIC_IRQChannelPreemptionPriority = NVIC_PRE_PRIORITY_0;
    NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_Struct);
	GPIO_ConfigEXTILine(DI2_EXTI_LINE_SOURCE, DI2_EXTI_PORT, DI2_EXTI_PIN);


	#if 1
	// DI_CWAKE
    InitStruct.Pin            = DI_CWAKE_PIN;
    GPIO_InitPeripheral(DI_CWAKE_PORT, &InitStruct);
    EXTI_Struct.EXTI_Line    = DI_CWAKE_EXTI_LINE;
    EXTI_Struct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_Struct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_Struct.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_Struct);
    NVIC_Struct.NVIC_IRQChannel = DI_CWAKE_IRQN;
    NVIC_Struct.NVIC_IRQChannelSubPriority = NVIC_SUB_PRIORITY_0;
    NVIC_Struct.NVIC_IRQChannelPreemptionPriority = NVIC_PRE_PRIORITY_0;
    NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_Struct);
	GPIO_ConfigEXTILine(DI_CWAKE_EXTI_LINE_SOURCE, DI_CWAKE_EXTI_PORT, DI_CWAKE_EXTI_PIN);
	#endif


	// DI_LOAD
    InitStruct.Pin            = DI_LOAD_PIN;
    GPIO_InitPeripheral(DI_LOAD_PORT, &InitStruct);
    EXTI_Struct.EXTI_Line    = DI_LOAD_EXTI_LINE;
    EXTI_Struct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_Struct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_Struct.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_Struct);
    NVIC_Struct.NVIC_IRQChannel = DI_LOAD_IRQN;
    NVIC_Struct.NVIC_IRQChannelSubPriority = NVIC_SUB_PRIORITY_0;
    NVIC_Struct.NVIC_IRQChannelPreemptionPriority = NVIC_PRE_PRIORITY_0;
    NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_Struct);
	GPIO_ConfigEXTILine(DI_LOAD_EXTI_LINE_SOURCE, DI_LOAD_EXTI_PORT, DI_LOAD_EXTI_PIN);
}


void InitWakeUp_RTCMode(void) {
	InitWakeUp_Base();
	// RTC_WKTimeConfig();
}


void InitWakeUp_NormalMode(void) {
	InitWakeUp_Base();
}


//如果是standby模式的话，PA0的wkup不用配
//通讯唤醒对深度休眠不能起效果。
void InitWakeUp_DeepMode(void) {
	InitWakeUp_Base();
}

void IOstatus_Base(void) {
    GPIO_InitType InitStruct;
    
	// __delay_ms(1000);
	// __delay_ms(1000);
	// __delay_ms(1000);
	// __delay_ms(1000);

    /* Enable GPIO clock */
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA, ENABLE);
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOB, ENABLE);
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOC, ENABLE);
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOD, ENABLE);
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOE, ENABLE);
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOF, ENABLE);
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOG, ENABLE);
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOH, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);

	ADC_DeInit(ADC1);

	InitStruct.Pin = GPIO_PIN_ALL;
	InitStruct.GPIO_Mode = GPIO_MODE_ANALOG;
	GPIO_InitPeripheral(GPIOA, &InitStruct);

	InitStruct.Pin = GPIO_PIN_ALL;
	InitStruct.GPIO_Mode = GPIO_MODE_ANALOG;
	GPIO_InitPeripheral(GPIOB, &InitStruct);
	
	InitStruct.Pin = GPIO_PIN_ALL;
	InitStruct.GPIO_Mode = GPIO_MODE_ANALOG;
	GPIO_InitPeripheral(GPIOC, &InitStruct);
	
	InitStruct.Pin = GPIO_PIN_ALL;
	InitStruct.GPIO_Mode = GPIO_MODE_ANALOG;
	GPIO_InitPeripheral(GPIOD, &InitStruct);
	
	InitStruct.Pin = GPIO_PIN_ALL;
	InitStruct.GPIO_Mode = GPIO_MODE_ANALOG;
	GPIO_InitPeripheral(GPIOE, &InitStruct);


    // 推挽输出类型
    InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    InitStruct.GPIO_Mode      = GPIO_MODE_OUTPUT_PP;
    InitStruct.GPIO_Alternate = GPIO_NO_AF;
    InitStruct.GPIO_Pull      = GPIO_NO_PULL;
    InitStruct.GPIO_Current   = GPIO_DS_8mA;


    InitStruct.Pin = PWSV_LDO_PIN;
    GPIO_InitPeripheral(PWSV_LDO_PORT, &InitStruct);

    InitStruct.Pin = PWSV_CTR_PIN;
    GPIO_InitPeripheral(PWSV_CTR_PORT, &InitStruct);

    // InitStruct.Pin = PWSV_BUCK_PIN;
    // GPIO_InitPeripheral(PWSV_BUCK_PORT, &InitStruct);

    InitStruct.Pin = RELAY_K4_PIN;
    GPIO_InitPeripheral(RELAY_K4_PORT, &InitStruct);

    InitStruct.Pin = LED1_PIN;
    GPIO_InitPeripheral(LED1_PORT, &InitStruct);

	IWDG_Freeze_Enable(ENABLE);					// 冻结看门狗
	// MCUO_DEBUG_LED1 = 1;
	MCUO_PWSV_LDO = 0;
	delay_ms_sleep(200);
	MCUO_PWSV_CTR = 1;

	// MCUO_RELAY_K4 = 0;
	// delay_ms_sleep(1000);
	// delay_ms_sleep(1000);
	// delay_ms_sleep(1000);
	// delay_ms_sleep(1000);

}

void IOstatus_NormalMode(void) {
	/*
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);		//使能PWR外设时钟，待机模式，RTC，看门狗
	PWR_BackupAccessCmd(ENABLE);    						//允许访问RTC
	RCC_LSEConfig(RCC_LSE_ON);								//使能外部LSE晶振，RCC_LSE_Bypass旁路的意思应该是使能这个LSE时钟，但是单片机不用，外围电路用?
	//RCC_LSICmd(DISABLE);
	*/

	InitIO();
	// InitDelay();

	// InitRTC();		//这个函数能降低150uA的功耗，具体是上面那三句话，具体原因不明
	IOstatus_Base();
}


void IOstatus_RTCMode(void) {
	InitIO();
	// InitDelay();
	// InitE2PROM();		//内部EEPROM，不需要初始化
	// InitRTC();
	
	IOstatus_Base();
}


void IOstatus_DeepMode(void) {
	InitIO();
	// InitDelay();
	IOstatus_Base();
}

//长期更新数据
void RefreshData_SleepFunction(void) 
{
	SleepElement.u8_ExtComCnt = RTC_ExtComCnt;				//上位机串口通讯就不会进入休眠
	SleepElement.u16_VCellMax = g_stCellInfoReport.u16VCellMax;
	SleepElement.u16_VCellMin = g_stCellInfoReport.u16VCellMin;
	SleepElement.u16_CurChg = g_stCellInfoReport.u16Ichg;
	SleepElement.u16_CurDsg = g_stCellInfoReport.u16IDischg;
}


void GetData_SleepFunction(void) 
{
	//SleepElement.u8_ToSleepFlag;
}


//一次性赋值
void InitData_SleepFunc(void) 
{
	SleepElement.u16_TimeRTC_First = OtherElement.u16Sleep_TimeRTC;
	SleepElement.u16_TimeRTC_Hiccup = OtherElement.u16Sleep_TimeRTC;
	SleepElement.u16_RTC_WakeUpTime = OtherElement.u16Sleep_RTC_WakeUpTime;
	SleepElement.u16_VNormal = OtherElement.u16Sleep_VNormal;
	SleepElement.u16_TimeNormal = OtherElement.u16Sleep_TimeNormal;
	SleepElement.u16_Vlow = OtherElement.u16Sleep_Vlow;
	SleepElement.u16_TimeVlow = OtherElement.u16Sleep_TimeVlow;
	SleepElement.u16_VirCur_Chg = OtherElement.u16Sleep_VirCur_Chg;
	SleepElement.u16_VirCur_Dsg = OtherElement.u16Sleep_VirCur_Dsg;

	//定制版处理，时间修改地方
	SleepElement.u16_Force_RTC_First = 0;	//未使用，置零
	SleepElement.u16_Force_RTC_Hiccup = 0;	//未使用，置零
	SleepElement.u16_Force_RTC_WKtime = 0;	//未使用，置零
	SleepElement.u16_Force_Normal_T = 0;	//意味着直接进入
	SleepElement.u16_Force_Vlow_T = 0;		//意味着直接进入
}


void EnterSleepJudge(void) {
	// IsSleepStartUp();
}

UINT16 test_sleep[4] = {0};
void SleepExFunc(void)
{
	if(MCUI_DI1 == 1 || MCUI_DI2 == 1
		|| MCUI_DI_LOAD == 1 || MCUI_DI_CWAKE == 1) {
		RTC_ExtComCnt++;
	} else {
		SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2 = 1;
	}

	test_sleep[0] = MCUI_DI1;
	test_sleep[1] = MCUI_DI2;
	test_sleep[2] = MCUI_DI_LOAD;
	test_sleep[3] = MCUI_DI_CWAKE;
}

/*
开机正常运行 ——> 休眠(标志位写这里，但是目前除了记录没啥用) ——> 唤醒
 ——> 复位 ——> 初始化 ——> 正常运行。
*/
void App_SleepDeal(void)
{
	static UINT8 su8_StartUp_Flag = 0;
	static UINT16 su16_StartUp_Tcnt = 0;

	if(SystemStatus.bits.b1StartUpBMS) {				// TODO，这里先屏蔽，等项目稳定再开放，开机完毕再进入
		// return;
	}
	else {
		SystemStatus.bits.b1Status_ToSleep = 1;
	}

	if(SleepElement.u8_ToSleepFlag) {					// 开机完毕再进入
		LogRecord_Flag.bits.Log_Sleep = 1;
		// return;
	}
	
	if(su8_StartUp_Flag == 0) {
		if(0 == g_st_SysTimeFlag.bits.b1Sys1000msFlag2) {
			return;
		}

		if(++su16_StartUp_Tcnt >= 5) {
			su16_StartUp_Tcnt = 0;
			su8_StartUp_Flag = 1;
		}
		return;
	}

	// return;

	SleepExFunc();
	RefreshData_SleepFunction();
	SleepCtrl(g_st_SysTimeFlag.bits.b1Sys1000msFlag1, System_OnOFF_Func.bits.b1OnOFF_Sleep);
	//SleepTest();		//需要立刻进入，则取消屏蔽即刻
}

void SleepTest(void) 
{
	static UINT8 su8_aaa = 0;
	if(0 == g_st_SysTimeFlag.bits.b1Sys1000msFlag1) {
		return;
	}

	RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);

	if(++su8_aaa > 5) {
		// SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2 = 1;
		PWR_EnterSLEEPMode(PWR_SLEEP_NOW, PWR_SLEEPENTRY_WFI);
		PWR_EnterSTOPMode(PWR_REGULATOR_LOWPOWER, PWR_STOPENTRY_WFI);
		PWR_EnterSTOPMode(PWR_REGULATOR_NORMAL, PWR_STOPENTRY_WFI);
	}
}

