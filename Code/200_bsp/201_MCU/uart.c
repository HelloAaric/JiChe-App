#include "uart.h"
#include "n32h47x_48x_usart.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_gpio.h"
#include "misc.h"
#include "io_define.h"
#include "stdio.h"

void InitUART_CommonUpper1(void)
{
    USART_InitType USART_InitStructure;
    GPIO_InitType GPIO_InitStructure;
    NVIC_InitType NVIC_InitStructure;

    // RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_UART5, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_USART4, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* Enable the USARTz Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = UART_UPPER1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    GPIO_InitStructure.Pin            = UART_UPPER1_TxPin;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = UART_UPPER1_TxPin_AF;
	GPIO_InitStructure.GPIO_Pull      = GPIO_PULL_UP;
    GPIO_InitPeripheral(UART_UPPER1_PORT, &GPIO_InitStructure);

    /* Configure USARTz Rx as alternate function input and pull-up */
    GPIO_InitStructure.Pin            = UART_UPPER1_RxPin;
    GPIO_InitStructure.GPIO_Alternate = UART_UPPER1_RxPin_AF;
    GPIO_InitPeripheral(UART_UPPER1_PORT, &GPIO_InitStructure);  

    /* USARTz configuration */
    USART_InitStructure.BaudRate            = UART_UPPER1_BAUD;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.OverSampling        = USART_16OVER;
    USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;

    /* Configure USARTz */
    USART_Init(UART_UPPER1, &USART_InitStructure);
    
    USART_CfgDriverAssertTime(UART_UPPER1, 1);
    USART_CfgDriverdeassertTime(UART_UPPER1, 0);
    USART_DriverPolaritySet(UART_UPPER1, DISABLE);
    USART_DriverModeSet(UART_UPPER1, ENABLE);
    
    /* Enable USARTz Receive and Transmit interrupts */
    USART_ConfigInt(UART_UPPER1, USART_INT_RXDNE, ENABLE);

    /* Enable the USARTz */
    USART_Enable(UART_UPPER1, ENABLE);
    
    // Sci_DataInit(&g_stCurrentMsgPtr_SCI1);
}


void InitUART_CommonUpper2(void)
{
    USART_InitType USART_InitStructure;
    GPIO_InitType GPIO_InitStructure;
    NVIC_InitType NVIC_InitStructure;

    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_UART7, ENABLE);

    /* Enable the USARTz Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = UART_UPPER2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    GPIO_InitStructure.Pin            = UART_UPPER2_TxPin;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = UART_UPPER2_TxPin_AF;
	GPIO_InitStructure.GPIO_Pull      = GPIO_PULL_UP;
    GPIO_InitPeripheral(UART_UPPER2_PORT, &GPIO_InitStructure);

    /* Configure USARTz Rx as alternate function input and pull-up */
    GPIO_InitStructure.Pin            = UART_UPPER2_RxPin;
    GPIO_InitStructure.GPIO_Alternate = UART_UPPER2_RxPin_AF;
    GPIO_InitPeripheral(UART_UPPER2_PORT, &GPIO_InitStructure);  

    /* USARTz configuration */
    USART_InitStructure.BaudRate            = UART_UPPER2_BAUD;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.OverSampling        = USART_16OVER;
    USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;

    /* Configure USARTz */
    USART_Init(UART_UPPER2, &USART_InitStructure);
    UART_UPPER2->CTRL3 |= 1<<6;

    // USART_CfgDriverAssertTime(UART_UPPER2, 1);
    // USART_CfgDriverdeassertTime(UART_UPPER2, 0);
    // USART_DriverPolaritySet(UART_UPPER2, DISABLE);
    // USART_DriverModeSet(UART_UPPER2, ENABLE);
    
    /* Enable USARTz Receive and Transmit interrupts */
    USART_ConfigInt(UART_UPPER2, USART_INT_RXDNE, ENABLE);

    /* Enable the USARTz */
    USART_Enable(UART_UPPER2, ENABLE);
    
    // Sci_DataInit(&g_stCurrentMsgPtr_SCI1);
}

void InitUART_CommonUpper3(void)
{
    USART_InitType USART_InitStructure;
    GPIO_InitType GPIO_InitStructure;
    NVIC_InitType NVIC_InitStructure;

    // RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_UART5, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_USART1, ENABLE);
    // RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_USART3, ENABLE);

    /* Enable the USARTz Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = UART_UPPER3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Pull      = GPIO_NO_PULL;

    GPIO_InitStructure.Pin            = UART_UPPER3_TxPin;
    GPIO_InitStructure.GPIO_Alternate = UART_UPPER3_TxPin_AF;
    GPIO_InitPeripheral(UART_UPPER3_PORT, &GPIO_InitStructure);

    /* Configure USARTz Rx as alternate function input and pull-up */
    GPIO_InitStructure.Pin            = UART_UPPER3_RxPin;
    GPIO_InitStructure.GPIO_Alternate = UART_UPPER3_RxPin_AF;
    GPIO_InitPeripheral(UART_UPPER3_PORT, &GPIO_InitStructure);  

    /* USARTz configuration */
    USART_InitStructure.BaudRate            = UART_UPPER3_BAUD;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.OverSampling        = USART_16OVER;
    USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;


    /* Configure USARTz */
    USART_Init(UART_UPPER3, &USART_InitStructure);
    UART_UPPER3->CTRL3 |= 1<<6;

    // USART_CfgDriverAssertTime(UART_UPPER2, 1);
    // USART_CfgDriverdeassertTime(UART_UPPER2, 0);
    // USART_DriverPolaritySet(UART_UPPER2, DISABLE);
    // USART_DriverModeSet(UART_UPPER2, ENABLE);
    
    /* Enable USARTz Receive and Transmit interrupts */
    USART_ConfigInt(UART_UPPER3, USART_INT_RXDNE, ENABLE);

    /* Enable the USARTz */
    USART_Enable(UART_UPPER3, ENABLE);

    // Sci_DataInit(&g_stCurrentMsgPtr_SCI1);
}

#if 1
int fputc(int ch, FILE *f)
{
    #if 1
	MCUO_RS485_EN = 1;
	MCUO_RS485_EN2 = 1;
	USART_SendData(UART7, (uint8_t) ch);                         //发送字符
	while (USART_GetFlagStatus(UART7, USART_FLAG_TXC) == RESET);   //检查是否发送完毕

	MCUO_RS485_EN = 0;
	MCUO_RS485_EN2 = 1;
	
	return (ch);
    #endif
}
#endif

