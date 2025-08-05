#include "adc_deal.h"

#include "n32h47x_48x_adc.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_tim.h"
#include "n32h47x_48x_dma.h"
#include "n32h47x_48x_gpio.h"

#include "io_define.h"
#include "System_Monitor.h"
#include "sys_timer.h"
#include "pub_func.h"

#if 1
#define LENGTH_TBLTEMP_PORT_10K    ((UINT16)56)
//12位，4096最大
const UINT16 iSheldTemp_10K_old[LENGTH_TBLTEMP_PORT_10K] = {
    //AD		(Temp+40)*10
    3860	,	100	,	//-30
    3786	,	150	,	//-25
    3693	,	200	,	//-20
    3582	,	250	,	//-15
    3450	,	300	,	//-10
    3296	,	350	,	//-5
    3122	,	400	,	//0
    2929	,	450	,	//5
    2721	,	500	,	//10
    2500	,	550	,	//15
    2275	,	600	,	//20
    2048	,	650	,	//25
    1828	,	700	,	//30
    1617    ,	750	,	//35
    1421    ,	800	,	//40
    1240    ,	850	,	//45
    1078    ,	900	,	//50
    933 	,	950	,	//55
    806 	,	1000,	//60
    697 	,	1050,	//65
    601 	,	1100,	//70
    519 	,	1150,	//75
    445 	,	1200,	//80
    386 	,	1250,	//85
    335 	,	1300,	//90
    289 	,	1350,	//95
    250 	,	1400,	//100
    217 	,	1450,	//105
};


//12位，4096最大
const UINT16 iSheldTemp_10K_old2[LENGTH_TBLTEMP_PORT_10K] = {
    //AD		(Temp+40)*10
    3498	,	100	,	//-30
    3431	,	150	,	//-25
    3350	,	200	,	//-20
    3252	,	250	,	//-15
    3135	,	300	,	//-10
    2998	,	350	,	//-5
    2839	,	400	,	//0
    2664    ,	450	,	//5
    2473	,	500	,	//10
    2272	,	550	,	//15
    2066	,	600	,	//20
    1862	,	650	,	//25
    1660	,	700	,	//30
    1470    ,	750	,	//35
    1293    ,	800	,	//40
    1130    ,	850	,	//45
    983    ,	900	,	//50
    852 	,	950	,	//55
    737 	,	1000,	//60
    636 	,	1050,	//65
    548 	,	1100,	//70
    472 	,	1150,	//75
    407 	,	1200,	//80
    352 	,	1250,	//85
    304 	,	1300,	//90
    264 	,	1350,	//95
    230 	,	1400,	//100
    200 	,	1450,	//105
};


const UINT16 iSheldTemp_10K[LENGTH_TBLTEMP_PORT_10K] = {
    //AD		(Temp+40)*10
    3422	,	100	,	//-30
    3342	,	150	,	//-25
    3248	,	200	,	//-20
    3139	,	250	,	//-15
    3013	,	300	,	//-10
    2875	,	350	,	//-5
    2723	,	400	,	//0
    2561    ,	450	,	//5
    2390	,	500	,	//10
    2215	,	550	,	//15
    2037,	600	,	//20
    1862	,	650	,	//25
    1691	,	700	,	//30
    1527    ,	750	,	//35
    1372    ,	800	,	//40
    1227    ,	850	,	//45
    1094    ,	900	,	//50
    973 	,	950	,	//55
    862 	,	1000,	//60
    765 	,	1050,	//65
    678 	,	1100,	//70
    601 	,	1150,	//75
    533 	,	1200,	//80
    472 	,	1250,	//85
    419 	,	1300,	//90
    372 	,	1350,	//95
    330 	,	1400,	//100
    294 	,	1450,	//105
};

