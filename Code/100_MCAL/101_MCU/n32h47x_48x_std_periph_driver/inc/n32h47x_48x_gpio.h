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
 *\*\file n32h47x_48x_gpio.h
 *\*\author Nations
 *\*\version v1.0.0
 *\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
 **/

#ifndef __N32H47X_48X_GPIO_H__
#define __N32H47X_48X_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32h47x_48x.h"


/*** GPIO Structure Definition Start ***/

/** GPIO Init Structure Definition **/
typedef struct
{
    uint32_t         Pin;            /* Specifies the GPIO pins to be configured. */
    uint32_t         GPIO_Mode;      /* Specifies the operating mode for the selected pins. */
    uint32_t         GPIO_Pull;      /* Specifies the Pull-up or Pull-Down activation for the selected pins. */
    uint32_t         GPIO_Slew_Rate; /* Specify the reverse speed for the selected pins. */
    uint32_t         GPIO_Current;   /* Driving capability of the select pins. */
    uint32_t         GPIO_Alternate; /* Peripheral to be connected to the selected pins. */
}GPIO_InitType;

/** Bit_SET and Bit_RESET enumeration **/
typedef enum
{
    Bit_RESET = 0,
    Bit_SET
}Bit_OperateType;

/*** GPIO Structure Definition End ***/


/*** GPIO Macro Definition Start ***/
#define GPIO_GET_INDEX(GPIOX_BASE)      (((GPIOX_BASE) - GPIOA_BASE)/0x400UL)

#define GPIO_GET_PERIPH(INDEX)      ((GPIO_Module*)(GPIOA_BASE + (INDEX) * 0x400UL))

/** Bit mask define **/
#define GPIO_PMODE_BIT_MASK                 (GPIO_PMODE_PMODE0)
#define GPIO_PUPD_BIT_MASK                  (GPIO_PUPD_PUPD0)
#define GPIO_AFSEL_BIT_MASK                 (GPIO_AFSEL0_AFSEL0)
#define GPIO_DS_BIT_MASK                    (GPIO_DS_DS0)
#define GPIO_SR_BIT_MASK                    (GPIO_SR_SR0)
#define AFIO_EXTI_BIT_MASK                  (AFIO_EXTI_CFG1_EXTI0)

/** GPIO_mode_define **/
/*     bit[0:1]: IO mode
 *\*\  bit[4]:   Open-drain output mark 
 */
#define GPIO_MODE_OD_MARK                   (GPIO_POTYPE_OPEN_DRAIN << 4)

#define GPIO_MODE_INPUT                     (GPIO_PMODE_INPUT)                          /* Input Floating Mode */
#define GPIO_MODE_OUTPUT_PP                 (GPIO_PMODE_OUTPUT)                         /* Output Push Pull Mode */
#define GPIO_MODE_OUTPUT_OD                 (GPIO_PMODE_OUTPUT | GPIO_MODE_OD_MARK)     /* Output Open Drain Mode */
#define GPIO_MODE_AF_PP                     (GPIO_PMODE_AF_OUTPUT)                      /* Alternate Function Push Pull Mode  */
#define GPIO_MODE_AF_OD                     (GPIO_PMODE_AF_OUTPUT | GPIO_MODE_OD_MARK)  /* Alternate Function Open Drain Mode */
#define GPIO_MODE_ANALOG                    (GPIO_PMODE_ANALOG)                         /* Analog Mode */

