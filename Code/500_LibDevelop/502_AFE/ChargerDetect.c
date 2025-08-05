#include "ChargerDetect.h"
/******************************************************************************
;  *   	@MCU				 STM32F103C8T6
;  *   	@Create Date         2023.03.15
;  *    @Official website		 http://www.devechip.com/
;  *----------------------Abstract Description---------------------------------
;  *			    						充电器检测打印输出
**************************************************************************************/

/**
	* @说明	查询充电器状态打印输出
	* @参数	
	* @返回值	
	* @注	
*/
void ChargerDetect(void){
	if(g_AfeRegs.R1.PD==1)//打印对应报警
	{		
		PRINT_AFE_INFO("Charger Connected! \r\n");
	}

}

