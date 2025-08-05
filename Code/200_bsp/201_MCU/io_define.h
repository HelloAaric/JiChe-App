#ifndef _IO_DEFINE_H
#define _IO_DEFINE_H

#include "basic_data_type.h"
#include "n32h47x_48x_gpio.h"

typedef struct _16_Bits_Struct {
    UINT16 bit0 : 1;
    UINT16 bit1 : 1;
    UINT16 bit2 : 1;
    UINT16 bit3 : 1;
    UINT16 bit4 : 1;
    UINT16 bit5 : 1;
    UINT16 bit6 : 1;
    UINT16 bit7 : 1;
    UINT16 bit8 : 1;
    UINT16 bit9 : 1;
    UINT16 bit10 : 1;
    UINT16 bit11 : 1;
    UINT16 bit12 : 1;
    UINT16 bit13 : 1;
    UINT16 bit14 : 1;
    UINT16 bit15 : 1;
} Bits_16_TypeDef;

#define PORT_OUT_GPIOA          ((Bits_16_TypeDef *)(&(GPIOA->POD)))
#define PORT_OUT_GPIOB          ((Bits_16_TypeDef *)(&(GPIOB->POD)))
#define PORT_OUT_GPIOC          ((Bits_16_TypeDef *)(&(GPIOC->POD)))
#define PORT_OUT_GPIOD          ((Bits_16_TypeDef *)(&(GPIOD->POD)))
#define PORT_OUT_GPIOE          ((Bits_16_TypeDef *)(&(GPIOE->POD)))
#define PORT_OUT_GPIOF          ((Bits_16_TypeDef *)(&(GPIOF->POD)))
#define PORT_OUT_GPIOG          ((Bits_16_TypeDef *)(&(GPIOG->POD)))
#define PORT_OUT_GPIOH          ((Bits_16_TypeDef *)(&(GPIOH->POD)))

#define PORT_IN_GPIOA           ((Bits_16_TypeDef *)(&(GPIOA->PID)))
#define PORT_IN_GPIOB           ((Bits_16_TypeDef *)(&(GPIOB->PID)))
#define PORT_IN_GPIOC           ((Bits_16_TypeDef *)(&(GPIOC->PID)))
#define PORT_IN_GPIOD           ((Bits_16_TypeDef *)(&(GPIOD->PID)))
#define PORT_IN_GPIOE           ((Bits_16_TypeDef *)(&(GPIOE->PID)))
#define PORT_IN_GPIOF           ((Bits_16_TypeDef *)(&(GPIOF->PID)))
#define PORT_IN_GPIOG           ((Bits_16_TypeDef *)(&(GPIOG->PID)))
#define PORT_IN_GPIOH           ((Bits_16_TypeDef *)(&(GPIOH->PID)))


#if 0
//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+0x30)
#define GPIOB_ODR_Addr    (GPIOB_BASE+0x30)
#define GPIOC_ODR_Addr    (GPIOC_BASE+0x30)
#define GPIOD_ODR_Addr    (GPIOD_BASE+0x30)
#define GPIOE_ODR_Addr    (GPIOE_BASE+0x30)
#define GPIOF_ODR_Addr    (GPIOF_BASE+0x30)
#define GPIOG_ODR_Addr    (GPIOG_BASE+0x30)

#define GPIOA_IDR_Addr    (GPIOA_BASE+0x2C)
#define GPIOB_IDR_Addr    (GPIOB_BASE+0x2C)
#define GPIOC_IDR_Addr    (GPIOC_BASE+0x2C)
#define GPIOD_IDR_Addr    (GPIOD_BASE+0x2C)
#define GPIOE_IDR_Addr    (GPIOE_BASE+0x2C)
#define GPIOF_IDR_Addr    (GPIOF_BASE+0x2C)
#define GPIOG_IDR_Addr    (GPIOG_BASE+0x2C)

//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入
#endif


