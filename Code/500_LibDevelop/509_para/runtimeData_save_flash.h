#ifndef _RUNTIMEDATA_SAVE_FLASH_H
#define _RUNTIMEDATA_SAVE_FLASH_H

#include "basic_data_type.h"

void runtimeSOC_PageErase(void);
// SOC字，默认为8个字
void runtimeSOC_Get_SocWord(UINT32* word);
// SOC单位，默认8个字
void runtimeSOC_Write_SocWord(UINT32* word);


void runtimeLOG_ResetAll(void);
// LOG字，默认为4个字。返回最新的LOG字。
void runtimeLOG_Get_LogWord_Latest(UINT32* word);
// 返回LOG字节长度，同时返回所有100个LOG字，未满返回LOG目前长度
UINT16 runtimeLOG_Get_LogPage(UINT32** word);
// LOG单位，默认4个字
void runtimeLOG_Write_LogWord(UINT32* word);
#endif	/* _RUNTIMEDATA_SAVE_FLASH_H */
