#ifndef _ADC_DEAL_H
#define _ADC_DEAL_H

#include "basic_data_type.h"

//AD��������ö��
enum tagInfoForADCArray
{
    ADC_TEMP_EV1,                     	// �ⲿ�¶�
    ADC_TEMP_EV2,                     	// �ⲿ�¶�
    ADC_CURR,                         	// ����
    ADC_EXT_3_3V,                     	//
    //ADC_VBC,                         	// ĸ�ߵ�ѹ
    //ADC_TEMP_MOS1,                    	// MOS2�¶�
    //ADC_TEMP_MOS2,                    	// MOS1�¶�
    //ADC_EXT_C1,                       	// ��1�ڵ�ص�ѹ
    //ADC_EXT_C2,                       	// ��2�ڵ�ص�ѹ

	ADC_NUM		                            // ADC number
};


extern INT32 gi32_ADCResult1[ADC_NUM];             	//ADC�������
extern UINT16 gu16_Small_DsgCur;
void InitADC(void);
void App_ADC(void);

#endif	/* _ADC_DEAL_H */
