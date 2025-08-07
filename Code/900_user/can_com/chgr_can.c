#include "chgr_can.h"
#include "can.h"
#include "sys_timer.h"
#include "System_Monitor.h"
#include "pub_func.h"

CanTxMsg TxMessage;
CanRxMsg RxMessage;
volatile union Can_Status Can_Status_Flag;

// 充电状态变量
Chgr_StateType g_ChgrState = CHGR_STATE_IDLE;
uint32_t g_ChgrStateTimer = 0;
uint8_t g_HandshakeSuccess = 0;

// 充电机参数
uint16_t g_ChgrMaxVolt = 0;
uint16_t g_ChgrMinVolt = 0;
uint16_t g_ChgrMaxCurr = 0;
uint16_t g_ChgrMinCurr = 0;

// BMS参数
uint8_t g_BmsBatteryType = 0;
uint16_t g_BmsVolt = 0;
uint16_t g_BmsCapacity = 0;
uint8_t g_BmsVendor = 0;

void Init_ChgrCan(void)
{
    InitCan();

    TxMessage.TxHeader.ErrorState     = FDCAN_ESI_PASSIVE;
    TxMessage.TxHeader.BitRateSwitch  = FDCAN_BRS_OFF;
    TxMessage.TxHeader.FDFormat       = FDCAN_CLASSIC_CAN;
    TxMessage.TxHeader.TxEventFifo    = FDCAN_NO_TX_EVENTS;
    TxMessage.TxHeader.MsgMarker      = 0x55;
}

void Chgr_SendMessage(uint32_t frameId, uint8_t* data, uint8_t len)
{
	TxMessage.TxHeader.IdType = FDCAN_EXTENDED_ID;
    TxMessage.TxHeader.ID = frameId;
    TxMessage.TxHeader.DataLength = len;
    memcpy(TxMessage.TxData, data, len);
    CanSendMsg(ID_CAN1, &TxMessage);
}

