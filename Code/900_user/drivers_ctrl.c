#include "drivers_ctrl.h"
#include "System_Monitor.h"
#include "lib_IODrivers.h"
#include "sci_com.h"
#include "CBC.h"
#include "charger_loader_func.h"
#include "sys_timer.h"
#include "n32h47x_48x_gpio.h"
#include "parameter_save.h"
#include "charger_loader_func.h"
#include "io_define.h"
#include "sleep_user.h"
#include "short_current.h"
#include "JiChe_APP.h"


enum RELAY_CTRL_STATUS RelayCtrl_Command = RELAY_PRE_DET;
enum MOS_CTRL_STATUS MOSCtrl_Command = MOS_PRE_DET;

volatile union Switch_OnOFF_Function Switch_OnOFF_Func;
UINT8 gu8_DsgFirstOpen_Flag = 0;


//长期更新数据
void RefreshData_Drivers(void) {
	static UINT8 su8_OnOFF_Status = 0;

	//需要不间断赋值的参数
	Driver_Element.Fault_Flag.all = g_stCellInfoReport.unMdlFault_Third.all;
	Driver_Element.Fault_Flag.bits.b1IdischgOcp = g_stCellInfoReport.unMdlFault_First.bits.b1IdischgOcp;
	Driver_Element.Fault_Flag.bits.b1IdischgOcp |= g_stCellInfoReport.unMdlFault_Second.bits.b1IdischgOcp;
	Driver_Element.Fault_Flag.bits.b1IdischgOcp |= g_stCellInfoReport.unMdlFault_Third.bits.b1IdischgOcp;
	
	Driver_Element.u16_CurChg = g_stCellInfoReport.u16Ichg;
	Driver_Element.u16_CurDsg = g_stCellInfoReport.u16IDischg;

	//信息交换区
	if(SystemStatus.bits.b1Status_BnCloseIO || SystemStatus.bits.b1Status_HeatCloseIO\
		|| SystemStatus.bits.b1Status_ChgCloseIO\
		|| System_ErrFlag.u8ErrFlag_Com_AFE1 || System_ErrFlag.u8ErrFlag_Com_AFE2\
		/*|| System_ErrFlag.u8ErrFlag_Com_EEPROM || System_ErrFlag.u8ErrFlag_Store_EEPROM\*/
		|| CBC_Element.u8CBC_CHG_ErrFlag || CBC_Element.u8CBC_DSG_ErrFlag\
		|| System_ERROR_UserCallback(ERROR_STATUS_TEMP_BREAK)\
		/*|| Sleep_Mode.all & 0x2F != 0*/) {
		Driver_Element.DriverForceExt.bits.b2_DriverOFF_Flag = FORCE_CLOSE_MODE;		//CBC保护放到这里
	}
	else {
		Driver_Element.DriverForceExt.bits.b2_DriverOFF_Flag = FORCE_KEEP_MODE;
	}
	

	//这个写法很巧妙，刚开始执行一个动作，如果执行了，转向下一个动作，不执行则继续等待。相互切换
	switch(su8_OnOFF_Status) {
		case 0:
			if(Driver_Element.u8_FuncOFF_Flag) {
				System_OnOFF_Func.bits.b1OnOFF_MOS_Relay = 0;
				
				if(Driver_Element.MosRelay_Status.bits.b1_FuncOFF_OV) { 	//具体是哪个出事了，留给后续使用
					//Sleep_Mode.bits.b1VcellOVP = 1;		//艾阳动力旧版(深山老林无电枪在线检测功能)使用了
					ChargerLoad_Func.bits.b1OFFDriver_Ovp = 1;
				}
				else if(Driver_Element.MosRelay_Status.bits.b1_FuncOFF_UV) {
					//Sleep_Mode.bits.b1VcellUVP = 1;		//艾阳动力使用了
					ChargerLoad_Func.bits.b1OFFDriver_Uvp = 1;
				}
				else if(Driver_Element.MosRelay_Status.bits.b1_FuncOFF_Ocp_Ichg) {
					ChargerLoad_Func.bits.b1OFFDriver_ChgOcp = 1;
				}
				else if(Driver_Element.MosRelay_Status.bits.b1_FuncOFF_Ocp_Idsg) {
					ChargerLoad_Func.bits.b1OFFDriver_DsgOcp = 1;
				}
				else if(Driver_Element.MosRelay_Status.bits.b1_FuncOFF_Ocp_Imain) {
					//ChargerLoad_Func.bits.b1OFFDriver_Ocp = 1;
					//已被取消
				}
				else if(Driver_Element.MosRelay_Status.bits.b1_FuncOFF_Vdelta) {
					ChargerLoad_Func.bits.b1OFFDriver_Vdelta = 1;
				}

				su8_OnOFF_Status = 1;
			}
			break;
			
		case 1:
			if(System_OnOFF_Func.bits.b1OnOFF_MOS_Relay) {
				Driver_Element.u8_FuncOFF_Flag = 0;		//复原

				if(Driver_Element.MosRelay_Status.bits.b1_FuncOFF_OV) {		//具体是哪个出事了，留给后续使用
					Driver_Element.MosRelay_Status.bits.b1_FuncOFF_OV = 0;
				}
				else if(Driver_Element.MosRelay_Status.bits.b1_FuncOFF_UV) {
					Driver_Element.MosRelay_Status.bits.b1_FuncOFF_UV = 0;
				}
				else if(Driver_Element.MosRelay_Status.bits.b1_FuncOFF_Ocp_Ichg) {
					Driver_Element.MosRelay_Status.bits.b1_FuncOFF_Ocp_Ichg = 0;
				}
				else if(Driver_Element.MosRelay_Status.bits.b1_FuncOFF_Ocp_Idsg) {
					Driver_Element.MosRelay_Status.bits.b1_FuncOFF_Ocp_Idsg = 0;
				}
				else if(Driver_Element.MosRelay_Status.bits.b1_FuncOFF_Ocp_Imain) {
					Driver_Element.MosRelay_Status.bits.b1_FuncOFF_Ocp_Imain = 0;
				}
				else if(Driver_Element.MosRelay_Status.bits.b1_FuncOFF_Vdelta) {
					Driver_Element.MosRelay_Status.bits.b1_FuncOFF_Vdelta = 0;
				}

				su8_OnOFF_Status = 0;
			}
			break;
			
		default:
			break;
	}
}

