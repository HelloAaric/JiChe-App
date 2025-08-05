#ifndef _RUNTIMEDATA_SAVE_FLASH_H
#define _RUNTIMEDATA_SAVE_FLASH_H

#include "basic_data_type.h"

void runtimeSOC_PageErase(void);
// SOC�֣�Ĭ��Ϊ8����
void runtimeSOC_Get_SocWord(UINT32* word);
// SOC��λ��Ĭ��8����
void runtimeSOC_Write_SocWord(UINT32* word);


void runtimeLOG_ResetAll(void);
// LOG�֣�Ĭ��Ϊ4���֡��������µ�LOG�֡�
void runtimeLOG_Get_LogWord_Latest(UINT32* word);
// ����LOG�ֽڳ��ȣ�ͬʱ��������100��LOG�֣�δ������LOGĿǰ����
UINT16 runtimeLOG_Get_LogPage(UINT32** word);
// LOG��λ��Ĭ��4����
void runtimeLOG_Write_LogWord(UINT32* word);
#endif	/* _RUNTIMEDATA_SAVE_FLASH_H */