__IO UINT16	gu16_ADCValFilter1[ADC_NUM];
INT32 gu32_ADCValFilterNext1[ADC_NUM];          //ADC数据缓存2，问题解决了，原来是UINT32，在计算过程出错了！
INT32 gi32_ADCResult1[ADC_NUM];             	//ADC结果保存

__IO UINT16	gu16_ADCValFilter2[ADC_NUM];
INT32 gu32_ADCValFilterNext2[ADC_NUM];          //ADC数据缓存2
INT32 gi32_ADCResult2[ADC_NUM];             	//ADC结果保存

void InitADC1_GPIO(void)
{
    GPIO_InitType GPIO_InitStructure;
    
    GPIO_InitStruct(&GPIO_InitStructure);
    /* Configure PA.01  as analog input ----------------------------------------*/
    GPIO_InitStructure.Pin       = GPIO_PIN_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_ANALOG;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
}

void InitADC1_TIMER(void)
{
    TIM_TimeBaseInitType TIM_TimeBaseStructure;
    OCInitType TIM_OCInitStructure;
    /* GTIM1 configuration ------------------------------------------------------*/
    /* Time base configuration */

    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_GTIM1, ENABLE);

    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.Period    = 1000 - 1;
    TIM_TimeBaseStructure.Prescaler = 48 - 1;
    TIM_TimeBaseStructure.ClkDiv    = TIM_CLK_DIV1;
    TIM_TimeBaseStructure.CounterMode   = TIM_CNT_MODE_UP;
    TIM_InitTimeBase(GTIM1, &TIM_TimeBaseStructure);
    /* PWM1 Mode configuration: Channel1 */
    TIM_InitOcStruct(&TIM_OCInitStructure);
    TIM_OCInitStructure.OCMode      = TIM_OCMODE_PWM1;
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.Pulse       = 500 - 1;
    TIM_OCInitStructure.OCPolarity  = TIM_OC_POLARITY_HIGH;
    // TIM_InitOc1(GTIM1, &TIM_OCInitStructure);
    // TIM_ConfigOc1Preload(GTIM1, TIM_OC_PRE_LOAD_ENABLE);
    
    /* PWM1 Mode configuration: Channel4 */
    TIM_InitOc4(GTIM1, &TIM_OCInitStructure);
    TIM_ConfigOc4Preload(GTIM1, TIM_OC_PRE_LOAD_ENABLE);

    TIM_ConfigArPreload(GTIM1, ENABLE);

    TIM_Enable(GTIM1, ENABLE);

}

DMA_InitType DMA_InitStructure;
void InitADC1_DMA(void)
{
    // DMA_InitType DMA_InitStructure;
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_DMA1, ENABLE);

    DMA_DeInit(DMA1_CH1);
    DMA_InitStructure.PeriphAddr     = (uint32_t)&ADC1->DAT;
    DMA_InitStructure.MemAddr        = (uint32_t)(&gu16_ADCValFilter1[0]);
    DMA_InitStructure.Direction      = DMA_DIR_PERIPH_SRC;
    DMA_InitStructure.BufSize        = 1;
    DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_DISABLE;
    // DMA_InitStructure.MemoryInc      = DMA_MEM_INC_ENABLE;
    DMA_InitStructure.MemoryInc      = DMA_MEM_INC_DISABLE;
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_WIDTH_HALFWORD;
    DMA_InitStructure.MemDataSize    = DMA_MEM_DATA_WIDTH_HALFWORD;
    DMA_InitStructure.CircularMode   = DMA_MODE_CIRCULAR;
    DMA_InitStructure.Priority       = DMA_PRIORITY_HIGH;
    DMA_InitStructure.Mem2Mem        = DMA_M2M_DISABLE;
    // DMA_InitStructure.Mem2Mem        = DMA_M2M_ENABLE;
    DMA_Init(DMA1_CH1, &DMA_InitStructure);
    
    DMA_RequestRemap(DMA_REMAP_ADC1, DMA1_CH1, ENABLE);
    /* Enable DMA1 channel1 */
    DMA_EnableChannel(DMA1_CH1, ENABLE);
}


