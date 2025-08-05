#include "uart_lcd.h"


#include "data_deal.h"
#include "System_Monitor.h"
#include "uart_upper.h"
#include "ProductionID.h"

typedef enum _R_LCD_FILL_DATA_TYPE {
	LCD_FILL_DATA_WARNING,
	LCD_FILL_DATA_PROTECT,
	LCD_FILL_DATA_SYS_FAULT,
	LCD_FILL_DATA_SYS_STATUS,
	LCD_FILL_DATA_FUNCTION_STATUS,
	LCD_FILL_DATA_PROTECT_STATUS2,
	LCD_FILL_DATA_FUNCTION_STATUS2,
}R_LCD_FILL_DATA_TYPE;

typedef union __LCD_WARNING {
    UINT16 all;
    struct _LCD_WARNING {
        UINT8 b1_Warn_CellOvp			    :1;   	//
        UINT8 b1_Warn_CellUvp			    :1;   	//
        UINT8 b1_Warn_BatOvp			    :1;   	//
        UINT8 b1_Warn_BatUvp			    :1;   	//
        
        UINT8 b1_Warn_Res1	 		        :1;   	//
	    UINT8 b1_Warn_Res2  		        :1;   	//
        UINT8 b1_Warn_IchgOcp			    :1;   	//
        UINT8 b1_Warn_IdischgOcp		    :1;   	//

        UINT8 b1_Warn_CellChgOtp		    :1;   	//
        UINT8 b1_Warn_CellDischgOtp 	    :1;   	//
	    UINT8 b1_Warn_CellChgUtp		    :1;   	//
	    UINT8 b1_Warn_CellDischgUtp 	    :1;   	//

        UINT8 b1_Warn_EnvOtp		        :1;   	//
	    UINT8 b1_Warn_EnvUtp 	            :1;   	//
	    UINT8 b1_Warn_TmosOtp			    :1;   	//
        UINT8 b1_Warn_SocLow			    :1;   	//
    }bits;
}LCD_WARNING;

typedef union __LCD_PROTECTION {
    UINT16 all;
    struct _LCD_PROTECTION {
        UINT8 b1_Protect_CellOvp			:1;   	//
        UINT8 b1_Protect_CellUvp			:1;   	//
        UINT8 b1_Protect_BatOvp			    :1;   	//
        UINT8 b1_Protect_BatUvp			    :1;   	//
        
        UINT8 b1_Protect_CBC	 		    :1;   	//
	    UINT8 b1_Protect_Cheat  		    :1;   	//
        UINT8 b1_Protect_IchgOcp			:1;   	//
        UINT8 b1_Protect_IdischgOcp		    :1;   	//

        UINT8 b1_Protect_CellChgOtp		    :1;   	//
        UINT8 b1_Protect_CellDischgOtp 	    :1;   	//
	    UINT8 b1_Protect_CellChgUtp		    :1;   	//
	    UINT8 b1_Protect_CellDischgUtp 	    :1;   	//

        UINT8 b1_Protect_EnvOtp		        :1;   	//
	    UINT8 b1_Protect_EnvUtp 	        :1;   	//
	    UINT8 b1_Protect_TmosOtp			:1;   	//
        UINT8 b1_Protect_Unhook			    :1;   	//
    }bits;
}LCD_PROTECTION;

typedef union __LCD_SYS_FAULT {
    UINT16 all;
    struct _LCD_SYS_FAULT {
        UINT8 b1_FAULT_CHG_MOS	            :1;   	//
        UINT8 b1_FAULT_DSG_MOS	            :1;   	//
        UINT8 b1_FAULT_NTC		            :1;   	//
        UINT8 b1_FAULT_RES1		            :1;   	//
        
        UINT8 b1_FAULT_Cell	                :1;   	//
	    UINT8 b1_FAULT_AFE                  :1;   	//
        UINT8 b1_FAULT_RES2	                :1;   	//
        UINT8 b1_FAULT_LimitCur	            :1;   	//

        UINT8 b1_FAULT_DCDC		            :1;   	//
        UINT8 b1_FAULT_RES3 	            :1;   	//
	    UINT8 b1_FAULT_RES4		            :1;   	//
	    UINT8 b1_FAULT_RES5 	            :1;   	//

        UINT8 b1_FAULT_RES6		            :1;   	//
	    UINT8 b1_FAULT_RES7 	            :1;   	//
	    UINT8 b1_FAULT_RES8		            :1;   	//
        UINT8 b1_FAULT_HEAT		            :1;   	//
    }bits;
}LCD_SYS_FAULT;

