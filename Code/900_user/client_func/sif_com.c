#include "sif_com.h"
#include "basic_data_type.h"
#include "io_define.h"
#include "io_ctrl.h"

// 500us为一周期
#define SIF_VERSION                                 1

#define SIF_SYNC_LOW                                (1*2*20)
#define SIF_SYNC_HIGH                               (1*2*2)
#define SIF_SYNC_PERIOD                             (SIF_SYNC_LOW + SIF_SYNC_HIGH)

#define SIF_DATA_1_LOW                              (1*1)
#define SIF_DATA_1_HIGH                             (1*2)
#define SIF_DATA_1_PERIOD                           (SIF_DATA_1_LOW + SIF_DATA_1_HIGH)

#define SIF_DATA_0_LOW                              (1*2)
#define SIF_DATA_0_HIGH                             (1*1)
#define SIF_DATA_0_PERIOD                           (SIF_DATA_0_LOW + SIF_DATA_0_HIGH)

#define SIF_STOP_LOW                                (1*2*5)
#define SIF_STOP_HIGH                               (1*2*50)
#define SIF_STOP_PERIOD                             (SIF_STOP_LOW + SIF_STOP_HIGH)

typedef enum {
    STATE_IDLE,                         // 空闲状态
    STATE_SYNC,                         // 同步
    STATE_SEND_DATA,                    // 数据发送
    STATE_RECEIVE_DATA,                 // 数据接收
    STATE_STOP,                         // 停止
    STATE_ERROR,                        // 接收错误
} SIF_STATE;


typedef enum {
    SIF_RX_START,                       // 发现数据，开始接收
    SIF_RX_CONT,
    SIF_RX_OK,
    SIF_RX_ERROR,
} SIF_RX_STATE;

volatile SIF_FRAME gst_SifFrame;


UINT8 gu8_RxHigh_Tosc = 0;
UINT8 gu8_RxLow_Tosc = 0;

// bit的时间tosc不能太长，长于16，这里出错
UINT16 gu16_Data_IOfit = 0;
UINT8 gu8_Data_IOfit_Num = 0;

// 0和1对应的二进制拟合数
UINT16 SIF_1_VALUE = 0;
UINT16 SIF_0_VALUE = 0;

BOOL sif_tx_sync(void)
{
    static uint8_t sif_sync_tosc  = 0;

    ++sif_sync_tosc;
    if(sif_sync_tosc <= SIF_SYNC_LOW) {
        MCUO_SIF_COM = 0;
    } else if(sif_sync_tosc <= SIF_SYNC_PERIOD) {
        MCUO_SIF_COM = 1;
    }

    // 立刻结束
    if(sif_sync_tosc == SIF_SYNC_PERIOD) {
        sif_sync_tosc = 0;
        return TRUE;
    }

    return FALSE;
}

BOOL sif_tx_stop(void)
{
    static uint8_t sif_sync_tosc  = 0;

    ++sif_sync_tosc;
    if(sif_sync_tosc <= SIF_STOP_LOW) {
        MCUO_SIF_COM = 0;
    } else if(sif_sync_tosc <= SIF_STOP_PERIOD) {
        MCUO_SIF_COM = 1;
    }

    // 等于，立刻结束，必须要等于
    // if(sif_sync_tosc == SIF_STOP_PERIOD) {
    if(sif_sync_tosc >= SIF_STOP_PERIOD) {
        sif_sync_tosc = 0;
        return TRUE;
    }

    return FALSE;
}

BOOL sif_tx_sendBit(UINT8 bit)
{
    static uint8_t sif_bit_tosc  = 0;

    if(bit) {
        ++sif_bit_tosc;
        if(sif_bit_tosc <= SIF_DATA_1_LOW) {
            MCUO_SIF_COM = 0;
        } else if(sif_bit_tosc <= SIF_DATA_1_PERIOD) {
            MCUO_SIF_COM = 1;
        }

        if(sif_bit_tosc == SIF_DATA_1_PERIOD) {
            sif_bit_tosc = 0;
            return TRUE;
        }
    } else {
        ++sif_bit_tosc;
        if(sif_bit_tosc <= SIF_DATA_0_LOW) {
            MCUO_SIF_COM = 0;
        } else if(sif_bit_tosc <= SIF_DATA_0_PERIOD) {
            MCUO_SIF_COM = 1;
        }

        if(sif_bit_tosc == SIF_DATA_0_PERIOD) {
            sif_bit_tosc = 0;
            return TRUE;
        }
    }

    return FALSE;
}