ADC_InitType ADC_InitStructure;
void InitADC_ADC1(void)
{
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_ADC1,ENABLE);

    /* ADC1 configuration ------------------------------------------------------*/
    ADC_InitStructure.WorkMode       = ADC_WORKMODE_INDEPENDENT;
    ADC_InitStructure.MultiChEn      = DISABLE;
    ADC_InitStructure.ContinueConvEn = DISABLE;
    ADC_InitStructure.ExtTrigSelect  = ADC_EXT_TRIG_REG_CONV_GTIM1_CC4;
    ADC_InitStructure.DatAlign       = ADC_DAT_ALIGN_R;
    ADC_InitStructure.ChsNumber      = 1;
    ADC_InitStructure.Resolution     = ADC_DATA_RES_12BIT;
    ADC_Init(ADC1, &ADC_InitStructure);
    /* ADC1 regular channel4 configuration */
    ADC_ConfigRegularChannel(ADC1, ADC1_Channel_02_PA3, 1, ADC_SAMP_TIME_CYCLES_13_5);
    /* Set injected sequencer length */
    ADC_ConfigInjectedSequencerLength(ADC1, 1);

    /* Enable automatic injected conversion start after regular one */
    ADC_EnableAutoInjectedConv(ADC1, ENABLE);

    /* Enable JEOC interrupt */
    ADC_ConfigInt(ADC1, ADC_INT_JENDC, ENABLE);
    
    /* Enable ADC1 DMA */
    ADC_SetDMATransferMode(ADC1, ADC_MULTI_REG_DMA_EACH_ADC);

    /* Enable ADC1 */
    ADC_Enable(ADC1, ENABLE);
    /*Check ADC Ready*/
    while(ADC_GetFlagStatus(ADC1,ADC_FLAG_RDY) == RESET);
    /* Start ADC1 calibration */
    ADC_CalibrationOperation(ADC1,ADC_CALIBRATION_SIGNAL_MODE);
    /* Check the end of ADC1 calibration */
    while (ADC_GetCalibrationStatus(ADC1));
}

void InitADC1(void)
{
	UINT8 i;

	InitADC1_GPIO();
	InitADC1_TIMER();
	InitADC1_DMA();
	InitADC_ADC1();

	for(i = 0; i < ADC_NUM; i++) {
        gu16_ADCValFilter1[i] = 0;
        gi32_ADCResult1[i] = 0;
		gu32_ADCValFilterNext1[i] = 0;
    }
}

