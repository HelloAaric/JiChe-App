#include "lib_sleepFunc.h"
#include "n32h47x_48x_pwr.h"
#include "n32h47x_48x_rcc.h"


#ifndef TRUE
#define TRUE		1
#endif

#ifndef FALSE
#define FALSE		0
#endif

typedef enum _SLEEP_CNT {
FIRST = 0, HICCUP
}SLEEP_CNT;

typedef enum _SLEEP_SYSMODE {
NORMAL_MODE = 0, HICCUP_MODE, DEEP_MODE
}SLEEP_SYSMODE;


#define RTC_WT_Protect 		5         	//出现保护进入休眠打嗝时间
#define RTC_WT_Normal 		5         	//正常状态进入休眠打嗝时间
#define RTC_WT_Force 		5         	//外部强制进入休眠打嗝时间

#define SleepInitOC 		10	  		//1000ms时基，打嗝模式起来，初始化延时后判断是否再进入休眠
#define SleepInitCBC 		10	  		//1000ms时基，打嗝模式起来，初始化延时后判断是否再进入休眠
#define SleepInitForce 		10	  		//200ms时基，打嗝模式起来，初始化延时后判断是否再进入休眠
#define SleepInitOVP 		240	  		//1000ms时基，打嗝模式起来，初始化延时后判断是否再进入休眠
#define SleepInitUVP 		240	  		//1000ms时基，打嗝模式起来，初始化延时后判断是否再进入休眠
#define SleepInitNormal1 	100	  		//1000ms时基，打嗝模式起来，初始化延时后判断是否再进入休眠
#define SleepInitNormal2 	150	  		//1000ms时基，打嗝模式起来，发现电流则延长至30s判断


enum SLEEP_STATUS{
	SLEEP_STATUS_INIT = 0,	
	SLEEP_STATUS_SHIFT,	
	SLEEP_STATUS_CONTINUE,
	SLEEP_STATUS_FORCED_L1,
	SLEEP_STATUS_FORCED_L2,
	SLEEP_STATUS_FORCED_L3,
	SLEEP_STATUS_OVERCUR,
	SLEEP_STATUS_OVDELTA,
	SLEEP_STATUS_CBC,
	SLEEP_STATUS_VCELLOVP,
	SLEEP_STATUS_VCELLUVP,
	SLEEP_STATUS_NORMAL_SELECT,
	SLEEP_STATUS_NORMAL_L1,
	SLEEP_STATUS_NORMAL_L2,
	SLEEP_STATUS_NORMAL_L3,
	SLEEP_STATUS_TEST
};


enum SLEEP_STATUS Sleep_Status = SLEEP_STATUS_INIT;
UINT8 gu8_SleepHiccup = 0;
//UINT8 gu8_SleepHiccup = 0;

Sleep_Element SleepElement;

UINT16 aaaa1 = 0;
UINT16 aaaa2 = 0;
UINT16 aaaa3 = 0;
UINT16 aaaa4 = 0;


void GotoSleep(SLEEP_SYSMODE mode);

void InitWakeUp_RTCMode_Alarm(void) {

}


void IORecover_RTCMode(void) {
	NVIC_SystemReset();
}


void IORecover_NormalMode(void) {
	//TIM_Cmd(TIM3, ENABLE);	//用于App_SleepTest()函数
	NVIC_SystemReset();				//由于直接走下去导致各种因为现场破坏无法进入正常工作模式，完美的解决办法是复位再来过
}


void IORecover_DeepMode(void) {
	NVIC_SystemReset();
}


//wkup不用配
void Sys_StandbyMode(void) {  
	RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);	//使能PWR外设时钟，这句话是否需要？030不需要也能进入休眠
	PWR_EnterSTANDBYMode();
}

void Sys_StopMode(void) {
	RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);
	// PWR_EnterSLEEPMode(PWR_SLEEP_NOW, PWR_SLEEPENTRY_WFI);
	PWR_EnterSTOPMode(PWR_REGULATOR_LOWPOWER, PWR_STOPENTRY_WFI);
}


