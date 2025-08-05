#include "can.h"
#include "misc.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_rcc.h"
#include "io_define.h"

#if 1
#define TEST_BUF_SIZE                               (32)
#define TEST_FRAME_NUMBER                           (2)

uint32_t Node1_ram[0x100];
uint32_t Node2_ram[0x100];

FDCAN_MsgRamType Node1_msg;
FDCAN_MsgRamType Node2_msg;

FDCAN_TxHeaderType TxHeader_Node1;
FDCAN_TxHeaderType TxHeader_Node2;

FDCAN_RxHeaderType RxHeader_Node1;
FDCAN_RxHeaderType RxHeader_Node2;


// 任意端口都能做can通讯
void InitNode1_GPIO(void)
{
    GPIO_InitType InitStruct;

    InitStruct.Pin            = NODE1_TX_PIN;
    InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    InitStruct.GPIO_Mode      = GPIO_MODE_AF_PP;
    InitStruct.GPIO_Alternate = NODE1_TX_PIN_AF;
    InitStruct.GPIO_Pull      = GPIO_NO_PULL;
    InitStruct.GPIO_Current   = GPIO_DS_8mA;
    GPIO_InitPeripheral(NODE1_TX_PORT,&InitStruct);

    InitStruct.Pin            = NODE1_RX_PIN;
    InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    InitStruct.GPIO_Mode      = GPIO_MODE_INPUT;
    InitStruct.GPIO_Alternate = NODE1_RX_PIN_AF;
    InitStruct.GPIO_Pull      = GPIO_PULL_UP;
    InitStruct.GPIO_Current   = GPIO_DS_8mA;
    GPIO_InitPeripheral(NODE1_RX_PORT,&InitStruct);
}

void InitNode1_Mode(void)
{
    FDCAN_InitType InitParam;
        
    // 使能时钟选择和分频4
    RCC_ConfigFDCANPllClk(RCC_FDCAN_PLLSRC_DIV4);
    /* Select PLL as FDCAN clock source */
    RCC_ConfigFDCANClksrc(RCC_FDCAN_CLKSRC_PLL);

    /* Enable NODE1 clock */
    RCC_EnableAPB1PeriphClk(NODE1_PERIPH,ENABLE);

    /* Reset NODE1 register */
    RCC_EnableAPB1PeriphReset(NODE1_PERIPH);

    /** FDCAN config parameter **/
    /*
        目前250K
        48MHz/4 = 12MHz
        12/(36+11+1) = 12/48 = 0.250MHz = 250K
    */
    InitParam.FrameFormat           = FDCAN_FRAME_CLASSIC;          /* Frame format */
    InitParam.Mode                  = FDCAN_MODE_NORMAL;            /* Work mode */
    InitParam.Prescaler             = 1;                            /* Nominal timing  */
    InitParam.SyncJumpWidth         = 8;
    InitParam.TimeSeg1              = 36;
    InitParam.TimeSeg2              = 11;
    InitParam.MsgRamStrAddr         = (uint32_t)Node1_ram;          /* Msg ram start address, shared by all FDCAN modules */
    InitParam.MsgRamOffset          = 0;                            /* Current NODE1 msg ram start offset  */
    InitParam.pMsgInfo              = &Node1_msg;
    InitParam.StdFilterSize         = 32;                           /* Standard filter list */
    InitParam.ExtFilterSize         = 0;                            /* Extended filter list */
    InitParam.RxFifo0Size           = 32;                           /* Rx FIFO 0*/
    InitParam.RxFifo0DataSize       = FDCAN_DATA_BYTES_8;
    InitParam.RxFifo1Size           = 0;                            /* Rx FIFO 1*/
    InitParam.RxFifo1DataSize       = FDCAN_DATA_BYTES_8;
    InitParam.RxBufferSize          = 0;                            /* Dedicate Rx buffer */
    InitParam.RxBufferDataSize      = FDCAN_DATA_BYTES_8;
    InitParam.TxBufferSize          = 0;                            /* Tx buffer*/
    InitParam.TxBufferDataSize      = FDCAN_DATA_BYTES_8;
    InitParam.TxFifoQueueSize       = 16;                           /* Tx FIFO */
    InitParam.TxFifoQueueMode       = FDCAN_TX_FIFO_MODE;
    InitParam.TxEventSize           = 0;                            /* Tx event fifo */
    InitParam.AutoRetransmission    = ENABLE;                       /* Enable auto retransmission */
    InitParam.TransmitPause         = DISABLE;                      /* Disable transmit pause*/
    InitParam.ProtocolException     = ENABLE;                       /* Enable Protocol Exception Handling */

    /* Init NODE1 */
    FDCAN_Init(NODE1, &InitParam);
}

