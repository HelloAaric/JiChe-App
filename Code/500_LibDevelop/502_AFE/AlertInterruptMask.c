/******************************************************************************
;  *   	@MCU				 STM32F103C8T6
;  *   	@Create Date         2023.03.29
;  *    @Official website		 http://www.devechip.com/
;  *----------------------Abstract Description---------------------------------
;  *			    								报警控制驱动
**************************************************************************************/
#include "AlertInterruptMask.h"

/**
	* @说明	中断屏蔽设置
	* @参数	u8 模式
	* @返回值	
	* @注	
*/
void InterruptShield_Config(u8 mode){

	memset((u8 *)&g_AfeRegs+121,mode,1);//R121一次性写入
	DVC11XX_WriteRegs(AFE_ADDR_R(121), 1);
}
