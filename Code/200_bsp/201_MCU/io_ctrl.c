#include "io_ctrl.h"
#include "io_define.h"
#include "n32h47x_48x_rcc.h"

void InitIO(void)
{
    GPIO_InitType InitStruct;
    
    /* Enable GPIO clock */
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA, ENABLE);
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOB, ENABLE);
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOC, ENABLE);
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOD, ENABLE);
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOE, ENABLE);
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOF, ENABLE);
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOG, ENABLE);
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOH, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
    // RCC_ConfigPclk1(RCC_HCLK_DIV4);

    GPIO_ConfigPinRemap(0, 0, GPIO_RMP_SWJ_SWD);
    
    // 推挽输出类型
    InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    InitStruct.GPIO_Mode      = GPIO_MODE_OUTPUT_PP;
    InitStruct.GPIO_Alternate = GPIO_NO_AF;
    InitStruct.GPIO_Pull      = GPIO_NO_PULL;
    InitStruct.GPIO_Current   = GPIO_DS_8mA;

    InitStruct.Pin = LED0_PIN;
    GPIO_InitPeripheral(LED0_PORT, &InitStruct);
    InitStruct.Pin = LED1_PIN;
    GPIO_InitPeripheral(LED1_PORT, &InitStruct);
    InitStruct.Pin = LED2_PIN;
    GPIO_InitPeripheral(LED2_PORT, &InitStruct);
    InitStruct.Pin = LED3_PIN;
    GPIO_InitPeripheral(LED3_PORT, &InitStruct);
    InitStruct.Pin = LED_RED_PIN;
    GPIO_InitPeripheral(LED_RED_PORT, &InitStruct);

    
    InitStruct.Pin = SHDN_PIN;
    GPIO_InitPeripheral(SHDN_PORT, &InitStruct);

    InitStruct.Pin = PWSV_LDO_PIN;
    GPIO_InitPeripheral(PWSV_LDO_PORT, &InitStruct);

    InitStruct.Pin = PWSV_CTR_PIN;
    GPIO_InitPeripheral(PWSV_CTR_PORT, &InitStruct);

    // InitStruct.Pin = PWSV_BUCK_PIN;
    // GPIO_InitPeripheral(PWSV_BUCK_PORT, &InitStruct);

    InitStruct.Pin = WK_AFE_PIN;
    GPIO_InitPeripheral(WK_AFE_PORT, &InitStruct);

    InitStruct.Pin = RELAY_K1_PIN;
    GPIO_InitPeripheral(RELAY_K1_PORT, &InitStruct);
    InitStruct.Pin = RELAY_K2_PIN;
    GPIO_InitPeripheral(RELAY_K2_PORT, &InitStruct);
    InitStruct.Pin = RELAY_K3_PIN;
    GPIO_InitPeripheral(RELAY_K3_PORT, &InitStruct);
    InitStruct.Pin = RELAY_K4_PIN;
    GPIO_InitPeripheral(RELAY_K4_PORT, &InitStruct);
    InitStruct.Pin = RELAY_K5_PIN;
    GPIO_InitPeripheral(RELAY_K5_PORT, &InitStruct);


    InitStruct.Pin = RS485_EN_PIN;
    GPIO_InitPeripheral(RS485_EN_PORT, &InitStruct);
    InitStruct.Pin = RS485_EN2_PIN;
    GPIO_InitPeripheral(RS485_EN2_PORT, &InitStruct);
    InitStruct.Pin = RS485_EN3_PIN;
    GPIO_InitPeripheral(RS485_EN3_PORT, &InitStruct);

    // InitStruct.Pin = DI_CWAKE_PIN;
    // GPIO_InitPeripheral(DI_CWAKE_PORT, &InitStruct);


    // 浮空输入类型
    InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    InitStruct.GPIO_Mode      = GPIO_MODE_INPUT;
    InitStruct.GPIO_Alternate = GPIO_NO_AF;
    InitStruct.GPIO_Pull      = GPIO_NO_PULL;
    InitStruct.GPIO_Current   = GPIO_DS_8mA;

    // InitStruct.Pin = DI_CBC_PIN;
    // GPIO_InitPeripheral(DI_CBC_PORT, &InitStruct);
    InitStruct.Pin = DI1_PIN;
    GPIO_InitPeripheral(DI1_PORT, &InitStruct);
    InitStruct.Pin = DI2_PIN;
    GPIO_InitPeripheral(DI2_PORT, &InitStruct);
    InitStruct.Pin = DI_LOAD_PIN;
    GPIO_InitPeripheral(DI_LOAD_PORT, &InitStruct);

    // InitStruct.GPIO_Pull      = GPIO_PULL_UP;
    InitStruct.Pin = DI_CWAKE_PIN;
    GPIO_InitPeripheral(DI_CWAKE_PORT, &InitStruct);


    // InitStruct.Pin = SC_CUR_PIN;
    // GPIO_InitPeripheral(SC_CUR_PORT, &InitStruct);
    MCUO_RS485_EN = 1;
    MCUO_RS485_EN2 = 1;
    MCUO_RS485_EN3 = 1;

    MCUO_DEBUG_LED_RED = 1;
}

void Sif_DataOut(void)
{
	GPIO_InitType  InitStruct;
    InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    InitStruct.GPIO_Mode      = GPIO_MODE_OUTPUT_PP;
    InitStruct.GPIO_Alternate = GPIO_NO_AF;
    InitStruct.GPIO_Pull      = GPIO_NO_PULL;
    InitStruct.GPIO_Current   = GPIO_DS_8mA;

    InitStruct.Pin = SIF_PIN;
    GPIO_InitPeripheral(SIF_PORT, &InitStruct);
}

void Sif_DataIn(void)
{
	GPIO_InitType  InitStruct;

    InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    InitStruct.GPIO_Mode      = GPIO_MODE_INPUT;
    InitStruct.GPIO_Alternate = GPIO_NO_AF;
    InitStruct.GPIO_Pull      = GPIO_NO_PULL;
    InitStruct.GPIO_Current   = GPIO_DS_8mA;

    InitStruct.Pin = SIF_PIN;
    GPIO_InitPeripheral(SIF_PORT, &InitStruct);
}
