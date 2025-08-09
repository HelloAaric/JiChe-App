#ifndef _CHGR_CAN_H
#define _CHGR_CAN_H

#include "basic_data_type.h"
#include "can.h"

union Can_Status {
    UINT8   all;
    struct Can_StatusBit {
		UINT8 b1Can_BusOFF			:1;
		UINT8 b1Can_Received 		:1;
		UINT8 b1Can_Send            :1;
		UINT8 b1Can_Fault           :1;

		UINT8 b1Can_BusOFF_TestSd	:1;
		UINT8 b1Rcved				:3;
     }bits;
};

// 协议帧ID定义
#define CIM1_FRAME_ID     0x18C8F456
#define CIM2_FRAME_ID     0x18C9F456
#define CRM_FRAME_ID      0x18CAF456
#define CML_FRAME_ID      0x18CBF456
#define CRO_FRAME_ID      0x18CCF456
#define CCS_FRAME_ID      0x18CDF456
#define CST_FRAME_ID      0x18CEF456
#define CSD_FRAME_ID      0x18CFF456
#define BIM1_FRAME_ID     0x186456F4
#define BIM2_FRAME_ID     0x186556F4
#define BRM_FRAME_ID      0x186656F4
#define BCP_FRAME_ID      0x186756F4
#define BRO_FRAME_ID      0x186856F4
#define BCL_FRAME_ID      0x186956F4
#define BCS_FRAME_ID      0x186A56F4
#define BST_FRAME_ID      0x186B56F4
#define BSD_FRAME_ID      0x186C56F4

// 充电阶段枚举
typedef enum {
    CHARGE_PHASE_IDLE = 0,
    CHARGE_PHASE_HANDSHAKE,
    CHARGE_PHASE_CONFIG,
    CHARGE_PHASE_CHARGING,
    CHARGE_PHASE_FINISH,
    CHARGE_PHASE_ERROR
} Charge_Phase_t;

extern volatile union Can_Status Can_Status_Flag;
extern CanRxMsg RxMessage;

void App_ChgrCan(void);
void Init_ChgrCan(void);

#endif	/* _CHGR_CAN_H */