void SleepDeal_Continue(void)
{
	static SLEEP_SYSMODE s_u8SleepModeSelect = NORMAL_MODE;
	
	if(SleepElement.Sleep_Mode.bits.b1TestSleep) {
		s_u8SleepModeSelect = NORMAL_MODE;
	}
	else if(SleepElement.Sleep_Mode.bits.b1OverCurSleep) {
		s_u8SleepModeSelect = DEEP_MODE;
	}
	else if(SleepElement.Sleep_Mode.bits.b1OverVdeltaSleep) {
		s_u8SleepModeSelect = DEEP_MODE;
	}
	else if(SleepElement.Sleep_Mode.bits.b1CBCSleep) {
		s_u8SleepModeSelect = DEEP_MODE;
	}
	else if(SleepElement.Sleep_Mode.bits.b1VcellOVP) {
		//s_u8SleepModeSelect = HICCUP_MODE;
		s_u8SleepModeSelect = DEEP_MODE;
	}
	else if(SleepElement.Sleep_Mode.bits.b1VcellUVP) {
		//s_u8SleepModeSelect = HICCUP_MODE;
		s_u8SleepModeSelect = DEEP_MODE;
	}
	else if(SleepElement.Sleep_Mode.bits.b1ForceToSleep_L1) {
		s_u8SleepModeSelect = HICCUP_MODE;
	}
	else if(SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2) {
		s_u8SleepModeSelect = NORMAL_MODE;
	}
	else if(SleepElement.Sleep_Mode.bits.b1ForceToSleep_L3) {
		s_u8SleepModeSelect = DEEP_MODE;
	}	
	else if(SleepElement.Sleep_Mode.bits.b1NormalSleep_L1) {
		s_u8SleepModeSelect = HICCUP_MODE;
	}
	else if(SleepElement.Sleep_Mode.bits.b1NormalSleep_L2) {
		s_u8SleepModeSelect = NORMAL_MODE;
	}
	else if(SleepElement.Sleep_Mode.bits.b1NormalSleep_L3) {
		s_u8SleepModeSelect = DEEP_MODE;
	}
	else {
		s_u8SleepModeSelect = NORMAL_MODE;
	}

	GotoSleep(s_u8SleepModeSelect);
}


void SleepDeal_OverCurrent(void) {
	static UINT8 s_u8SleepStatus = FIRST;
	static UINT32 s_u32SleepFirstCnt = 0;
	static UINT32 s_u32SleepHiccupCnt = 0;

	if(!SleepElement.Sleep_Mode.bits.b1OverCurSleep) {		//加强雍余设计
		Sleep_Status = SLEEP_STATUS_SHIFT;		//其实这个可以不要，设计要求，除了这个函数可以把这个标志位去除外，别的地方不可以便可
		return;
	}

	switch(s_u8SleepStatus) {
		case FIRST:
			if(++s_u32SleepFirstCnt > 0) {		//留下位置，后续第一次后进入需要延时则这里加
				s_u32SleepFirstCnt = 0;
				s_u8SleepStatus = HICCUP;
				Sleep_Status = SLEEP_STATUS_CONTINUE;
			}
			break;
			
		case HICCUP:
			if(++s_u32SleepHiccupCnt > SleepInitOC) {
				s_u32SleepHiccupCnt = 0;
				Sleep_Status = SLEEP_STATUS_CONTINUE;
			}
			break;
			
		default:
			s_u8SleepStatus = FIRST;		//下个回合再来
			break;
	}


	if(0) {										//如果检测到没问题，则退出休眠
		SleepElement.Sleep_Mode.bits.b1OverCurSleep = 0;		//放到switch语句外面，FIRST和HICCUP两个都有效
		Sleep_Status = SLEEP_STATUS_SHIFT;
		s_u8SleepStatus = FIRST;
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
	}

}


void SleepDeal_VcellOVP(void) {

}


void SleepDeal_VcellUVP(void) {
	static UINT8 s_u8SleepStatus = FIRST;
	static UINT32 s_u32SleepFirstCnt = 0;
	static UINT32 s_u32SleepHiccupCnt = 0;

	if(!SleepElement.Sleep_Mode.bits.b1VcellUVP) {		//加强雍余设计
		Sleep_Status = SLEEP_STATUS_SHIFT;	//其实这个可以不要，设计要求，除了这个函数可以把这个标志位去除外，别的地方不可以便可
		return ;
	}

	switch(s_u8SleepStatus) {
		case FIRST:
			if(++s_u32SleepFirstCnt > 0) {		//留下位置，后续第一次后进入需要延时则这里加
				s_u32SleepFirstCnt = 0;
				s_u8SleepStatus = HICCUP;
				Sleep_Status = SLEEP_STATUS_CONTINUE;
			}
			break;
			
		case HICCUP:
			if(++s_u32SleepHiccupCnt > SleepInitUVP) {
				s_u32SleepHiccupCnt = 0;
				Sleep_Status = SLEEP_STATUS_CONTINUE;
			}
			break;
			
		default:
			s_u8SleepStatus = FIRST;		//下个回合再来
			break;
	}

	if(0) {									//如果检测到没问题，则退出休眠
		SleepElement.Sleep_Mode.bits.b1VcellUVP = 0;		//放到switch语句外面，FIRST和HICCUP两个都有效
		Sleep_Status = SLEEP_STATUS_SHIFT;
		s_u8SleepStatus = FIRST;
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
	}
}


