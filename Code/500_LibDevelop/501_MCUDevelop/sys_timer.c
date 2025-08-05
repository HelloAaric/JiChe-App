#include "sys_timer.h"
#include "io_define.h"

volatile union SYS_TIME g_st_SysTimeFlag;


void App_SysTime(void)
{
	static UINT8 s_u8Cnt1ms = 0;

	static UINT32 su32_Cnt10ms = 0;
	static UINT8 s_u8Cnt10ms_select = 0;
	
	// static UINT8 s_u8Cnt10ms = 0;
	//static UINT8 s_u8Cnt20ms = 0;
	static UINT8 s_u8Cnt50ms = 0;
	static UINT8 s_u8Cnt100ms = 0;
	
	static UINT8 s_u8Cnt200ms1 = 0;
	static UINT8 s_u8Cnt200ms2 = 4;
	static UINT8 s_u8Cnt200ms3 = 8;
	static UINT8 s_u8Cnt200ms4 = 12;
	static UINT8 s_u8Cnt200ms5 = 16;

	static UINT8 s_u8Cnt1000ms1 = 0;
	static UINT8 s_u8Cnt1000ms2 = 33;
	static UINT8 s_u8Cnt1000ms3 = 66;

	g_st_SysTimeFlag.bits.b1Sys1msFlag = 0;
	if(s_u8Cnt1ms != g_u81msClockCnt) {				//1ms��ʱ��־
		s_u8Cnt1ms = g_u81msClockCnt;
		g_st_SysTimeFlag.bits.b1Sys1msFlag = 1;
	}

	g_st_SysTimeFlag.bits.b1Sys10msFlag1 = 0;  
	g_st_SysTimeFlag.bits.b1Sys10msFlag2 = 0;
	g_st_SysTimeFlag.bits.b1Sys10msFlag3 = 0;
	g_st_SysTimeFlag.bits.b1Sys10msFlag4 = 0;
	g_st_SysTimeFlag.bits.b1Sys10msFlag5 = 0;
	#if 0
	if(s_u8Cnt10ms != g_u810msClockCnt) {		//10ms��ʱ��־
		s_u8Cnt10ms = g_u810msClockCnt;
		switch(g_u810msClockCnt) {
	#endif
	#if 1
	if(su32_Cnt10ms != gu32_2msClockCnt) {		//10ms��ʱ��־
		su32_Cnt10ms = gu32_2msClockCnt;
		if(s_u8Cnt10ms_select >= 5) {
			s_u8Cnt10ms_select = 0;				//�ָ�
		}
		switch(s_u8Cnt10ms_select++) {
	#endif
			case 0:
				g_st_SysTimeFlag.bits.b1Sys10msFlag1 = 1;
				break;

			case 1:
				s_u8Cnt50ms++;
				g_st_SysTimeFlag.bits.b1Sys10msFlag2 = 1;
				break;

			case 2:
				s_u8Cnt100ms++;
				g_st_SysTimeFlag.bits.b1Sys10msFlag3 = 1;
				break;

			case 3:
				s_u8Cnt200ms1++;		//������һ��������һ��ѭ��Ȼ����λ��������BUG������
				s_u8Cnt200ms2++;		//���������һ��10ms������ı��־λ���䲻�ٽ���
				s_u8Cnt200ms3++;
				s_u8Cnt200ms4++;
				s_u8Cnt200ms5++;
				g_st_SysTimeFlag.bits.b1Sys10msFlag4 = 1;
				break;

			case 4:
				s_u8Cnt1000ms1++;
				s_u8Cnt1000ms2++;
				s_u8Cnt1000ms3++;
				g_st_SysTimeFlag.bits.b1Sys10msFlag5 = 1;
				break;

			default:
				break;
		}
	}

	g_st_SysTimeFlag.bits.b1Sys50msFlag = 0;
	if(s_u8Cnt50ms >= 5) {
		s_u8Cnt50ms = 0;
		g_st_SysTimeFlag.bits.b1Sys50msFlag = 1;		//50ms��ʱ��־
	}


	g_st_SysTimeFlag.bits.b1Sys100msFlag = 0;
	if(s_u8Cnt100ms >= 10) {
		s_u8Cnt100ms = 0;
		g_st_SysTimeFlag.bits.b1Sys100msFlag = 1;		//100ms��ʱ��־
	}

	g_st_SysTimeFlag.bits.b1Sys200msFlag1 = 0;
	g_st_SysTimeFlag.bits.b1Sys200msFlag2 = 0;
	g_st_SysTimeFlag.bits.b1Sys200msFlag3 = 0;
	g_st_SysTimeFlag.bits.b1Sys200msFlag4 = 0;
	g_st_SysTimeFlag.bits.b1Sys200msFlag5 = 0;
	if(s_u8Cnt200ms1 >= 20) {
		s_u8Cnt200ms1 = 0;
		g_st_SysTimeFlag.bits.b1Sys200msFlag1 = 1;		//200ms��ʱ��־
	}
	if(s_u8Cnt200ms2 >= 20) {
		s_u8Cnt200ms2 = 0;
		g_st_SysTimeFlag.bits.b1Sys200msFlag2 = 1;		//200ms��ʱ��־
	}
	if(s_u8Cnt200ms3 >= 20) {
		s_u8Cnt200ms3 = 0;
		g_st_SysTimeFlag.bits.b1Sys200msFlag3 = 1;		//200ms��ʱ��־
	}
	if(s_u8Cnt200ms4 >= 20) {
		s_u8Cnt200ms4 = 0;
		g_st_SysTimeFlag.bits.b1Sys200msFlag4 = 1;		//200ms��ʱ��־
		MCUO_DEBUG_LED_RED = !MCUO_DEBUG_LED_RED;
		//MCUI_DO_CWAKE = !MCUI_DO_CWAKE;
		// MCUI_DO_CWAKE = 0;
		// MCUO_DEBUG_LED2 = !MCUO_DEBUG_LED2;
		// printf("OK!\n");
	}
	
	if(s_u8Cnt200ms5 >= 20) {
		s_u8Cnt200ms5 = 0;
		g_st_SysTimeFlag.bits.b1Sys200msFlag5 = 1;		//200ms��ʱ��־
	}


	g_st_SysTimeFlag.bits.b1Sys1000msFlag1 = 0;
	g_st_SysTimeFlag.bits.b1Sys1000msFlag2 = 0;
	g_st_SysTimeFlag.bits.b1Sys1000msFlag3 = 0;
	if(s_u8Cnt1000ms1 >= 100) {
		s_u8Cnt1000ms1 = 0;
		g_st_SysTimeFlag.bits.b1Sys1000msFlag1 = 1;		//1000ms��ʱ��־
	}
	if(s_u8Cnt1000ms2 >= 100) {
		s_u8Cnt1000ms2 = 0;
		g_st_SysTimeFlag.bits.b1Sys1000msFlag2 = 1;		//1000ms��ʱ��־
	}
	if(s_u8Cnt1000ms3 >= 100) {
		s_u8Cnt1000ms3 = 0;
		g_st_SysTimeFlag.bits.b1Sys1000msFlag3 = 1;		//1000ms��ʱ��־
		//MCUO_DEBUG_LED2 = !MCUO_DEBUG_LED2;
		// printf("%s\n", "aaavvvf");
		//printf("%d\n", g_stCellInfoReport.u16VCell[0]);
//		printf("%d\n", su32_test++);
//		printf("c is 0x%04x\n", g_stCellInfoReport.u16VCellTotle);
		
	}
}
