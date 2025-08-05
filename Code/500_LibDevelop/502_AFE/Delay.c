/******************************************************************************
;  *   	@MCU				 STM32F103C8T6
;  *   	@Create Date         2023.01.20
;  *    @Official website		 http://www.devechip.com/
;  *----------------------Abstract Description---------------------------------
;  *			         					 定时器设置                              		
******************************************************************************/
#include "DVC11XX.h"

#include "timer.h"

TIM_TimeBaseInitType TIM_TimeBaseStructure;

uint16_t PrescalerValue = 0;


/**
	* @说明	GTIM2初始化
	* @参数	
	* @返回值	
	* @注	
*/
void delay_init(void)
{
//	RCC_ClocksTypeDef RCC_Clocks;
//  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
//  RCC_GetClocksFreq(&RCC_Clocks);
	
//  TIM_TimeBaseInitStructure.TIM_Prescaler = (RCC_Clocks.HCLK_Frequency/1000000) - 1;
//  TIM_TimeBaseInitStructure.TIM_Period = 1;
//  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Down;
//  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
  RCC_ClocksType RCC_Clocks;	
  /* TIMx, GPIOx clocks enable */
  RCC_ConfigPclk1(RCC_HCLK_DIV4);/* The maximum operating clock of GTIM1-7/BTIM1-2 is 180MHz,the division frequency of PCLK1 cannot be 1 or 2 divisions */
  RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_GTIM2,ENABLE);
//  RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOC,ENABLE);
	
  /* Compute the prescaler value */
  PrescalerValue = (RCC_Clocks.HclkFreq/1000000) - 1;

  /* Time base configuration */
  TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.Period        = 1;
  TIM_TimeBaseStructure.Prescaler     = (RCC_Clocks.HclkFreq/1000000) - 1;
  TIM_TimeBaseStructure.ClkDiv        = TIM_CLK_DIV1;
  TIM_TimeBaseStructure.CounterMode   = TIM_CNT_MODE_DOWN;

	TIM_InitTimeBase(GTIM2, &TIM_TimeBaseStructure);

  /* Prescaler configuration */
  TIM_ConfigPrescaler(GTIM2, PrescalerValue, TIM_PSC_RELOAD_MODE_IMMEDIATE);

	/* GTIM2 enable update irq */
  TIM_ConfigInt(GTIM2, TIM_INT_UPDATE, ENABLE);

  /* GTIM2 enable counter */
  TIM_Enable(GTIM2, ENABLE);	
}
/**
	* @说明	GTIM2延时函数
	* @参数	延时长度
	* @返回值	
	* @注	单位 us
*/
void delay_us(u16 us){//最大2^16 us，误差+5us
  #if 0
	if(us>0){
		GTIM2->CNT = us-1; // 向计数器装要递减的数，减到0后会触发定时器的TIM_FLAG_UpDate标志位
    GTIM2->CTRL1 |= TIM_CTRL1_CNTEN; // 使能计数器， 计数器开始递减
    while((GTIM2->STS & TIM_FLAG_UPDATE) == 0); // 等到计数器减到0
    GTIM2->CTRL1 &= (~TIM_CTRL1_CNTEN); //关闭计数器
    GTIM2->CTRL1 &= ~TIM_FLAG_UPDATE; // 清除定时器变为0的标志位
	}
    #endif
    __delay_us(us);
}
/**
	* @说明	延时函数
	* @参数	延时长度
	* @返回值	
	* @注	单位 ms
*/
void delay_ms(u16 ms){
//	while(ms>0){
//		delay_us(1000);
//		ms--;
//	}
}