void SleepDeal_Vdelta(void) {

}


void SleepDeal_CBC(void) {
	static UINT8 s_u8SleepStatus = FIRST;
	static UINT32 s_u32SleepFirstCnt = 0;
	static UINT32 s_u32SleepHiccupCnt = 0;

	if(!SleepElement.Sleep_Mode.bits.b1CBCSleep) {		//加强雍余设计
		Sleep_Status = SLEEP_STATUS_SHIFT;	//其实这个可以不要，设计要求，除了这个函数可以把这个标志位去除外，别的地方不可以便可
		return ;
	}

	switch(s_u8SleepStatus) {
		case FIRST:
			if(++s_u32SleepFirstCnt > 0) {		//留下位置，后续第一次后进入需要延时则这里加
				s_u32SleepFirstCnt = 0;
				s_u8SleepStatus = HICCUP;
				Sleep_Status = SLEEP_STATUS_CONTINUE;
			}
			break;
			
		case HICCUP:
			if(++s_u32SleepHiccupCnt > SleepInitCBC) {
				s_u32SleepHiccupCnt = 0;
				Sleep_Status = SLEEP_STATUS_CONTINUE;
			}
			break;
			
		default:
			s_u8SleepStatus = FIRST;		//下个回合再来
			break;
	}

	if(0) {									//如果检测到没问题，则退出休眠
		SleepElement.Sleep_Mode.bits.b1CBCSleep = 0;		//放到switch语句外面，FIRST和HICCUP两个都有效
		Sleep_Status = SLEEP_STATUS_SHIFT;
		s_u8SleepStatus = FIRST;
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
	}
}


void SleepDeal_Forced_L1(void) {
	static UINT8 s_u8SleepStatus = FIRST;
	static UINT32 s_u32SleepFirstCnt = 0;
	static UINT32 s_u32SleepHiccupCnt = 0;

	if(!SleepElement.Sleep_Mode.bits.b1ForceToSleep_L1) {		//加强雍余设计
		Sleep_Status = SLEEP_STATUS_SHIFT;
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
		return ;
	}

	switch(s_u8SleepStatus) {
		case FIRST:
			if(++s_u32SleepFirstCnt > 0) {	//直接进去
				s_u32SleepFirstCnt = 0;
				s_u8SleepStatus = HICCUP;
				Sleep_Status = SLEEP_STATUS_CONTINUE;
			}
			break;
			
		case HICCUP:
			if(++s_u32SleepHiccupCnt > 0) {
				s_u32SleepHiccupCnt = 0;
				Sleep_Status = SLEEP_STATUS_CONTINUE;
			}
			break;
			
		default:
			s_u8SleepStatus = FIRST;		//下个回合再来
			break;
	}

	if(0) {										//如果检测到没问题，则退出休眠
		//Sleep_Mode.bits.b1ForceToSleep = 0;
		//Sleep_Status = SLEEP_HICCUP_SHIFT;
		s_u8SleepStatus = FIRST;			//直接回到第一次，force只有一次，不是打嗝休眠模式
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
	}

}


void SleepDeal_Forced_L2(void) {
	static UINT8 s_u8SleepStatus = FIRST;
	static UINT32 s_u32SleepFirstCnt = 0;
	static UINT32 s_u32SleepHiccupCnt = 0;

	if(!SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2) {		//加强雍余设计
		Sleep_Status = SLEEP_STATUS_SHIFT;
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
		return;
	}
	
	switch(s_u8SleepStatus) {
		case FIRST:
			if(++s_u32SleepFirstCnt > 0) {	//直接进去
				s_u32SleepFirstCnt = 0;
				s_u8SleepStatus = HICCUP;
				Sleep_Status = SLEEP_STATUS_CONTINUE;
			}
			break;
			
		case HICCUP:
			if(++s_u32SleepHiccupCnt > 0) {
				s_u32SleepHiccupCnt = 0;
				Sleep_Status = SLEEP_STATUS_CONTINUE;
			}
			break;
			
		default:
			s_u8SleepStatus = FIRST;		//下个回合再来
			break;
	}

	if(0) {										//如果检测到没问题，则退出休眠
		//Sleep_Mode.bits.b1ForceToSleep = 0;
		//Sleep_Status = SLEEP_HICCUP_SHIFT;
		s_u8SleepStatus = FIRST;			//直接回到第一次，force只有一次，不是打嗝休眠模式
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
	}
}


