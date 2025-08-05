#ifndef SHUTDOWN_H__
#define SHUTDOWN_H__
#include "I2C.h"
#include "DVC11XX.h"
#define GPIO_DVC_WKUP_PIN	               GPIO_Pin_0//���Ѱ���
#define GPIO_DVC_SD_PIN	               	 GPIO_Pin_1//�ضϰ���

extern bool	wakeup_flag;
extern bool shutdown_flag;

extern bool DVC11XX_InitRegs_Twice(void);
void Shutdown(void);

#endif
