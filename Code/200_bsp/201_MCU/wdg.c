#include "wdg.h"
#include "n32h47x_48x_rcc.h"

__IO uint32_t LsiFreq = 32000;

//使用LSI，32KHz
void Init_IWDG(void)
{
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);
    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_WriteConfig(IWDG_WRITE_ENABLE);
    /* IWDG counter clock: LSI/32 */
    IWDG_SetPrescalerDiv(IWDG_PRESCALER_DIV32);
    /* Set counter reload value to obtain 250ms IWDG TimeOut.
       Counter Reload Value = 250ms/IWDG counter clock period
                            = 250ms / (LSI/128)
                            = 100ms / (LsiFreq/32)
                            = LsiFreq/(32 * 4)
                            = LsiFreq/128 */
    // log_debug("LsiFreq is: %d\n", LsiFreq);
    // 改为1s
    IWDG_CntReload(LsiFreq / 32);
    /* Reload IWDG counter */
    IWDG_ReloadKey();
    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();
}