void SleepDeal_Forced_L3(void) {
	static UINT8 s_u8SleepStatus = FIRST;
	static UINT32 s_u32SleepFirstCnt = 0;
	static UINT32 s_u32SleepHiccupCnt = 0;

	if(!SleepElement.Sleep_Mode.bits.b1ForceToSleep_L3) {			//加强雍余设计
		Sleep_Status = SLEEP_STATUS_SHIFT;
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;		
		return ;
	}

	switch(s_u8SleepStatus) {
		case FIRST:
			if(++s_u32SleepFirstCnt > 0) {	//直接进去
				s_u32SleepFirstCnt = 0;
				s_u8SleepStatus = HICCUP;
				Sleep_Status = SLEEP_STATUS_CONTINUE;
			}
			break;
			
		case HICCUP:
			if(++s_u32SleepHiccupCnt > 0) {
				s_u32SleepHiccupCnt = 0;
				Sleep_Status = SLEEP_STATUS_CONTINUE;
			}
			break;
			
		default:
			s_u8SleepStatus = FIRST;		//下个回合再来
			break;
	}

	if(0) {										//如果检测到没问题，则退出休眠
		//Sleep_Mode.bits.b1ForceToSleep = 0;
		//Sleep_Status = SLEEP_HICCUP_SHIFT;
		s_u8SleepStatus = FIRST;			//直接回到第一次，force只有一次，不是打嗝休眠模式
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
	}
}


void SleepDeal_Normal_L1(void) {
	static UINT8 s_u8SleepStatus = FIRST;
	static UINT32 s_u32SleepFirstCnt = 0;
	static UINT32 s_u32SleepHiccupCnt = 0;
	static UINT8 su8_SleepExtComCnt = 0;
	static UINT16 su16_Sleep_TimeRTC_First = 0;
	static UINT16 su16_Sleep_TimeRTC_Hiccup = 0;
	
	if((SleepElement.Sleep_Mode.all&0xFFF1) != 0) {		//核心
		SleepElement.Sleep_Mode.bits.b1NormalSleep_L1 = 0;
		SleepElement.Sleep_Mode.bits.b1NormalSleep_L2 = 0;
		SleepElement.Sleep_Mode.bits.b1NormalSleep_L3 = 0;
		Sleep_Status = SLEEP_STATUS_SHIFT;
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
		return ;
	}
	if(gu8_SleepHiccup) {					//因为RESET的问题，只能存标志位了
		s_u8SleepStatus = HICCUP;
		gu8_SleepHiccup = 0;
	}
	
	//u16Sleep_TimeNormal修改后全部复位，或者有通讯
	if(su8_SleepExtComCnt != SleepElement.u8_ExtComCnt || su16_Sleep_TimeRTC_First != SleepElement.u16_TimeRTC_First\
		|| su16_Sleep_TimeRTC_Hiccup != SleepElement.u16_TimeRTC_Hiccup) {
		su8_SleepExtComCnt = SleepElement.u8_ExtComCnt;
		su16_Sleep_TimeRTC_First = SleepElement.u16_TimeRTC_First;
		su16_Sleep_TimeRTC_Hiccup = SleepElement.u16_TimeRTC_Hiccup;
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
	}

	switch(s_u8SleepStatus) {
		case FIRST:
			if(SleepElement.u16_TimeRTC_First == 0) {
				//当恢复时间为0时默认RTC不进入休眠
			}
			else {
				if(++s_u32SleepFirstCnt > (UINT32)SleepElement.u16_TimeRTC_First*60) {
				//if(++s_u32SleepFirstCnt >= 5) {			//这个，第一次个后面都是一样
					//FlashWriteOneHalfWord(FLASH_ADDR_SLEEP_ELEMENT, SleepElement.Sleep_Mode.all);
					//FlashWriteOneHalfWord(FLASH_ADDR_RTC_WK_TIME, SleepElement.u16_TimeRTC_First);
					s_u32SleepFirstCnt = 0;
					s_u8SleepStatus = HICCUP;
					Sleep_Status = SLEEP_STATUS_CONTINUE;
				}
			}
			break;
			
		case HICCUP:
			if(++s_u32SleepHiccupCnt > (UINT32)SleepElement.u16_TimeRTC_Hiccup*60) {
				//FlashWriteOneHalfWord(FLASH_ADDR_RTC_WK_TIME, SleepElement.u16_TimeRTC_Hiccup);
				s_u32SleepHiccupCnt = 0;
				Sleep_Status = SLEEP_STATUS_CONTINUE;
			}
			break;
			
		default:
			s_u8SleepStatus = FIRST;		//下个回合再来
			break;
	}

	if(SleepElement.u16_CurChg > SleepElement.u16_VirCur_Chg || SleepElement.u16_CurDsg > SleepElement.u16_VirCur_Dsg) {
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
	}

	if(SleepElement.u16_VCellMin <= SleepElement.u16_VNormal) {
		SleepElement.Sleep_Mode.bits.b1NormalSleep_L1 = 0;
		//FlashWriteOneHalfWord(FLASH_ADDR_SLEEP_ELEMENT, SleepElement.Sleep_Mode.all);
		Sleep_Status = SLEEP_STATUS_SHIFT;
		s_u8SleepStatus = FIRST;
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
	}

	aaaa1 = s_u32SleepFirstCnt;
	aaaa2 = s_u32SleepHiccupCnt;
	//s_u32SleepFirstCnt = 0;		//还没调好L1不进入休眠。
}


