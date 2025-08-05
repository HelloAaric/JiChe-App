/**
*     Copyright (c) 2023, Nations Technologies Inc.
* 
*     All rights reserved.
*
*     This software is the exclusive property of Nations Technologies Inc. (Hereinafter 
* referred to as NATIONS). This software, and the product of NATIONS described herein 
* (Hereinafter referred to as the Product) are owned by NATIONS under the laws and treaties
* of the People's Republic of China and other applicable jurisdictions worldwide.
*
*     NATIONS does not grant any license under its patents, copyrights, trademarks, or other 
* intellectual property rights. Names and brands of third party may be mentioned or referred 
* thereto (if any) for identification purposes only.
*
*     NATIONS reserves the right to make changes, corrections, enhancements, modifications, and 
* improvements to this software at any time without notice. Please contact NATIONS and obtain 
* the latest version of this software before placing orders.

*     Although NATIONS has attempted to provide accurate and reliable information, NATIONS assumes 
* no responsibility for the accuracy and reliability of this software.
* 
*     It is the responsibility of the user of this software to properly design, program, and test 
* the functionality and safety of any application made of this information and any resulting product. 
* In no event shall NATIONS be liable for any direct, indirect, incidental, special,exemplary, or 
* consequential damages arising in any way out of the use of this software or the Product.
*
*     NATIONS Products are neither intended nor warranted for usage in systems or equipment, any
* malfunction or failure of which may cause loss of human life, bodily injury or severe property 
* damage. Such applications are deemed, "Insecure Usage".
*
*     All Insecure Usage shall be made at user's risk. User shall indemnify NATIONS and hold NATIONS 
* harmless from and against all claims, costs, damages, and other liabilities, arising from or related 
* to any customer's Insecure Usage.

*     Any express or implied warranty with regard to this software or the Product, including,but not 
* limited to, the warranties of merchantability, fitness for a particular purpose and non-infringement
* are disclaimed to the fullest extent permitted by law.

*     Unless otherwise explicitly permitted by NATIONS, anyone may not duplicate, modify, transcribe
* or otherwise distribute this software for any purposes, in whole or in part.
*
*     NATIONS products and technologies shall not be used for or incorporated into any products or systems
* whose manufacture, use, or sale is prohibited under any applicable domestic or foreign laws or regulations. 
* User shall comply with any applicable export control laws and regulations promulgated and administered by 
* the governments of any countries asserting jurisdiction over the parties or transactions.
**/

/**
*\*\file n32h47x_48x_it.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/
#include "n32h47x_48x_it.h"
#include "n32h47x_48x_tim.h"
#include "n32h47x_48x_usart.h"
#include "n32h47x_48x_exti.h"
#include "can_com.h"
#include "sci_com.h"
#include "System_Monitor.h"
#include "CBC.h"
#include "io_define.h"
#include "charger_loader_func.h"


/*** Cortex-M4 Processor Exceptions Handlers ***/

/**
*\*\name    NMI_Handler.
*\*\fun     This function handles NMI exception.
*\*\param   none
*\*\return  none 
**/
void NMI_Handler(void)
{
   
}

/**
*\*\name    HardFault_Handler.
*\*\fun     This function handles Hard Fault exception.
*\*\param   none
*\*\return  none 
**/
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/**
*\*\name    MemManage_Handler.
*\*\fun     This function handles Memory Manage exception.
*\*\param   none
*\*\return  none 
**/
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/**
*\*\name    BusFault_Handler.
*\*\fun     This function handles Bus Fault exception.
*\*\param   none
*\*\return  none 
**/
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/**
*\*\name    UsageFault_Handler.
*\*\fun     This function handles Usage Fault exception.
*\*\param   none
*\*\return  none 
**/
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/**
*\*\name    SVC_Handler.
*\*\fun     This function handles SVCall exception.
*\*\param   none
*\*\return  none 
**/
void SVC_Handler(void)
{
}

/**
*\*\name    DebugMon_Handler.
*\*\fun     This function handles Debug Monitor exception.
*\*\param   none
*\*\return  none 
**/
void DebugMon_Handler(void)
{
}

/**
*\*\name    SysTick_Handler.
*\*\fun     This function handles SysTick Handler.
*\*\param   none
*\*\return  none 
**/
void SysTick_Handler(void)
{   
}

/** N32H47X Peripherals Interrupt Handlers, interrupt handler's name please refer 
     to the startup file (startup_n32h4xx.s) **/

/**
*\*\name    TIMx_IRQHandler.
*\*\fun     This function handles TIMx interrupts.
*\*\param   none
*\*\return  none 
**/
void TIMx_IRQHandler(void)
{

}