/** GPIO pins define **/
#define GPIO_PIN_0                          ((uint16_t)0x0001U)  /* Pin 0 selected    */
#define GPIO_PIN_1                          ((uint16_t)0x0002U)  /* Pin 1 selected    */
#define GPIO_PIN_2                          ((uint16_t)0x0004U)  /* Pin 2 selected    */
#define GPIO_PIN_3                          ((uint16_t)0x0008U)  /* Pin 3 selected    */
#define GPIO_PIN_4                          ((uint16_t)0x0010U)  /* Pin 4 selected    */
#define GPIO_PIN_5                          ((uint16_t)0x0020U)  /* Pin 5 selected    */
#define GPIO_PIN_6                          ((uint16_t)0x0040U)  /* Pin 6 selected    */
#define GPIO_PIN_7                          ((uint16_t)0x0080U)  /* Pin 7 selected    */
#define GPIO_PIN_8                          ((uint16_t)0x0100U)  /* Pin 8 selected    */
#define GPIO_PIN_9                          ((uint16_t)0x0200U)  /* Pin 9 selected    */
#define GPIO_PIN_10                         ((uint16_t)0x0400U)  /* Pin 10 selected   */
#define GPIO_PIN_11                         ((uint16_t)0x0800U)  /* Pin 11 selected   */
#define GPIO_PIN_12                         ((uint16_t)0x1000U)  /* Pin 12 selected   */
#define GPIO_PIN_13                         ((uint16_t)0x2000U)  /* Pin 13 selected   */
#define GPIO_PIN_14                         ((uint16_t)0x4000U)  /* Pin 14 selected   */
#define GPIO_PIN_15                         ((uint16_t)0x8000U)  /* Pin 15 selected   */
#define GPIO_PIN_ALL                        ((uint16_t)0xFFFFU)  /* All pins selected */
#define GPIO_PIN_NONE                       ((uint16_t)0x0000U)  /* None pin selected */


/** GPIO Pull-Up or Pull-Down Activation **/
#define GPIO_NO_PULL                        (GPIO_PUPD_NO_PULL)     /* No Pull-up or Pull-down activation */
#define GPIO_PULL_UP                        (GPIO_PUPD_PULL_UP)     /* Pull-up activation */
#define GPIO_PULL_DOWN                      (GPIO_PUPD_PULL_DOWN)   /* Pull-down activation */

/** GPIO Drive strength config **/
#define GPIO_DC_2mA                         (GPIO_DS_2mA)
#define GPIO_DC_4mA                         (GPIO_DS_4mA)
#define GPIO_DC_8mA                         (GPIO_DS_8mA)
#define GPIO_DC_12mA                        (GPIO_DS_12mA)

/** slew rate config **/
#define GPIO_SLEW_RATE_FAST                 (GPIO_SR_FAST_SLEW)
#define GPIO_SLEW_RATE_SLOW                 (GPIO_SR_SLOW_SLEW)

/** GPIO port sources **/
#define GPIOA_PORT_SOURCE                   ((uint8_t)0x00U)
#define GPIOB_PORT_SOURCE                   ((uint8_t)0x01U)
#define GPIOC_PORT_SOURCE                   ((uint8_t)0x02U)
#define GPIOD_PORT_SOURCE                   ((uint8_t)0x03U)
#define GPIOE_PORT_SOURCE                   ((uint8_t)0x04U)
#define GPIOF_PORT_SOURCE                   ((uint8_t)0x05U)
#define GPIOG_PORT_SOURCE                   ((uint8_t)0x06U)
#define GPIOH_PORT_SOURCE                   ((uint8_t)0x07U)

/** GPIO pin sources **/
#define GPIO_PIN_SOURCE0                    ((uint8_t)0x00U) /* Pin 0 selected    */
#define GPIO_PIN_SOURCE1                    ((uint8_t)0x01U) /* Pin 1 selected    */
#define GPIO_PIN_SOURCE2                    ((uint8_t)0x02U) /* Pin 2 selected    */
#define GPIO_PIN_SOURCE3                    ((uint8_t)0x03U) /* Pin 3 selected    */
#define GPIO_PIN_SOURCE4                    ((uint8_t)0x04U) /* Pin 4 selected    */
#define GPIO_PIN_SOURCE5                    ((uint8_t)0x05U) /* Pin 5 selected    */
#define GPIO_PIN_SOURCE6                    ((uint8_t)0x06U) /* Pin 6 selected    */
#define GPIO_PIN_SOURCE7                    ((uint8_t)0x07U) /* Pin 7 selected    */
#define GPIO_PIN_SOURCE8                    ((uint8_t)0x08U) /* Pin 8 selected    */
#define GPIO_PIN_SOURCE9                    ((uint8_t)0x09U) /* Pin 9 selected    */
#define GPIO_PIN_SOURCE10                   ((uint8_t)0x0AU) /* Pin 10 selected   */
#define GPIO_PIN_SOURCE11                   ((uint8_t)0x0BU) /* Pin 11 selected   */
#define GPIO_PIN_SOURCE12                   ((uint8_t)0x0CU) /* Pin 12 selected   */
#define GPIO_PIN_SOURCE13                   ((uint8_t)0x0DU) /* Pin 13 selected   */
#define GPIO_PIN_SOURCE14                   ((uint8_t)0x0EU) /* Pin 14 selected   */
#define GPIO_PIN_SOURCE15                   ((uint8_t)0x0FU) /* Pin 15 selected   */

