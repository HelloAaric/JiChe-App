#ifndef _TIMER_H
#define _TIMER_H


#include "basic_data_type.h"


extern volatile uint8_t g_u810msClockCnt;
extern volatile uint8_t g_u81msClockCnt;
extern volatile UINT32 gu32_2msClockCnt;

void __delay_us(uint32_t us);
void __delay_ms(uint32_t ms);

void InitTimer(void);

#endif	/* _TIMER_H */
