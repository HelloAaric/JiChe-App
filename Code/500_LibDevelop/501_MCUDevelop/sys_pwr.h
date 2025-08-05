#ifndef _SYS_PWR_H
#define _SYS_PWR_H


#define MCU_RESET()	            NVIC_SystemReset()

void InitSys_Power(void);
void App_Sys_FlashUpdate(void);
void App_SysPower(void);

#endif	/* _SYS_PWR_H */

