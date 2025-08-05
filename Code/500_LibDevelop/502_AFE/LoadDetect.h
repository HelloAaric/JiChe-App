#ifndef LOADDETECT_H__
#define LOADDETECT_H__

#include "DVC11XX.h"

#define LoadDetect_Threshold 2000

extern void CleanError(void);

void LoadDetect(void);
void LoadPullUp(void);

#endif