void EXTI0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_LINE0) == SET) {
        // GPIO_ConfigEXTILine(EXTI_LINE_SOURCE0,PortSrc[key_cnt],PinSrc[key_cnt]);
        #if 0
        CBC_Element.u8CBC_DSG_ErrFlag = 1;
        System_OnOFF_Func.bits.b1OnOFF_MOS_Relay = 0;
        ChargerLoad_Func.bits.b1OFFDriver_CBC = 1;
        MCUO_RELAY_PRECHG = 0;
        MCUO_RELAY_CHG = 0;
        MCUO_RELAY_DSG = 0;
        #endif
        EXTI_ClrITPendBit(EXTI_LINE0);
    }
}

void EXTI1_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_LINE1) == SET) {  
        EXTI_ClrITPendBit(EXTI_LINE1);
    }
}

void EXTI2_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_LINE2) == SET) {
        EXTI_ClrITPendBit(EXTI_LINE2);
    }
}

void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_LINE5) == SET) {
        EXTI_ClrITPendBit(EXTI_LINE5);
    }

    if(EXTI_GetITStatus(EXTI_LINE8) == SET) {
        EXTI_ClrITPendBit(EXTI_LINE8);
    }
}

void EXTI15_10_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_LINE11) == SET) {
        CBC_Element.u8CBC_DSG_ErrFlag = 1;
        System_OnOFF_Func.bits.b1OnOFF_MOS_Relay = 0;
        MCUO_RELAY_PRECHG = 0;
        MCUO_RELAY_CHG = 0;
        MCUO_RELAY_DSG = 0;
        EXTI_ClrITPendBit(EXTI_LINE11);
    }
}


void USART4_IRQHandler(void)
{
    if ((USART_GetFlagStatus(USART4, USART_FLAG_RXDNE) != RESET) && (USART_GetIntStatus(USART4, USART_INT_RXDNE) != RESET))
    {
        /* Read one byte from the receive data register */
		Sci1_CommonUpper_FaultChk();
		Sci1_CommonUpper_Rx_Deal(&g_stCurrentMsgPtr_SCI1);
    }
}

void UART7_IRQHandler(void)
{
    if ((USART_GetFlagStatus(UART7, USART_FLAG_RXDNE) != RESET) && (USART_GetIntStatus(UART7, USART_INT_RXDNE) != RESET))
    {
        /* Read one byte from the receive data register */
		Sci2_CommonUpper_FaultChk();
		Sci2_CommonUpper_Rx_Deal(&g_stCurrentMsgPtr_SCI2);
    }
}

void UART5_IRQHandler(void)
{
    if ((USART_GetFlagStatus(UART5, USART_FLAG_RXDNE) != RESET) && (USART_GetIntStatus(UART5, USART_INT_RXDNE) != RESET))
    {
        /* Read one byte from the receive data register */
		Sci3_CommonUpper_FaultChk();
		Sci3_CommonUpper_Rx_Deal(&g_stCurrentMsgPtr_SCI3);
    }
}


void USART1_IRQHandler(void)
{
    if ((USART_GetFlagStatus(USART1, USART_FLAG_RXDNE) != RESET) && (USART_GetIntStatus(USART1, USART_INT_RXDNE) != RESET))
    {
        /* Read one byte from the receive data register */
		Sci3_CommonUpper_FaultChk();
		Sci3_CommonUpper_Rx_Deal(&g_stCurrentMsgPtr_SCI3);
    }
}


void USART3_IRQHandler(void)
{
    if ((USART_GetFlagStatus(USART3, USART_FLAG_RXDNE) != RESET) && (USART_GetIntStatus(USART3, USART_INT_RXDNE) != RESET))
    {
        /* Read one byte from the receive data register */
		Sci3_CommonUpper_FaultChk();
		Sci3_CommonUpper_Rx_Deal(&g_stCurrentMsgPtr_SCI3);
    }
}

void FDCAN3_INT0_IRQHandler(void)
{
    if(FDCAN_GetIntFlag(NODE1, FDCAN_FLAG_RX_FIFO0_WATERMARK) == SET) {
        FDCAN_ClearFlag(NODE1, FDCAN_FLAG_RX_FIFO0_WATERMARK);
        FDCAN_GetRxMsg(NODE1, FDCAN_RX_FIFO0, &RxMessage.RxHeader, RxMessage.RxData);
        Can_Status_Flag.bits.b1Can_Received = 1;
    }

    if(FDCAN_GetIntFlag(NODE1, FDCAN_FLAG_RX_FIFO0_NEW_MESSAGE) == SET) {
        FDCAN_ClearFlag(NODE1, FDCAN_FLAG_RX_FIFO0_NEW_MESSAGE);
        FDCAN_GetRxMsg(NODE1, FDCAN_RX_FIFO0, &RxMessage.RxHeader, RxMessage.RxData);
        Can_Status_Flag.bits.b1Can_Received = 1;
    }


    // FDCAN_ActivateInt(NODE1, FDCAN_FLAG_RX_FIFO0_NEW_MESSAGE, 0);
}