//这个函数这样写对了吗
void GetData_Drivers(void) {
	SystemStatus.all = ((UINT32)(Driver_Element.MosRelay_Status.all&0x00FE))|(SystemStatus.all&0xFFFFFF01);
}


void InitData_Drivers(void) {
	Driver_Element.u16_PreChg_Time = OtherElement.u16Sys_PreChg_Time;

	Driver_Element.u16_PreChg_Duty = 100;
	Driver_Element.u16_PreChg_Period = 10;

	//Driver_Element.u16_VirCur_Chg = OtherElement.u16Sleep_VirCur_Chg;
	//Driver_Element.u16_VirCur_Dsg = OtherElement.u16Sleep_VirCur_Dsg;
	
	//为了处理管子打开，有可能虚电流导致电池放空的现象(主接触器类型尤为明显
	//MOS带预的驱动(接触器类型没有这个机制)，改为写死2A，充电电流大于2A则退出预充机制，立刻打开放电管
	Driver_Element.u16_VirCur_Chg = 0;
	Driver_Element.u16_VirCur_Dsg = 0;

	Driver_Element.u8_DriverCtrl_Right = 0;			// 内外一起控制
}

// 预充大于90%停止预充
void Drivers_PreCHG(void)
{
	// 预充时间拉长为5s，如果外部充满了，则手动停止预充
	if(Driver_Element.MosRelay_Status.bits.b1Status_MOS_PRE == OPEN_MODE) {
		if(g_stCellInfoReport.u16VCellpack >= (g_stCellInfoReport.u16VCellTotle - g_stCellInfoReport.u16VCellTotle/10)) {
			if(Driver_Element.DriverForceExt.bits.b2_DriverOFF_Flag == FORCE_KEEP_MODE) {
				Driver_Element.DriverForceExt.bits.b2_DriverOFF_Flag = FORCE_CLOSE_PRECHG_MODE;
			}
		}
	} else {
		// 需要手动结束，全凭外部控制。
		// 如果是强制断开，这个也没效果了。
		if(Driver_Element.DriverForceExt.bits.b2_DriverOFF_Flag == FORCE_CLOSE_PRECHG_MODE) {
			Driver_Element.DriverForceExt.bits.b2_DriverOFF_Flag = FORCE_KEEP_MODE;
		}
	}
}