/** EXTI line sources **/
#define EXTI_LINE_SOURCE0                   ((uint8_t)0x00U) /* EXTI_line_0  */
#define EXTI_LINE_SOURCE1                   ((uint8_t)0x01U) /* EXTI_line_1  */
#define EXTI_LINE_SOURCE2                   ((uint8_t)0x02U) /* EXTI_line_2  */
#define EXTI_LINE_SOURCE3                   ((uint8_t)0x03U) /* EXTI_line_3  */
#define EXTI_LINE_SOURCE4                   ((uint8_t)0x04U) /* EXTI_line_4  */
#define EXTI_LINE_SOURCE5                   ((uint8_t)0x05U) /* EXTI_line_5  */
#define EXTI_LINE_SOURCE6                   ((uint8_t)0x06U) /* EXTI_line_6  */
#define EXTI_LINE_SOURCE7                   ((uint8_t)0x07U) /* EXTI_line_7  */
#define EXTI_LINE_SOURCE8                   ((uint8_t)0x08U) /* EXTI_line_8  */
#define EXTI_LINE_SOURCE9                   ((uint8_t)0x09U) /* EXTI_line_9  */
#define EXTI_LINE_SOURCE10                  ((uint8_t)0x0AU) /* EXTI_line_10 */
#define EXTI_LINE_SOURCE11                  ((uint8_t)0x0BU) /* EXTI_line_11 */
#define EXTI_LINE_SOURCE12                  ((uint8_t)0x0CU) /* EXTI_line_12 */
#define EXTI_LINE_SOURCE13                  ((uint8_t)0x0DU) /* EXTI_line_13 */
#define EXTI_LINE_SOURCE14                  ((uint8_t)0x0EU) /* EXTI_line_14 */
#define EXTI_LINE_SOURCE15                  ((uint8_t)0x0FU) /* EXTI_line_15 */

/** GPIOx_Alternate_function_selection Alternate function selection **/
#define GPIO_NO_AF                          (GPIO_AFSEL_AF0 )
#define GPIO_AF0                            (GPIO_AFSEL_AF0 )
#define GPIO_AF1                            (GPIO_AFSEL_AF1 )
#define GPIO_AF2                            (GPIO_AFSEL_AF2 )
#define GPIO_AF3                            (GPIO_AFSEL_AF3 )
#define GPIO_AF4                            (GPIO_AFSEL_AF4 )
#define GPIO_AF5                            (GPIO_AFSEL_AF5 )
#define GPIO_AF6                            (GPIO_AFSEL_AF6 )
#define GPIO_AF7                            (GPIO_AFSEL_AF7 )
#define GPIO_AF8                            (GPIO_AFSEL_AF8 )
#define GPIO_AF9                            (GPIO_AFSEL_AF9 )
#define GPIO_AF10                           (GPIO_AFSEL_AF10)
#define GPIO_AF11                           (GPIO_AFSEL_AF11)
#define GPIO_AF12                           (GPIO_AFSEL_AF12)
#define GPIO_AF13                           (GPIO_AFSEL_AF13)
#define GPIO_AF14                           (GPIO_AFSEL_AF14)
#define GPIO_AF15                           (GPIO_AFSEL_AF15)
#define GPIO_AF16                           (GPIO_AFSEL_AF16)
#define GPIO_AF17                           (GPIO_AFSEL_AF17)
#define GPIO_AF18                           (GPIO_AFSEL_AF18)
#define GPIO_AF19                           (GPIO_AFSEL_AF19)
#define GPIO_AF20                           (GPIO_AFSEL_AF20)
#define GPIO_AF21                           (GPIO_AFSEL_AF21)
#define GPIO_AF22                           (GPIO_AFSEL_AF22)
#define GPIO_AF23                           (GPIO_AFSEL_AF23)
#define GPIO_AF24                           (GPIO_AFSEL_AF24)
#define GPIO_AF25                           (GPIO_AFSEL_AF25)
#define GPIO_AF26                           (GPIO_AFSEL_AF26)
#define GPIO_AF27                           (GPIO_AFSEL_AF27)
#define GPIO_AF28                           (GPIO_AFSEL_AF28)
#define GPIO_AF29                           (GPIO_AFSEL_AF29)
#define GPIO_AF30                           (GPIO_AFSEL_AF30)
#define GPIO_AF31                           (GPIO_AFSEL_AF31)

