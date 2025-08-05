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


#define RTC_WT_Protect 		5         	//���ֱ����������ߴ���ʱ��
#define RTC_WT_Normal 		5         	//����״̬�������ߴ���ʱ��
#define RTC_WT_Force 		5         	//�ⲿǿ�ƽ������ߴ���ʱ��

#define SleepInitOC 		10	  		//1000msʱ��������ģʽ��������ʼ����ʱ���ж��Ƿ��ٽ�������
#define SleepInitCBC 		10	  		//1000msʱ��������ģʽ��������ʼ����ʱ���ж��Ƿ��ٽ�������
#define SleepInitForce 		10	  		//200msʱ��������ģʽ��������ʼ����ʱ���ж��Ƿ��ٽ�������
#define SleepInitOVP 		240	  		//1000msʱ��������ģʽ��������ʼ����ʱ���ж��Ƿ��ٽ�������
#define SleepInitUVP 		240	  		//1000msʱ��������ģʽ��������ʼ����ʱ���ж��Ƿ��ٽ�������
#define SleepInitNormal1 	100	  		//1000msʱ��������ģʽ��������ʼ����ʱ���ж��Ƿ��ٽ�������
#define SleepInitNormal2 	150	  		//1000msʱ��������ģʽ���������ֵ������ӳ���30s�ж�


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
	//TIM_Cmd(TIM3, ENABLE);	//����App_SleepTest()����
	NVIC_SystemReset();				//����ֱ������ȥ���¸�����Ϊ�ֳ��ƻ��޷�������������ģʽ�������Ľ���취�Ǹ�λ������
}


void IORecover_DeepMode(void) {
	NVIC_SystemReset();
}


