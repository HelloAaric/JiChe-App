/******************************************************************************
;  *   	@MCU				 STM32F103C8T6
;  *   	@Create Date         2023.01.20
;  *    @Official website		 http://www.devechip.com/
;  *----------------------Abstract Description---------------------------------
;  *			          				AFE�����λ                             		
******************************************************************************/
#include "Reset.h"

/**
	* @˵��	AFE�����λ
	* @����	
	* @����ֵ	�ɹ�/ʧ��
	* @ע	
*/
bool AFE_RST(void){

	g_AfeRegs.R1.CST=0xd;//CSTд��1101
		
	return DVC11XX_WriteRegs(AFE_ADDR_R(1), 1);//���͸�λָ��

}
