#include "sys_pwr.h"
#include "io_define.h"
#include "sci_com.h"
#include "sys_timer.h"

void InitSys_Power(void)
{
    // MCUO_PWSV_BUCK = 1;
    MCUO_PWSV_CTR = 0;
    MCUO_PWSV_LDO = 0;       // 休眠置高供电便可。


    // MCUO_WK_AFE = 1;
    // __delay_ms(10);
    // MCUO_WK_AFE = 0;

	// MCUO_AFE_WAKE_BOTH_SHDN = 1;
	// __delay_ms(100);
	// MCUO_AFE_WAKE_BOTH_SHDN = 0;

    // MCUO_RELAY_K4 = 1;
}


void App_SysPower(void)
{
    static UINT16 su16_StartUp_Tcnt = 0;

	if(0 == g_st_SysTimeFlag.bits.b1Sys10msFlag1) {
		return;
	}

    if(++su16_StartUp_Tcnt >= 100) {
        MCUO_PWSV_CTR = 0;
        MCUO_PWSV_LDO = 1;              // 全速运行
    }
}


void App_Sys_FlashUpdate(void)
{
    if(1 == u8FlashUpdateFlag) {
        __delay_ms(10);
		u8FlashUpdateFlag = 0;
		// __disable_fault_irq();
        // NVIC_DisableIRQ;
        MCU_RESET();			//reset避免了一切中断初始化问题
    }
}