void Chgr_ProcessReceivedMessage(CanRxMsg* rxMsg)
{
    uint8_t* data = rxMsg->RxData;
    uint32_t frameId = rxMsg->RxHeader.ID;

    switch(frameId)
    {
        case BIM1_FRAME_ID:
            // 解析BMS编号报文1
            g_BmsBatteryType = data[0];
            g_BmsVolt = (data[1] << 8) | data[0];
            g_BmsCapacity = (data[3] << 8) | data[2];
            g_BmsVendor = data[5];

            // 发送充电机辨识结果
            if(g_ChgrState == CHGR_STATE_HANDSHAKE)
            {
                uint8_t crmData[8] = {0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
                Chgr_SendMessage(CRM_FRAME_ID, crmData, 8);
                g_HandshakeSuccess = 1;
                g_ChgrState = CHGR_STATE_CONFIG;
                g_ChgrStateTimer = GetSysTick();
            }
            break;

        case BCP_FRAME_ID:
            // 解析电池充电参数
            if(g_ChgrState == CHGR_STATE_CONFIG)
            {
                // 发送充电机准备就绪
                uint8_t croData[8] = {0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
                Chgr_SendMessage(CRO_FRAME_ID, croData, 8);
                g_ChgrState = CHGR_STATE_CHARGING;
                g_ChgrStateTimer = GetSysTick();
            }
            break;

        case BCL_FRAME_ID:
            // 处理电池充电需求
            if(g_ChgrState == CHGR_STATE_CHARGING)
            {
                uint16_t reqVolt = (data[1] << 8) | data[0];
                uint16_t reqCurr = (data[3] << 8) | data[2];
                uint8_t constVoltStage = data[4];

                // 这里应该根据需求调整充电机输出
                // 然后发送充电机充电状态
                uint8_t ccsData[8] = {0};
                ccsData[0] = (reqVolt >> 8) & 0xFF;
                ccsData[1] = reqVolt & 0xFF;
                ccsData[2] = (reqCurr >> 8) & 0xFF;
                ccsData[3] = reqCurr & 0xFF;
                // 填充其他数据字段...
                Chgr_SendMessage(CCS_FRAME_ID, ccsData, 8);
            }
            break;

        case BST_FRAME_ID:
            // 处理BMS终止充电
            if(g_ChgrState == CHGR_STATE_CHARGING)
            {
                uint8_t reason = data[0];
                uint8_t faultCode = data[1];

                // 发送充电机终止充电
                uint8_t cstData[8] = {0};
                cstData[0] = 0x80;  // BMS主动终止
                cstData[1] = faultCode;
                Chgr_SendMessage(CST_FRAME_ID, cstData, 8);

                // 发送充电机统计数据
                uint8_t csdData[8] = {0};
                // 填充统计数据...
                Chgr_SendMessage(CSD_FRAME_ID, csdData, 8);

                g_ChgrState = CHGR_STATE_FINISH;
                g_ChgrStateTimer = GetSysTick();
            }
            break;

        // 其他帧类型的处理...
        default:
            break;
    }
}

void App_ChgrCan(void)
{
    if(STARTUP_CONT == System_FUNC_StartUp(SYSTEM_FUNC_STARTUP_CAN)) {
        // return;
    }

    if(0 == g_st_SysTimeFlag.bits.b1Sys10msFlag2) {
        return;
    }

    // 检查是否有CAN消息接收
	if(Can_Status_Flag.bits.b1Can_Received) {
		Chgr_ProcessReceivedMessage(&RxMessage);
		Can_Status_Flag.bits.b1Can_Received = 0;
	}

    // 状态机处理
    switch(g_ChgrState)
    {
        case CHGR_STATE_IDLE:
            // 初始状态，等待开始充电
            g_ChgrState = CHGR_STATE_HANDSHAKE;
            g_ChgrStateTimer = GetSysTick();
            break;

        case CHGR_STATE_HANDSHAKE:
            // 握手阶段
            if(CheckTimeOut(g_ChgrStateTimer, 5000))  // 5秒超时
            {
                // 发送充电机编号报文1和2
                uint8_t cim1Data[8] = {0x02, 0x02, 0x64, 0x00, 0x64, 0x00, 0x02, 0x00};
                Chgr_SendMessage(CIM1_FRAME_ID, cim1Data, 8);

                uint8_t cim2Data[8] = {0x07, 0xE3, 0x0A, 0x1C, 0x00, 0x02, 0x00, 0x00};
                Chgr_SendMessage(CIM2_FRAME_ID, cim2Data, 8);

                g_ChgrStateTimer = GetSysTick();
            }
            break;

        case CHGR_STATE_CONFIG:
            // 配置阶段
            if(CheckTimeOut(g_ChgrStateTimer, 5000))  // 5秒超时
            {
                // 发送充电机最大输出能力
                uint8_t cmlData[8] = {0x00, 0x78, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00};
                Chgr_SendMessage(CML_FRAME_ID, cmlData, 8);
                g_ChgrStateTimer = GetSysTick();
            }
            break;

        case CHGR_STATE_CHARGING:
            // 充电阶段
            if(CheckTimeOut(g_ChgrStateTimer, 250))  // 250ms周期
            {
                // 周期性发送充电机充电状态
                // 这里应该获取实际的电压和电流值
                uint8_t ccsData[8] = {0x00, 0x78, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00};
                Chgr_SendMessage(CCS_FRAME_ID, ccsData, 8);
                g_ChgrStateTimer = GetSysTick();
            }
            break;

        case CHGR_STATE_FINISH:
            // 充电结束
            if(CheckTimeOut(g_ChgrStateTimer, 5000))  // 5秒后回到空闲状态
            {
                g_ChgrState = CHGR_STATE_IDLE;
            }
            break;

        case CHGR_STATE_ERROR:
            // 错误处理
            if(CheckTimeOut(g_ChgrStateTimer, 5000))  // 5秒后重试
            {
                g_ChgrState = CHGR_STATE_IDLE;
            }
            break;

        default:
            break;
    }
}