//使用范例
void App_DI1_Switch(void) {
	#ifdef _DI_SWITCH_DSG_ONOFF
	static UINT16 su16_AntiShake_Cnt1 = 0;
	
	#if (defined _MOS)
	if((1 == MCUI_ENI_DI1) && (g_stCellInfoReport.u16Ichg < 20)) {
		if(++su16_AntiShake_Cnt1 >= 2) {
			su16_AntiShake_Cnt1 = 2;
			Driver_Element.DriverForceExt.bits.b2_Force_MOS_DSG = FORCE_CLOSE_MODE;
		}
	}
	else {
		if(su16_AntiShake_Cnt1) {
			--su16_AntiShake_Cnt1;
			return;
		}
		Driver_Element.DriverForceExt.bits.b2_Force_MOS_DSG = FORCE_KEEP_MODE;
	}
	#else
	if(1 == MCUI_ENI_DI1) {
		if(++su16_AntiShake_Cnt1 >= 2) {
			su16_AntiShake_Cnt1 = 2;
			Driver_Element.DriverForceExt.bits.b2_Force_Relay_DSG = FORCE_CLOSE_MODE;
		}
	}
	else {
		if(su16_AntiShake_Cnt1) {
			--su16_AntiShake_Cnt1;
			return;
		}
		Driver_Element.DriverForceExt.bits.b2_Force_Relay_DSG = FORCE_KEEP_MODE;
	}
	#endif
	#endif

	#ifdef _DI_SWITCH_SYS_ONOFF
	static UINT16 su16_AntiShake_Cnt1 = 0;
	
	if(0 == MCUI_DI1) {
		if(++su16_AntiShake_Cnt1 >= 100*3) {
			su16_AntiShake_Cnt1 = 2;
			SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2 = 1;
		}
	}
	else {
		if(su16_AntiShake_Cnt1) {
			--su16_AntiShake_Cnt1;
			return;
		}
		//SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2 = 0;
	}
	#endif
}