/** SPI mode definition in AFIO **/
#define AFIO_SPI1_NSS                       (AFIO_RMP_CFG_SPI1_NSS)
#define AFIO_SPI2_NSS                       (AFIO_RMP_CFG_SPI2_NSS)
#define AFIO_SPI3_NSS                       (AFIO_RMP_CFG_SPI3_NSS)
#define AFIO_SPI4_NSS                       (AFIO_RMP_CFG_SPI4_NSS)
#define AFIO_SPI5_NSS                       (AFIO_RMP_CFG_SPI5_NSS)
#define AFIO_SPI6_NSS                       (AFIO_RMP_CFG_SPI6_NSS)
#define AFIO_SPI_NSS_HIGH_IMPEDANCE         ((uint32_t)0x00000000U)
#define AFIO_SPI_NSS_HIGH_LEVEL             (AFIO_RMP_CFG_SPI1_NSS)

/** JTAG-SWD port remap **/
#define GPIO_RMP_SWJ_MARK                   ((uint32_t)0x80000000U)
#define GPIO_RMP_SWJ_JTAG                   (GPIO_RMP_SWJ_MARK)
#define GPIO_RMP_SWJ_JTAG_NO_NJTRST         (AFIO_RMP_CFG_SWJ_CFG_0|GPIO_RMP_SWJ_MARK)
#define GPIO_RMP_SWJ_SWD                    (AFIO_RMP_CFG_SWJ_CFG_1|GPIO_RMP_SWJ_MARK)
#define GPIO_RMP_SWJ_DISABLE                (AFIO_RMP_CFG_SWJ_CFG_2|GPIO_RMP_SWJ_MARK)

/** IO digtal filter definition in AFIO **/
#define AFIO_IO_FILTER_MASK                 (AFIO_RMP_CFG_IO_FILTER)
#define AFIO_IO_FILTER_OFFSET               (REG_BIT17_OFFSET)

/** FEMC NADV pin definition in AFIO  **/
#define AFIO_FEMC_NADV_PIN_USED             ((uint32_t)0x00000000U)
#define AFIO_FEMC_NADV_PIN_NOT_USED         (AFIO_RMP_CFG_FEMC_NADV)

/** EXTI Analog Filter definition in AFIO  **/
#define AFIO_EXTI_FILTER_ENABLE             ((uint32_t)0x00000000U)
#define AFIO_EXTI_FILTER_DISABLE            (AFIO_RMP_CFG_EXTI_FILTER)

/** XSPI XIP big-endian definition during writting **/
#define AFIO_XSPI_XIP_LITTLE_ENDIAN         ((uint32_t)0x00000000U)
#define AFIO_XSPI_XIP_BIG_ENDIAN            (AFIO_RMP_CFG_XSPI_XIP_WR_BE)

/** XSPI half-duplex mode define **/
#define AFIO_XSPI_HALF_DUPLEX_DISABLE       ((uint32_t)0x00000000U)
#define AFIO_XSPI_HALF_DUPLEX_ENABLE        (AFIO_RMP_CFG_XSPI_HALF_DUPLEX)

/** XSPI dual-quad mode define **/
#define AFIO_XSPI_DUAL_QUAD_DISABLE         ((uint32_t)0x00000000U)
#define AFIO_XSPI_DUAL_QUAD_ENABLE          (AFIO_RMP_CFG_XSPI_DUAL_QUAD)

