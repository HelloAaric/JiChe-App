#ifndef SLEEP_H__
#define SLEEP_H__
#include "I2C.h"
#include "DVC11XX.h"

#define TWK	 //��ʱ����
#ifndef	TWK
#define CWK	 //��������
#endif

#ifdef CWK
#define OCC2_E	0 //����ʹ��
#define OCD2_E	0	//����ʹ��
#define SCD_E 	1	//��·ʹ��
#endif

void Sleep_config(void);
void EXTI_initial(void);
void Sleep_Handle(void);

#endif