void Drivers_External_Ctrl(void)
{
	static UINT8 su8_PreChgStatus = CLOSE_MODE;
	static UINT16 su16_PreChg_Tcnt = 0;
	static UINT8 su8_CtrlERR_PreChg_NUMcnt = 0;

	static UINT8 su8_ChgStatus = CLOSE_MODE;
	static UINT16 su16_Chg_Tcnt = 0;
	static UINT8 su8_CtrlERR_Chg_NUMcnt = 0;

	static UINT8 su8_DsgStatus = CLOSE_MODE;
	static UINT16 su16_Dsg_Tcnt = 0;
	static UINT8 su8_CtrlERR_Dsg_NUMcnt = 0;


	switch(su8_PreChgStatus) {
		case CLOSE_MODE:
			if(Driver_Element.MosRelay_Status.bits.b1Status_MOS_PRE == OPEN_MODE) {
				PCHG_OnOFF(OPEN_MODE);
				if(AFE_Data_ALL.FET_Status.PDSGF == OPEN_MODE) {
					su8_PreChgStatus = OPEN_MODE;
					su16_PreChg_Tcnt = 0;
					su8_CtrlERR_PreChg_NUMcnt = 0;
				} else {
					su16_PreChg_Tcnt++;
				}

				// 出问题，去判断
				if(su16_PreChg_Tcnt >= 100) {
					su16_PreChg_Tcnt = 0;
					su8_PreChgStatus = 3;
				}
			}
			break;
		case OPEN_MODE:
			if(Driver_Element.MosRelay_Status.bits.b1Status_MOS_PRE == CLOSE_MODE) {
				PCHG_OnOFF(CLOSE_MODE);
				if(AFE_Data_ALL.FET_Status.PDSGF == CLOSE_MODE) {
					su8_PreChgStatus = CLOSE_MODE;
					su16_PreChg_Tcnt = 0;
					su8_CtrlERR_PreChg_NUMcnt = 0;
				} else {
					su16_PreChg_Tcnt++;
				}

				// 出问题
				if(su16_PreChg_Tcnt >= 100) {
					su16_PreChg_Tcnt = 0;
					su8_PreChgStatus = 3;
				}
			}
			break;
		case 3:
			if(AFE_Data_ALL.Warning.sBit.SCD) {
				// 如果是短路导致的不一致，不作处理
				su8_PreChgStatus = 4;
			} else {
				// 如果出现这个错误，人为检查。
				su8_CtrlERR_PreChg_NUMcnt++;
				if(su8_CtrlERR_PreChg_NUMcnt >= 20) {
					su8_PreChgStatus = 0xFF;
				}

				System_ERROR_UserCallback(ERROR_BLUETOOTH);
				CleanError();
				// 回到之前的状态继续检测
				su8_PreChgStatus = AFE_Data_ALL.FET_Status.PDSGF;
			}
			break;

		case 4:
			// 等待短路解除
			if(AFE_Data_ALL.Warning.sBit.SCD == 0) {
				su8_PreChgStatus = CLOSE_MODE;
			}
			break;

		case 0xFF:
			// 等重启
			System_ERROR_UserCallback(ERROR_AFE1);
			break;

		default:
			break;
	}

	switch(su8_ChgStatus) {
		case CLOSE_MODE:
			if(Driver_Element.MosRelay_Status.bits.b1Status_MOS_CHG == OPEN_MODE) {
				CHG_OnOFF(OPEN_MODE);
				if(AFE_Data_ALL.FET_Status.CHGF == OPEN_MODE) {
					su8_ChgStatus = OPEN_MODE;
					su16_Chg_Tcnt = 0;
					su8_CtrlERR_Chg_NUMcnt = 0;
				} else {
					su16_Chg_Tcnt++;
				}

				// 出问题，去判断
				if(su16_Chg_Tcnt >= 100) {
					su16_Chg_Tcnt = 0;
					su8_ChgStatus = 3;
				}
			}
			break;
		case OPEN_MODE:
			if(Driver_Element.MosRelay_Status.bits.b1Status_MOS_CHG == CLOSE_MODE) {
				CHG_OnOFF(CLOSE_MODE);
				if(AFE_Data_ALL.FET_Status.CHGF == CLOSE_MODE) {
					su8_ChgStatus = CLOSE_MODE;
					su16_Chg_Tcnt = 0;
					su8_CtrlERR_Chg_NUMcnt = 0;
				} else {
					su16_Chg_Tcnt++;
				}

				// 出问题
				if(su16_Chg_Tcnt >= 100) {
					su16_Chg_Tcnt = 0;
					su8_ChgStatus = 3;
				}
			}
			break;
		case 3:
			if(AFE_Data_ALL.Warning.sBit.SCD) {
				// 如果是短路导致的不一致，不作处理
				su8_ChgStatus = 4;
			} else {
				// 如果出现这个错误，人为检查。
				su8_CtrlERR_Chg_NUMcnt++;
				if(su8_CtrlERR_Chg_NUMcnt >= 20) {
					su8_ChgStatus = 0xFF;
				}

				System_ERROR_UserCallback(ERROR_BLUETOOTH);
				CleanError();
				// 回到之前的状态继续检测
				su8_ChgStatus = AFE_Data_ALL.FET_Status.CHGF;
			}
			break;

		case 4:
			// 等待短路解除
			if(AFE_Data_ALL.Warning.sBit.SCD == 0) {
				su8_ChgStatus = CLOSE_MODE;
			}
			break;

		case 0xFF:
			// 等重启
			System_ERROR_UserCallback(ERROR_AFE1);
			break;

		default:
			break;
	}

	switch(su8_DsgStatus) {
		case CLOSE_MODE:
			if(Driver_Element.MosRelay_Status.bits.b1Status_MOS_DSG == OPEN_MODE) {
				DSG_OnOFF(OPEN_MODE);
				if(AFE_Data_ALL.FET_Status.DSGF == OPEN_MODE) {
					su8_DsgStatus = OPEN_MODE;
					su16_Dsg_Tcnt = 0;
					su8_CtrlERR_Dsg_NUMcnt = 0;
				} else {
					su16_Dsg_Tcnt++;
				}

				// 出问题，去判断
				if(su16_Dsg_Tcnt >= 100) {
					su16_Dsg_Tcnt = 0;
					su8_DsgStatus = 3;
				}
			}
			break;
		case OPEN_MODE:
			if(Driver_Element.MosRelay_Status.bits.b1Status_MOS_DSG == CLOSE_MODE) {
				DSG_OnOFF(CLOSE_MODE);
				if(AFE_Data_ALL.FET_Status.DSGF == CLOSE_MODE) {
					su8_DsgStatus = CLOSE_MODE;
					su16_Dsg_Tcnt = 0;
					su8_CtrlERR_Dsg_NUMcnt = 0;
				} else {
					su16_Dsg_Tcnt++;
				}

				// 出问题
				if(su16_Dsg_Tcnt >= 100) {
					su16_Dsg_Tcnt = 0;
					su8_DsgStatus = 3;
				}
			}
			break;
		case 3:
			if(AFE_Data_ALL.Warning.sBit.SCD) {
				// 如果是短路导致的不一致，不作处理
				su8_DsgStatus = 4;
			} else {
				// 如果出现这个错误，人为检查。
				su8_CtrlERR_Dsg_NUMcnt++;
				if(su8_CtrlERR_Dsg_NUMcnt >= 20) {
					su8_DsgStatus = 0xFF;
				}

				System_ERROR_UserCallback(ERROR_BLUETOOTH);
				CleanError();
				// 回到之前的状态继续检测
				su8_DsgStatus = AFE_Data_ALL.FET_Status.DSGF;
			}
			break;

		case 4:
			// 等待短路解除
			if(AFE_Data_ALL.Warning.sBit.SCD == 0) {
				su8_DsgStatus = CLOSE_MODE;
			}
			break;

		case 0xFF:
			// 等重启
			System_ERROR_UserCallback(ERROR_AFE1);
			break;

		default:
			break;
	}
}


