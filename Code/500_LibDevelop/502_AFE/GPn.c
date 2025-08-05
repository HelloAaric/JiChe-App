/******************************************************************************
;  *   	@MCU				 STM32F103C8T6
;  *   	@Create Date         2023.01.20
;  *    @Official website		 http://www.devechip.com/
;  *----------------------Abstract Description---------------------------------
;  *			    					GPn�ܽŸ�������
**************************************************************************************/
#include "DVC11XX.h"
#include "GPn.h"

/**
	* @˵��	GP1��������
	* @����	u8 ����λ      0x00 �ر�     0x01 ��������������    0x02 ģ���ѹ����
												 0x03 �������Ӳ�߿���
	* @����ֵ	
	* @ע	
*/
void GP1_ModeConfig(u8 mode){

	if(mode<=4){
	g_AfeRegs.R116.GP1M=mode;

	DVC11XX_WriteRegs(AFE_ADDR_R(116),1);
}
}
/**
	* @˵��	GP2��������
	* @����	u8 ����λ      0x00 �ر�     0x01 ��������������    0x02 ģ���ѹ����
												 0x06 �ж����  0x07 Ԥ�ŵͱ����
	* @����ֵ	
	* @ע	
*/
void GP2_ModeConfig(u8 mode){

	if((mode<=7)&&!((mode>2)&&(mode<6))){
	g_AfeRegs.R116.GP2M=mode;

	DVC11XX_WriteRegs(AFE_ADDR_R(116),1);	
}
}

/**
	* @˵��	GP3��������
	* @����	u8 ����λ      0x00 �ر�     0x01 ��������������    0x02 ģ���ѹ����
												 0x06 �ж����  0x07 Ԥ��ͱ����
	* @����ֵ	
	* @ע	
*/
void GP3_ModeConfig(u8 mode){

	if((mode<=7)&&!((mode>2)&&(mode<6))){
	g_AfeRegs.R116.GP3M=mode;

	DVC11XX_WriteRegs(AFE_ADDR_R(116),1);	
}
}

/**
	* @˵��	GP4��������
	* @����	u8 ����λ      0x00 �ر�     0x01 ��������������    0x02 ģ���ѹ����
												 0x03 �ŵ�����Ӳ�߿���
	* @����ֵ	
	* @ע	
*/
void GP4_ModeConfig(u8 mode){

	if(mode<=4){
	g_AfeRegs.R117.GP4M=mode;

	DVC11XX_WriteRegs(AFE_ADDR_R(117),1);	
}
}
/**
	* @˵��	GP5��������
	* @����	u8 ����λ      0x00 �ر�     0x01 ��������������    0x02 ģ���ѹ����
												 0x06 �ж����  0x07 ���ͱ����
	* @����ֵ	
	* @ע	
*/
void GP5_ModeConfig(u8 mode){

	if((mode<=7)&&!((mode>2)&&(mode<6))){
	g_AfeRegs.R117.GP5M=mode;

	DVC11XX_WriteRegs(AFE_ADDR_R(117),1);	
}
}

/**
	* @˵��	GP6��������
	* @����	u8 ����λ      0x00 �ر�     0x01 ��������������    0x02 ģ���ѹ����
												 0x06 �ж����  0x07 �ŵ�ͱ����
	* @����ֵ	
	* @ע	
*/
void GP6_ModeConfig(u8 mode){

	if((mode<=7)&&!((mode>2)&&(mode<6))){

	g_AfeRegs.R117.GP6M=mode;

	DVC11XX_WriteRegs(AFE_ADDR_R(117),1);	
}
}

/**
	* @˵��	GP��ģ���ѹ�������
	* @����	u8 GP��
	* @����ֵ	u16 ��ѹֵ
	* @ע	
*/
float GPn_Analog_Input_voltage(u8 GP){
	float temp_voltage;
	u16 GP_value;

	DVC11XX_ReadRegs(AFE_ADDR_R(17+GP),2);//��ȡ��ӦGP��

	GP_value=(g_AfeRegs.R17_28.VGP[GP].VGP_H<<8)|g_AfeRegs.R17_28.VGP[GP].VGP_L;//���ݻ�ԭ

	temp_voltage=	GP_value * 0.1;//GP_value * _lsbVCELL *1000 ��λmv

	return temp_voltage;
}


/**
	* @˵��	�򿪶�ӦFET
	* @����	
	* @����ֵ	
	* @ע	
*/
void FET_Open(void){
#ifdef MODEA								//ģʽA���д򿪳�ŵ�
	g_AfeRegs.R81.CHGC	=	0x3;
	g_AfeRegs.R81.DSGC	=	0x3;
#else												//ģʽB����Ԥ�ȳ�ŵ�
	g_AfeRegs.R81.PCHGC	=	1;
	g_AfeRegs.R81.PDSGC	=	1;
#endif
	DVC11XX_WriteRegs(AFE_ADDR_R(81),1);	

}

void GPnDemo_Cal(void)
{
//	if(!MODE)
//	{//ģʽA
//		float GPn_volage;

//		GPn_volage	=	GPn_Analog_Input_voltage(GP3);//ģ���������
//		PRINT_AFE_INFO("GPn_volage = %.1f mV\r\n",GPn_volage);	
//		
//		if(g_AfeRegs.R4_6.CHGF)//CHG flag
//		{
//			PRINT_AFE_INFO("CHG on\r\n");
//		}

//		if(g_AfeRegs.R4_6.DSGF)//DSG flag
//		{
//			PRINT_AFE_INFO("DSG on\r\n");
//		}

//		if(CHG_status)
//		{//CHG�ͱ����
//			PRINT_AFE_INFO("LOW SIDE CHG on\r\n");
//			CHG_reset;//CHG_req ����
//		}

//		if(DSG_status)
//		{//DSG�ͱ����
//			PRINT_AFE_INFO("LOW SIDE DSG on\r\n");
//			DSG_reset;//DSG_req ����
//		}
//				
//		if(EVENT_alert_req)
//		{//�жϷ���
//			PRINT_AFE_INFO("AFE INT HAPPENED! Check The Register\r\n");
//			EVENT_alert_req=0;//alert_req ����
//		}
//	}
//	else	
//	{//ģʽB
//		if(g_AfeRegs.R4_6.PCHGF)//PCHG flag
//		{
//			PRINT_AFE_INFO("PCHG on\r\n");
//		}

//		if(g_AfeRegs.R4_6.PDSGF)//PDSG flag
//		{
//			PRINT_AFE_INFO("PDSG on\r\n");
//		}

////			if(PCHG_status)
////			{//PCGD�ͱ����
////					PRINT_AFE_INFO("LOW SIDE  PCHG on\r\n");
////					PCHG_reset;//PCHG_req ����
////			}

////			if(PDSG_status)
////			{//
////					PRINT_AFE_INFO("LOW SIDE  PDSG on\r\n");
////					PCHG_reset;//PDSG_req ����
////			}
//	}
		}

/**
	* @˵��	�жϱ�������
	* @����	u8 Ctrl
	* @����ֵ	
	* @ע	
*/
void INT_Ctrl(u8 Ctrl){

memset((u8 *)&g_AfeRegs+121,Ctrl,1);//һ����д��INT_CTRLλ

DVC11XX_WriteRegs(AFE_ADDR_R(121),1);//д��AFE
}

