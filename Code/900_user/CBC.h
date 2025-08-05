#ifndef _CBC_H
#define _CBC_H

#include "basic_data_type.h"


struct CBC_ELEMENT {
	UINT8 u8CBC_CHG_ErrFlag;	//����CBC������־λ
	UINT8 u8CBC_CHG_Cnt;		//���ֳ��CBC�Ĵ���
	UINT8 u8CBC_DSG_ErrFlag;	//����CBC������־λ
	UINT8 u8CBC_DSG_Cnt;		//���ַŵ�CBC�Ĵ���
};

extern struct CBC_ELEMENT CBC_Element;

// void InitCBC(NVIC_OnOFF NVIC_Status);
void App_CBC(void);

#endif	/* _CBC_H */

