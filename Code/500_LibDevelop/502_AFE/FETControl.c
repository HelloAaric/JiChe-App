/******************************************************************************
;  *   	@MCU				 STM32F103C8T6
;  *   	@Create Date         2023.09.20
;  *    @Official website		 http://www.devechip.com/
;  *----------------------Abstract Description---------------------------------
;  *			    								FET控制驱动
**************************************************************************************/
#include "FETControl.h"

/**
	* @说明	预充驱动开关
	* @参数	bool 开/关
	* @返回值	成功/失败
	* @注	
*/
bool PCHG_FETControl(bool abel){

	g_AfeRegs.R81.PCHGC=abel;

	return DVC11XX_WriteRegs(AFE_ADDR_R(81),1);
}
/**
	* @说明	预放驱动开关
	* @参数	bool 开/关
	* @返回值	成功/失败
	* @注	
*/
bool PDSG_FETControl(bool abel){

	g_AfeRegs.R81.PDSGC=abel;

	return DVC11XX_WriteRegs(AFE_ADDR_R(81),1);
}

/**
	* @说明	充电驱动开关
	* @参数	u8 模式
	* @返回值	成功/失败
	* @注	
*/
bool CHG_FETControl(u8 mode){

	if(mode==2)
	g_AfeRegs.R84.CBDM=0;

	g_AfeRegs.R81.CHGC=mode;

	return DVC11XX_WriteRegs(AFE_ADDR_R(81),4);
}

/**
	* @说明	放电驱动开关
	* @参数	u8 模式
	* @返回值	成功/失败
	* @注	
*/
bool DSG_FETControl(u8 mode){

	if(mode==2)
	g_AfeRegs.R83.DBDM=0;

	g_AfeRegs.R81.DSGC=mode;

	return DVC11XX_WriteRegs(AFE_ADDR_R(81),3);
}

/**
	* @说明	高边DSG输出模式设置
	* @参数	bool 模式
	* @返回值	成功/失败
	* @注	
*/
bool DSGM_Control(bool mode){

	g_AfeRegs.R81.DSGM=mode;

	return DVC11XX_WriteRegs(AFE_ADDR_R(81),1);
}

/**
	* @说明	高边DSG下拉强度设置
	* @参数	u8 强度等级 0~30
	* @返回值	成功/失败
	* @注	
*/
bool DPC_Config(u8 DPC){

	if(DPC<31){
	g_AfeRegs.R82.DPC=DPC;

	return DVC11XX_WriteRegs(AFE_ADDR_R(82),1);
	}
	return ERROR;
}
/**
	* @说明	续流保护阈值设置
	* @参数	u16 保护阈值 单位 uV
	* @返回值	成功/失败
	* @注	保护阈值=BDPT*40uV
*/
bool BDPT_Config(u16 value){
	
	if(value<=10200){
	g_AfeRegs.R102.BDPT=value/40;

	return DVC11XX_WriteRegs(AFE_ADDR_R(102),1);
	}
	else
	return ERROR;
}
/**
	* @说明	电荷泵输出设置
	* @参数	u8 模式
	* @返回值	成功/失败
	* @注	
*/
bool ChargePump_Control(u8 mode){
	
	g_AfeRegs.R109.CPVS=mode;

	return DVC11XX_WriteRegs(AFE_ADDR_R(109),1);
}

/**
	* @说明	高边驱动屏蔽设置
	* @参数	bool 开/关
	* @返回值	成功/失败
	* @注	保护阈值=BDPT*40uV
*/
bool HSFM_Control(bool mode){
	
	g_AfeRegs.R85.HSFM=mode;

	return DVC11XX_WriteRegs(AFE_ADDR_R(85),1);
}

