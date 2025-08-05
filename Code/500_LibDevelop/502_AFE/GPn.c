/******************************************************************************
;  *   	@MCU				 STM32F103C8T6
;  *   	@Create Date         2023.01.20
;  *    @Official website		 http://www.devechip.com/
;  *----------------------Abstract Description---------------------------------
;  *			    					GPn管脚复用驱动
**************************************************************************************/
#include "DVC11XX.h"
#include "GPn.h"

/**
	* @说明	GP1复用配置
	* @参数	u8 控制位      0x00 关闭     0x01 热敏电阻检测输入    0x02 模拟电压输入
												 0x03 充电驱动硬线控制
	* @返回值	
	* @注	
*/
void GP1_ModeConfig(u8 mode){

	if(mode<=4){
	g_AfeRegs.R116.GP1M=mode;

	DVC11XX_WriteRegs(AFE_ADDR_R(116),1);
}
}
/**
	* @说明	GP2复用配置
	* @参数	u8 控制位      0x00 关闭     0x01 热敏电阻检测输入    0x02 模拟电压输入
												 0x06 中断输出  0x07 预放低边输出
	* @返回值	
	* @注	
*/
void GP2_ModeConfig(u8 mode){

	if((mode<=7)&&!((mode>2)&&(mode<6))){
	g_AfeRegs.R116.GP2M=mode;

	DVC11XX_WriteRegs(AFE_ADDR_R(116),1);	
}
}

/**
	* @说明	GP3复用配置
	* @参数	u8 控制位      0x00 关闭     0x01 热敏电阻检测输入    0x02 模拟电压输入
												 0x06 中断输出  0x07 预充低边输出
	* @返回值	
	* @注	
*/
void GP3_ModeConfig(u8 mode){

	if((mode<=7)&&!((mode>2)&&(mode<6))){
	g_AfeRegs.R116.GP3M=mode;

	DVC11XX_WriteRegs(AFE_ADDR_R(116),1);	
}
}

/**
	* @说明	GP4复用配置
	* @参数	u8 控制位      0x00 关闭     0x01 热敏电阻检测输入    0x02 模拟电压输入
												 0x03 放电驱动硬线控制
	* @返回值	
	* @注	
*/
void GP4_ModeConfig(u8 mode){

	if(mode<=4){
	g_AfeRegs.R117.GP4M=mode;

	DVC11XX_WriteRegs(AFE_ADDR_R(117),1);	
}
}
/**
	* @说明	GP5复用配置
	* @参数	u8 控制位      0x00 关闭     0x01 热敏电阻检测输入    0x02 模拟电压输入
												 0x06 中断输出  0x07 充电低边输出
	* @返回值	
	* @注	
*/
void GP5_ModeConfig(u8 mode){

	if((mode<=7)&&!((mode>2)&&(mode<6))){
	g_AfeRegs.R117.GP5M=mode;

	DVC11XX_WriteRegs(AFE_ADDR_R(117),1);	
}
}

/**
	* @说明	GP6复用配置
	* @参数	u8 控制位      0x00 关闭     0x01 热敏电阻检测输入    0x02 模拟电压输入
												 0x06 中断输出  0x07 放电低边输出
	* @返回值	
	* @注	
*/
void GP6_ModeConfig(u8 mode){

	if((mode<=7)&&!((mode>2)&&(mode<6))){

	g_AfeRegs.R117.GP6M=mode;

	DVC11XX_WriteRegs(AFE_ADDR_R(117),1);	
}
}

/**
	* @说明	GP脚模拟电压输入计算
	* @参数	u8 GP脚
	* @返回值	u16 电压值
	* @注	
*/
float GPn_Analog_Input_voltage(u8 GP){
	float temp_voltage;
	u16 GP_value;

	DVC11XX_ReadRegs(AFE_ADDR_R(17+GP),2);//读取对应GP脚

	GP_value=(g_AfeRegs.R17_28.VGP[GP].VGP_H<<8)|g_AfeRegs.R17_28.VGP[GP].VGP_L;//数据还原

	temp_voltage=	GP_value * 0.1;//GP_value * _lsbVCELL *1000 单位mv

	return temp_voltage;
}


/**
	* @说明	打开对应FET
	* @参数	
	* @返回值	
	* @注	
*/
void FET_Open(void){
#ifdef MODEA								//模式A仅有打开充放电
	g_AfeRegs.R81.CHGC	=	0x3;
	g_AfeRegs.R81.DSGC	=	0x3;
#else												//模式B仅打开预先充放电
	g_AfeRegs.R81.PCHGC	=	1;
	g_AfeRegs.R81.PDSGC	=	1;
#endif
	DVC11XX_WriteRegs(AFE_ADDR_R(81),1);	

}

void GPnDemo_Cal(void)
{
//	if(!MODE)
//	{//模式A
//		float GPn_volage;

//		GPn_volage	=	GPn_Analog_Input_voltage(GP3);//模拟输入计算
//		PRINT_AFE_INFO("GPn_volage = %.1f mV\r\n",GPn_volage);	
//		
//		if(g_AfeRegs.R4_6.CHGF)//CHG flag
//		{
//			PRINT_AFE_INFO("CHG on\r\n");
//		}

//		if(g_AfeRegs.R4_6.DSGF)//DSG flag
//		{
//			PRINT_AFE_INFO("DSG on\r\n");
//		}

//		if(CHG_status)
//		{//CHG低边输出
//			PRINT_AFE_INFO("LOW SIDE CHG on\r\n");
//			CHG_reset;//CHG_req 清零
//		}

//		if(DSG_status)
//		{//DSG低边输出
//			PRINT_AFE_INFO("LOW SIDE DSG on\r\n");
//			DSG_reset;//DSG_req 清零
//		}
//				
//		if(EVENT_alert_req)
//		{//中断发生
//			PRINT_AFE_INFO("AFE INT HAPPENED! Check The Register\r\n");
//			EVENT_alert_req=0;//alert_req 清零
//		}
//	}
//	else	
//	{//模式B
//		if(g_AfeRegs.R4_6.PCHGF)//PCHG flag
//		{
//			PRINT_AFE_INFO("PCHG on\r\n");
//		}

//		if(g_AfeRegs.R4_6.PDSGF)//PDSG flag
//		{
//			PRINT_AFE_INFO("PDSG on\r\n");
//		}

////			if(PCHG_status)
////			{//PCGD低边输出
////					PRINT_AFE_INFO("LOW SIDE  PCHG on\r\n");
////					PCHG_reset;//PCHG_req 清零
////			}

////			if(PDSG_status)
////			{//
////					PRINT_AFE_INFO("LOW SIDE  PDSG on\r\n");
////					PCHG_reset;//PDSG_req 清零
////			}
//	}
		}

/**
	* @说明	中断报警控制
	* @参数	u8 Ctrl
	* @返回值	
	* @注	
*/
void INT_Ctrl(u8 Ctrl){

memset((u8 *)&g_AfeRegs+121,Ctrl,1);//一次性写入INT_CTRL位

DVC11XX_WriteRegs(AFE_ADDR_R(121),1);//写入AFE
}