void SleepDeal_Normal_L2(void) {
	static UINT8 s_u8SleepStatus = FIRST;
	static UINT32 s_u32SleepFirstCnt = 0;
	static UINT32 s_u32SleepHiccupCnt = 0;
	static UINT8 su8_SleepExtComCnt = 0;
	static UINT16 su16_Sleep_TimeNormal = 0;

	if((SleepElement.Sleep_Mode.all&0xFFF1) != 0) {
		SleepElement.Sleep_Mode.bits.b1NormalSleep_L1 = 0;
		SleepElement.Sleep_Mode.bits.b1NormalSleep_L2 = 0;
		SleepElement.Sleep_Mode.bits.b1NormalSleep_L3 = 0;
		Sleep_Status = SLEEP_STATUS_SHIFT;
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
		return;
	}

	if(su8_SleepExtComCnt != SleepElement.u8_ExtComCnt || su16_Sleep_TimeNormal != SleepElement.u16_TimeNormal) {
		su8_SleepExtComCnt = SleepElement.u8_ExtComCnt;
		su16_Sleep_TimeNormal = SleepElement.u16_TimeNormal;
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
	}


	switch(s_u8SleepStatus) {
		case FIRST:
			if(++s_u32SleepFirstCnt > (UINT32)SleepElement.u16_TimeNormal*60) {
			//if(++s_u32SleepFirstCnt >= 1) {			//这个，第一次个后面都是一样
				s_u32SleepFirstCnt = 0;
				s_u8SleepStatus = HICCUP;
				Sleep_Status = SLEEP_STATUS_CONTINUE;
			}
			break;
			
		case HICCUP:
			if(++s_u32SleepHiccupCnt > (UINT32)SleepElement.u16_TimeNormal*60) {
			//if(++s_u32SleepHiccupCnt >= 1) {
				s_u32SleepHiccupCnt = 0;
				Sleep_Status = SLEEP_STATUS_CONTINUE;
			}
			break;
			
		default:
			s_u8SleepStatus = FIRST;		//下个回合再来
			break;
	}

	if(SleepElement.u16_CurChg > SleepElement.u16_VirCur_Chg || SleepElement.u16_CurDsg > SleepElement.u16_VirCur_Dsg) {
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
	}

	if(SleepElement.u16_VCellMin < SleepElement.u16_Vlow\
		|| SleepElement.u16_VCellMin > SleepElement.u16_VNormal) {	//触发条件才跳转，别的时间不跳转
		SleepElement.Sleep_Mode.bits.b1NormalSleep_L2 = 0;
		Sleep_Status = SLEEP_STATUS_SHIFT;
		s_u8SleepStatus = FIRST;
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
	}
	aaaa3 = s_u32SleepFirstCnt;
}


