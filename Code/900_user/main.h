#ifndef MAIN_H
#define MAIN_H


// #define _DEBUG_CODE
#define _IAP					//如果工程文件地址变的话，加上这个和烧IAP的project才行

//#define _DI_SWITCH_SYS_ONOFF	//DI开关用作休眠
//#define _DI_SWITCH_DSG_ONOFF	//DI开关用作控制放电接触器或者MOS

void InitSystemWakeUp(void);

#endif	/* MAIN_H */