// LED
#define LED0_PORT                                   GPIOA
#define LED0_PIN                                    GPIO_PIN_4
#define MCUO_DEBUG_LED0 	                        (PORT_OUT_GPIOA->bit4)
#define LED1_PORT                                   GPIOA
#define LED1_PIN                                    GPIO_PIN_5
#define MCUO_DEBUG_LED1 	                        (PORT_OUT_GPIOA->bit5)
#define LED2_PORT                                   GPIOA
#define LED2_PIN                                    GPIO_PIN_6
#define MCUO_DEBUG_LED2 	                        (PORT_OUT_GPIOA->bit6)
#define LED3_PORT                                   GPIOA
#define LED3_PIN                                    GPIO_PIN_7
#define MCUO_DEBUG_LED3 	                        (PORT_OUT_GPIOA->bit7)
#define LED_RED_PORT                                GPIOA
#define LED_RED_PIN                                 GPIO_PIN_11
#define MCUO_DEBUG_LED_RED 	                        (PORT_OUT_GPIOA->bit11)
// #define LED_RED_PORT                                GPIOA
// #define LED_RED_PIN                                 GPIO_PIN_8
// #define MCUO_DEBUG_LED_RED 	                        (PORT_OUT_GPIOA->bit8)




// 有则改端口，没有则统一一个端口，执行空就好了。
// 485-1
extern UINT8 NOR_VALUE;
#define RS485_EN_PORT                               GPIOA
#define RS485_EN_PIN                                GPIO_PIN_12
#define MCUO_RS485_EN                               NOR_VALUE
// 485-2
#define RS485_EN2_PORT                              GPIOA
#define RS485_EN2_PIN                               GPIO_PIN_12
#define MCUO_RS485_EN2                              NOR_VALUE
// 485-3
#define RS485_EN3_PORT                              GPIOA
#define RS485_EN3_PIN                               GPIO_PIN_12
#define MCUO_RS485_EN3                              (PORT_OUT_GPIOA->bit12)


// LDO高有效
#define PWSV_LDO_PORT                               GPIOB
#define PWSV_LDO_PIN                                GPIO_PIN_3
#define MCUO_PWSV_LDO                               (PORT_OUT_GPIOB->bit3)		//

// DCDC低有效
#define PWSV_CTR_PORT                               GPIOC
#define PWSV_CTR_PIN                                GPIO_PIN_6
#define MCUO_PWSV_CTR                               (PORT_OUT_GPIOC->bit6)		//

#define WK_AFE_PORT                                 GPIOB
#define WK_AFE_PIN                                  GPIO_PIN_3
#define MCUO_WK_AFE                                 (PORT_OUT_GPIOB->bit3)

// #define SC_CUR_PORT                                 GPIOA
// #define SC_CUR_PIN                                  GPIO_PIN_12
// #define SC_CUR_EXTI_LINE                            EXTI_LINE_SOURCE0
// #define SC_CUR_EXTI_PORT                            GPIOA_PORT_SOURCE
// #define SC_CUR_EXTI_PIN                             GPIO_PIN_SOURCE12
// #define MCUI_SC_CUR                                 (PORT_IN_GPIOA->bit12)


#define DI1_PORT                                    GPIOA
#define DI1_PIN                                     GPIO_PIN_0
#define DI1_EXTI_PORT                               GPIOA_PORT_SOURCE
#define DI1_EXTI_PIN                                GPIO_PIN_SOURCE0
#define DI1_EXTI_LINE                               EXTI_LINE0
#define DI1_EXTI_LINE_SOURCE                        EXTI_LINE_SOURCE0
#define DI1_IRQN                                    EXTI0_IRQn                  // 这线中断三个都得0
#define MCUI_DI1 	                                (PORT_IN_GPIOA->bit0)		//

