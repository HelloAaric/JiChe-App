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

	// CBC����
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
		//++CBC_Element.u8CBC_CHG_Cnt;				//���ܷ������ȻBUG����
		if(CBC_Element.u8CBC_CHG_Cnt >= 2) {		//�����ν������̹ص�ȫ��
			System_OnOFF_Func.bits.b1OnOFF_MOS_Relay = 0;
			ChargerLoad_Func.bits.b1OFFDriver_CBC = 1;
			CBC_Element.u8CBC_CHG_Cnt = 0;
			CBC_Element.u8CBC_CHG_ErrFlag = 0;
		}
		else {
			if(++su16_CBC_CHG_TCnt > 30) {			//�������Ҫ�����渴λ����Ϊ���Ϊ0���п��������һ��else�ж�
				su16_CBC_CHG_TCnt = 0;
				++CBC_Element.u8CBC_CHG_Cnt;		//�򿪣���һ��
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

//��InitDAC()��InitPWM()�������
void CBC_CtrlRelay(void)
{
	static UINT8 su8_CBCctrl = 0;

	if(SystemStatus.bits.b1StartUpBMS) {		//��ζ�Ź����Ѿ����Կ�ʼ�ж��Ƿ�򿪻��ǹر�
		return;									//�Ӵ������Ƿ����ָ��ŵ����⣿����˵Ҫ��ȫ�򿪲Ŵ�����жϣ������۲�
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
				// ������ԭ����
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

	if(SystemStatus.bits.b1StartUpBMS) {	//��ζ�Ź����Ѿ����Կ�ʼ�ж��Ƿ�򿪻��ǹر�
		return;								//�Ӵ������Ƿ����ָ��ŵ����⣿����˵Ҫ��ȫ�򿪲Ŵ�����жϣ������۲�
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

	if(SystemStatus.bits.b1StartUpBMS) {	// ��ζ�Ź����Ѿ����Կ�ʼ�ж��Ƿ�򿪻��ǹر�
		return;								// �Ӵ������Ƿ����ָ��ŵ����⣿����˵Ҫ��ȫ�򿪲Ŵ�����жϣ������۲�
	}										// ���µ�FLASH��˵������ôд����Ϊ��Ԥ��Ĵ��ڣ�����ͬʱ��ʼִ��MOS�������
											// ����Ҫ�ڿ�ʼ�ͳ�ʼ���ˡ�

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