/** ETH mode definition in AFIO **/
#define AFIO_ETH_MODE_MII                   ((uint32_t)0x00000000U)
#define AFIO_ETH_MODE_RMII                  (AFIO_RMP_CFG_ETH_MODE)

/** XSPI flash2 enable in dual-quad mode **/
#define AFIO_XSPI_DUAL_QUAD_FLASH2_DISABLE  ((uint32_t)0x00000000U)
#define AFIO_XSPI_DUAL_QUAD_FLASH2_ENABLE   (AFIO_RMP_CFG_XSPI_FLASH2)

/** XSPI NSS pin input define in master mode **/
#define AFIO_XSPI_NSS_INPUT_DISABLE         ((uint32_t)0x00000000U)
#define AFIO_XSPI_NSS_INPUT_ENABLE          (AFIO_RMP_CFG_XSPI_NSS_IN)

/** XSPI NSS pin input select in master mode **/
#define AFIO_XSPI_NSS_INPUT_NSS0            ((uint32_t)0x00000000U)
#define AFIO_XSPI_NSS_INPUT_NSS1            (AFIO_RMP_CFG_XSPI_NSS_SEL)

/** XSPI RXDS sample delay define  **/
#define AFIO_XSPI_RXDS_DELAY_NONE           ((uint32_t)0x00000000U)
#define AFIO_XSPI_RXDS_DELAY_1HCLK          (AFIO_EMC_CFG_XSPI_RXDS_0)
#define AFIO_XSPI_RXDS_DELAY_2HCLK          (AFIO_EMC_CFG_XSPI_RXDS_1)
#define AFIO_XSPI_RXDS_DELAY_MASK           (AFIO_EMC_CFG_XSPI_RXDS)

/** XSPI big-endian define during writting in non-XIP mode **/
#define AFIO_XSPI_WRITE_LITTLE_ENDIAN       ((uint32_t)0x00000000U)
#define AFIO_XSPI_WRITE_BIG_ENDIAN          (AFIO_EMC_CFG_XSPI_WR_BE)

/** XSPI big-endian define during reading in non-XIP mode **/
#define AFIO_XSPI_READ_LITTLE_ENDIAN        ((uint32_t)0x00000000U)
#define AFIO_XSPI_READ_BIG_ENDIAN           (AFIO_EMC_CFG_XSPI_RD_BE)

/** XSPI setup or hold time extension define **/
#define AFIO_XSPI_NO_TIME_EXTENSION         ((uint32_t)0x00000000U)
#define AFIO_XSPI_SETUP_TIME_EXTENSION      (AFIO_EMC_CFG_XSPI_EXTEND_0)
#define AFIO_XSPI_HOLD_TIME_EXTENSION       (AFIO_EMC_CFG_XSPI_EXTEND_1)
#define AFIO_XSPI_SETUP_HOLD_TIME_EXTENSION (AFIO_EMC_CFG_XSPI_EXTEND)

/** XSPI NSS pin high time when idle **/
#define AFIO_XSPI_NSS_HIGH_1CLK             ((uint32_t)0x00000000U)
#define AFIO_XSPI_NSS_HIGH_2CLK             ((uint32_t)0x00000400U)
#define AFIO_XSPI_NSS_HIGH_3CLK             ((uint32_t)0x00000800U)
#define AFIO_XSPI_NSS_HIGH_4CLK             ((uint32_t)0x00000C00U)
#define AFIO_XSPI_NSS_HIGH_5CLK             ((uint32_t)0x00001000U)
#define AFIO_XSPI_NSS_HIGH_6CLK             ((uint32_t)0x00001400U)
#define AFIO_XSPI_NSS_HIGH_7CLK             ((uint32_t)0x00001800U)
#define AFIO_XSPI_NSS_HIGH_8CLK             ((uint32_t)0x00001C00U)
#define AFIO_XSPI_NSS_HIGH_9CLK             ((uint32_t)0x00002000U)
#define AFIO_XSPI_NSS_HIGH_10CLK            ((uint32_t)0x00002400U)
#define AFIO_XSPI_NSS_HIGH_11CLK            ((uint32_t)0x00002800U)
#define AFIO_XSPI_NSS_HIGH_12CLK            ((uint32_t)0x00002C00U)
#define AFIO_XSPI_NSS_HIGH_13CLK            ((uint32_t)0x00003000U)
#define AFIO_XSPI_NSS_HIGH_14CLK            ((uint32_t)0x00003400U)
#define AFIO_XSPI_NSS_HIGH_15CLK            ((uint32_t)0x00003800U)
#define AFIO_XSPI_NSS_HIGH_16CLK            ((uint32_t)0x00003C00U)

