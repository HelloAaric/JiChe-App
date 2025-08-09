#include "chgr_can.h"
#include "can.h"
#include "sys_timer.h"
#include "System_Monitor.h"
#include "pub_func.h"

CanTxMsg TxMessage;
CanRxMsg RxMessage;
volatile union Can_Status Can_Status_Flag;

void Init_ChgrCan(void)
{
    InitCan();

    TxMessage.TxHeader.ErrorState     = FDCAN_ESI_PASSIVE;
    TxMessage.TxHeader.BitRateSwitch  = FDCAN_BRS_OFF;
    TxMessage.TxHeader.FDFormat       = FDCAN_CLASSIC_CAN;
    TxMessage.TxHeader.TxEventFifo    = FDCAN_NO_TX_EVENTS;
    TxMessage.TxHeader.MsgMarker      = 0x55;

}

void App_ChgrCan(void)
{
    if(STARTUP_CONT == System_FUNC_StartUp(SYSTEM_FUNC_STARTUP_CAN)) {
        // return;
    }

    if(0 == g_st_SysTimeFlag.bits.b1Sys10msFlag2) {
        return;
    }

    if(Can_Status_Flag.bits.b1Can_Received == 1) {
        Can_Status_Flag.bits.b1Can_Received = 0;
        ChgrCan_ReceiveDeal();
    }

    ChgrCan_TransmitDeal();
}
