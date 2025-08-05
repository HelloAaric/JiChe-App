#include "short_current.h"
#include "JiChe_APP.h"
#include "data_deal.h"
#include "System_Monitor.h"
#include "charger_loader_func.h"
#include "sys_timer.h"
#include "io_define.h"

// �и�Ĭ��Ӱ�죬�����û����
void AFE_MosInit(void)
{
    // �ŵ����ο��ƼĴ���
    g_AfeRegs.R83.DBDM = 1;         // ���������ڷŵ� NFET �������������ֵʱ��Ӱ�� DSG �������״̬
    g_AfeRegs.R83.DPDM = 1;         // PDSG ����ʱ��Ӱ�� DSG �������״̬
    g_AfeRegs.R83.DDM = 1;          // DON ���벻Ӱ�� DSG �������״̬
    g_AfeRegs.R83.DWM = 0;          // I2C ��ʱ�ر� DSG �������
    g_AfeRegs.R83.PCDM = 1;         // DON ���벻Ӱ�� PCHG �������״̬ 
    g_AfeRegs.R83.PCCM = 1;         // CON ���벻Ӱ�� PCHG �������״̬
    g_AfeRegs.R83.PCWM = 0;         // I2C ��ʱ�ر� PCHG �������
    g_AfeRegs.R83.PDDM = 1;         // DON ���벻Ӱ�� PDSG �������״̬

    // ������ο��ƼĴ���
    g_AfeRegs.R84.CBDM = 1;         // �ŵ�������ڳ�� NFET �������������ֵʱ��Ӱ�� CHG �������״̬
    g_AfeRegs.R84.CPCM = 1;         // PCHG ����ʱ��Ӱ�� CHG �������״̬
    g_AfeRegs.R84.CCM = 1;          // CON ���벻Ӱ�� CHG �������״̬
    g_AfeRegs.R84.CDM = 1;          // DON ���벻Ӱ�� CHG �������״̬
    g_AfeRegs.R84.CSM = 0;          // �ŵ��·ʱӰ�� CHG �������
    g_AfeRegs.R84.CO2M = 1;         // 2 ���ŵ����ʱ��Ӱ�� CHG �������
    g_AfeRegs.R84.CO1M = 1;         // 1 ���ŵ����ʱ��Ӱ�� CHG �������
    g_AfeRegs.R84.CWM = 0;          // I2C ��ʱ�ر� CHG �������

    // ���üĴ���
    DVC11XX_WriteRegs(AFE_ADDR_R(83), 2);


    // �������ã���Դ�����������������۲�һ������ʲô��
    // FET_Config();
    // DSGΪԴ���������ģʽ
    // �ǲ������ﵼ��24����������أ�
    DSGM_Control(0);                // ��ɱ����ģʽ
    HSFM_Control(DISABLE);          // ���������θ߱�������Ҳ������߱�����
    DPC_Config(16);                 // ����ǿ��16

    // ȫ���Ͽ�
    #if 0
    PDSG_FETControl(1);
    CHG_FETControl(3);
    DSG_FETControl(3);
    #endif
    PDSG_FETControl(0);
    CHG_FETControl(0);
    DSG_FETControl(0);
}

extern UINT16 gu16_Rs_uO;
void AFE_ProtectInit(void)
{
    UINT16 u16_shortVol_mv = 0;
    UINT16 u16_shortT_us = 0;

    // ������Щ����
    OV_Config(0, 0, DISABLE);
    UV_Config(0, 0, DISABLE);
    OCD1_Config(0, 0);
    OCD2_Config(0, 0, DISABLE);
    OCC1_Config(0, 0);
    OCC2_Config(0, 0, DISABLE);

    // R119����������Ϊ0xC0�����Ź��Ѿ��رա���������
    g_AfeRegs.R119.IWT = 0;
    
    // ʹ�ܶ�·����
    // 10mV��10us������������Լ��㡣
    u16_shortVol_mv = OtherElement.u16CS_Cur_DSGmax/10;
    u16_shortVol_mv = u16_shortVol_mv*gu16_Rs_uO/1000;      // A * m�� = mV
    u16_shortT_us = OtherElement.u16CBC_DelayT/10;
    SCD_Config(u16_shortVol_mv, u16_shortT_us, ENABLE);
}