void InitNode1_Filter(void)
{
    FDCAN_FilterType FilterParam;

    /* Configure standard ID reception filter to Rx buffer 0 */
    FilterParam.IdType          = FDCAN_STANDARD_ID;
    FilterParam.FilterIndex     = 0;
    FilterParam.FilterType      = FDCAN_FILTER_RANGE;
    FilterParam.FilterConfig    = FDCAN_FILTER_TO_RXFIFO0;
    FilterParam.FilterID1       = 0x080;
    FilterParam.FilterID2       = 0x09F;
    FDCAN_ConfigFilter(NODE1,&FilterParam);
}

void InitNode1_NVIC(void)
{
	NVIC_InitType NVIC_Struct;

    NVIC_Struct.NVIC_IRQChannel = NODE1_IRQN;
    NVIC_Struct.NVIC_IRQChannelSubPriority = NVIC_SUB_PRIORITY_0;
    NVIC_Struct.NVIC_IRQChannelPreemptionPriority = NVIC_PRE_PRIORITY_0;
    NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_Struct);
}


void InitNode1(void)
{
	InitNode1_GPIO();
	InitNode1_Mode();
	InitNode1_Filter();
    InitNode1_NVIC();

    FDCAN_ConfigGlobalFilter(NODE1,
                                FDCAN_REJECT_STD,
                                FDCAN_REJECT_EXT,
                                FDCAN_FILTER_STD_REMOTE,
                                FDCAN_FILTER_EXT_REMOTE);

    // 这两句话是干啥的，能去掉吗
    FDCAN_ConfigTSPrescaler(NODE1, FDCAN_TIMESTAMP_PRESC_16);
    FDCAN_Config_TS(NODE1, FDCAN_TIMESTAMP_INTERNAL);

    #if 0
    // 水位，满3个触发中断的意思？
    FDCAN_ConfigFifoWatermark(NODE1, FDCAN_RX_FIFO0, 3);
    FDCAN_ConfigIntLine(NODE1, FDCAN_FLAG_RX_FIFO0_WATERMARK, FDCAN_INTERRUPT_LINE0);
    FDCAN_ActivateInt(NODE1, FDCAN_FLAG_RX_FIFO0_WATERMARK, 0);
    #endif

    #if 1
    FDCAN_ConfigIntLine(NODE1, FDCAN_FLAG_RX_FIFO0_NEW_MESSAGE, FDCAN_INTERRUPT_LINE0);
    FDCAN_ActivateInt(NODE1, FDCAN_FLAG_RX_FIFO0_NEW_MESSAGE, 0);
    #endif

    /* Start the FDCAN module */
    FDCAN_Start(NODE1);
}


void InitNode2(void)
{

}

void InitCan(void)
{
    InitNode1();
    InitNode2();
}

void CanSendMsg(CAN_ID canID, CanTxMsg* pTxFrame)
{
    switch(canID) {
        case ID_CAN1:
            FDCAN_AddMsgToTxFifoQ(NODE1, &(pTxFrame->TxHeader), pTxFrame->TxData);
            break;
        case ID_CAN2:
            FDCAN_AddMsgToTxFifoQ(NODE2, &(pTxFrame->TxHeader), pTxFrame->TxData);
            break;
        case ID_CAN3:
            break;
        default:
            break;
    }
}
#endif