void SleepDeal_Normal_L3(void) {
	static UINT8 s_u8SleepStatus = FIRST;
	static UINT32 s_u32SleepFirstCnt = 0;
	static UINT32 s_u32SleepHiccupCnt = 0;
	//static UINT8 su8_SleepExtComCnt = 0;
	static UINT16 su16_Sleep_TimeNormal = 0; 

	if((SleepElement.Sleep_Mode.all&0xFFF1) != 0) {
		SleepElement.Sleep_Mode.bits.b1NormalSleep_L1 = 0;
		SleepElement.Sleep_Mode.bits.b1NormalSleep_L2 = 0;
		SleepElement.Sleep_Mode.bits.b1NormalSleep_L3 = 0;
		Sleep_Status = SLEEP_STATUS_SHIFT;
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
		return;
	}

	#if 0
	if(su8_SleepExtComCnt != SleepElement.u8_ExtComCnt || su16_Sleep_TimeNormal != SleepElement.u16_TimeVlow) {
		su8_SleepExtComCnt = SleepElement.u8_ExtComCnt;
		su16_Sleep_TimeNormal = SleepElement.u16_TimeVlow;
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
	}
	#endif

	//低电压休眠，这个无论是否有通讯都会进入休眠
	if(su16_Sleep_TimeNormal != SleepElement.u16_TimeVlow) {
		su16_Sleep_TimeNormal = SleepElement.u16_TimeVlow;
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
	}
	
	switch(s_u8SleepStatus) {
		case FIRST:
			if(++s_u32SleepFirstCnt > (UINT32)SleepElement.u16_TimeVlow*60) {
			//if(++s_u32SleepFirstCnt >= 1) {			//这个，第一次个后面都是一样
				s_u32SleepFirstCnt = 0;
				s_u8SleepStatus = HICCUP;
				Sleep_Status = SLEEP_STATUS_CONTINUE;
			}
			break;
			
		case HICCUP:
			if(++s_u32SleepHiccupCnt > (UINT32)SleepElement.u16_TimeVlow*60) {
			//if(++s_u32SleepHiccupCnt >= 1) {
				s_u32SleepHiccupCnt = 0;
				Sleep_Status = SLEEP_STATUS_CONTINUE;
			}
			break;
			
		default:
			s_u8SleepStatus = FIRST;		//下个回合再来
			break;
	}

	if(SleepElement.u16_CurChg > SleepElement.u16_VirCur_Chg || SleepElement.u16_CurDsg > SleepElement.u16_VirCur_Dsg) {
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
	}

	if(SleepElement.u16_VCellMin >= SleepElement.u16_Vlow) {	//触发条件才跳转，别的时间不跳转
		SleepElement.Sleep_Mode.bits.b1NormalSleep_L3 = 0;
		Sleep_Status = SLEEP_STATUS_SHIFT;
		s_u8SleepStatus = FIRST;
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
	}
	aaaa4 = s_u32SleepFirstCnt;
}


//这个地方，IO控制策略要改一下，起来延时1s再打开管子会不会更好？不过现象貌似直接打开没问题
//这个作为主循环，如果开头判断出现了别的错误，则跳出主循环，去执行别的
//关于这里和IO控制主函数的逻辑问题，A，最开头关于Sleep的return问题。B，休眠起来IO是否立刻打开的问题
void SleepDeal_Normal_Select(void) {
	if((SleepElement.Sleep_Mode.all&0xFFF1) != 0) {				//核心
		SleepElement.Sleep_Mode.bits.b1NormalSleep_L1 = 0;
		SleepElement.Sleep_Mode.bits.b1NormalSleep_L2 = 0;
		SleepElement.Sleep_Mode.bits.b1NormalSleep_L3 = 0;
		Sleep_Status = SLEEP_STATUS_SHIFT;
		return;
	}
	
	if(SleepElement.u16_CurChg <= SleepElement.u16_VirCur_Chg\
		&& SleepElement.u16_CurDsg <= SleepElement.u16_VirCur_Dsg) {
		if(SleepElement.u16_VCellMin < SleepElement.u16_Vlow) {
			SleepElement.Sleep_Mode.bits.b1NormalSleep_L3 = 1;
			Sleep_Status = SLEEP_STATUS_NORMAL_L3;
		}
		else if(SleepElement.u16_VCellMin > SleepElement.u16_VNormal) {
			SleepElement.Sleep_Mode.bits.b1NormalSleep_L1 = 1;
			Sleep_Status = SLEEP_STATUS_NORMAL_L1;
		}
		else {			//等号均纳入L2
			SleepElement.Sleep_Mode.bits.b1NormalSleep_L2 = 1;
			Sleep_Status = SLEEP_STATUS_NORMAL_L2;
		}
	}
	else {
		//有电流则继续在这个函数循环
	}
}