BOOL sif_tx_sendData(void)
{
    static UINT8 su8_step = 0;
    static UINT8 su8_data = 0;
    static UINT8 su8_dataByte_Cnt = 0;
    static UINT8 su8_dataBit_Cnt = 0;

    // 下一次进来得立刻执行，不得拖延，这个时序很难搞。
    switch(su8_step) {
        case 0:
            su8_data = gst_SifFrame.u8_TxData[su8_dataByte_Cnt];
            su8_step = 1;
            su8_dataBit_Cnt = 0;
            break;

        case 1:
            // if(su8_dataBit_Cnt >= 8) {
            if(su8_dataBit_Cnt >= 7) {          // 要提前换数据
                // su8_dataBit_Cnt = 0;
                su8_step = 0;
                su8_dataByte_Cnt++;             // 下一个字节
            }
            break;

        default:
            break;
    }

    if(sif_tx_sendBit((su8_data>>7) & 0x01) == TRUE) {
        su8_data <<= 1;
        su8_dataBit_Cnt++;
    }

    // 发完15个字节
    if(su8_dataByte_Cnt >= gst_SifFrame.TxLength) {
        su8_data = 0;
        su8_step = 0;
        su8_dataBit_Cnt = 0;
        su8_dataByte_Cnt = 0;
        return TRUE;
    }

    return FALSE;
}

/*
1. 衔接必须立刻完成，不能有空挡。
2. 完整跑一轮。
3. 500us 时基
*/
void SIF_SendData_Handle(void)
{
    static SIF_STATE state = STATE_IDLE;
    
    switch(state) {
        case STATE_IDLE:
            Sif_DataOut();
            MCUO_SIF_COM = 0;
            if(gst_SifFrame.TxLength != 1 && gst_SifFrame.RxTxJudge == 1) {
                state = STATE_SYNC;
            }
            break;

        case STATE_SYNC:
            if(sif_tx_sync() == TRUE) {
                state = STATE_SEND_DATA;
            }
            break;

        case STATE_SEND_DATA:
            if(sif_tx_sendData() == TRUE) {
                state = STATE_STOP;
            }
            break;

        case STATE_STOP:
            if(sif_tx_stop() == TRUE) {
                state = STATE_IDLE;
                gst_SifFrame.TxLength = 0;
                gst_SifFrame.RxTxJudge = 0;
                Sif_DataIn();
            }
            break;

        default:
            break;
    }
}


SIF_RX_STATE sif_rx_sync(void)
{
    static UINT8 su8_startUp_Flag = 0;

    switch(su8_startUp_Flag) {
        case 0:
            // 等待第一个下降沿到来
            if(gu8_RxLow_Tosc) {
                gu8_RxHigh_Tosc = 0;
               su8_startUp_Flag = 1;
                return SIF_RX_START;
            }
            break;
        case 1:
            // 期间出现高电平。直接退出。
            if(0 != gu8_RxHigh_Tosc) {
                return SIF_RX_ERROR;
            }

            if(gu8_RxLow_Tosc >= SIF_SYNC_LOW) {
                su8_startUp_Flag = 2;
            }
            break;

        case 2:
            // 低电平时间太长，报错
            if(gu8_RxLow_Tosc >= SIF_SYNC_LOW + 10) {
                return SIF_RX_ERROR;
            }

            // 等待第一个上升沿到来
            if(gu8_RxHigh_Tosc) {
                gu8_RxLow_Tosc = 0;
                su8_startUp_Flag = 3;
            }
            break;

        case 3:
            // 期间出现低电平。直接退出。
            if(0 != gu8_RxLow_Tosc) {
                return SIF_RX_ERROR;
            }

            // 高电平时间够了，立刻结束
            if(gu8_RxHigh_Tosc >= SIF_SYNC_HIGH) {
                su8_startUp_Flag = 4;
                return SIF_RX_OK;
            }
            break;

        default:
            break;
    }

    return SIF_RX_CONT;
}