// �ŵ����� TASK_JiChe_InitFun() �ٴγ�ʼ��
/*
���ڶ�·����
1. ʹ�ܶ�·������
2. ʹ����λ���Ĳ�����
3. �����Զ��Ͽ��ŵ�MOS��ͬʱ���öϿ����MOS��
4. AFE���ò���Ū��һ�������ˣ��ֿ��ɡ���ʼ��Ū��ǰ�棬����mainѭ���ܡ�
5. ������ö�·������ǰ��ġ�
6. ������ѭ���Ϳ��Կ���MOS�ˡ�

����AFE����MOS
1. ʹ���Լ��ı�����ϵ��
2. ����AFE�ı�����ϵ��ֻ������·��
3. ʹ���ڲ�����MOS������MOS��
4. ͬʱʹ�ñ�־λ������AFE���MOS���ﵽ��·ͬʱ����źš�
5. ����ɼ��������źŲ�һ�£���ȷ���ǲ��Ƕ�·��������Ƕ�·���� ��������
6. ���ߴ����� APP����

TODO
1. ˼��ʹ���ⲿMOS��ȫ��˼��MOS�Ƿ�д���ˡ�
    MOS�������ơ����ڹ۲�MOS�Ƿ������Ҫ���һ�¡�
2. ��·�Ƿ��Ѿ�OK��
3. ��·����������ơ�
*/
void InitShortCur(void)
{
    // gu16_Rs_uO = OtherElement.u16Sys_CS_Res*1000 / OtherElement.u16Sys_CS_Res_Num;
    AFE_ProtectInit();
    AFE_MosInit();
}

void App_ShortCurDeal_AFE(void)
{
    static UINT8 su8_CBC_status = 0;
    static UINT8 su8_startUpDelay_Tcnt = 0;
    static UINT8 su8_startUp_Flash = 0;

    if(AFE_Data_ALL.JiChe_StartUp == 1 && su8_startUp_Flash == 0) {
        su8_startUp_Flash = 1;
        InitShortCur();
    }

	if(0 == g_st_SysTimeFlag.bits.b1Sys1000msFlag3) {
		return;
    }

    switch(su8_CBC_status) {
        case 0:
            if(++su8_startUpDelay_Tcnt >= 2) {
                su8_startUpDelay_Tcnt = 0;
                su8_CBC_status = 1;
            }
            break;

        case 1:
            if(AFE_Data_ALL.Warning.sBit.SCD) {
                System_ERROR_UserCallback(ERROR_CBC_DSG);
                // CBC_Element.u8CBC_DSG_ErrFlag = 1;			// �Ͽ�MOS��ʹ�����������������ˣ��������ǰ���˲�������
                System_OnOFF_Func.bits.b1OnOFF_MOS_Relay = 0;   // ֻ����һ��Ҳ���ǲ��С�
                ChargerLoad_Func.bits.b1OFFDriver_CBC = 1;      // ������ԭ����

                su8_CBC_status = 2;
            }
            break;

        case 2:
            // ���ڲ�ѯ���ǹ��
	        // ChargerLoad_Func.bits.b1ON_Charger_AllSeries = AFE_Data_ALL.ChargeDetect;
	        // ChargerLoad_Func.bits.b1ON_Charger_AllSeries = MCUI_DI_CWAKE;
            // �ȴ����ǹ��������
            if(ChargerLoad_Func.bits.b1OFFDriver_CBC == 0) {
                // ��������
                CleanError();
                su8_CBC_status = 0;
            }
           break;
        default:
            break;
    }
}

void PCHG_OnOFF(UINT8 OnOFF)
{
    if(OnOFF) {
        PDSG_FETControl(0x01);
    } else {
        PDSG_FETControl(0x00);
    }
}

void CHG_OnOFF(UINT8 OnOFF)
{
    if(OnOFF) {
        CHG_FETControl(0x03);
    } else {
        CHG_FETControl(0x00);
    }
}

void DSG_OnOFF(UINT8 OnOFF)
{
    if(OnOFF) {
        DSG_FETControl(0x03);
    } else {
        DSG_FETControl(0x00);
    }
}

