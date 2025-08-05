#ifndef TEMPERATURE_H__
#define TEMPERATURE_H__

#include "DVC11XX.h"


#define GP(n)	n
//------------------------------------------------------------------------------
void Read_NFRT(void);
void GPM_Temperature (void);
float DVC11XX_Calc_ChipTemp(void);
float DVC11XX_Calc_BatTemp(u8 GP);
void Read_NFRT (void);
void GPM_Temperature (void);
float DVC11XX_Calc_ChipTemp(void);
//------------------------------------------------------------------------------
#endif