void InitADC2_GPIO(void)
{
    GPIO_InitType GPIO_InitStructure;
    
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_ANALOG;

    GPIO_InitStructure.Pin = ADC_NTC1_PIN;
    GPIO_InitPeripheral(ADC_NTC1_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.Pin = ADC_CUR_PIN;
    GPIO_InitPeripheral(ADC_CUR_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = ADC_3_3V_PIN;
    GPIO_InitPeripheral(ADC_3_3V_PORT, &GPIO_InitStructure);
}

void InitADC2_TIMER(void)
{
    TIM_TimeBaseInitType TIM_TimeBaseStructure;
    OCInitType TIM_OCInitStructure;
    /* GTIM1 configuration ------------------------------------------------------*/
    /* Time base configuration */

    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_GTIM4, ENABLE);

    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.Period    = 1000 - 1;
    TIM_TimeBaseStructure.Prescaler = 48 - 1;
    TIM_TimeBaseStructure.ClkDiv    = TIM_CLK_DIV1;
    TIM_TimeBaseStructure.CounterMode   = TIM_CNT_MODE_UP;
    TIM_InitTimeBase(GTIM4, &TIM_TimeBaseStructure);
    /* PWM1 Mode configuration: Channel1 */
    TIM_InitOcStruct(&TIM_OCInitStructure);
    TIM_OCInitStructure.OCMode      = TIM_OCMODE_PWM1;
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.Pulse       = 500 - 1;
    TIM_OCInitStructure.OCPolarity  = TIM_OC_POLARITY_HIGH;
    // TIM_InitOc1(GTIM4, &TIM_OCInitStructure);
    // TIM_ConfigOc1Preload(GTIM4, TIM_OC_PRE_LOAD_ENABLE);
    
    /* PWM1 Mode configuration: Channel4 */
    TIM_InitOc4(GTIM4, &TIM_OCInitStructure);
    TIM_ConfigOc4Preload(GTIM4, TIM_OC_PRE_LOAD_ENABLE);

    TIM_ConfigArPreload(GTIM4, ENABLE);

    TIM_Enable(GTIM4, ENABLE);
}

DMA_InitType DMA_InitStructure2;
void InitADC2_DMA(void)
{
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_DMA1, ENABLE);

    DMA_DeInit(DMA1_CH2);
    DMA_InitStructure2.PeriphAddr     = (uint32_t)&ADC2->DAT;
    DMA_InitStructure2.MemAddr        = (uint32_t)(&gu16_ADCValFilter1[1]);
    DMA_InitStructure2.Direction      = DMA_DIR_PERIPH_SRC;
    DMA_InitStructure2.BufSize        = 3;
    DMA_InitStructure2.PeriphInc      = DMA_PERIPH_INC_DISABLE;
    // DMA_InitStructure2.PeriphInc      = DMA_PERIPH_INC_ENABLE;
    DMA_InitStructure2.MemoryInc      = DMA_MEM_INC_ENABLE;
    // DMA_InitStructure2.MemoryInc      = DMA_MEM_INC_DISABLE;
    DMA_InitStructure2.PeriphDataSize = DMA_PERIPH_DATA_WIDTH_HALFWORD;
    DMA_InitStructure2.MemDataSize    = DMA_MEM_DATA_WIDTH_HALFWORD;
    DMA_InitStructure2.CircularMode   = DMA_MODE_CIRCULAR;
    DMA_InitStructure2.Priority       = DMA_PRIORITY_HIGH;
    DMA_InitStructure2.Mem2Mem        = DMA_M2M_DISABLE;
    // DMA_InitStructure2.Mem2Mem        = DMA_M2M_ENABLE;
    DMA_Init(DMA1_CH2, &DMA_InitStructure2);
    
    DMA_RequestRemap(DMA_REMAP_ADC2, DMA1_CH2, ENABLE);
    /* Enable DMA1 channel1 */
    DMA_EnableChannel(DMA1_CH2, ENABLE);
}

