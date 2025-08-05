#include "CBC.h"
#include "sys_timer.h"
#include "System_Monitor.h"
#include "charger_loader_func.h"

#include "io_define.h"
#include "misc.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_exti.h"
// #include "afe_cbc.h"
#include "drivers_ctrl.h"

typedef enum NVIC_ONOFF {
	NVIC_INIT = 0,
	NVIC_OPEN ,
	NVIC_CLOSE,
}NVIC_OnOFF;

struct CBC_ELEMENT CBC_Element;

void InitCBC(NVIC_OnOFF NVIC_Status)
{
	GPIO_InitType InitStruct;
    EXTI_InitType EXTI_Struct;
    NVIC_InitType NVIC_Struct;

    InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    InitStruct.GPIO_Mode      = GPIO_MODE_INPUT;
    InitStruct.GPIO_Alternate = GPIO_NO_AF;
    InitStruct.GPIO_Pull      = GPIO_NO_PULL;
    InitStruct.GPIO_Current   = GPIO_DS_8mA;

	// CBC输入
    InitStruct.Pin            = DI_CBC_PIN;
    GPIO_InitPeripheral(DI_CBC_PORT, &InitStruct);
    EXTI_Struct.EXTI_Line    = DI_CBC_EXTI_LINE;
    EXTI_Struct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_Struct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_Struct.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_Struct);
    NVIC_Struct.NVIC_IRQChannel = DI_CBC_IRQN;
    NVIC_Struct.NVIC_IRQChannelSubPriority = NVIC_SUB_PRIORITY_0;
    NVIC_Struct.NVIC_IRQChannelPreemptionPriority = NVIC_PRE_PRIORITY_0;
    NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_Struct);
	GPIO_ConfigEXTILine(DI_CBC_EXTI_LINE_SOURCE, DI_CBC_EXTI_PORT, DI_CBC_EXTI_PIN);


	switch(NVIC_Status) {
		case NVIC_INIT:
			break;
	
		case NVIC_OPEN:
			break;
			
		case NVIC_CLOSE:
			break;
			
		default:
			break;
	}
}

void CBC_TimeCtrl_Relay(void) {

	static UINT16 su16_CBC_CHG_TCnt = 0;
	static UINT16 su16_CBC_DSG_TCnt = 0;
	static UINT16 su16_CBC_Recover_TCnt = 0;

	if(0 == g_st_SysTimeFlag.bits.b1Sys1000msFlag1) {
		return;
	}

	if(CBC_Element.u8CBC_CHG_ErrFlag) {
		//++CBC_Element.u8CBC_CHG_Cnt;				//不能放这里，不然BUG错了
		if(CBC_Element.u8CBC_CHG_Cnt >= 2) {		//第三次进来立刻关掉全部
			System_OnOFF_Func.bits.b1OnOFF_MOS_Relay = 0;
			ChargerLoad_Func.bits.b1OFFDriver_CBC = 1;
			CBC_Element.u8CBC_CHG_Cnt = 0;
			CBC_Element.u8CBC_CHG_ErrFlag = 0;
		}
		else {
			if(++su16_CBC_CHG_TCnt > 30) {			//这个不需要在下面复位，因为这个为0才有可能跳最后一个else判断
				su16_CBC_CHG_TCnt = 0;
				++CBC_Element.u8CBC_CHG_Cnt;		//打开，计一次
				CBC_Element.u8CBC_CHG_ErrFlag = 0;
			}
		}
		if(su16_CBC_Recover_TCnt)su16_CBC_Recover_TCnt = 0;
	}
	else if(CBC_Element.u8CBC_DSG_ErrFlag) {
		if(CBC_Element.u8CBC_DSG_Cnt >= 2) {
			System_OnOFF_Func.bits.b1OnOFF_MOS_Relay = 0;
			ChargerLoad_Func.bits.b1OFFDriver_CBC = 1;
			CBC_Element.u8CBC_DSG_Cnt = 0;
			CBC_Element.u8CBC_DSG_ErrFlag = 0;
		}
		else {
			if(++su16_CBC_DSG_TCnt > 30) {
				su16_CBC_DSG_TCnt = 0;
				++CBC_Element.u8CBC_DSG_Cnt;
				CBC_Element.u8CBC_DSG_ErrFlag = 0;
			}
		}
		if(su16_CBC_Recover_TCnt)su16_CBC_Recover_TCnt = 0;
	}
	else {
		if(CBC_Element.u8CBC_CHG_Cnt) {
			if(++su16_CBC_Recover_TCnt > 60) {
				su16_CBC_Recover_TCnt = 0;
				CBC_Element.u8CBC_CHG_Cnt = 0;
			}
		}
		else if(CBC_Element.u8CBC_DSG_Cnt) {
			if(++su16_CBC_Recover_TCnt > 60) {
				su16_CBC_Recover_TCnt = 0;
				CBC_Element.u8CBC_DSG_Cnt = 0;
			}
		}
		if(su16_CBC_CHG_TCnt)su16_CBC_CHG_TCnt = 0;
		if(su16_CBC_DSG_TCnt)su16_CBC_DSG_TCnt = 0;
	}
}

