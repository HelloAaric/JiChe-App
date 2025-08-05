#include "sci_com.h"
#include "uart.h"

void InitSci(void)
{
	InitData_CommonUpper();
	InitUART_CommonUpper1();
	InitUART_CommonUpper2();
	InitUART_CommonUpper3();
}


void App_Sci(void)
{
	App_CommonUpper();
	// App_UartClient_Updata();
}
