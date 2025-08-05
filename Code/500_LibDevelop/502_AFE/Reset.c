/******************************************************************************
;  *   	@MCU				 STM32F103C8T6
;  *   	@Create Date         2023.01.20
;  *    @Official website		 http://www.devechip.com/
;  *----------------------Abstract Description---------------------------------
;  *			          				AFE软件复位                             		
******************************************************************************/
#include "Reset.h"

/**
	* @说明	AFE软件复位
	* @参数	
	* @返回值	成功/失败
	* @注	
*/
bool AFE_RST(void){

	g_AfeRegs.R1.CST=0xd;//CST写入1101
		
	return DVC11XX_WriteRegs(AFE_ADDR_R(1), 1);//发送复位指令

}