ADC_InitType ADC_InitStructure2;
void InitADC_ADC2(void)
{
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_ADC2,ENABLE);

    /* ADC1 configuration ------------------------------------------------------*/
    ADC_InitStructure2.WorkMode       = ADC_WORKMODE_INDEPENDENT;
    ADC_InitStructure2.MultiChEn      = ENABLE;
    ADC_InitStructure2.ContinueConvEn = DISABLE;
    ADC_InitStructure2.ExtTrigSelect  = ADC_EXT_TRIG_REG_CONV_GTIM4_CC4;
    ADC_InitStructure2.DatAlign       = ADC_DAT_ALIGN_R;
    ADC_InitStructure2.ChsNumber      = 3;
    ADC_InitStructure2.Resolution     = ADC_DATA_RES_12BIT;
    ADC_Init(ADC2, &ADC_InitStructure2);
    /* ADC1 regular channel4 configuration */
    /* Set injected sequencer length */
    // ADC_ConfigInjectedSequencerLength(ADC2, 3);
    ADC_ConfigRegularChannel(ADC2, ADC2_Channel_05_PC4, 1, ADC_SAMP_TIME_CYCLES_13_5);
    ADC_ConfigRegularChannel(ADC2, ADC2_Channel_12_PB2, 2, ADC_SAMP_TIME_CYCLES_13_5);
    ADC_ConfigRegularChannel(ADC2, ADC2_Channel_15_PB15, 3, ADC_SAMP_TIME_CYCLES_13_5);


    ADC_ConfigInjectedSequencerLength(ADC2, 3);

    // ADC_SetDMATransferMode(ADC2, ADC_MULTI_REG_DMA_LIMIT_RES12_10B);

    /* Enable automatic injected conversion start after regular one */
    ADC_EnableAutoInjectedConv(ADC2, ENABLE);

    /* Enable JEOC interrupt */
    ADC_ConfigInt(ADC2, ADC_INT_JENDC, ENABLE);
    
    /* Enable ADC1 DMA */
    ADC_SetDMATransferMode(ADC2, ADC_MULTI_REG_DMA_EACH_ADC);

    /* Enable ADC1 */
    ADC_Enable(ADC2, ENABLE);
    /*Check ADC Ready*/
    while(ADC_GetFlagStatus(ADC2, ADC_FLAG_RDY) == RESET);
    /* Start ADC1 calibration */
    ADC_CalibrationOperation(ADC2, ADC_CALIBRATION_SIGNAL_MODE);
    /* Check the end of ADC1 calibration */
    while (ADC_GetCalibrationStatus(ADC2));

}

void InitADC2(void)
{
	InitADC2_GPIO();
	InitADC2_TIMER();
	InitADC2_DMA();
	InitADC_ADC2();
}


#define AD_CalNum           8
#define AD_CalNum_2         3
void ADC_V3_3(void)
{
	static UINT8 s8ADcnt = 0;
	INT32 t_i32temp = 0;
	if(s8ADcnt++ < AD_CalNum) {
		gu32_ADCValFilterNext1[ADC_EXT_3_3V] += (UINT32)gu16_ADCValFilter1[ADC_EXT_3_3V];
	} else {
		s8ADcnt = 0;
		t_i32temp = gu32_ADCValFilterNext1[ADC_EXT_3_3V]>>AD_CalNum_2; 	// 读取AD值，mV
		t_i32temp = t_i32temp > 0 ? t_i32temp : 0;
        t_i32temp = t_i32temp*3300/4096*2;          // mv
		gi32_ADCResult1[ADC_EXT_3_3V] = ((t_i32temp - gi32_ADCResult1[ADC_EXT_3_3V]) >> 3) + gi32_ADCResult1[ADC_EXT_3_3V];	//mV
	}
}

UINT16 tempAD_AD1 = 0;
UINT16 tempAD_AD2 = 0;
UINT16 tempAD_AD3 = 0;
UINT16 tempAD_AD4 = 0;
void ADC_TTC(void)
{
	INT32 t_i32temp = 0;

    // 古鑫的写法
    // t_i32temp = su16_ADC_Value[ADC_4051_IN];
    // t_i32temp = GetEndValue(iSheldTemp_10K_2, (UINT16)LENGTH_TBLTEMP_PORT_10K, (UINT16)t_i32temp);
    // gi32_ADCResult_4051[su8_channel] = t_i32temp;

	t_i32temp = (INT32)gu16_ADCValFilter1[ADC_TEMP_EV1];		// 读取AD值
    t_i32temp = GetEndValue(iSheldTemp_10K, (UINT16)LENGTH_TBLTEMP_PORT_10K, (UINT16)t_i32temp);
    gu32_ADCValFilterNext1[ADC_TEMP_EV1] = (((t_i32temp << 10) - gu32_ADCValFilterNext1[ADC_TEMP_EV1]) >> 3) + gu32_ADCValFilterNext1[ADC_TEMP_EV1];
	gi32_ADCResult1[ADC_TEMP_EV1] = (UINT16)((gu32_ADCValFilterNext1[ADC_TEMP_EV1] + 512)>>10);

	t_i32temp = (INT32)gu16_ADCValFilter1[ADC_TEMP_EV2];		// 读取AD值
    t_i32temp = GetEndValue(iSheldTemp_10K, (UINT16)LENGTH_TBLTEMP_PORT_10K, (UINT16)t_i32temp);
    gu32_ADCValFilterNext1[ADC_TEMP_EV2] = (((t_i32temp << 10) - gu32_ADCValFilterNext1[ADC_TEMP_EV2]) >> 3) + gu32_ADCValFilterNext1[ADC_TEMP_EV2];
	gi32_ADCResult1[ADC_TEMP_EV2] = (UINT16)((gu32_ADCValFilterNext1[ADC_TEMP_EV2] + 512)>>10);
}

