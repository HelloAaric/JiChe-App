#include "sys_timer.h"

#include "n32h47x_48x.h"
#include "misc.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_tim.h"


volatile uint8_t g_u81msCnt=0;
volatile uint8_t g_u810msClockCnt=0;
volatile uint8_t g_u81msClockCnt = 0;
volatile uint8_t gu8_200msCnt = 0;
volatile uint8_t gu8_200msAccClock_Flag = 0;

volatile UINT32 gu32_2msClockCnt = 0;

/**
 *\*\name    DBG_SysTick_Config.
 *\*\fun     System tick configuration.
 *\*\param   ticks :system tick
 *\*\return  none
**/
static uint32_t DBG_SysTick_Config(uint32_t ticks)
{ 
    if (ticks > SysTick_LOAD_RELOAD_Msk)  return (1);            /* Reload value impossible */
                                                               
    SysTick->LOAD  = (ticks & SysTick_LOAD_RELOAD_Msk) - 1;      /* set reload register */
    //NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);  /* set Priority for Cortex-M4 System Interrupts */
    SysTick->VAL   = 0;                                          /* Load the SysTick Counter Value */
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | 
                     SysTick_CTRL_ENABLE_Msk;                    /* Enable SysTick IRQ and SysTick Timer */
    SysTick->CTRL  &= (~SysTick_CTRL_TICKINT_Msk);
    return (0);                                                  /* Function successful */
}

/**
 *\*\name    systick_delay_us.
 *\*\fun     microsecond delay.
 *\*\param   us :any number
 *\*\return  none
**/
void __delay_us(uint32_t us)
{
    uint32_t i;
    RCC_ClocksType RCC_Clocks;

    RCC_GetClocksFreqValue(&RCC_Clocks);
    DBG_SysTick_Config(RCC_Clocks.SysclkFreq / 1000000);

    for(i = 0; i < us; i++) {
        /* When the counter value decreases to 0, bit 16 of the CRTL register will be set to 1 */
        /* When set to 1, reading this bit will clear it to 0 */
        while( !((SysTick->CTRL) & (1 << 16)));
    }
    /* Turn off the SysTick timer */
    SysTick->CTRL &= (~SysTick_CTRL_ENABLE_Msk);
}

/**
 *\*\name    systick_delay_ms.
 *\*\fun     millisecond delay.
 *\*\param   ms :any number
 *\*\return  none
**/
void __delay_ms(uint32_t ms)
{
    uint32_t i;
    RCC_ClocksType RCC_Clocks;
    
    RCC_GetClocksFreqValue(&RCC_Clocks);
    DBG_SysTick_Config(RCC_Clocks.SysclkFreq / 1000);

    for(i = 0; i < ms; i++) {
        /* When the counter value decreases to 0, bit 16 of the CRTL register will be set to 1 */
        /* When set to 1, reading this bit will clear it to 0 */
        while( !((SysTick->CTRL) & (1 << 16)));
    }
    /* Turn off the SysTick timer */
    SysTick->CTRL &= (~SysTick_CTRL_ENABLE_Msk);
}

void InitTimer(void)
{
    NVIC_InitType NVIC_InitStructure;
    TIM_TimeBaseInitType TIM_TimeBaseStructure;

    // RCC_ConfigPclk1(RCC_HCLK_DIV4);
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_BTIM1, ENABLE);

    /* Time base configuration */
    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.Period        = 500 - 1;
    TIM_TimeBaseStructure.Prescaler     = 48 - 1;
    TIM_TimeBaseStructure.ClkDiv        = TIM_CLK_DIV1;
    TIM_TimeBaseStructure.CounterMode   = TIM_CNT_MODE_UP;

    TIM_InitTimeBase(BTIM1, &TIM_TimeBaseStructure);
    /* Prescaler configuration */
    // 看起来不需要
    // TIM_ConfigPrescaler(BTIM1, PrescalerValue, TIM_PSC_RELOAD_MODE_IMMEDIATE);
    /* BTIM1 enable update irq */
    TIM_ConfigInt(BTIM1, TIM_INT_UPDATE, ENABLE);

    /* Enable the TIMx Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = BTIM1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* BTIM1 enable counter */
    TIM_Enable(BTIM1, ENABLE);
}

extern void SIF_SendData_Handle(void);

void BTIM1_IRQHandler(void)
{
    if(TIM_GetIntStatus(BTIM1, TIM_INT_UPDATE) != RESET) {
        TIM_ClrIntPendingBit(BTIM1, TIM_INT_UPDATE);
		if((++g_u81msCnt) >= 2) {               //1ms
            g_u81msCnt = 0;
            g_u81msClockCnt ++;

            if(g_u81msClockCnt >= 2) {          //2ms
                g_u81msClockCnt = 0;
                g_u810msClockCnt++;
                gu32_2msClockCnt++;
                if(g_u810msClockCnt >= 5) {     //10ms
                    g_u810msClockCnt = 0;
                }
            }

            if(++gu8_200msCnt >= 200) {
                gu8_200msCnt = 0;
                gu8_200msAccClock_Flag = 1;
            }
        }

        // SIF_SendData_Handle();
    }
}
