#include "balance.h"
#include "sci_com.h"
#include "JiChe_APP.h"

void InitData_Balance(void)
{

    
}

void App_BalanceProcess(void)
{
    UINT32 newBals = 0;
    UINT16 u16_Vcellmin = 0;
    UINT16 u16_Vcellmax = 0;
    UINT16 u16_BalanceVol = 0;
    UINT16 u16_BalanceDiff = 0;
    UINT32 u32_BalanceStatus = 0;
    UINT8 i = 0;

    u16_Vcellmin = g_stCellInfoReport.u16VCellMin;          // mV
    u16_Vcellmax = g_stCellInfoReport.u16VCellMax;
    u16_BalanceVol = OtherElement.u16Balance_OpenVoltage;
    u16_BalanceDiff = OtherElement.u16Balance_OpenWindow;

    // 刷新当前均衡位
    // uiBalMaskFlags = (g_AfeRegs.R103_R105.CB[0]<<16)+(g_AfeRegs.R103_R105.CB[1]<<8)+g_AfeRegs.R103_R105.CB[2];
    u32_BalanceStatus = AFE_Data_ALL.CellsBalancStatus.All;

    /*
        1. 未过温，这个先舍弃。
        2. 最低电压高于均衡开启最小电压、压差高于均衡开启阈值。
        3. 充电均衡？静态均衡？
    */
    if((u16_Vcellmin > u16_BalanceVol)
        && (u16_Vcellmax - u16_Vcellmin) >= u16_BalanceDiff
        && (1)
        && (1)) {
        for(i = 0; i < SeriesNum; i++) {
            if( (g_stCellInfoReport.u16VCell[i] - u16_Vcellmin) >= u16_BalanceDiff) {
                newBals	|= (1<<i);
            }
        }
    }

    // 均衡位变化
    if(newBals != u32_BalanceStatus) {
        u32_BalanceStatus = newBals;
        Balance_Contrl(u32_BalanceStatus);
    }

    g_stCellInfoReport.u16BalanceFlag1 = u32_BalanceStatus & 0x0000FFFF;
    g_stCellInfoReport.u16BalanceFlag2 = (u32_BalanceStatus>>16);
}
