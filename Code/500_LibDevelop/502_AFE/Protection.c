/******************************************************************************
;  *   	@MCU				 STM32F103C8T6
;  *   	@Create Date         2023.08.20
;  *    @Official website		 http://www.devechip.com/
;  *----------------------Abstract Description---------------------------------
;  *			          AFE保护功能配置处理                              		
******************************************************************************/
#include "Protection.h"

/**
	* @说明	一级放电过流配置
	* @参数	u16 阈值 250-63750uv 步进 250uv
					u16 DLY  8-2040ms 步进 8ms
	* @返回值	成功/失败
	* @注	
*/
bool OCD1_Config(u16 TH,u16 DLY){

if(TH>=250&&TH<=63750&&DLY>=8&&DLY<=2040){//未超量程
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
	* @说明	二级放电过流配置
	* @参数	u16 TH 4-256mv 步进4mv
					u16 DLY 4-1020ms 步进4ms
					bool enable enable/disable
	* @返回值	成功/失败
	* @注	
*/
bool OCD2_Config(u16 TH,u16 DLY,bool enable){//

if(!enable){//关闭OCD2
	g_AfeRegs.R94.OCD2E=0;
	return DVC11XX_WriteRegs(AFE_ADDR_R(94),1);
}
else{
	if(TH<=256&&DLY<=1020){//未超量程
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
	* @说明	一级充电过流配置
	* @参数	u16 阈值 250-63750uv 步进 250uv
					u16 DLY  8-2040ms 步进 8ms
	* @返回值	成功/失败
	* @注	
*/
bool OCC1_Config(u16 TH,u16 DLY){
if(TH>=250&&TH<=63750&&DLY>=8&&DLY<=2040){//未超量程
g_AfeRegs.R90.OCC1T=TH/250;
g_AfeRegs.R92.OCC1D=DLY/8-1;
return DVC11XX_WriteRegs(AFE_ADDR_R(90),1)&DVC11XX_WriteRegs(AFE_ADDR_R(92),1);
}
else if(TH==0)//关闭
{
g_AfeRegs.R90.OCC1T=0;
return DVC11XX_WriteRegs(AFE_ADDR_R(90),1);
}
else
return ERROR;
}
/**
	* @说明	二级充电过流配置
	* @参数	u16 TH 4-128mv 步进4mv
					u16 DLY 4-1020ms 步进4ms
					bool enable enable/disable
	* @返回值	成功/失败
	* @注	
*/
bool OCC2_Config(u16 TH,u16 DLY,bool enable){//
if(!enable){//关闭OCC2
	g_AfeRegs.R95.OCC2E=0;
	return DVC11XX_WriteRegs(AFE_ADDR_R(95),1);
}
else{
	if(TH<=128&&DLY<=1020){//未超量程
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
	* @说明	过压配置
	* @参数	u16 TH 500-4595mv 步进1mv
					u16 DLY 200-8000ms 200-1000步进100, 1000-8000步进1000
	* @返回值	成功/失败
	* @注	
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
	if(TH>500&&TH<=4595&&DLY>=200&&DLY<=8000){//未超量程
		if(DLY>1000)//1s以上步进1s
		g_AfeRegs.R113.COVD=DLY/1000+7;	
		else
		g_AfeRegs.R113.COVD=DLY/100-2;	//1s以下步进100ms	
	g_AfeRegs.R112.COVT_H=TH>>4;//u12 高8位
	g_AfeRegs.R113.COVT_L=TH&0x0f;//u12 低4位
	return DVC11XX_WriteRegs(AFE_ADDR_R(112),2);
	}
	else
	return ERROR;
	}
}
/**
	* @说明	欠压配置
	* @参数	u16 TH 1-4095mv 步进1mv
					u16 DLY 200-8000ms 200-1000步进100, 1000-8000步进1000
	* @返回值	成功/失败
	* @注	
*/
bool UV_Config(u16 TH,u16 DLY,bool enable){
	if(!enable)
	{
		g_AfeRegs.R114.CUVT_H=0;
		g_AfeRegs.R115.CUVT_L=0;
		return DVC11XX_WriteRegs(AFE_ADDR_R(114),2);
	}
	else{
	if(TH>=1&&TH<=4095&&DLY>=200&&DLY<=8000){//未超量程
		if(DLY>1000)//1s以上步进1s
		g_AfeRegs.R115.CUVD=DLY/1000+7;	
		else
		g_AfeRegs.R115.CUVD=DLY/100-2;//1s以下步进100ms	
		g_AfeRegs.R114.CUVT_H=TH>>4;//u12 高8位
		g_AfeRegs.R115.CUVT_L=TH&0x0f;//u12 低4位
		return DVC11XX_WriteRegs(AFE_ADDR_R(114),2);
	}
	else
	return ERROR;
	}

}
/**
	* @说明	放电短路配置
	* @参数	u16 阈值 10-640mv 步进 10mv
					float DLY  0-1992us 步进 7.81us
	* @返回值	成功/失败
	* @注	
*/
bool SCD_Config(u16 TH,float DLY,bool enable){
	if(!enable){//关闭SCD
		g_AfeRegs.R98.SCDE=0;
		return DVC11XX_WriteRegs(AFE_ADDR_R(98),1);
	}
	else{
		if(TH>=10&&TH<=310&&DLY<=1992){//未超量程
		g_AfeRegs.R98.SCDT=TH/10;
		g_AfeRegs.R99.SCDD=DLY/7.81f;
		g_AfeRegs.R98.SCDE=enable;
		return DVC11XX_WriteRegs(AFE_ADDR_R(98),2);
		}
		else
		return ERROR;
}

}