//wkup������
void Sys_StandbyMode(void) {  
	RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);	//ʹ��PWR����ʱ�ӣ���仰�Ƿ���Ҫ��030����ҪҲ�ܽ�������
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

	if(!SleepElement.Sleep_Mode.bits.b1OverCurSleep) {		//��ǿӺ�����
		Sleep_Status = SLEEP_STATUS_SHIFT;		//��ʵ������Բ�Ҫ�����Ҫ�󣬳�������������԰������־λȥ���⣬��ĵط������Ա��
		return;
	}

	switch(s_u8SleepStatus) {
		case FIRST:
			if(++s_u32SleepFirstCnt > 0) {		//����λ�ã�������һ�κ������Ҫ��ʱ�������
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
			s_u8SleepStatus = FIRST;		//�¸��غ�����
			break;
	}


	if(0) {										//�����⵽û���⣬���˳�����
		SleepElement.Sleep_Mode.bits.b1OverCurSleep = 0;		//�ŵ�switch������棬FIRST��HICCUP��������Ч
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

	if(!SleepElement.Sleep_Mode.bits.b1VcellUVP) {		//��ǿӺ�����
		Sleep_Status = SLEEP_STATUS_SHIFT;	//��ʵ������Բ�Ҫ�����Ҫ�󣬳�������������԰������־λȥ���⣬��ĵط������Ա��
		return ;
	}

	switch(s_u8SleepStatus) {
		case FIRST:
			if(++s_u32SleepFirstCnt > 0) {		//����λ�ã�������һ�κ������Ҫ��ʱ�������
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
			s_u8SleepStatus = FIRST;		//�¸��غ�����
			break;
	}

	if(0) {									//�����⵽û���⣬���˳�����
		SleepElement.Sleep_Mode.bits.b1VcellUVP = 0;		//�ŵ�switch������棬FIRST��HICCUP��������Ч
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

	if(!SleepElement.Sleep_Mode.bits.b1CBCSleep) {		//��ǿӺ�����
		Sleep_Status = SLEEP_STATUS_SHIFT;	//��ʵ������Բ�Ҫ�����Ҫ�󣬳�������������԰������־λȥ���⣬��ĵط������Ա��
		return ;
	}

	switch(s_u8SleepStatus) {
		case FIRST:
			if(++s_u32SleepFirstCnt > 0) {		//����λ�ã�������һ�κ������Ҫ��ʱ�������
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
			s_u8SleepStatus = FIRST;		//�¸��غ�����
			break;
	}

	if(0) {									//�����⵽û���⣬���˳�����
		SleepElement.Sleep_Mode.bits.b1CBCSleep = 0;		//�ŵ�switch������棬FIRST��HICCUP��������Ч
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

	if(!SleepElement.Sleep_Mode.bits.b1ForceToSleep_L1) {		//��ǿӺ�����
		Sleep_Status = SLEEP_STATUS_SHIFT;
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
		return ;
	}

	switch(s_u8SleepStatus) {
		case FIRST:
			if(++s_u32SleepFirstCnt > 0) {	//ֱ�ӽ�ȥ
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
			s_u8SleepStatus = FIRST;		//�¸��غ�����
			break;
	}

	if(0) {										//�����⵽û���⣬���˳�����
		//Sleep_Mode.bits.b1ForceToSleep = 0;
		//Sleep_Status = SLEEP_HICCUP_SHIFT;
		s_u8SleepStatus = FIRST;			//ֱ�ӻص���һ�Σ�forceֻ��һ�Σ����Ǵ�������ģʽ
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
	}

}


void SleepDeal_Forced_L2(void) {
	static UINT8 s_u8SleepStatus = FIRST;
	static UINT32 s_u32SleepFirstCnt = 0;
	static UINT32 s_u32SleepHiccupCnt = 0;

	if(!SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2) {		//��ǿӺ�����
		Sleep_Status = SLEEP_STATUS_SHIFT;
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
		return;
	}
	
	switch(s_u8SleepStatus) {
		case FIRST:
			if(++s_u32SleepFirstCnt > 0) {	//ֱ�ӽ�ȥ
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
			s_u8SleepStatus = FIRST;		//�¸��غ�����
			break;
	}

	if(0) {										//�����⵽û���⣬���˳�����
		//Sleep_Mode.bits.b1ForceToSleep = 0;
		//Sleep_Status = SLEEP_HICCUP_SHIFT;
		s_u8SleepStatus = FIRST;			//ֱ�ӻص���һ�Σ�forceֻ��һ�Σ����Ǵ�������ģʽ
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
	}
}


void SleepDeal_Forced_L3(void) {
	static UINT8 s_u8SleepStatus = FIRST;
	static UINT32 s_u32SleepFirstCnt = 0;
	static UINT32 s_u32SleepHiccupCnt = 0;

	if(!SleepElement.Sleep_Mode.bits.b1ForceToSleep_L3) {			//��ǿӺ�����
		Sleep_Status = SLEEP_STATUS_SHIFT;
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;		
		return ;
	}

	switch(s_u8SleepStatus) {
		case FIRST:
			if(++s_u32SleepFirstCnt > 0) {	//ֱ�ӽ�ȥ
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
			s_u8SleepStatus = FIRST;		//�¸��غ�����
			break;
	}

	if(0) {										//�����⵽û���⣬���˳�����
		//Sleep_Mode.bits.b1ForceToSleep = 0;
		//Sleep_Status = SLEEP_HICCUP_SHIFT;
		s_u8SleepStatus = FIRST;			//ֱ�ӻص���һ�Σ�forceֻ��һ�Σ����Ǵ�������ģʽ
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
	
	if((SleepElement.Sleep_Mode.all&0xFFF1) != 0) {		//����
		SleepElement.Sleep_Mode.bits.b1NormalSleep_L1 = 0;
		SleepElement.Sleep_Mode.bits.b1NormalSleep_L2 = 0;
		SleepElement.Sleep_Mode.bits.b1NormalSleep_L3 = 0;
		Sleep_Status = SLEEP_STATUS_SHIFT;
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
		return ;
	}
	if(gu8_SleepHiccup) {					//��ΪRESET�����⣬ֻ�ܴ��־λ��
		s_u8SleepStatus = HICCUP;
		gu8_SleepHiccup = 0;
	}
	
	//u16Sleep_TimeNormal�޸ĺ�ȫ����λ��������ͨѶ
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
				//���ָ�ʱ��Ϊ0ʱĬ��RTC����������
			}
			else {
				if(++s_u32SleepFirstCnt > (UINT32)SleepElement.u16_TimeRTC_First*60) {
				//if(++s_u32SleepFirstCnt >= 5) {			//�������һ�θ����涼��һ��
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
			s_u8SleepStatus = FIRST;		//�¸��غ�����
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
	//s_u32SleepFirstCnt = 0;		//��û����L1���������ߡ�
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
			//if(++s_u32SleepFirstCnt >= 1) {			//�������һ�θ����涼��һ��
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
			s_u8SleepStatus = FIRST;		//�¸��غ�����
			break;
	}

	if(SleepElement.u16_CurChg > SleepElement.u16_VirCur_Chg || SleepElement.u16_CurDsg > SleepElement.u16_VirCur_Dsg) {
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
	}

	if(SleepElement.u16_VCellMin < SleepElement.u16_Vlow\
		|| SleepElement.u16_VCellMin > SleepElement.u16_VNormal) {	//������������ת�����ʱ�䲻��ת
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

	//�͵�ѹ���ߣ���������Ƿ���ͨѶ�����������
	if(su16_Sleep_TimeNormal != SleepElement.u16_TimeVlow) {
		su16_Sleep_TimeNormal = SleepElement.u16_TimeVlow;
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
	}
	
	switch(s_u8SleepStatus) {
		case FIRST:
			if(++s_u32SleepFirstCnt > (UINT32)SleepElement.u16_TimeVlow*60) {
			//if(++s_u32SleepFirstCnt >= 1) {			//�������һ�θ����涼��һ��
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
			s_u8SleepStatus = FIRST;		//�¸��غ�����
			break;
	}

	if(SleepElement.u16_CurChg > SleepElement.u16_VirCur_Chg || SleepElement.u16_CurDsg > SleepElement.u16_VirCur_Dsg) {
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
	}

	if(SleepElement.u16_VCellMin >= SleepElement.u16_Vlow) {	//������������ת�����ʱ�䲻��ת
		SleepElement.Sleep_Mode.bits.b1NormalSleep_L3 = 0;
		Sleep_Status = SLEEP_STATUS_SHIFT;
		s_u8SleepStatus = FIRST;
		if(s_u32SleepFirstCnt)s_u32SleepFirstCnt = 0;
		if(s_u32SleepHiccupCnt)s_u32SleepHiccupCnt = 0;
	}
	aaaa4 = s_u32SleepFirstCnt;
}


//����ط���IO���Ʋ���Ҫ��һ�£�������ʱ1s�ٴ򿪹��ӻ᲻����ã���������ò��ֱ�Ӵ�û����
//�����Ϊ��ѭ���������ͷ�жϳ����˱�Ĵ�����������ѭ����ȥִ�б��
//���������IO�������������߼����⣬A���ͷ����Sleep��return���⡣B����������IO�Ƿ����̴򿪵�����
void SleepDeal_Normal_Select(void) {
	if((SleepElement.Sleep_Mode.all&0xFFF1) != 0) {				//����
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
		else {			//�Ⱥž�����L2
			SleepElement.Sleep_Mode.bits.b1NormalSleep_L2 = 1;
			Sleep_Status = SLEEP_STATUS_NORMAL_L2;
		}
	}
	else {
		//�е�����������������ѭ��
	}
}


//�ܹ�����Ҫ��һ�ģ���Ȼ������Ա̫��ά����
void SleepDeal_Shift(void) {
	if(SleepElement.Sleep_Mode.bits.b1TestSleep != 0) {
		Sleep_Status = SLEEP_STATUS_TEST;
	}
	else if(SleepElement.Sleep_Mode.bits.b1OverCurSleep != 0) {
		//Sleep_Status = SLEEP_STATUS_CONTINUE;			//�ܹ��Ѹģ���������غ������ٽ�������
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
	else {		//û�����ϸ��ֱ���ֱ�ӽ�����ѭ��
		Sleep_Status = SLEEP_STATUS_NORMAL_SELECT;
	}
}


void SleepDeal_Test(void) {
	static UINT16 s_u16HaltTestCnt = 0;
	if(!SleepElement.Sleep_Mode.bits.b1TestSleep) {	//��ǿӺ�����
		Sleep_Status = SLEEP_STATUS_SHIFT;
		return;
	}

	if(++s_u16HaltTestCnt >= 2) {		//10s����Test
		s_u16HaltTestCnt = 0;
		Sleep_Status = SLEEP_STATUS_CONTINUE;
	}
}


//����Ϊ��RTC�źŻ�������ȫ�������ⲿ��ʵ���ϾͲ�����
void GotoSleep(SLEEP_SYSMODE mode)
{
	switch(mode) {
		case HICCUP_MODE:
			BkpRam_WriteUINT16(BKP_SRAM_ADDR_SLEEP_FLAG, BKP_SRAM_HICCUP_SLEEP_VALUE);
			IOstatus_RTCMode();
			InitWakeUp_RTCMode();
			//InitWakeUp_RTCMode_ByIO();
			//InitWakeUp_RTCMode_Alarm();	//����Ȩ����ȥ
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
			//Sys_StandbyMode();		//�����ƿ��ⲿIO������
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
	//�������
	SleepElement.Sleep_Mode.all = 0;
	//SleepElement.Sleep_Mode.all = FlashReadOneHalfWord(FLASH_ADDR_SLEEP_ELEMENT);
	if(SleepElement.Sleep_Mode.all == 0xFFFF) {
		SleepElement.Sleep_Mode.all = 0;
		//FlashWriteOneHalfWord(FLASH_ADDR_SLEEP_ELEMENT, SleepElement.Sleep_Mode.all);
	}
	
	if(SleepElement.Sleep_Mode.bits.b1NormalSleep_L1 || SleepElement.Sleep_Mode.bits.b1ForceToSleep_L1) {
		//����ע��һ�£��ڲ��ı�־λ������£���ô�޸�EEPROM
		//���ԵĻ������޸�һ�±�־λ���ٸ�ԭ�ɡ�
		gu8_SleepHiccup = 1;
		//RTC��λ���������������
	}

	Sleep_Status = SLEEP_STATUS_SHIFT;
}

//����״����������(���ѭ����������������ѭ����CBC����ѭ����ѹ����󱣻�ѭ��)
//ͨ��SleepDeal_NormalQuit()-->������ת�������������ѭ��
//SleepElement.Sleep_Mode��־λ�ǽ�����������ѭ�����ⲿ���������ֱ������״������������ȥ�Ƚ������ߣ�Ȼ���Զ����ѣ��ٻص���غ���ѭ����
//���ԣ������ǣ�
//SleepElement.Sleep_Mode��־-->SleepDeal_Normal(����ѭ��)-->SleepDeal_NormalQuit(��ת)-->SleepDeal_Continue(����)-->���ѽ������ѭ������
//���ϼܹ���̫������̫�����Ա�������Աά������̫����ʵ�����̣��ѱ��޸�Ϊ���¡�
//���ѽ������ѭ�����������е�һ��FIRST�ͺ���HICCUPģʽ����������������Ե�һ�������̽��룬�ڶ��ο�ʼ���ý���
//SleepElement.Sleep_Mode��־-->SleepDeal_Normal(����ѭ��)-->SleepDeal_NormalQuit(��ת)-->���ѽ������ѭ������-->SleepDeal_Continue(����)

/*
�������µ�ȥ��flash��sleep���߼�
1. ֱ�Ӷ��ῴ�Ź���ֱ�ӽ������ߣ���ʹ��flash��ת��
2. ʹ�ñ������߼�������ȫ���䡣
˼��һ��������⡣
3. ���ѣ����Ǹ�λ��������ֱ�Ӱ�˳�����У�ԭ�����£�
	1. IO���õ����п������д��󡣺ܸ��ӣ�������ԭ��
	2. ��һЩû��Init����������զ�죿
	3. �ⲿ�������ˣ���Щ������Ҫ���¸�λ����զ�죿

4. ���ܹ���
������������ ����> ����(��־λд�������Ŀǰ���˼�¼ûɶ��) ����> ����
 ����> ��λ ����> ��ʼ�� ����> �������С�
*/
#include "timer.h"
void SleepCtrl(UINT8 TimeBase_1s, UINT8 OnOFF_Ctrl) {
	if(!OnOFF_Ctrl) {									//�и����ʣ��ǲ������̹��ˣ�����Ҫ��ԭ�أ���������Ҫ�ص���ԭ��
		return;											//Sleep�Ļ������ֱ�Ӳ���ȥ�������򿪻�����ϴεĲ���
	}													//��������ô�����������Ҫȫ�̸�ԭ����ʱ������˵��Ŀǰ�ǽ����ϴεĲ���

	if(0 == TimeBase_1s && !SleepElement.Sleep_Mode.bits.b1ForceToSleep_L1\
		&& !SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2 && !SleepElement.Sleep_Mode.bits.b1ForceToSleep_L3) {
		return;								//�����ǿ�ƽ������ߵ��������������ߣ�������
	}

	switch(Sleep_Status) {
		case SLEEP_STATUS_INIT:
			InitSleepFunction();
			break;
		case SLEEP_STATUS_SHIFT:		//���������������SleepDeal_Continue()��Ȼ�������ѭ��
			SleepDeal_Shift();			//������ת����������ִ��һ��û�½������ѭ������
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
			SleepDeal_Vdelta();				//Ŀǰѹ�����ֱ�ӽ������߲�������������
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