typedef union __LCD_SYS_STATUS {
    UINT16 all;
    struct _LCD_SYS_STATUS {
        UINT8 b1_Status_LimitCur	        :1;   	//
        UINT8 b1_Status_ChgMos	            :1;   	//
        UINT8 b1_Status_DsgMos		        :1;   	//
        UINT8 b1_Status_res1		        :1;   	//
        
        UINT8 b1_Status_Chg_Reverse	        :1;   	//
	    UINT8 b1_Status_ACIN                :1;   	//
        UINT8 b1_Status_res2	            :1;   	//
        UINT8 b1_Status_Heat	            :1;   	//

        UINT8 b1_Status_ChgNow		        :1;   	//
        UINT8 b1_Status_DsgNow 	            :1;   	//
	    UINT8 b1_Status_FullNow		        :1;   	//
	    UINT8 b1_Status_IdleNow 	        :1;   	//

        UINT8 b1_Status_res3		        :1;   	//
	    UINT8 b1_Status_res4 	            :1;   	//
	    UINT8 b1_Status_res5		        :1;   	//
        UINT8 b1_Status_res6		        :1;   	//
    }bits;
}LCD_SYS_STATUS;

typedef union __LCD_FUNCTION_STATUS {
    UINT16 all;
    struct _LCD_FUNCTION_STATUS {
        UINT8 b1_FunStatus_LimitCur	        :1;   	//
        UINT8 b1_FunStatus_Hook	            :1;   	//
        UINT8 b1_FunStatus_Bell		        :1;   	//
        UINT8 b1_FunStatus_WarnLED		    :1;   	//
        
        UINT8 b1_FunStatus_Heat	            :1;   	//
	    UINT8 b1_FunStatus_Cheat            :1;   	//
        UINT8 b1_FunStatus_CellOvp	        :1;   	//
        UINT8 b1_FunStatus_CellUvp	        :1;   	//

        UINT8 b1_FunStatus_BatOvp		    :1;   	//
        UINT8 b1_FunStatus_BatUvp 	        :1;   	//
	    UINT8 b1_FunStatus_IchgOcp		    :1;   	//
	    UINT8 b1_FunStatus_IdischgOcp 	    :1;   	//

        UINT8 b1_FunStatus_CellOtp		    :1;   	//
	    UINT8 b1_FunStatus_CellUtp 	        :1;   	//
	    UINT8 b1_FunStatus_EnvOtp		    :1;   	//
        UINT8 b1_FunStatus_MOSOtp		    :1;   	//
    }bits;
}LCD_FUNCTION_STATUS;

typedef union __LCD_PROTECTION2 {
    UINT16 all;
    struct _LCD_PROTECTION2 {
        UINT8 b1_Protect2_Bat2			    :1;   	//
        UINT8 b1_Protect2_Cur2			    :1;   	//
        UINT8 b1_Protect2_res1			    :1;   	//
        UINT8 b1_Protect2_res2			    :1;   	//

        UINT8 b1_Protect2_res3	 		    :1;   	//
	    UINT8 b1_Protect2_res4  		    :1;   	//
        UINT8 b1_Protect2_res5			    :1;   	//
        UINT8 b1_Protect2_res6		        :1;   	//

        UINT8 b1_Protect2_res7		        :1;   	//
        UINT8 b1_Protect2_res8 	            :1;   	//
	    UINT8 b1_Protect2_res9		        :1;   	//
	    UINT8 b1_Protect2_res10 	        :1;   	//

        UINT8 b1_Protect2_res11		        :1;   	//
	    UINT8 b1_Protect2_res12 	        :1;   	//
	    UINT8 b1_Protect2_res13			    :1;   	//
        UINT8 b1_Protect2_res14			    :1;   	//
    }bits;
}LCD_PROTECTION2;

typedef union __LCD_FUNCTION2 {
    UINT16 all;
    struct _LCD_FUNCTION2 {
        UINT8 b1_Func2_Bat2			        :1;   	//
        UINT8 b1_Func2_Cur2			        :1;   	//
        UINT8 b1_Func2_res1			        :1;   	//
        UINT8 b1_Func2_res2			        :1;   	//

        UINT8 b1_Func2_res3	 		        :1;   	//
	    UINT8 b1_Func2_res4  		        :1;   	//
        UINT8 b1_Func2_res5			        :1;   	//
        UINT8 b1_Func2_res6		            :1;   	//

        UINT8 b1_Func2_res7		            :1;   	//
        UINT8 b1_Func2_res8 	            :1;   	//
	    UINT8 b1_Func2_res9		            :1;   	//
	    UINT8 b1_Func2_res10 	            :1;   	//

        UINT8 b1_Func2_res11		        :1;   	//
	    UINT8 b1_Func2_res12 	            :1;   	//
	    UINT8 b1_Func2_res13			    :1;   	//
        UINT8 b1_Func2_res14			    :1;   	//
    }bits;
}LCD_FUNCTION2;