SIF_RX_STATE sif_rx_receiveBit(UINT8* bit)
{
    // 0和1的周期是一样的。
    if(gu8_Data_IOfit_Num >= SIF_DATA_1_PERIOD) {
        if(gu16_Data_IOfit == SIF_1_VALUE) {
            bit = 1;
            return SIF_RX_OK;
        } else if(gu16_Data_IOfit == SIF_0_VALUE) {
            bit = 0;
            return SIF_RX_OK;
        } else {
            return SIF_RX_ERROR;
        }
    }

    return SIF_RX_CONT;
}


SIF_RX_STATE sif_rx_receiveData(void)
{
    SIF_RX_STATE result = SIF_RX_CONT;

    static UINT8 su8_data_temp = 0;
    static UINT8 su8_dataByte_Cnt = 0;
    static UINT8 su8_dataBit_Cnt = 0;

    UINT8 u8_bit = 0;

    result = sif_rx_receiveBit(&u8_bit);
    if(result == SIF_RX_OK) {
        su8_data_temp = su8_data_temp << 1;
        su8_data_temp |= u8_bit;
        ++su8_dataBit_Cnt;
        gu16_Data_IOfit = 0;
        gu8_Data_IOfit_Num = 0;
    } else if(result == SIF_RX_ERROR) {
        // 只要有一个出问题，就全部舍弃。
        gu16_Data_IOfit = 0;
        gu8_Data_IOfit_Num = 0;
        return SIF_RX_ERROR;
    }

    if(su8_dataBit_Cnt >= 8) {
        su8_dataBit_Cnt = 0;
        gst_SifFrame.u8_RxData[su8_dataByte_Cnt++] = su8_data_temp;
        su8_data_temp = 0;
    }

    if(su8_dataByte_Cnt >= gst_SifFrame.RxLength) {
        su8_dataByte_Cnt = 0;
        // gst_SifFrame.RxLength = 0;                  // 不在这里清空，还没完
        return SIF_RX_OK;
    }

    return SIF_RX_CONT;
}

SIF_RX_STATE sif_rx_stop(void)
{

}


void SIF_ReceiveData_Handle(void)
{
    static SIF_STATE state = STATE_IDLE;

    SIF_RX_STATE result = SIF_RX_CONT;
    // 默认就是接收模式
    if(MCUI_SIF_COM) {
        gu8_RxHigh_Tosc++;
        gu16_Data_IOfit = (gu16_Data_IOfit << 1) | 1;
        gu8_Data_IOfit_Num++;
    } else {
        gu8_RxLow_Tosc++;
        gu16_Data_IOfit = (gu16_Data_IOfit << 1) | 0;
        gu8_Data_IOfit_Num++;
    }

    switch(state) {
        case STATE_IDLE:
            if(gu8_RxHigh_Tosc > 0) {
                gu8_RxLow_Tosc = 0;
                state = STATE_SYNC;
            }
            gst_SifFrame.sif_status = 0;
            break;

        case STATE_SYNC:
            result = sif_rx_sync();
            if(result == SIF_RX_START) {
                // 宣告开始接收数据
                gst_SifFrame.sif_status = 2;
                // TODO，写到这里，加了两个标志位，后续完善规则efgsrgdrgdrghdtfghb
            } else if(result == SIF_RX_OK) {
                // 立刻开始下一步准备工作
                gu16_Data_IOfit = 0;
                gu8_Data_IOfit_Num = 0;
                state = STATE_RECEIVE_DATA;
            } else if(result == SIF_RX_ERROR) {
                state = STATE_ERROR;
            }
            break;

        case STATE_RECEIVE_DATA:
            result = sif_rx_receiveData();
            if(result == SIF_RX_OK) {
                state = STATE_STOP;
            } else if(result == SIF_RX_ERROR) {
                state = STATE_ERROR;
            }
            break;

        case STATE_STOP:
            result = sif_rx_stop();
            if(result == SIF_RX_OK) {
                state = STATE_IDLE;
                gst_SifFrame.RxTxJudge = 0;
                Sif_DataIn();
            } else if(result == SIF_RX_ERROR) {
                state = STATE_ERROR;
            }
            break;

        case STATE_ERROR:
            Sif_DataIn();
            gst_SifFrame.RxTxJudge = 0;
            gst_SifFrame.dealResult = 0;
            state = STATE_IDLE;
            break;

        default:
            break;
    }
}

