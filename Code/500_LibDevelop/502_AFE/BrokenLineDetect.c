/******************************************************************************
;  *   	@MCU				 STM32F103C8T6
;  *   	@Create Date         2023.03.16
;  *    @Official website		 http://www.devechip.com/
;  *----------------------Abstract Description---------------------------------
;  *			    								���߼������
**************************************************************************************/
#include "BrokenLineDetect.h"

/**
	* @˵��	���߼��ʹ��
	* @����	
	* @����ֵ	
	* @ע	
*/
void COW_enable(void){

	g_AfeRegs.R109.COW=1;//��λ

	DVC11XX_WriteRegs(AFE_ADDR_R(109), 1);//д��
}


/**
	* @˵��	���߼������ӡ���
	* @����	
	* @����ֵ	
	* @ע	
*/
void BrokenLineDetect(void){
u8 i;
int uw_cellvotage[AFE_MAX_CELL_CNT];
	
	for(i=0;i<AFE_MAX_CELL_CNT;i++)
	{
		uw_cellvotage[i]=DVC11XX_Calc_VCell(i);//���������ѹ

		if(uw_cellvotage[i]==0)//���ߴ���ӡ���
		{
			PRINT_AFE_INFO("CELL %d Broken!\r\n",i+1);
		}
	}

}

