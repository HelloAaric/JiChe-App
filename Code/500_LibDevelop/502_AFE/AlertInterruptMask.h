#ifndef ALERTINTERRUPTMASK_H__
#define ALERTINTERRUPTMASK_H__
#include "DVC11XX.h"

extern u8 EVENT_alert_req;

void GP2_ModeConfig(u8 mode);
void InterruptShield_Config(u8 mode);
void Warning_handle(void);
#endif