void SIF_Init(void)
{
    UINT16 i = 0;

    for(i = 0; i < SIF_DATA_1_LOW; ++i) {
        SIF_1_VALUE = (SIF_1_VALUE << 1) | 0;
    }
    for(i = 0; i < SIF_DATA_1_HIGH; ++i) {
        SIF_1_VALUE = (SIF_1_VALUE << 1) | 1;
    }

    for(i = 0; i < SIF_DATA_0_LOW; ++i) {
        SIF_0_VALUE = (SIF_0_VALUE << 1) | 0;
    }
    for(i = 0; i < SIF_DATA_0_HIGH; ++i) {
        SIF_0_VALUE = (SIF_0_VALUE << 1) | 1;
    }
}

void SIF_Handle(void)
{
    static UINT8 su8_startUp_Flag = 0;

    // 开机初始化
    if(su8_startUp_Flag == 0) {
        su8_startUp_Flag = 1;
        SIF_Init();
    }

    if(gst_SifFrame.RxTxJudge == 1) {
        SIF_SendData_Handle();
    } else {
        SIF_ReceiveData_Handle();
    }
}


#if 0
static void sif_send_data_handle(uint8_t state)
{
 
    switch(state)
    {
        case STATE_SYNC:                //同步模式
            if (sif_sync_tosc < SIF_SYNC - 2) {
                sif_turn_off();
            } else {
                sif_turn_on();
            }
            sif_sync_tosc++;
            if (sif_sync_tosc >= SIF_SYNC)
            {
                state_mode = SEND_DATA;
                sif_sync_tosc = 0;
                bit_cnt = 7;
                byte_cnt = 0;
                sif_send_tosc = 0;
            }
            break;
        case SEND_DATA:    //发送数据 
            static uint8_t res;            
            uint8_t count = SIF_SEND_COUNT;
            static uint8_t nums = sizeof(uint8_t) * 8;
            uint8_t *p = (uint8_t *)result;
            sif_send_tosc = sif_send_tosc % count;
 
            uint8_t data = (p[byte_cnt] >> bit_cnt) & 0x1;
            
            if (data)
            {
                if (sif_send_tosc == 0)
                {
                    sif_turn_off();
                    sif_send_tosc ++;
                }
                else if (sif_send_tosc == 1)
                {
                    sif_turn_on();
                    sif_send_tosc ++;
                } else {
                    sif_send_tosc = 0;
                }
            }
            else
            {
                if (sif_send_tosc == 0)
                {
                    sif_turn_off();
                    sif_send_tosc++;
                }
                else if (sif_send_tosc == 2)
                {
                    sif_turn_on();
                    sif_send_tosc = 0;
                } else
                {
                    sif_send_tosc++;
                }
            }
            if (sif_send_tosc == 0)
            {
                if (--bit_cnt < 0)
                {
                    byte_cnt++;
                    bit_cnt = 7;
                }
                if (byte_cnt >= length)
                {
                    state_mode = SEND_DATA_COMPLETE;
                    break;
                }
            }
            break;
        case SEND_DATA_COMPLETE://数据发送完成，将标志位清0
            state_mode = 0;
            sif_turn_off();
            length = 0;
            memset(result, 0, sizeof(result));
            break;
        default:
            break;
        }
}
#endif