void KeyReleaseCBCDect(void)
{

}

//有InitDAC()和InitPWM()函数相关
void CBC_CtrlRelay(void)
{
	static UINT8 su8_CBCctrl = 0;

	if(SystemStatus.bits.b1StartUpBMS) {		//意味着管子已经可以开始判断是否打开还是关闭
		return;									//接触器打开是否会出现干扰的问题？还是说要完全打开才打开这个中断？后续观察
	}

	if(0 == g_st_SysTimeFlag.bits.b1Sys10msFlag5) {
		return;
	}

	switch(su8_CBCctrl) {
		case 0:
			if(CBC_Element.u8CBC_DSG_ErrFlag) {
				System_OnOFF_Func.bits.b1OnOFF_MOS_Relay = 0;
				System_ERROR_UserCallback(ERROR_CBC_DSG);
				System_ERROR_UserCallback(ERROR_DSG_SHORT);
				su8_CBCctrl = 1;
			}
			break;

		case 1:
			if(!CBC_Element.u8CBC_DSG_ErrFlag) {
				su8_CBCctrl = 0;
			}
			break;

		default:
			break;
	}
}

void CBC_CtrlMOS(void)
{
	static UINT8 su8_CBCctrl = 0;

	switch(su8_CBCctrl) {
		case 0:
			if(CBC_Element.u8CBC_DSG_ErrFlag) {
				System_OnOFF_Func.bits.b1OnOFF_MOS_Relay = 0;
				// CBC_Element.u8CBC_DSG_ErrFlag = 0;
				System_ERROR_UserCallback(ERROR_CBC_DSG);
				System_ERROR_UserCallback(ERROR_DSG_SHORT);
				su8_CBCctrl = 1;
			}
			break;
		case 1:
			if(!CBC_Element.u8CBC_DSG_ErrFlag) {
				su8_CBCctrl = 0;
			}
			break;
		default:
			break;
	}
}

void CBC_Recovery(void)
{
	static UINT8 su8_CBC_status = 0;
	switch(su8_CBC_status) {
		case 0:
			if(CBC_Element.u8CBC_DSG_ErrFlag) {
				// 开启复原功能
				ChargerLoad_Func.bits.b1OFFDriver_CBC = 1;
				su8_CBC_status = 1;
			}
			break;

		case 1:
			if(ChargerLoad_Func.bits.b1OFFDriver_CBC == 0) {
				CBC_Element.u8CBC_DSG_ErrFlag = 0;
				su8_CBC_status = 0;
			}
			break;
		default:
			break;
	}

}

void CBC_Relay(void)
{
	static UINT8 su8_CBCStartUp_Flag = 0;

	if(SystemStatus.bits.b1StartUpBMS) {	//意味着管子已经可以开始判断是否打开还是关闭
		return;								//接触器打开是否会出现干扰的问题？还是说要完全打开才打开这个中断？后续观察
	}

	switch(su8_CBCStartUp_Flag) {
		case 0:
			InitCBC(NVIC_INIT);
			// InitCBC(NVIC_OPEN);
			su8_CBCStartUp_Flag = 1;
			break;
		case 1:
			CBC_CtrlRelay();
			CBC_Recovery();
			break;
		default:
			su8_CBCStartUp_Flag = 0;
			break;
	}
}

void CBC_MOS(void)
{
	static UINT8 su8_CBCStartUp_Flag = 0;

	if(SystemStatus.bits.b1StartUpBMS) {	// 意味着管子已经可以开始判断是否打开还是关闭
		return;								// 接触器打开是否会出现干扰的问题？还是说要完全打开才打开这个中断？后续观察
	}										// 最新的FLASH来说，能这么写，因为有预充的存在，而且同时开始执行MOS和这个。
											// 不需要在开始就初始化了。

	switch(su8_CBCStartUp_Flag) {
		case 0:
			InitCBC(NVIC_INIT);
			// InitCBC(NVIC_OPEN);
			su8_CBCStartUp_Flag = 1;
			break;
		case 1:
			CBC_CtrlMOS();
			CBC_Recovery();
			break;
		default:
			su8_CBCStartUp_Flag = 0;
			break;
	}
}

void App_CBC(void)
{
	#ifdef _MOS
	CBC_MOS();
	#else
	CBC_Relay();
	#endif
}
