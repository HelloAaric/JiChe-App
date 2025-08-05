#ifndef SLEEP_H__
#define SLEEP_H__
#include "I2C.h"
#include "DVC11XX.h"

#define TWK	 //定时唤醒
#ifndef	TWK
#define CWK	 //电流唤醒
#endif

#ifdef CWK
#define OCC2_E	0 //过冲使能
#define OCD2_E	0	//过放使能
#define SCD_E 	1	//短路使能
#endif

void Sleep_config(void);
void EXTI_initial(void);
void Sleep_Handle(void);

#endif