#define DI2_PORT                                    GPIOA
#define DI2_PIN                                     GPIO_PIN_2
#define DI2_EXTI_PORT                               GPIOA_PORT_SOURCE
#define DI2_EXTI_PIN                                GPIO_PIN_SOURCE2
#define DI2_EXTI_LINE                               EXTI_LINE2
#define DI2_EXTI_LINE_SOURCE                        EXTI_LINE_SOURCE2
#define DI2_IRQN                                    EXTI2_IRQn
#define MCUI_DI2	                                (PORT_IN_GPIOA->bit2)		//

#define DI_LOAD_PORT                                GPIOC
#define DI_LOAD_PIN                                 GPIO_PIN_8
#define DI_LOAD_EXTI_PORT                           GPIOC_PORT_SOURCE
#define DI_LOAD_EXTI_PIN                            GPIO_PIN_SOURCE8
#define DI_LOAD_EXTI_LINE                           EXTI_LINE8
#define DI_LOAD_EXTI_LINE_SOURCE                    EXTI_LINE_SOURCE8
#define DI_LOAD_IRQN                                EXTI9_5_IRQn
#define MCUI_DI_LOAD	                            (PORT_IN_GPIOC->bit8)		//

#define DI_CWAKE_PORT                               GPIOC
#define DI_CWAKE_PIN                                GPIO_PIN_5
#define DI_CWAKE_EXTI_PORT                          GPIOC_PORT_SOURCE
#define DI_CWAKE_EXTI_PIN                           GPIO_PIN_SOURCE5
#define DI_CWAKE_EXTI_LINE                          EXTI_LINE5
#define DI_CWAKE_EXTI_LINE_SOURCE                   EXTI_LINE_SOURCE5
#define DI_CWAKE_IRQN                               EXTI9_5_IRQn
#define MCUI_DI_CWAKE	                            (PORT_IN_GPIOC->bit5)		//
#define MCUI_DO_CWAKE	                            (PORT_OUT_GPIOC->bit5)		//

#define DI_CBC_PORT                                 GPIOB
#define DI_CBC_PIN                                  GPIO_PIN_11
#define DI_CBC_EXTI_PORT                            GPIOB_PORT_SOURCE
#define DI_CBC_EXTI_PIN                             GPIO_PIN_SOURCE11
#define DI_CBC_EXTI_LINE                            EXTI_LINE11
#define DI_CBC_EXTI_LINE_SOURCE                     EXTI_LINE_SOURCE11
#define DI_CBC_IRQN                                 EXTI15_10_IRQn
#define MCUI_DI_CBC	                                (PORT_IN_GPIOB->bit11)		//



#define RELAY_K1_PORT                               GPIOA
#define RELAY_K1_PIN                                GPIO_PIN_15
#define MCUO_RELAY_K1	                            (PORT_OUT_GPIOA->bit15)
#define RELAY_K2_PORT                               GPIOC
#define RELAY_K2_PIN                                GPIO_PIN_12
#define MCUO_RELAY_K2	                            (PORT_OUT_GPIOC->bit12)
#define RELAY_K3_PORT                               GPIOD
#define RELAY_K3_PIN                                GPIO_PIN_2
#define MCUO_RELAY_K3	                            (PORT_OUT_GPIOD->bit2)
#define RELAY_K4_PORT                               GPIOB
#define RELAY_K4_PIN                                GPIO_PIN_8
#define MCUO_RELAY_K4	                            (PORT_OUT_GPIOB->bit8)
#define RELAY_K5_PORT                               GPIOB
#define RELAY_K5_PIN                                GPIO_PIN_9
#define MCUO_RELAY_K5	                            (PORT_OUT_GPIOB->bit9)

#define RELAY_PRECHG_PORT                           RELAY_K1_PORT
#define RELAY_PRECHG_PIN                            RELAY_K1_PIN
#define MCUO_RELAY_PRECHG                           MCUO_RELAY_K1
#define RELAY_CHG_PORT                              RELAY_K5_PORT
#define RELAY_CHG_PIN                               RELAY_K5_PIN
#define MCUO_RELAY_CHG                              MCUO_RELAY_K5
#define RELAY_DSG_PORT                              RELAY_K2_PORT
#define RELAY_DSG_PIN                               RELAY_K2_PIN
#define MCUO_RELAY_DSG                              MCUO_RELAY_K2

