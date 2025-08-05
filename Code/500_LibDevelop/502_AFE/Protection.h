#ifndef PROTECTION_H__
#define PROTECTION_H__
#include "DVC11XX.h"


extern void CleanError(void);
bool OCD1_Config(u16 TH,u16 DLY);
bool OCD2_Config(u16 TH,u16 DLY,bool enable);
bool OCC1_Config(u16 TH,u16 DLY);
bool OCC2_Config(u16 TH,u16 DLY,bool enable);
bool OV_Config(u16 TH,u16 DLY,bool enable);
bool UV_Config(u16 TH,u16 DLY,bool enable);
bool SCD_Config(u16 TH,float DLY,bool enable);
void Protection_Config(void);
void Protection_handle(void);
#endif
