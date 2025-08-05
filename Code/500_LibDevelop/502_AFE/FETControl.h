#ifndef FETCONTROL_H__
#define FETCONTROL_H__
#include "DVC11XX.h"


enum {				 //DSG Output MODE
DSGM_CP=0,		 //��ɱ�
DSGM_FC				 //��Դ
};
enum {				 //CHG DSG MODE
FET_Close=1,	 //�ر�FET����
FET_Close_BFCO,//�ر�FET�������������ڵ�������������ֵ����FET����
FET_OPEN
};		 //����FET����

enum{					 //ChargePump mode
CP_Close=0,		 //�رյ�ɱ�
CP_6V,				 //��ɱ�6V
CP_7V,
CP_8V,
CP_9V,
CP_10V,
CP_11V,
CP_12V,				 //��ɱ�12V
};

bool PCHG_FETControl(bool abel);
bool PDSG_FETControl(bool abel);
bool CHG_FETControl(u8 mode);
bool DSG_FETControl(u8 mode);
bool DSGM_Control(bool mode);
bool DPC_Config(u8 DPC);
bool BDPT_Config(u16 value);
bool ChargePump_Control(u8 mode);
bool HSFM_Control(bool mode);
#endif