#define MCUO_HEAT_CTRL                              MCUO_RELAY_K4
#define MCUO_RELAY_CHG2                             MCUO_RELAY_K5               // 不懂

#define ADC_NTC0_PORT                               GPIOA
#define ADC_NTC0_PIN                                GPIO_PIN_3
#define ADC_NTC1_PORT                               GPIOC
#define ADC_NTC1_PIN                                GPIO_PIN_4
#define ADC_CUR_PORT                                GPIOB
#define ADC_CUR_PIN                                 GPIO_PIN_2
// #define ADC_24V_PORT                                GPIOC
// #define ADC_24V_PIN                                 GPIO_PIN_5
// #define ADC_12V_PORT                                GPIOB
// #define ADC_12V_PIN                                 GPIO_PIN_0
#define ADC_3_3V_PORT                               GPIOB
#define ADC_3_3V_PIN                                GPIO_PIN_15


// PWM
#define TIM_PWM                                     GTIM2
#define TIM_PWM_CLK                                 RCC_APB1_PERIPH_GTIM2

#define PWM_OUT1_PORT								GPIOB
#define PWM_OUT1_PIN								GPIO_PIN_0
#define PWM_OUT1_AF                    			    GPIO_AF3
#define PWM_OUT1_CHANNEL						    TIM_InitOc3
#define PWM_OUT1_PRELOAD                            TIM_ConfigOc3Preload

#define PWM_OUT2_SC_PORT							GPIOC
#define PWM_OUT2_SC_PIN								GPIO_PIN_7
#define PWM_OUT2_SC_AF                    			GPIO_AF6
#define PWM_OUT2_SC_CHANNEL						    TIM_InitOc2
#define PWM_OUT2_SC_PRELOAD                         TIM_ConfigOc2Preload


#define PWM_IN1_PORT								GPIOB
#define PWM_IN1_PIN								    GPIO_PIN_1
#define PWM_IN1_AF                    			    GPIO_AF3
#define PWM_IN1_CHANNEL							    TIM_CHANNEL_4


// CAN
// CAN1
#define NODE1                                       (FDCAN3)
#define NODE1_PERIPH                                (RCC_APB1_PERIPH_FDCAN3)    
#define NODE1_TX_PORT                               (GPIOC)
#define NODE1_TX_PIN                                (GPIO_PIN_10)
#define NODE1_TX_PIN_AF                             (GPIO_AF29)
#define NODE1_RX_PORT                               (GPIOC)
#define NODE1_RX_PIN                                (GPIO_PIN_11)
#define NODE1_RX_PIN_AF                             (GPIO_AF30)
#define NODE1_IRQN                                  FDCAN3_INT0_IRQn

// CAN2
#define NODE2                                       (FDCAN2)
#define NODE2_PERIPH                                (RCC_APB1_PERIPH_FDCAN2)
#define NODE2_TX_PORT                               (GPIOA)
#define NODE2_TX_PIN                                (GPIO_PIN_8)
#define NODE2_TX_PIN_AF                             (GPIO_AF27)
#define NODE2_RX_PORT                               (GPIOA)
#define NODE2_RX_PIN                                (GPIO_PIN_9)
#define NODE2_RX_PIN_AF                             (GPIO_AF28)



// UART
#define UART_UPPER1_PORT                            GPIOC
#define UART_UPPER1_RxPin                           GPIO_PIN_1
#define UART_UPPER1_RxPin_AF                        GPIO_AF10
#define UART_UPPER1_TxPin                           GPIO_PIN_0
#define UART_UPPER1_TxPin_AF                        GPIO_AF10
#define UART_UPPER1                                 USART4
#define UART_UPPER1_BAUD                            19200
#define UART_UPPER1_IRQn                            USART4_IRQn


