#ifndef _SCI_COM_H
#define _SCI_COM_H

#include "uart_upper.h"


/*============Sci===========*/
//每个串口只能选一种通讯
#define _COMMOM_UPPER_SCI1
//#define _CLIENT_SCI1

#define _COMMOM_UPPER_SCI2
//#define _CLIENT_SCI2

#define _COMMOM_UPPER_SCI3
//#define _CLIENT_SCI3
/*============Sci===========*/


void InitSci(void);
void App_Sci(void);


#endif	/* _SCI_COM_H */

