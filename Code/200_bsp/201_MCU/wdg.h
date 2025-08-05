#ifndef _WDG_H
#define _WDG_H

#include "n32h47x_48x_iwdg.h"

#define Feed_IWatchDog IWDG_ReloadKey()

void Init_IWDG(void);

#endif	/* _WDG_H */
