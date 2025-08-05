/******************************************************************************
;  *   	@MCU				 STM32F103C8T6
;  *   	@Create Date         2023.01.20
;  *    @Official website		 http://www.devechip.com/
;  *----------------------Abstract Description---------------------------------
;  *			         					 ��ʱ������                              		
******************************************************************************/
#include "DVC11XX.h"

#include "timer.h"

TIM_TimeBaseInitType TIM_TimeBaseStructure;

uint16_t PrescalerValue = 0;


/**
	* @˵��	GTIM2��ʼ��
	* @����	
	* @����ֵ	
	* @ע	
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
	* @˵��	GTIM2��ʱ����
	* @����	��ʱ����
	* @����ֵ	
	* @ע	��λ us
*/
void delay_us(u16 us){//���2^16 us�����+5us
  #if 0
	if(us>0){
		GTIM2->CNT = us-1; // �������װҪ�ݼ�����������0��ᴥ����ʱ����TIM_FLAG_UpDate��־λ
    GTIM2->CTRL1 |= TIM_CTRL1_CNTEN; // ʹ�ܼ������� ��������ʼ�ݼ�
    while((GTIM2->STS & TIM_FLAG_UPDATE) == 0); // �ȵ�����������0
    GTIM2->CTRL1 &= (~TIM_CTRL1_CNTEN); //�رռ�����
    GTIM2->CTRL1 &= ~TIM_FLAG_UPDATE; // �����ʱ����Ϊ0�ı�־λ
	}
    #endif
    __delay_us(us);
}
/**
	* @˵��	��ʱ����
	* @����	��ʱ����
	* @����ֵ	
	* @ע	��λ ms
*/
void delay_ms(u16 ms){
//	while(ms>0){
//		delay_us(1000);
//		ms--;
//	}
}
