/******************************************************************************
;  *   	@MCU				 STM32F103C8T6
;  *   	@Create Date         2023.03.22
;  *    @Official website		 http://www.devechip.com/
;  *----------------------Abstract Description---------------------------------
;  *			    								睡眠控制处理
**************************************************************************************/
//#include "AlertInterruptMask.h"
#include "Sleep.h"
/**
	* @说明	打开定时睡眠
	* @参数	
	* @返回值	
	* @注	各阈值根据实际情况写入
*/
void Sleep_config(void){

	DVC11XX_ReadRegs(AFE_ADDR_R(85), 17);
#ifdef TWK	//定时唤醒
	g_AfeRegs.R120.TIWK=1;//开启定时唤醒
	g_AfeRegs.R120.TWSE=1;//10秒
	DVC11XX_WriteRegs(AFE_ADDR_R(120),1);
#endif
#ifdef CWK //电流唤醒
	g_AfeRegs.R85.CAES=1;//休眠CADC使能
	g_AfeRegs.R86.C1OS=1;//8ms
	DVC11XX_WriteRegs(AFE_ADDR_R(85),2);
	//二级充放电过流唤醒
	g_AfeRegs.R94.OCD2E=OCD2_E;//二级放电过流使能
	g_AfeRegs.R94.OCD2T=2;//8mV	二级放电过流阈值
	g_AfeRegs.R95.OCC2E=OCC2_E;//二级充电过流使能
	g_AfeRegs.R95.OCC2T=2;//8mV	二级充电过流阈值
	g_AfeRegs.R96.OCD2D=4;//二级放电过流时延 16ms
	g_AfeRegs.R97.OCC2D=4;//二级充电过流时延 16ms
	//短路唤醒
	g_AfeRegs.R98.SCDE=SCD_E;//放电短路保护使能
	g_AfeRegs.R98.SCDT=2;//20mV	放电短路阈值
	g_AfeRegs.R99.SCDD=1;//7.81us 短路保护延时
	//
	g_AfeRegs.R101.CWT=50;//0.5mV 电流唤醒阈值
	DVC11XX_WriteRegs(AFE_ADDR_R(94),8);
#endif
//	GP2_ModeConfig(0x06);//INT
//	InterruptShield_Config(0x7F);//GP中断触发选择
	
}


