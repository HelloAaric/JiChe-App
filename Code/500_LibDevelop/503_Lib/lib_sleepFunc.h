#ifndef SLEEPFUNCTION_H
#define SLEEPFUNCTION_H

#include "n32h47x_48x.h"

typedef uint32_t  	UINT32;
typedef uint16_t 	UINT16;
typedef uint8_t  	UINT8;
typedef int32_t  	INT32;
typedef UINT8		BOOL;

#define BKP_SRAM_ADDR_SLEEP_FLAG                0x40023010	            // ���߹ؼ���־λ
#define BKP_SRAM_NORMAL_SLEEP_VALUE    	        (0x1234)
#define BKP_SRAM_DEEP_SLEEP_VALUE    		    (0x1235)
#define BKP_SRAM_HICCUP_SLEEP_VALUE    	        (0x1236)
#define BKP_SRAM_SLEEP_RESET_VALUE    		    (0x0000)


//����������ܣ�����Ϊ��
/*
#define FLASH_ADDR_RTC_WK_TIME 			0x0803C000			//RTC���߻���ʱ�����
#define FLASH_VALUE_RTC_WK_TIME_RESET	((UINT16)0xFFFF)
#define FLASH_ADDR_SLEEP_ELEMENT 		0x0803C800			//������ز���
#define FLASH_VALUE_SLEEP_ELEMENT_RESET	((UINT16)0xFFFF)
*/

//��������������־λ��ʵ������һ��״̬�����(���κ�ʱ��SleepModeֻ��һ�������������ֶ������)
//�����Ҿ������ģ���Ϊδ����Ŀ�п��ܳ��ֶ������ͬʱ����
typedef union SLEEP_MODE{
    UINT16   all;
    struct SleepModeFlagBit {
		UINT8 b1TestSleep        	:1;		//b1ForceToSleep���Դ��棬ȡ��
		UINT8 b1NormalSleep_L1      :1;
		UINT8 b1NormalSleep_L2      :1;
		UINT8 b1NormalSleep_L3      :1;
		
		UINT8 b1ForceToSleep_L1     :1;		//�ⲿ�ٿؽ����һ������
        UINT8 b1ForceToSleep_L2     :1;		//�ⲿ�ٿؽ���ڶ�������
		UINT8 b1ForceToSleep_L3     :1;		//�ⲿ�ٿؽ������������
        UINT8 b1ForceToSleep_L1_Out :1;		//�ⲿ�ٿص�һ�������˳�����ģʽ��־λ

		UINT8 b1OverCurSleep        :1;
        UINT8 b1OverVdeltaSleep     :1;
		UINT8 b1CBCSleep       		:1;
		UINT8 b1VcellOVP			:1;
		
		UINT8 b1VcellUVP			:1;
		UINT8 Res2					:3;		//8L����÷������ź�STM32һ��
     }bits;
}SleepMode;


typedef struct SLEEP_ELEMENT {
	//ֻ��Ҫ��ֵһ�εĲ���
	UINT16 u16_TimeRTC_First;		//min����һ�ν���RTC����ʱ�䣬0��ʾ����������
	UINT16 u16_TimeRTC_Hiccup;		//min������ģʽ����RTC����ʱ��
	UINT16 u16_RTC_WakeUpTime;		//min��RTC����ʱ��	
    UINT16 u16_VNormal;      		//mV
	UINT16 u16_TimeNormal;			//min
    UINT16 u16_Vlow;     			//mV
	UINT16 u16_TimeVlow;			//min
	UINT16 u16_VirCur_Chg;     		//A*10
    UINT16 u16_VirCur_Dsg;    		//A*10
	//ֻ��Ҫ��ֵһ�εĲ������½ӿ�
	UINT16 u16_Force_RTC_First;		//δʹ�ã�min����һ�ν���RTC����ʱ�䣬0��ʾ����������
	UINT16 u16_Force_RTC_Hiccup;	//δʹ�ã�min������ģʽ����RTC����ʱ��
	UINT16 u16_Force_RTC_WKtime;	//δʹ�ã�min��RTC����ʱ��
	UINT16 u16_Force_Normal_T;		//min
	UINT16 u16_Force_Vlow_T;		//min


	//��Ҫ����ϸ�ֵ�Ĳ���
	UINT8  u8_ExtComCnt;			//�ⲿͨѶ����
	UINT16 u16_VCellMax;        	//mV
	UINT16 u16_VCellMin;        	//mV
	UINT16 u16_CurChg;				//A*10
	UINT16 u16_CurDsg;				//A*10

	//�ܻ�ȡ�����Ϣ�Ĳ���
	//Driver_OnOFF_Etc MosRelay_Status;	//���Ǿ���ʡ�ռ䣬Ŀ����8L��103������
	UINT8 u8_ToSleepFlag;			//��Ϊ1ʱ��1s��������ߣ����ʱ��Ϊ0
	
	//��Ϣ������
	volatile SleepMode Sleep_Mode;		//�����ⲿ���ƽ������߱�־λ
}Sleep_Element;


// �ⲿʵ�ֺ���
extern void InitWakeUp_RTCMode(void);
extern void InitWakeUp_NormalMode(void);
extern void InitWakeUp_DeepMode(void);

extern void IOstatus_RTCMode(void);
extern void IOstatus_NormalMode(void);
extern void IOstatus_DeepMode(void);

extern void BkpRam_WriteUINT16(UINT32 adress, UINT16 data);
extern UINT16 BkpRam_ReadUINT16(UINT32 adress);


// �ⲿ����ʹ�õĺ����ͱ���
extern UINT16 aaaa1;
extern UINT16 aaaa2;
extern UINT16 aaaa3;
extern UINT16 aaaa4;
extern Sleep_Element SleepElement;
void SleepCtrl(UINT8 TimeBase_1s, UINT8 OnOFF_Ctrl);

#endif	/* SLEEPFUNCTION_H */



/*
1����ѹ����ѹ��һ���ǽ���RTCģʽ
>>�����������
2������ֱ�ӽ������ߣ�������������ģʽ(���ٻ��ǰ���ʱ���أ�)�Ľӿ�
>>������Ŀ��ʱ��Ҳ�ܸ�
3��ֱ�ӽ���RTC
>>ͬ����2
4��ֱ�ӽ���������ߣ�
>>ͬ����2
5����ı���״���������ѹ������������ܱ�ȡ��������������ߣ�
>>������2������һ���ټ����Լ����߼�
3��Test����Ҫ���ϣ����Ե�����
>>��ʹ�ã�b1ForceToSleep��ȡ��

*/
