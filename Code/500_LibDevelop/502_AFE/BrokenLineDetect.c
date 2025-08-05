/******************************************************************************
;  *   	@MCU				 STM32F103C8T6
;  *   	@Create Date         2023.03.16
;  *    @Official website		 http://www.devechip.com/
;  *----------------------Abstract Description---------------------------------
;  *			    								断线检测驱动
**************************************************************************************/
#include "BrokenLineDetect.h"

/**
	* @说明	断线检测使能
	* @参数	
	* @返回值	
	* @注	
*/
void COW_enable(void){

	g_AfeRegs.R109.COW=1;//置位

	DVC11XX_WriteRegs(AFE_ADDR_R(109), 1);//写入
}


/**
	* @说明	断线检测结果打印输出
	* @参数	
	* @返回值	
	* @注	
*/
void BrokenLineDetect(void){
u8 i;
int uw_cellvotage[AFE_MAX_CELL_CNT];
	
	for(i=0;i<AFE_MAX_CELL_CNT;i++)
	{
		uw_cellvotage[i]=DVC11XX_Calc_VCell(i);//计算各串电压

		if(uw_cellvotage[i]==0)//断线串打印输出
		{
			PRINT_AFE_INFO("CELL %d Broken!\r\n",i+1);
		}
	}

}

