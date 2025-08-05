#ifndef _CBC_H
#define _CBC_H

#include "basic_data_type.h"


struct CBC_ELEMENT {
	UINT8 u8CBC_CHG_ErrFlag;	//出现CBC保护标志位
	UINT8 u8CBC_CHG_Cnt;		//出现充电CBC的次数
	UINT8 u8CBC_DSG_ErrFlag;	//出现CBC保护标志位
	UINT8 u8CBC_DSG_Cnt;		//出现放电CBC的次数
};

extern struct CBC_ELEMENT CBC_Element;

// void InitCBC(NVIC_OnOFF NVIC_Status);
void App_CBC(void);

#endif	/* _CBC_H */

