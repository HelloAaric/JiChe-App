/******************************************************************************
;  *   	@MCU				 STM32F103C8T6
;  *   	@Create Date         2023.03.29
;  *    @Official website		 http://www.devechip.com/
;  *----------------------Abstract Description---------------------------------
;  *			    								������������
**************************************************************************************/
#include "AlertInterruptMask.h"

/**
	* @˵��	�ж���������
	* @����	u8 ģʽ
	* @����ֵ	
	* @ע	
*/
void InterruptShield_Config(u8 mode){

	memset((u8 *)&g_AfeRegs+121,mode,1);//R121һ����д��
	DVC11XX_WriteRegs(AFE_ADDR_R(121), 1);
}
