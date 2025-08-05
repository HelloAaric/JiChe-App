#ifndef _CAN_CHGR_H
#define _CAN_CHGR_H

#include "can.h"
#include "can_com.h"

extern volatile union Can_Status Can_Status_Flag;
extern CanRxMsg RxMessage;

void Init_CanCom(void);
void App_CanCom(void);

#endif	/* _CAN_CHGR_COM_H */

