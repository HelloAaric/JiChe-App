#ifndef _CAN_COM_H
#define _CAN_COM_H

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


extern volatile union Can_Status Can_Status_Flag;
extern CanRxMsg RxMessage;

void Init_CanCom(void);
void App_CanCom(void);

#endif	/* _CAN_COM_H */

