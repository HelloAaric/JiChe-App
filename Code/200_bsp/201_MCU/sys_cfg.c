#include "sys_cfg.h"
#include "n32h47x_48x.h"
#include "n32h47x_48x_pwr.h"
#include "n32h47x_48x_rcc.h"


void Init_BKP_RAM(void)
{
    // 1. ʹ�� PWR �� BKP ʱ��
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR | RCC_APB1_PERIPH_BKP, ENABLE);

    // 2. ���������������޸� BKP SRAM��
    PWR_BackupAccessEnable(ENABLE);

    // ʹ��SRAM������Vbatģʽ
    // VBATģʽ��ָ΢������������Դ(VDD)�Ͽ�ʱ���ɱ��õ�Դ(VBAT���Ź���)ά���ض���·������ģʽ��
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


