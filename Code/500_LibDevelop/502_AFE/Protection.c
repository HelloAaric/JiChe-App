/******************************************************************************
;  *   	@MCU				 STM32F103C8T6
;  *   	@Create Date         2023.08.20
;  *    @Official website		 http://www.devechip.com/
;  *----------------------Abstract Description---------------------------------
;  *			          AFE�����������ô���                              		
******************************************************************************/
#include "Protection.h"

/**
	* @˵��	һ���ŵ��������
	* @����	u16 ��ֵ 250-63750uv ���� 250uv
					u16 DLY  8-2040ms ���� 8ms
	* @����ֵ	�ɹ�/ʧ��
	* @ע	
*/
bool OCD1_Config(u16 TH,u16 DLY){

if(TH>=250&&TH<=63750&&DLY>=8&&DLY<=2040){//δ������
g_AfeRegs.R89.OCD1T=TH/250;
g_AfeRegs.R91.OCD1D=DLY/8-1;
return DVC11XX_WriteRegs(AFE_ADDR_R(89),1)&DVC11XX_WriteRegs(AFE_ADDR_R(91),1);
}
else if(TH==0){
g_AfeRegs.R89.OCD1T=0;
return DVC11XX_WriteRegs(AFE_ADDR_R(89),1);
}
else
return ERROR;
}
/**
	* @˵��	�����ŵ��������
	* @����	u16 TH 4-256mv ����4mv
					u16 DLY 4-1020ms ����4ms
					bool enable enable/disable
	* @����ֵ	�ɹ�/ʧ��
	* @ע	
*/
bool OCD2_Config(u16 TH,u16 DLY,bool enable){//

if(!enable){//�ر�OCD2
	g_AfeRegs.R94.OCD2E=0;
	return DVC11XX_WriteRegs(AFE_ADDR_R(94),1);
}
else{
	if(TH<=256&&DLY<=1020){//δ������
	g_AfeRegs.R94.OCD2T=TH/4-1;
	g_AfeRegs.R96.OCD2D=DLY/4-1;
	g_AfeRegs.R94.OCD2E=enable;
	return DVC11XX_WriteRegs(AFE_ADDR_R(94),1)&DVC11XX_WriteRegs(AFE_ADDR_R(96),1);
	}
	else
	return ERROR;
}
}
/**
	* @˵��	һ������������
	* @����	u16 ��ֵ 250-63750uv ���� 250uv
					u16 DLY  8-2040ms ���� 8ms
	* @����ֵ	�ɹ�/ʧ��
	* @ע	
*/
bool OCC1_Config(u16 TH,u16 DLY){
if(TH>=250&&TH<=63750&&DLY>=8&&DLY<=2040){//δ������
g_AfeRegs.R90.OCC1T=TH/250;
g_AfeRegs.R92.OCC1D=DLY/8-1;
return DVC11XX_WriteRegs(AFE_ADDR_R(90),1)&DVC11XX_WriteRegs(AFE_ADDR_R(92),1);
}
else if(TH==0)//�ر�
{
g_AfeRegs.R90.OCC1T=0;
return DVC11XX_WriteRegs(AFE_ADDR_R(90),1);
}
else
return ERROR;
}
/**
	* @˵��	��������������
	* @����	u16 TH 4-128mv ����4mv
					u16 DLY 4-1020ms ����4ms
					bool enable enable/disable
	* @����ֵ	�ɹ�/ʧ��
	* @ע	
*/
bool OCC2_Config(u16 TH,u16 DLY,bool enable){//
if(!enable){//�ر�OCC2
	g_AfeRegs.R95.OCC2E=0;
	return DVC11XX_WriteRegs(AFE_ADDR_R(95),1);
}
else{
	if(TH<=128&&DLY<=1020){//δ������
	g_AfeRegs.R95.OCC2T=TH/4-1;
	g_AfeRegs.R97.OCC2D=DLY/4-1;
	g_AfeRegs.R95.OCC2E=enable;
	return DVC11XX_WriteRegs(AFE_ADDR_R(95),1)&DVC11XX_WriteRegs(AFE_ADDR_R(97),1);
	}
	else
	return ERROR;
}
}

/**
	* @˵��	��ѹ����
	* @����	u16 TH 500-4595mv ����1mv
					u16 DLY 200-8000ms 200-1000����100, 1000-8000����1000
	* @����ֵ	�ɹ�/ʧ��
	* @ע	
*/
bool OV_Config(u16 TH,u16 DLY,bool enable){
	if(!enable)
	{
		g_AfeRegs.R112.COVT_H=0;
		g_AfeRegs.R113.COVT_L=0;
		return DVC11XX_WriteRegs(AFE_ADDR_R(112),2);
	}
	else
	{
	if(TH>500&&TH<=4595&&DLY>=200&&DLY<=8000){//δ������
		if(DLY>1000)//1s���ϲ���1s
		g_AfeRegs.R113.COVD=DLY/1000+7;	
		else
		g_AfeRegs.R113.COVD=DLY/100-2;	//1s���²���100ms	
	g_AfeRegs.R112.COVT_H=TH>>4;//u12 ��8λ
	g_AfeRegs.R113.COVT_L=TH&0x0f;//u12 ��4λ
	return DVC11XX_WriteRegs(AFE_ADDR_R(112),2);
	}
	else
	return ERROR;
	}
}
/**
	* @˵��	Ƿѹ����
	* @����	u16 TH 1-4095mv ����1mv
					u16 DLY 200-8000ms 200-1000����100, 1000-8000����1000
	* @����ֵ	�ɹ�/ʧ��
	* @ע	
*/
bool UV_Config(u16 TH,u16 DLY,bool enable){
	if(!enable)
	{
		g_AfeRegs.R114.CUVT_H=0;
		g_AfeRegs.R115.CUVT_L=0;
		return DVC11XX_WriteRegs(AFE_ADDR_R(114),2);
	}
	else{
	if(TH>=1&&TH<=4095&&DLY>=200&&DLY<=8000){//δ������
		if(DLY>1000)//1s���ϲ���1s
		g_AfeRegs.R115.CUVD=DLY/1000+7;	
		else
		g_AfeRegs.R115.CUVD=DLY/100-2;//1s���²���100ms	
		g_AfeRegs.R114.CUVT_H=TH>>4;//u12 ��8λ
		g_AfeRegs.R115.CUVT_L=TH&0x0f;//u12 ��4λ
		return DVC11XX_WriteRegs(AFE_ADDR_R(114),2);
	}
	else
	return ERROR;
	}

}
/**
	* @˵��	�ŵ��·����
	* @����	u16 ��ֵ 10-640mv ���� 10mv
					float DLY  0-1992us ���� 7.81us
	* @����ֵ	�ɹ�/ʧ��
	* @ע	
*/
bool SCD_Config(u16 TH,float DLY,bool enable){
	if(!enable){//�ر�SCD
		g_AfeRegs.R98.SCDE=0;
		return DVC11XX_WriteRegs(AFE_ADDR_R(98),1);
	}
	else{
		if(TH>=10&&TH<=310&&DLY<=1992){//δ������
		g_AfeRegs.R98.SCDT=TH/10;
		g_AfeRegs.R99.SCDD=DLY/7.81f;
		g_AfeRegs.R98.SCDE=enable;
		return DVC11XX_WriteRegs(AFE_ADDR_R(98),2);
		}
		else
		return ERROR;
}

}

