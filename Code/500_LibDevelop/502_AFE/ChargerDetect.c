#include "ChargerDetect.h"
/******************************************************************************
;  *   	@MCU				 STM32F103C8T6
;  *   	@Create Date         2023.03.15
;  *    @Official website		 http://www.devechip.com/
;  *----------------------Abstract Description---------------------------------
;  *			    						���������ӡ���
**************************************************************************************/

/**
	* @˵��	��ѯ�����״̬��ӡ���
	* @����	
	* @����ֵ	
	* @ע	
*/
void ChargerDetect(void){
	if(g_AfeRegs.R1.PD==1)//��ӡ��Ӧ����
	{		
		PRINT_AFE_INFO("Charger Connected! \r\n");
	}

}

