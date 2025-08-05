#include "sys_cfg.h"
#include "n32h47x_48x.h"
#include "n32h47x_48x_pwr.h"
#include "n32h47x_48x_rcc.h"


void Init_BKP_RAM(void)
{
    // 1. 使能 PWR 和 BKP 时钟
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR | RCC_APB1_PERIPH_BKP, ENABLE);

    // 2. 解锁备份域（允许修改 BKP SRAM）
    PWR_BackupAccessEnable(ENABLE);

    // 使能SRAM运行在Vbat模式
    // VBAT模式是指微控制器在主电源(VDD)断开时，由备用电源(VBAT引脚供电)维持特定电路工作的模式。
    PWR_EnableBKPSRAMRetainInVbatMode(ENABLE);
}


void BkpRam_WriteUINT16(UINT32 adress, UINT16 data)
{
    *((__IO UINT16*) adress) = data;
}

UINT16 BkpRam_ReadUINT16(UINT32 adress)
{
    return *(( UINT16*) adress);
}