#define SLIDE_WINDOW_SIZE			32
static INT16 si16_SlideWin_SmallCur[SLIDE_WINDOW_SIZE] = {0};			// 滑窗滤波
static INT32 si32_SlideWin_SmallCur_Sum = {0};							// 滑窗滤波总值计算
UINT16 gu16_Small_DsgCur = 0;
UINT16 gu16_Small_DsgCur_Now = 0;
extern UINT16 gu16_Rs_uO;
#define B_RS                        200

void ADC_SmallDsgCur(void)
{
    static UINT8 su8_SlideWin_CurCnt = 0;

    si32_SlideWin_SmallCur_Sum -= si16_SlideWin_SmallCur[su8_SlideWin_CurCnt];
    si16_SlideWin_SmallCur[su8_SlideWin_CurCnt] = gu16_ADCValFilter1[ADC_CURR];
    si32_SlideWin_SmallCur_Sum += si16_SlideWin_SmallCur[su8_SlideWin_CurCnt];
    if(++su8_SlideWin_CurCnt >= SLIDE_WINDOW_SIZE) {
        su8_SlideWin_CurCnt = 0;
    }

    gi32_ADCResult1[ADC_CURR] = si32_SlideWin_SmallCur_Sum/SLIDE_WINDOW_SIZE;

    gi32_ADCResult1[ADC_CURR] = gi32_ADCResult1[ADC_CURR]*3300/4096;                // mV
    // gu16_Small_DsgCur_Now = gi32_ADCResult1[ADC_CURR]*gu16_Rs_uO/1000;           // mA，用这个，算错了有问题
    // gu16_Small_DsgCur_Now = (gi32_ADCResult1[ADC_CURR])*1000/gu16_Rs_uO/B_RS;    // mA
    // gu16_Small_DsgCur_Now = (gi32_ADCResult1[ADC_CURR])*1000*1000/(gu16_Rs_uO*B_RS);     // mA
    gu16_Small_DsgCur_Now = (gi32_ADCResult1[ADC_CURR])*1000/(gu16_Rs_uO*B_RS/1000);     // mA
    gu16_Small_DsgCur = gu16_Small_DsgCur_Now;		                                //


    tempAD_AD1 = gu16_ADCValFilter1[ADC_CURR]*3300/4096;
    tempAD_AD2 = gi32_ADCResult1[ADC_CURR];

    tempAD_AD3 = gu16_ADCValFilter1[ADC_TEMP_EV1];
	tempAD_AD4 = gu16_ADCValFilter1[ADC_EXT_3_3V];
    // tempAD_AD3 = gi32_ADCResult1[ADC_CURR];
    // tempAD_AD4 = gu16_Rs_uO;
}

void InitADC(void)
{
    InitADC1();
    InitADC2();
}

void App_ADC(void)
{
	if(STARTUP_CONT == System_FUNC_StartUp(SYSTEM_FUNC_STARTUP_ADC)) {
		//return;
	}
	
	if(0 == g_st_SysTimeFlag.bits.b1Sys100msFlag) {
		return;
	}

    ADC_TTC();
    ADC_SmallDsgCur();
    ADC_V3_3();
}

#endif