/* SHRTIM EXEV event definition in AFIO */
#define AFIO_SHRTIM_EXEV_1                   ((uint8_t)0x00U)
#define AFIO_SHRTIM_EXEV_2                   ((uint8_t)0x01U)
#define AFIO_SHRTIM_EXEV_3                   ((uint8_t)0x02U)
#define AFIO_SHRTIM_EXEV_4                   ((uint8_t)0x03U)
#define AFIO_SHRTIM_EXEV_5                   ((uint8_t)0x04U)
#define AFIO_SHRTIM_EXEV_6                   ((uint8_t)0x05U)
#define AFIO_SHRTIM_EXEV_7                   ((uint8_t)0x06U)
#define AFIO_SHRTIM_EXEV_8                   ((uint8_t)0x07U)
#define AFIO_SHRTIM_EXEV_9                   ((uint8_t)0x08U)
#define AFIO_SHRTIM_EXEV_10                  ((uint8_t)0x09U)

#define AFIO_SHRTIM_EXEV_BIT_MASK            (AFIO_SHRT_EXEV_CFG1_SHRT1_EXEV1)

/* EMC config */
#define AFIO_EMC_CLAMP1_RST                 (AFIO_EMC_CFG_CLAMP1_RST)
#define AFIO_EMC_CLAMP2_RST                 (AFIO_EMC_CFG_CLAMP2_RST)
#define AFIO_EMC_CLAMP3_RST                 (AFIO_EMC_CFG_CLAMP3_RST)
#define AFIO_EMC_CLAMP4_RST                 (AFIO_EMC_CFG_CLAMP4_RST)
#define AFIO_EMC_GBN1_RST                   (AFIO_EMC_CFG_GBN1_RST  )
#define AFIO_EMC_GBN2_RST                   (AFIO_EMC_CFG_GBN2_RST  )
#define AFIO_EMC_GBN3_RST                   (AFIO_EMC_CFG_GBN3_RST  )
#define AFIO_EMC_GBN4_RST                   (AFIO_EMC_CFG_GBN4_RST  )
#define AFIO_EMC_GB1_RST                    (AFIO_EMC_CFG_GB1_RST   )
#define AFIO_EMC_GB2_RST                    (AFIO_EMC_CFG_GB2_RST   )
#define AFIO_EMC_GB3_RST                    (AFIO_EMC_CFG_GB3_RST   )
#define AFIO_EMC_GB4_RST                    (AFIO_EMC_CFG_GB4_RST   )
#define AFIO_EMC_CLAMP1_DET                 (AFIO_EMC_CFG_CLAMP1_DET)
#define AFIO_EMC_CLAMP2_DET                 (AFIO_EMC_CFG_CLAMP2_DET)
#define AFIO_EMC_CLAMP3_DET                 (AFIO_EMC_CFG_CLAMP3_DET)
#define AFIO_EMC_CLAMP4_DET                 (AFIO_EMC_CFG_CLAMP4_DET)
#define AFIO_EMC_GBN1_DET                   (AFIO_EMC_CFG_GBN1_DET  )
#define AFIO_EMC_GBN2_DET                   (AFIO_EMC_CFG_GBN2_DET  )
#define AFIO_EMC_GBN3_DET                   (AFIO_EMC_CFG_GBN3_DET  )
#define AFIO_EMC_GBN4_DET                   (AFIO_EMC_CFG_GBN4_DET  )
#define AFIO_EMC_GB1_DET                    (AFIO_EMC_CFG_GB1_DET   )
#define AFIO_EMC_GB2_DET                    (AFIO_EMC_CFG_GB2_DET   )
#define AFIO_EMC_GB3_DET                    (AFIO_EMC_CFG_GB3_DET   )
#define AFIO_EMC_GB4_DET                    (AFIO_EMC_CFG_GB4_DET   )

