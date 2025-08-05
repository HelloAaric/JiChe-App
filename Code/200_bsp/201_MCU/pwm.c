#include "pwm.h"
#include "io_define.h"
#include "n32h47x_48x_tim.h"
#include "n32h47x_48x_rcc.h"
#include "data_deal.h"


UINT16 CCR1_Val       = 600;
UINT16 CCR2_Val       = 300;
UINT16 CCR3_Val       = 150;
UINT16 CCR4_Val       = 75;

extern UINT16 gu16_Rs_uO;
void InitPWM_OUT(void)
{
    GPIO_InitType GPIO_InitStructure;
    TIM_TimeBaseInitType TIM_TimeBaseStructure;
    OCInitType TIM_OCInitStructure;

    // 初始化结构体
    GPIO_InitStruct(&GPIO_InitStructure);
    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
    TIM_InitOcStruct(&TIM_OCInitStructure);

    /* GPIOx Configuration: Pin of TIMx */
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;

    GPIO_InitStructure.Pin = PWM_OUT1_PIN;
    GPIO_InitStructure.GPIO_Alternate = PWM_OUT1_AF;
    GPIO_InitPeripheral(PWM_OUT1_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = PWM_OUT2_SC_PIN;
    GPIO_InitStructure.GPIO_Alternate = PWM_OUT2_SC_AF;
    GPIO_InitPeripheral(PWM_OUT2_SC_PORT, &GPIO_InitStructure);


    /* The maximum operating clock of GTIM1-7/BTIM1-2 is 180MHz,the division frequency of PCLK1 cannot be 1 or 2 divisions */
    // 48MHz
    RCC_EnableAPB1PeriphClk(TIM_PWM_CLK, ENABLE);

    TIM_TimeBaseStructure.Period    = 1000 - 1;
    TIM_TimeBaseStructure.Prescaler = 48 - 1;
    TIM_TimeBaseStructure.ClkDiv    = TIM_CLK_DIV1;
    TIM_TimeBaseStructure.CounterMode   = TIM_CNT_MODE_UP;

    TIM_InitTimeBase(TIM_PWM, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.OCMode      = TIM_OCMODE_PWM1;
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.Pulse       = CCR3_Val;
    TIM_OCInitStructure.OCPolarity  = TIM_OC_POLARITY_HIGH;
    PWM_OUT1_CHANNEL(TIM_PWM, &TIM_OCInitStructure);
    PWM_OUT1_PRELOAD(TIM_PWM, TIM_OC_PRE_LOAD_ENABLE);

    CCR2_Val = OtherElement.u16CBC_Cur_DSG*gu16_Rs_uO/1000;  // mV * 10
    // CCR2_Val = CCR2_Val/10/3300 *1000;      // 比例
    CCR2_Val = CCR2_Val * 2 / 33;           // 增益10倍，u16CBC_Cur_DSG10倍，抵消
                                            // 分压一半，需要扩大
    // CCR2_Val = 600;
    TIM_OCInitStructure.Pulse       = CCR2_Val;
    PWM_OUT2_SC_CHANNEL(TIM_PWM, &TIM_OCInitStructure);
    PWM_OUT2_SC_PRELOAD(TIM_PWM, TIM_OC_PRE_LOAD_ENABLE);

    TIM_ConfigArPreload(TIM_PWM, ENABLE);
    /* TIMx enable counter */
    TIM_Enable(TIM_PWM, ENABLE);
}




void InitPWM_IN(void)
{


}


void InitPWM_IN222(void)
{
    TIM_TimeBaseInitType TIM_TimeBaseStructure2;
    OCInitType TIM_OCInitStructure2;
    uint32_t CCR1_Val_test       = 600;
    uint32_t PrescalerValue2 = 0;


    GPIO_InitType GPIO_InitStructure;

    GPIO_InitStruct(&GPIO_InitStructure);
    
    /* GPIOx Configuration: Pin of TIMx */
    GPIO_InitStructure.Pin        = GPIO_PIN_7;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF12;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);


    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_GTIM7, ENABLE);

    PrescalerValue2 = (uint32_t)((SystemCoreClock/2) / 12000000) - 1;
    /* Time base configuration */
    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure2);
    // TIM_TimeBaseStructure2.Period    = 1200;
    // TIM_TimeBaseStructure2.Prescaler = PrescalerValue2;

    TIM_TimeBaseStructure2.Period    = 1000 - 1;
    TIM_TimeBaseStructure2.Prescaler = 48 - 1;

    TIM_TimeBaseStructure2.ClkDiv    = TIM_CLK_DIV1;
    TIM_TimeBaseStructure2.CounterMode   = TIM_CNT_MODE_UP;

    TIM_InitTimeBase(GTIM7, &TIM_TimeBaseStructure2);


    TIM_InitOcStruct(&TIM_OCInitStructure2);
    TIM_OCInitStructure2.OCMode      = TIM_OCMODE_PWM1;
    TIM_OCInitStructure2.OutputState = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure2.Pulse       = CCR1_Val_test;
    TIM_OCInitStructure2.OCPolarity  = TIM_OC_POLARITY_HIGH;

    TIM_InitOc1(GTIM7, &TIM_OCInitStructure2);

    TIM_ConfigOc1Preload(GTIM7, TIM_OC_PRE_LOAD_ENABLE);

    TIM_ConfigArPreload(GTIM7, ENABLE);

    /* TIMx enable counter */
    TIM_Enable(GTIM7, ENABLE);

}


void InitPWM(void)
{
    InitPWM_OUT();
    InitPWM_IN();
    // InitPWM_IN222();
}