//架构决定要改一改，不然后期人员太难维护了
void SleepDeal_Shift(void) {
	if(SleepElement.Sleep_Mode.bits.b1TestSleep != 0) {
		Sleep_Status = SLEEP_STATUS_TEST;
	}
	else if(SleepElement.Sleep_Mode.bits.b1OverCurSleep != 0) {
		//Sleep_Status = SLEEP_STATUS_CONTINUE;			//架构已改，先跳到相关函数，再进入休眠
		Sleep_Status = SLEEP_STATUS_OVERCUR;
	}
	else if(SleepElement.Sleep_Mode.bits.b1OverVdeltaSleep != 0) {
		Sleep_Status = SLEEP_STATUS_OVDELTA;
	}
	else if(SleepElement.Sleep_Mode.bits.b1CBCSleep != 0) {
		Sleep_Status = SLEEP_STATUS_CBC;
	}
	else if(SleepElement.Sleep_Mode.bits.b1VcellOVP != 0) {
		Sleep_Status = SLEEP_STATUS_VCELLOVP;
	}
	else if(SleepElement.Sleep_Mode.bits.b1VcellUVP != 0) {
		Sleep_Status = SLEEP_STATUS_VCELLUVP;
	}
	else if(SleepElement.Sleep_Mode.bits.b1ForceToSleep_L1 != 0) {
		Sleep_Status = SLEEP_STATUS_FORCED_L1;
	}
	else if(SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2 != 0) {
		Sleep_Status = SLEEP_STATUS_FORCED_L2;
	}
	else if(SleepElement.Sleep_Mode.bits.b1ForceToSleep_L3 != 0) {
		Sleep_Status = SLEEP_STATUS_FORCED_L3;
	}	
	else {		//没有以上各种保护直接进入主循环
		Sleep_Status = SLEEP_STATUS_NORMAL_SELECT;
	}
}


void SleepDeal_Test(void) {
	static UINT16 s_u16HaltTestCnt = 0;
	if(!SleepElement.Sleep_Mode.bits.b1TestSleep) {	//加强雍余设计
		Sleep_Status = SLEEP_STATUS_SHIFT;
		return;
	}

	if(++s_u16HaltTestCnt >= 2) {		//10s——Test
		s_u16HaltTestCnt = 0;
		Sleep_Status = SLEEP_STATUS_CONTINUE;
	}
}


//化繁为简，RTC信号唤醒休眠全部交给外部，实际上就不管了
void GotoSleep(SLEEP_SYSMODE mode)
{
	switch(mode) {
		case HICCUP_MODE:
			BkpRam_WriteUINT16(BKP_SRAM_ADDR_SLEEP_FLAG, BKP_SRAM_HICCUP_SLEEP_VALUE);
			IOstatus_RTCMode();
			InitWakeUp_RTCMode();
			//InitWakeUp_RTCMode_ByIO();
			//InitWakeUp_RTCMode_Alarm();	//控制权交出去
			Sys_StopMode();
			IORecover_RTCMode();
			break;

		case NORMAL_MODE:
			BkpRam_WriteUINT16(BKP_SRAM_ADDR_SLEEP_FLAG, BKP_SRAM_NORMAL_SLEEP_VALUE);
			IOstatus_NormalMode();
			InitWakeUp_NormalMode();
			Sys_StopMode();
			IORecover_NormalMode();
			break;

		case DEEP_MODE:
			BkpRam_WriteUINT16(BKP_SRAM_ADDR_SLEEP_FLAG, BKP_SRAM_DEEP_SLEEP_VALUE);
			IOstatus_DeepMode();
			InitWakeUp_DeepMode();
			//Sys_StandbyMode();		//不能掌控外部IO，弃用
			Sys_StopMode();
			IORecover_DeepMode();
			break;

		default:
			BkpRam_WriteUINT16(BKP_SRAM_ADDR_SLEEP_FLAG, BKP_SRAM_NORMAL_SLEEP_VALUE);
			IOstatus_NormalMode();
			InitWakeUp_NormalMode();
			Sys_StopMode();
			IORecover_NormalMode();
			break;
	}
}


void InitSleepFunction(void) {
	//休眠相关
	SleepElement.Sleep_Mode.all = 0;
	//SleepElement.Sleep_Mode.all = FlashReadOneHalfWord(FLASH_ADDR_SLEEP_ELEMENT);
	if(SleepElement.Sleep_Mode.all == 0xFFFF) {
		SleepElement.Sleep_Mode.all = 0;
		//FlashWriteOneHalfWord(FLASH_ADDR_SLEEP_ELEMENT, SleepElement.Sleep_Mode.all);
	}
	
	if(SleepElement.Sleep_Mode.bits.b1NormalSleep_L1 || SleepElement.Sleep_Mode.bits.b1ForceToSleep_L1) {
		//这里注意一下，在不改标志位的情况下，怎么修改EEPROM
		//测试的话，先修改一下标志位，再复原吧。
		gu8_SleepHiccup = 1;
		//RTC复位后来到这里继续想
	}

	Sleep_Status = SLEEP_STATUS_SHIFT;
}

