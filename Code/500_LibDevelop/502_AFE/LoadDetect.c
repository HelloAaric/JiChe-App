/******************************************************************************
;  *   	@MCU				 STM32F103C8T6
;  *   	@Create Date         2023.03.16
;  *    @Official website		 http://www.devechip.com/
;  *----------------------Abstract Description---------------------------------
;  *			    						负载检测打印输出
**************************************************************************************/
#include "LoadDetect.h"
#include "Voltage.h"
/**
	* @说明	使能负载上拉
	* @参数	
	* @返回值	
	* @注	
*/
void LoadPullUp(void)
{
	g_AfeRegs.R81.LDPU=1;

	DVC11XX_WriteRegs(AFE_ADDR_R(81), 1);

	delay_ms(200);//上拉后等待一组AFE数据		

}


/**
	* @说明	查询充电器状态打印输出
	* @参数	
	* @返回值	
	* @注	
*/
void LoadDetect(void){

	u16 Vload;	
	
	Vload=DVC11XX_Calc_VLOAD();//计算Vload电压
	PRINT_AFE_INFO("Vload = %u mV\r\n", Vload);

	if(Vload>=LoadDetect_Threshold)//比较阈值2V
	{
		PRINT_AFE_INFO("Load Disconnect \r\n");
	}
	else
	{
		PRINT_AFE_INFO("Load Connecting \r\n");
	}
}

