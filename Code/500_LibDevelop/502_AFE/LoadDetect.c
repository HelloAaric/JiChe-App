/******************************************************************************
;  *   	@MCU				 STM32F103C8T6
;  *   	@Create Date         2023.03.16
;  *    @Official website		 http://www.devechip.com/
;  *----------------------Abstract Description---------------------------------
;  *			    						���ؼ���ӡ���
**************************************************************************************/
#include "LoadDetect.h"
#include "Voltage.h"
/**
	* @˵��	ʹ�ܸ�������
	* @����	
	* @����ֵ	
	* @ע	
*/
void LoadPullUp(void)
{
	g_AfeRegs.R81.LDPU=1;

	DVC11XX_WriteRegs(AFE_ADDR_R(81), 1);

	delay_ms(200);//������ȴ�һ��AFE����		

}


/**
	* @˵��	��ѯ�����״̬��ӡ���
	* @����	
	* @����ֵ	
	* @ע	
*/
void LoadDetect(void){

	u16 Vload;	
	
	Vload=DVC11XX_Calc_VLOAD();//����Vload��ѹ
	PRINT_AFE_INFO("Vload = %u mV\r\n", Vload);

	if(Vload>=LoadDetect_Threshold)//�Ƚ���ֵ2V
	{
		PRINT_AFE_INFO("Load Disconnect \r\n");
	}
	else
	{
		PRINT_AFE_INFO("Load Connecting \r\n");
	}
}

