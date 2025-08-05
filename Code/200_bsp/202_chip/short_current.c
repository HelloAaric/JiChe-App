#include "short_current.h"
#include "JiChe_APP.h"
#include "data_deal.h"
#include "System_Monitor.h"
#include "charger_loader_func.h"
#include "sys_timer.h"
#include "io_define.h"

// 有个默认影响，规格书没体现
void AFE_MosInit(void)
{
    // 放电屏蔽控制寄存器
    g_AfeRegs.R83.DBDM = 1;         // 充电电流大于放电 NFET 体二极管续流阈值时不影响 DSG 驱动输出状态
    g_AfeRegs.R83.DPDM = 1;         // PDSG 开启时不影响 DSG 驱动输出状态
    g_AfeRegs.R83.DDM = 1;          // DON 输入不影响 DSG 驱动输出状态
    g_AfeRegs.R83.DWM = 0;          // I2C 超时关闭 DSG 驱动输出
    g_AfeRegs.R83.PCDM = 1;         // DON 输入不影响 PCHG 驱动输出状态 
    g_AfeRegs.R83.PCCM = 1;         // CON 输入不影响 PCHG 驱动输出状态
    g_AfeRegs.R83.PCWM = 0;         // I2C 超时关闭 PCHG 驱动输出
    g_AfeRegs.R83.PDDM = 1;         // DON 输入不影响 PDSG 驱动输出状态

    // 充电屏蔽控制寄存器
    g_AfeRegs.R84.CBDM = 1;         // 放电电流大于充电 NFET 体二极管续流阈值时不影响 CHG 驱动输出状态
    g_AfeRegs.R84.CPCM = 1;         // PCHG 开启时不影响 CHG 驱动输出状态
    g_AfeRegs.R84.CCM = 1;          // CON 输入不影响 CHG 驱动输出状态
    g_AfeRegs.R84.CDM = 1;          // DON 输入不影响 CHG 驱动输出状态
    g_AfeRegs.R84.CSM = 0;          // 放电短路时影响 CHG 驱动输出
    g_AfeRegs.R84.CO2M = 1;         // 2 级放电过流时不影响 CHG 驱动输出
    g_AfeRegs.R84.CO1M = 1;         // 1 级放电过流时不影响 CHG 驱动输出
    g_AfeRegs.R84.CWM = 0;          // I2C 超时关闭 CHG 驱动输出

    // 配置寄存器
    DVC11XX_WriteRegs(AFE_ADDR_R(83), 2);


    // 下面配置，来源于这个函数。出问题观察一下少了什么。
    // FET_Config();
    // DSG为源随驱动输出模式
    // 是不是这里导致24串出问题的呢？
    DSGM_Control(0);                // 电荷泵输出模式
    HSFM_Control(DISABLE);          // 不允许屏蔽高边驱动，也即允许高边驱动
    DPC_Config(16);                 // 下拉强度16

    // 全部断开
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

    // 屏蔽这些保护
    OV_Config(0, 0, DISABLE);
    UV_Config(0, 0, DISABLE);
    OCD1_Config(0, 0);
    OCD2_Config(0, 0, DISABLE);
    OCC1_Config(0, 0);
    OCC2_Config(0, 0, DISABLE);

    // R119开机已配置为0xC0，看门狗已经关闭。不作处理。
    g_AfeRegs.R119.IWT = 0;
    
    // 使能短路保护
    // 10mV，10us，这个函数会自己算。
    u16_shortVol_mv = OtherElement.u16CS_Cur_DSGmax/10;
    u16_shortVol_mv = u16_shortVol_mv*gu16_Rs_uO/1000;      // A * mΩ = mV
    u16_shortT_us = OtherElement.u16CBC_DelayT/10;
    SCD_Config(u16_shortVol_mv, u16_shortT_us, ENABLE);
}


// 放到后面 TASK_JiChe_InitFun() 再次初始化
/*
关于短路保护
1. 使能短路保护。
2. 使用上位机的参数。
3. 让其自动断开放电MOS，同时配置断开充电MOS。
4. AFE配置不能弄成一个函数了，分开吧。初始化弄在前面，不在main循环跑。
5. 随后配置短路，覆盖前面的。
6. 进入主循环就可以开启MOS了。

关于AFE控制MOS
1. 使用自己的保护体系。
2. 屏蔽AFE的保护体系。只保留短路。
3. 使用内部控制MOS来控制MOS。
4. 同时使用标志位，控制AFE输出MOS。达到两路同时输出信号。
5. 如果采集回来的信号不一致，先确认是不是短路，如果不是短路，则报 蓝牙错误。
6. 断线错误用 APP错误。

TODO
1. 思考使用外部MOS，全局思考MOS是否写完了。
    MOS反馈机制。长期观察MOS是否和现在要求的一致。
2. 短路是否已经OK？
3. 短路后续处理机制。
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
                // CBC_Element.u8CBC_DSG_ErrFlag = 1;			// 断开MOS，使用下面这两个就行了，这个是以前的滤波操作。
                System_OnOFF_Func.bits.b1OnOFF_MOS_Relay = 0;   // 只用这一个也不是不行。
                ChargerLoad_Func.bits.b1OFFDriver_CBC = 1;      // 开启复原机制

                su8_CBC_status = 2;
            }
            break;

        case 2:
            // 定期查询充电枪。
	        // ChargerLoad_Func.bits.b1ON_Charger_AllSeries = AFE_Data_ALL.ChargeDetect;
	        // ChargerLoad_Func.bits.b1ON_Charger_AllSeries = MCUI_DI_CWAKE;
            // 等待充电枪插入解除。
            if(ChargerLoad_Func.bits.b1OFFDriver_CBC == 0) {
                // 擦除报警
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

