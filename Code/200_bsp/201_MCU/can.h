#ifndef _CAN_H
#define _CAN_H

#include "basic_data_type.h"
#include "n32h47x_48x_fdcan.h"

typedef struct {
    FDCAN_TxHeaderType TxHeader;
    uint8_t TxData[8];
} CanTxMsg;

typedef struct {
    FDCAN_RxHeaderType RxHeader;
    uint8_t RxData[8];
} CanRxMsg;

typedef enum _EN_CAN_ID {
	ID_CAN1,
	ID_CAN2,
	ID_CAN3,
	ID_CAN_PORT_MAX_NUM,
}CAN_ID;


void InitCan(void);
void CanSendMsg(CAN_ID canID, CanTxMsg* pTxFrame);

#endif	/* _CAN_H */
