#ifndef _ADC_DEAL_H
#define _ADC_DEAL_H

#include "basic_data_type.h"

//AD采样变量枚举
enum tagInfoForADCArray
{
    ADC_TEMP_EV1,                     	// 外部温度
    ADC_TEMP_EV2,                     	// 外部温度
    ADC_CURR,                         	// 电流
    ADC_EXT_3_3V,                     	//
    //ADC_VBC,                         	// 母线电压
    //ADC_TEMP_MOS1,                    	// MOS2温度
    //ADC_TEMP_MOS2,                    	// MOS1温度
    //ADC_EXT_C1,                       	// 第1节电池电压
    //ADC_EXT_C2,                       	// 第2节电池电压

	ADC_NUM		                            // ADC number
};


extern INT32 gi32_ADCResult1[ADC_NUM];             	//ADC结果保存
extern UINT16 gu16_Small_DsgCur;
void InitADC(void);
void App_ADC(void);

#endif	/* _ADC_DEAL_H */
