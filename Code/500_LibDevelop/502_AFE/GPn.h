#ifndef GPN_H__
#define GPN_H__
#include "DVC11XX.h"


//#define MODEA  //ģʽA GP1 GP4Ӳ�߿��ƣ�GP2�ж������GP3ģ�����룬GP5 GP6�ͱ����
#define MODEB //ģʽB GP2 GP3 Ԥ��ŵͱ������GP1��GP4-GP6�ر�

//#define HARDWARE_CONTROL	//Ӳ�߿���
#define HARDWARE_RELEASE	 //�ͷ�Ӳ�߿���
#ifdef MODEA
#define MODE 										0
#define	PCHG_reset							
#define	PDSG_reset	
#define	PCHG_status							0
#define	PDSG_status							0			
#define	CHG_reset								GPIO_ResetBits(GPIOA,GPIO_Pin_0)
#define	DSG_reset								GPIO_ResetBits(GPIOA,GPIO_Pin_1)	
#define GP1_DCHG_HC							GPIO_ResetBits(GPIOA,GPIO_Pin_7)//GP1 �رճ������
#define GP4_DDSG_HC							GPIO_ResetBits(GPIOA,GPIO_Pin_5)//GP4 �رշŵ�����
#define GP1_DCHG_RL							GPIO_SetBits(GPIOA,GPIO_Pin_7)//GP1 �ͷ�Ӳ�߿���
#define GP4_DDSG_RL							GPIO_SetBits(GPIOA,GPIO_Pin_5)//GP4	�ͷ�Ӳ�߿���		
#define	CHG_status							GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//GPIO(PA,0)
#define	DSG_status							GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)//GPIO(PA,1)
#else
#define MODE 										1
#define	CHG_reset								
#define	DSG_reset								
#define	PCHG_reset							GPIO_ResetBits(GPIOA,GPIO_Pin_0)
#define	PDSG_reset							GPIO_ResetBits(GPIOA,GPIO_Pin_1)
#define	GP2_DPDSG_HC						
#define	GP3_DPCHG_HC						
#define	GP2_DPDSG_RL						
#define	GP3_DPCHG_RL							
#define	CHG_status							0
#define	DSG_status							0		
#define	PCHG_status							GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//GPIO(PA,2)
#define	PDSG_status							GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)//GPIO(PA,3)
			
#endif
extern u8 EVENT_alert_req;
void GP1_ModeConfig(u8 mode);
void GP2_ModeConfig(u8 mode);
void GP3_ModeConfig(u8 mode);
void GP4_ModeConfig(u8 mode);
void GP5_ModeConfig(u8 mode);
void GP6_ModeConfig(u8 mode);
void GPn_ModeConfig(bool mode);
float GPn_Analog_Input_voltage(u8 GP);
void GPnDemo_Cal(void);
void FET_Open(void);
void INT_Ctrl(u8 Ctrl);
#endif