/** GPIO_Exported_Functions **/
void GPIO_DeInit(GPIO_Module* GPIOx);
void GPIO_DeInitPin(GPIO_Module* GPIOx, uint32_t Pin);
void GPIO_AFIOInitDefault(void);

void GPIO_InitPeripheral(GPIO_Module* GPIOx, GPIO_InitType * GPIO_InitStruct);
void GPIO_InitStruct(GPIO_InitType* GPIO_InitStruct);

uint8_t GPIO_ReadInputDataBit(GPIO_Module* GPIOx, uint16_t Pin);
uint16_t GPIO_ReadInputData(GPIO_Module* GPIOx);
uint8_t GPIO_ReadOutputDataBit(GPIO_Module* GPIOx, uint16_t Pin);
uint16_t GPIO_ReadOutputData(GPIO_Module* GPIOx);

void GPIO_SetBits(GPIO_Module* GPIOx, uint16_t Pin);
void GPIO_ResetBits(GPIO_Module* GPIOx, uint16_t Pin);
void GPIO_WriteBits(GPIO_Module* GPIOx, uint16_t Pin, Bit_OperateType BitCmd);
void GPIO_Write(GPIO_Module* GPIOx, uint16_t data_value);
void GPIO_TogglePin(GPIO_Module *GPIOx, uint16_t Pin);
void GPIO_ConfigPinLock(GPIO_Module* GPIOx, uint16_t Pin);

void GPIO_ConfigEXTILine(uint8_t LineSource, uint8_t PortSource, uint8_t PinSource);
void GPIO_ConfigPinRemap(uint8_t PortSource, uint8_t PinSource, uint32_t AlternateFunction);

void AFIO_Config_SPI_NSS_Mode(uint32_t AFIO_SPIx_NSS, uint32_t NSS_Mode);
void AFIO_ConfigIOFilter(uint32_t Filter_Cycle);
void AFIO_Config_FEMC_NADV_Pin(uint32_t NADV_Pin);
void AFIO_Config_EXTI_Filter(uint32_t EXTI_Filter);

void AFIO_Config_XSPI_XIP_BigEndian(uint32_t Endian);
void AFIO_Config_XSPI_HalfDuplexMode(uint32_t HalfDuplex);
void AFIO_Config_XSPI_DualQuadMode(uint32_t DualQuad);
void AFIO_Config_ETH_Mode(uint32_t ETH_Mode);
void AFIO_Config_XSPI_NSS_Input(uint32_t InputEnable, uint32_t InputPin);
void AFIO_Config_XSPI_RXDS_SampleDelay(uint32_t Delay);
void AFIO_Config_XSPI_BigEndian_Read(uint32_t Endian);
void AFIO_Config_XSPI_BigEndian_Write(uint32_t Endian);
void AFIO_Config_XSPI_TimeExtension(uint32_t Extension);
void AFIO_Config_XSPI_NSS_HighTime(uint32_t clk);

void AFIO_ConfigPinTol5V(GPIO_Module* GPIOx, uint32_t Pin,FunctionalState cmd);
void AFIO_ConfigPinFilter(GPIO_Module* GPIOx, uint32_t Pin,FunctionalState cmd);
void AFIO_Config_GPIOC_ReadRegDelay(FunctionalState cmd);
void AFIO_Config_SHRTIM_EXEV_Pin(uint8_t EXEV_Line, uint8_t PortSource, uint8_t PinSource);

void AFIO_Config_EMC_Funtion(uint32_t EMC_fun, FunctionalState cmd);
void AFIO_Set_EMC_Cnt(uint32_t cnt);
uint32_t AFIO_Get_EMC_Cnt(void);

#ifdef __cplusplus
}
#endif

#endif /* __N32H47X_48X_GPIO_H__ */