#define UART_UPPER2_PORT                            GPIOC
#define UART_UPPER2_RxPin                           GPIO_PIN_3
#define UART_UPPER2_RxPin_AF                        GPIO_AF13
#define UART_UPPER2_TxPin                           GPIO_PIN_2
#define UART_UPPER2_TxPin_AF                        GPIO_AF13
#define UART_UPPER2                                 UART7
#define UART_UPPER2_BAUD                            19200
#define UART_UPPER2_IRQn                            UART7_IRQn

#if 0
#define UART_UPPER3_PORT                            GPIOA
#define UART_UPPER3_RxPin                           GPIO_PIN_10
#define UART_UPPER3_RxPin_AF                        GPIO_AF18
#define UART_UPPER3_TxPin                           GPIO_PIN_9
#define UART_UPPER3_TxPin_AF                        GPIO_AF17
#define UART_UPPER3                                 USART3
#define UART_UPPER3_BAUD                            19200
#define UART_UPPER3_IRQn                            USART3_IRQn
#endif

#if 1
#define UART_UPPER3_PORT                            GPIOA
#define UART_UPPER3_RxPin                           GPIO_PIN_10
#define UART_UPPER3_RxPin_AF                        GPIO_AF5
#define UART_UPPER3_TxPin                           GPIO_PIN_9
#define UART_UPPER3_TxPin_AF                        GPIO_AF5
#define UART_UPPER3                                 USART1
#define UART_UPPER3_BAUD                            19200
#define UART_UPPER3_IRQn                            USART1_IRQn
#endif

#if 0
#define UART_UPPER3_PORT                            GPIOB
#define UART_UPPER3_RxPin                           GPIO_PIN_6
#define UART_UPPER3_RxPin_AF                        GPIO_AF1
#define UART_UPPER3_TxPin                           GPIO_PIN_7
#define UART_UPPER3_TxPin_AF                        GPIO_AF1
#define UART_UPPER3                                 USART1
#define UART_UPPER3_BAUD                            19200
#define UART_UPPER3_IRQn                            USART1_IRQn
#endif

// EEPROM
#define EEPROM_SDA_PORT                             GPIOB
#define EEPROM_SDA_PIN                              GPIO_PIN_13
#define MCUO_EEPROM_SDA 	                        (PORT_OUT_GPIOB->bit13)		//
#define MCUI_EEPROM_SDA 	                        (PORT_IN_GPIOB->bit13)		//
#define EEPROM_SCL_PORT                             GPIOB
#define EEPROM_SCL_PIN                              GPIO_PIN_12
#define MCUO_EEPROM_SCL 	                        (PORT_OUT_GPIOB->bit12)		//
#define E2PR_WP_PORT                                GPIOB
#define E2PR_WP_PIN                                 GPIO_PIN_14
#define MCUO_E2PR_WP                                (PORT_OUT_GPIOB->bit14)		//

#define SHDN_PORT                                   GPIOB
#define SHDN_PIN                                    GPIO_PIN_4
#define MCUO_AFE_WAKE_BOTH_SHDN	                    (PORT_OUT_GPIOB->bit4)		//
#define MCUO_SPI_NSS_AFE1 	                        (PORT_OUT_GPIOA->bit4)		//
#define MCUO_SPI_NSS_AFE2 	                        (PORT_OUT_GPIOB->bit5)		//
// #define MCUO_SPI_NSS_AFE1
// #define MCUO_SPI_NSS_AFE2




#define SIF_PORT                                    GPIOB
#define SIF_PIN                                     GPIO_PIN_0
#define MCUO_SIF_COM 	                            (PORT_OUT_GPIOB->bit0)
#define MCUI_SIF_COM	                            (PORT_IN_GPIOB->bit0)		//
// #define MCUO_SIF_COM                                NOR_VALUE

#endif	/* _IO_DEFINE_H */