//各种状况进入休眠(多个循环，例如正常休眠循环，CBC休眠循环，压差过大保护循环)
//通过SleepDeal_NormalQuit()-->主控跳转函数，跳到别的循环
//SleepElement.Sleep_Mode标志位是进入哪种休眠循环，外部决定，出现别的休眠状况，立刻跳过去先进入休眠，然后自动唤醒，再回到相关函数循环。
//所以，流程是：
//SleepElement.Sleep_Mode标志-->SleepDeal_Normal(正常循环)-->SleepDeal_NormalQuit(跳转)-->SleepDeal_Continue(休眠)-->唤醒进入相关循环函数
//以上架构因太过复杂太过难以被后续人员维护，不太契合实际流程，已被修改为如下。
//唤醒进入相关循环函数，含有第一次FIRST和后续HICCUP模式进入两种情况，所以第一次能立刻进入，第二次开始打嗝进入
//SleepElement.Sleep_Mode标志-->SleepDeal_Normal(正常循环)-->SleepDeal_NormalQuit(跳转)-->唤醒进入相关循环函数-->SleepDeal_Continue(休眠)

/*
关于最新的去除flash的sleep的逻辑
1. 直接冻结看门狗，直接进入休眠，不使用flash跳转。
2. 使用备份域？逻辑可以完全不变。
思考一下这个问题。
3. 唤醒，还是复位操作。不直接按顺序运行，原因如下：
	1. IO不好调。有可能运行错误。很复杂，很难找原因。
	2. 有一些没有Init函数的配置咋办？
	3. 外部环境变了，有些参数需要重新复位计算咋办？

4. 汇总过程
开机正常运行 ——> 休眠(标志位写这里，但是目前除了记录没啥用) ——> 唤醒
 ——> 复位 ——> 初始化 ——> 正常运行。
*/
#include "timer.h"
void SleepCtrl(UINT8 TimeBase_1s, UINT8 OnOFF_Ctrl) {
	if(!OnOFF_Ctrl) {									//有个疑问，是不是立刻关了，不需要复原呢，均衡是需要关掉复原。
		return;											//Sleep的话，如果直接不进去，后续打开会接着上次的步伐
	}													//暂且先这么做，后续如果要全盘复原，计时清零再说，目前是接着上次的步伐

	if(0 == TimeBase_1s && !SleepElement.Sleep_Mode.bits.b1ForceToSleep_L1\
		&& !SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2 && !SleepElement.Sleep_Mode.bits.b1ForceToSleep_L3) {
		return;								//如果是强制进入休眠的则必须快点进入休眠，不能拖
	}

	switch(Sleep_Status) {
		case SLEEP_STATUS_INIT:
			InitSleepFunction();
			break;
		case SLEEP_STATUS_SHIFT:		//先跳到这里，再跳到SleepDeal_Continue()，然后进入别的循环
			SleepDeal_Shift();			//主控跳转函数，开机执行一遍没事进入核心循环函数
			break;
		case SLEEP_STATUS_NORMAL_SELECT:
			SleepDeal_Normal_Select();
			break;
		case SLEEP_STATUS_NORMAL_L1:
			SleepDeal_Normal_L1();
			break;
		case SLEEP_STATUS_NORMAL_L2:
			SleepDeal_Normal_L2();
			break;
		case SLEEP_STATUS_NORMAL_L3:
			SleepDeal_Normal_L3();
			break;
		
		case SLEEP_STATUS_FORCED_L1:
			SleepDeal_Forced_L1();
			break;
		case SLEEP_STATUS_FORCED_L2:
			SleepDeal_Forced_L2();
			break;
		case SLEEP_STATUS_FORCED_L3:
			SleepDeal_Forced_L3();
			break;
		
		case SLEEP_STATUS_OVERCUR:
			SleepDeal_OverCurrent();
			break;
		case SLEEP_STATUS_OVDELTA:
			SleepDeal_Vdelta();				//目前压差过大直接进入休眠不起来，亮个灯
			break;
		case SLEEP_STATUS_CBC:
			SleepDeal_CBC();
			break;
		case SLEEP_STATUS_TEST:
			SleepDeal_Test();
			break;
		case SLEEP_STATUS_VCELLOVP:
			SleepDeal_VcellOVP();
			break;
		case SLEEP_STATUS_VCELLUVP:
			SleepDeal_VcellUVP();
			break;
		
		case SLEEP_STATUS_CONTINUE:
			SleepDeal_Continue();
			break;
		default:
			Sleep_Status = SLEEP_STATUS_SHIFT;
			break;
	}

	if(SLEEP_STATUS_CONTINUE == Sleep_Status) {
		SleepElement.u8_ToSleepFlag = 1;
	}
	else {
		SleepElement.u8_ToSleepFlag = 0;
	}
}