LCD_WARNING LCD_Warning;
LCD_PROTECTION LCD_Protection;
LCD_SYS_FAULT LCD_SysFault;
LCD_SYS_STATUS LCD_SysStatus;
LCD_FUNCTION_STATUS LCD_FunctionStatus;
LCD_PROTECTION2 LCD_Protect2;
LCD_FUNCTION2 LCD_Function2;

static UINT16 R_LCD_FillData(R_LCD_FILL_DATA_TYPE type)
{
    UINT16 result = 0;

	switch(type) {
		case LCD_FILL_DATA_WARNING:
            LCD_Warning.bits.b1_Warn_CellOvp = g_stCellInfoReport.unMdlFault_Second.bits.b1CellOvp;
            LCD_Warning.bits.b1_Warn_CellUvp = g_stCellInfoReport.unMdlFault_Second.bits.b1CellUvp;
            LCD_Warning.bits.b1_Warn_BatOvp = g_stCellInfoReport.unMdlFault_Second.bits.b1BatOvp;
            LCD_Warning.bits.b1_Warn_BatUvp = g_stCellInfoReport.unMdlFault_Second.bits.b1BatUvp;

            LCD_Warning.bits.b1_Warn_Res1 = 0;
            LCD_Warning.bits.b1_Warn_Res2 = 0;
            LCD_Warning.bits.b1_Warn_IchgOcp = g_stCellInfoReport.unMdlFault_Second.bits.b1IchgOcp;
            LCD_Warning.bits.b1_Warn_IdischgOcp = g_stCellInfoReport.unMdlFault_Second.bits.b1IdischgOcp;

            LCD_Warning.bits.b1_Warn_CellChgOtp = g_stCellInfoReport.unMdlFault_Second.bits.b1CellChgOtp;
            LCD_Warning.bits.b1_Warn_CellDischgOtp = g_stCellInfoReport.unMdlFault_Second.bits.b1CellDischgOtp;
            LCD_Warning.bits.b1_Warn_CellChgUtp = g_stCellInfoReport.unMdlFault_Second.bits.b1CellChgUtp;
            LCD_Warning.bits.b1_Warn_CellDischgUtp = g_stCellInfoReport.unMdlFault_Second.bits.b1CellDischgUtp;

            LCD_Warning.bits.b1_Warn_EnvOtp = 0;
            LCD_Warning.bits.b1_Warn_EnvUtp = 0;
            LCD_Warning.bits.b1_Warn_TmosOtp = g_stCellInfoReport.unMdlFault_Second.bits.b1TmosOtp;
            LCD_Warning.bits.b1_Warn_SocLow = g_stCellInfoReport.unMdlFault_Second.bits.b1SocLow;

            result = LCD_Warning.all;
			break;

		case LCD_FILL_DATA_PROTECT:
            LCD_Protection.bits.b1_Protect_CellOvp = g_stCellInfoReport.unMdlFault_Third.bits.b1CellOvp;
            LCD_Protection.bits.b1_Protect_CellUvp = g_stCellInfoReport.unMdlFault_Third.bits.b1CellUvp;
            LCD_Protection.bits.b1_Protect_BatOvp = g_stCellInfoReport.unMdlFault_Third.bits.b1BatOvp;
            LCD_Protection.bits.b1_Protect_BatUvp = g_stCellInfoReport.unMdlFault_Third.bits.b1BatUvp;

            LCD_Protection.bits.b1_Protect_CBC = 0;
            LCD_Protection.bits.b1_Protect_Cheat = 0;
            LCD_Protection.bits.b1_Protect_IchgOcp = g_stCellInfoReport.unMdlFault_Third.bits.b1IchgOcp;
            LCD_Protection.bits.b1_Protect_IdischgOcp = g_stCellInfoReport.unMdlFault_Third.bits.b1IdischgOcp;

            LCD_Protection.bits.b1_Protect_CellChgOtp = g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgOtp;
            LCD_Protection.bits.b1_Protect_CellDischgOtp = g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgOtp;
            LCD_Protection.bits.b1_Protect_CellChgUtp = g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgUtp;
            LCD_Protection.bits.b1_Protect_CellDischgUtp = g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgUtp;

            LCD_Protection.bits.b1_Protect_EnvOtp = 0;
            LCD_Protection.bits.b1_Protect_EnvUtp = 0;
            LCD_Protection.bits.b1_Protect_TmosOtp = g_stCellInfoReport.unMdlFault_Third.bits.b1TmosOtp;
            LCD_Protection.bits.b1_Protect_Unhook = 0;

            result = LCD_Protection.all;
			break;

		case LCD_FILL_DATA_SYS_FAULT:
            LCD_SysFault.bits.b1_FAULT_CHG_MOS = 0;
            LCD_SysFault.bits.b1_FAULT_DSG_MOS = 0;
            LCD_SysFault.bits.b1_FAULT_NTC = System_ERROR_UserCallback(ERROR_STATUS_TEMP_BREAK) >= 1 ? 1:0;
            LCD_SysFault.bits.b1_FAULT_RES1 = 0;

            LCD_SysFault.bits.b1_FAULT_Cell = 0;
            LCD_SysFault.bits.b1_FAULT_LimitCur = 0;
            LCD_SysFault.bits.b1_FAULT_AFE = (System_ERROR_UserCallback(ERROR_STATUS_AFE1)\
                                             + System_ERROR_UserCallback(ERROR_STATUS_AFE2)) >= 1 ? 1 : 0;
            LCD_SysFault.bits.b1_FAULT_DCDC = 0;

            result = LCD_SysFault.all;
			break;

		case LCD_FILL_DATA_SYS_STATUS:
            LCD_SysStatus.bits.b1_Status_LimitCur = 0;
            LCD_SysStatus.bits.b1_Status_ChgMos = SystemStatus.bits.b1Status_MOS_CHG;
            LCD_SysStatus.bits.b1_Status_DsgMos = SystemStatus.bits.b1Status_MOS_DSG;
            LCD_SysStatus.bits.b1_Status_Chg_Reverse = 0;

            LCD_SysStatus.bits.b1_Status_ACIN = 0;
            LCD_SysStatus.bits.b1_Status_Heat = SystemStatus.bits.b1Status_Heat;
            LCD_SysStatus.bits.b1_Status_ChgNow = g_stCellInfoReport.u16Ichg > 0 ? 1 : 0;
            LCD_SysStatus.bits.b1_Status_DsgNow = g_stCellInfoReport.u16IDischg > 0 ? 1 : 0;

            LCD_SysStatus.bits.b1_Status_FullNow = g_stCellInfoReport.SocElement.u16Soc >= 100 ? 1 : 0;
            LCD_SysStatus.bits.b1_Status_IdleNow = !(LCD_SysStatus.bits.b1_Status_ChgNow + LCD_SysStatus.bits.b1_Status_DsgNow);

            result = LCD_SysStatus.all;
			break;

		case LCD_FILL_DATA_FUNCTION_STATUS:
            LCD_FunctionStatus.bits.b1_FunStatus_LimitCur = 0;
            LCD_FunctionStatus.bits.b1_FunStatus_Hook = 0;
            LCD_FunctionStatus.bits.b1_FunStatus_Bell = 0;
            LCD_FunctionStatus.bits.b1_FunStatus_WarnLED = 0;

            LCD_FunctionStatus.bits.b1_FunStatus_Heat = 0;
            LCD_FunctionStatus.bits.b1_FunStatus_Cheat = 0;
            LCD_FunctionStatus.bits.b1_FunStatus_CellOvp = 1;
            LCD_FunctionStatus.bits.b1_FunStatus_CellUvp = 1;

            LCD_FunctionStatus.bits.b1_FunStatus_BatOvp = 1;
            LCD_FunctionStatus.bits.b1_FunStatus_BatUvp = 1;
            LCD_FunctionStatus.bits.b1_FunStatus_IchgOcp = 1;
            LCD_FunctionStatus.bits.b1_FunStatus_IdischgOcp = 1;

            LCD_FunctionStatus.bits.b1_FunStatus_CellOtp = 1;
            LCD_FunctionStatus.bits.b1_FunStatus_CellUtp = 1;
            LCD_FunctionStatus.bits.b1_FunStatus_EnvOtp = 0;
            LCD_FunctionStatus.bits.b1_FunStatus_MOSOtp = 1;

            result = LCD_FunctionStatus.all;
			break;

		case LCD_FILL_DATA_PROTECT_STATUS2:
            LCD_Protect2.bits.b1_Protect2_Bat2 = 0;
            LCD_Protect2.bits.b1_Protect2_Cur2 = 0;
            result = LCD_Protect2.all;
			break;

		case LCD_FILL_DATA_FUNCTION_STATUS2:
            LCD_Function2.bits.b1_Func2_Bat2 = 0;
            LCD_Function2.bits.b1_Func2_Cur2 = 0;
            result = LCD_Protect2.all;
			break;

		default:
            result = 0;
			break;
	}

    return result;
}