void InitMosRelay_DOx(void) {
	//PE9_MCUO_RELAY_PRE，PE9_MCUO_MOS_PRE
	InitDrivers_GPIO(RELAY_PRECHG_PORT, RELAY_PRECHG_PIN, GPIO_PreCHG);
	
	//PC6_MCUO_RELAY_CHG，PC6_MCUO_MOS_CHG
	InitDrivers_GPIO(RELAY_CHG_PORT, RELAY_CHG_PIN, GPIO_CHG);
	
	//PE14_MCUO_RELAY_DSG，PE14_MCUO_MOS_DSG
	InitDrivers_GPIO(RELAY_DSG_PORT, RELAY_DSG_PIN, GPIO_DSG);

	//PE14_MCUO_RELAY_MAIN，约定放电管为主接触器
	InitDrivers_GPIO(RELAY_DSG_PORT, RELAY_DSG_PIN, GPIO_MAIN);

	InitData_Drivers();
}


void App_MOS_Relay_Ctrl(void)
{
	#if defined(_MOS)	
	if(STARTUP_CONT == System_FUNC_StartUp(SYSTEM_FUNC_STARTUP_MOS)) {
		return;
	}
	#elif defined(_RELAY)	//同口分口问题
	if(STARTUP_CONT == System_FUNC_StartUp(SYSTEM_FUNC_STARTUP_RELAY)) {
		// return;
	}
	#endif
	
	if(0 == g_st_SysTimeFlag.bits.b1Sys10msFlag1) {
		return;
	}

	Drivers_PreCHG();
	App_DI1_Switch();
	RefreshData_Drivers();
	GetData_Drivers();
	Drivers_External_Ctrl();

	#if (defined _RELAY_SAME_DOOR_NO_PRECHG)
	Drivers_Ctrl(g_st_SysTimeFlag.bits.b1Sys10msFlag1, System_OnOFF_Func.bits.b1OnOFF_MOS_Relay, DRIVER_RELAY_SAME_DOOR_NO_PRECHG);
	#elif (defined _RELAY_SAME_DOOR_HAVE_PRECHG)
	Drivers_Ctrl(g_st_SysTimeFlag.bits.b1Sys10msFlag1, System_OnOFF_Func.bits.b1OnOFF_MOS_Relay, DRIVER_RELAY_SAME_DOOR_HAVE_PRECHG);
	#elif (defined _RELAY_DIFF_DOOR_NO_PRECHG)
	Drivers_Ctrl(g_st_SysTimeFlag.bits.b1Sys10msFlag1, System_OnOFF_Func.bits.b1OnOFF_MOS_Relay, DRIVER_RELAY_DIFF_DOOR_NO_PRECHG);
	#elif (defined _RELAY_DIFF_DOOR_HAVE_PRECHG)
	Drivers_Ctrl(g_st_SysTimeFlag.bits.b1Sys10msFlag1, System_OnOFF_Func.bits.b1OnOFF_MOS_Relay, DRIVER_RELAY_DIFF_DOOR_HAVE_PRECHG);
	#elif (defined _MOS_SAME_DOOR_NO_PRECHG)
	Drivers_Ctrl(g_st_SysTimeFlag.bits.b1Sys10msFlag1, System_OnOFF_Func.bits.b1OnOFF_MOS_Relay, DRIVER_MOS_SAME_DOOR_NO_PRECHG);
	#elif (defined _MOS_SAME_DOOR_HAVE_PRECHG)
	Drivers_Ctrl(g_st_SysTimeFlag.bits.b1Sys10msFlag1, System_OnOFF_Func.bits.b1OnOFF_MOS_Relay, DRIVER_MOS_SAME_DOOR_HAVE_PRECHG);
	#elif (defined _MOS_BOOTSTRAP_CIR)
	Drivers_Ctrl(g_st_SysTimeFlag.bits.b1Sys10msFlag1, System_OnOFF_Func.bits.b1OnOFF_MOS_Relay, DRIVER_MOS_BOOTSTRAP_CIR);
	#endif

	// MCUO_CHG_EN = 1;
	// MCUO_DSG_EN = 1;
}


void InitSwitchData_EEPROM(void) {
	Switch_OnOFF_Func.all = (UINT32)ReadEEPROM_Word_WithZone(EEPROM_ADDR_SWITCH_ONOFF);
	Switch_OnOFF_Func.all |= ((UINT32)ReadEEPROM_Word_WithZone(EEPROM_ADDR_SWITCH_ONOFF + 2)<<16);		//先扩大为32位再移位
}


//这个函数修改了，要修改EEPROM的上电标志位
void SwitchResetData_EEPROM(void) {
	Switch_OnOFF_Func.all = 0;
	WriteEEPROM_Word_WithZone(EEPROM_ADDR_SWITCH_ONOFF, (UINT16)(Switch_OnOFF_Func.all&0x0000FFFF));
	WriteEEPROM_Word_WithZone(EEPROM_ADDR_SWITCH_ONOFF + 2, (UINT16)(Switch_OnOFF_Func.all>>16));
}


// 一个测试功能的开放
void App_Switch_Ctrl(void) {
	if(0 == g_st_SysTimeFlag.bits.b1Sys10msFlag2) {
		return;
	}

}