void Uart_R_PC_LCD_ACK_0x03(UINT8 t_u8BuffTemp[])
{
	UINT16	u16SciTemp;
	UINT16	i,j;
	i = 0;

	// 充电为正，放电为负
	if(g_stCellInfoReport.u16IDischg) {
		u16SciTemp = 0xFFFF - g_stCellInfoReport.u16IDischg + 1;
	} else {
		u16SciTemp = g_stCellInfoReport.u16Ichg;
	}
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = g_stCellInfoReport.u16VCellTotle;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = g_stCellInfoReport.SocElement.u16Soc;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = g_stCellInfoReport.SocElement.u16Soh;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = g_stCellInfoReport.SocElement.u16CapacityNow;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = g_stCellInfoReport.SocElement.u16CapacityFull;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = g_stCellInfoReport.SocElement.u16CapacityFactory;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = g_stCellInfoReport.SocElement.u16Cycle_times;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// 预留
	u16SciTemp = 0;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// 告警状态
	u16SciTemp = R_LCD_FillData(LCD_FILL_DATA_WARNING);
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// 保护状态
	u16SciTemp = R_LCD_FillData(LCD_FILL_DATA_PROTECT);
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// 系统故障状态
	u16SciTemp = R_LCD_FillData(LCD_FILL_DATA_SYS_FAULT);
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// 系统状态
	u16SciTemp = R_LCD_FillData(LCD_FILL_DATA_SYS_STATUS);
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// 功能开关状态
	u16SciTemp = R_LCD_FillData(LCD_FILL_DATA_FUNCTION_STATUS);
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// 保护状态2
	u16SciTemp = R_LCD_FillData(LCD_FILL_DATA_PROTECT_STATUS2);
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// 功能开关状态2
	u16SciTemp = R_LCD_FillData(LCD_FILL_DATA_FUNCTION_STATUS2);
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// 预留
	u16SciTemp = 0;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = SeriesNum;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = g_stCellInfoReport.u16VCellMax;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = g_stCellInfoReport.u16VCellMin;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// 电芯温度个数
	u16SciTemp = 6;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// 电芯温度最大值
	u16SciTemp = (g_stCellInfoReport.u16TempMax - 400);
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// 电芯温度最小值
	u16SciTemp = (g_stCellInfoReport.u16TempMin - 400);
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// MOS管温度
	u16SciTemp = g_stCellInfoReport.u16Temperature[MOS_TEMP1];
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// 环境温度。
	u16SciTemp = (g_stCellInfoReport.u16TempMax - 400);
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// 预留1
	u16SciTemp = 0;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// 预留2
	u16SciTemp = 0;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;


	for(j = 0; j < 32; j++) {
		u16SciTemp = *(&g_stCellInfoReport.u16VCell[0]+j);
		t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
	}

	for(j = 0; j < 6; j++) {
		u16SciTemp = *(&g_stCellInfoReport.u16Temperature[0]+j);
		t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
	}

	// 电芯温度7
	u16SciTemp = 0;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// 电芯温度8
	u16SciTemp = 0;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// 单体电压最大位置
	u16SciTemp = g_stCellInfoReport.u16VCellMaxPosition;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// 单体电压最小位置
	u16SciTemp = g_stCellInfoReport.u16VCellMinPosition;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// 单体温度最大位置
	u16SciTemp = gu8_TcellMaxPosition;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// 单体温度最小位置
	u16SciTemp = gu8_TcellMinPosition;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// 保留
	u16SciTemp = 0;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = g_stCellInfoReport.u16BalanceFlag1;	// 200
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = g_stCellInfoReport.u16BalanceFlag2;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	#if 1	//TODO TEST
	u16SciTemp = 0;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// 并联电池组数
	u16SciTemp = 0;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// 并联状态
	u16SciTemp = 0;
	t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	for(j = 0; j < 10; j++) {
		u16SciTemp = 0;
		t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
	}	// 到这里92

	// BMS版本信息
	for(j = 0; j < 20; j++) {
		t_u8BuffTemp[i++] = ProductionInfor.BMS_SerialNumber[j];
	}
	// BMS生产信息
	for(j = 0; j < 20; j++) {
		t_u8BuffTemp[i++] = ProductionInfor.BMS_HardWareVersion[j];
	}
	// BMS PACK生产信息
	for(j = 0; j < 20; j++) {                   // 249
		t_u8BuffTemp[i++] = ProductionInfor.BMS_SoftWareVersion[j];
	}
	#endif
}

