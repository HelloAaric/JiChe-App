#ifndef IODRIVERS_H
#define IODRIVERS_H

#include "n32h47x_48x.h"

typedef uint32_t  UINT32;
typedef uint16_t UINT16;
typedef uint8_t  UINT8;

typedef enum _DRIVERS_STATUS {
	CLOSE_MODE = 0,
	OPEN_MODE = 1
}DriversStatus;

typedef enum _FORCE_STATUS {
	FORCE_KEEP_MODE = 0,
	FORCE_OPEN_MODE,
	FORCE_CLOSE_MODE,
	FORCE_CLOSE_PRECHG_MODE,
}FORCE_STATUS;

typedef enum GPIO_TYPE {
	GPIO_PreCHG = 0,
	GPIO_CHG,
	GPIO_DSG,
	GPIO_MAIN,
}GPIO_Type;


typedef union DRIVER_ONOFF_ETC {
    UINT16 all;
    struct OnOFF_Result {
		UINT8 b1Status_ToSleep				:1; 	//
		DriversStatus b1Status_MOS_PRE      :1;		//预充MOS管功能状态
		DriversStatus b1Status_MOS_CHG      :1;		//充电MOS管功能状态
		DriversStatus b1Status_MOS_DSG      :1;		//放电MOS管功能状态
		
		DriversStatus b1Status_Relay_PRE    :1;		//预充继电器功能状态
		DriversStatus b1Status_Relay_CHG    :1;		//分口充电继电器功能状态
		DriversStatus b1Status_Relay_DSG    :1;		//分口放电继电器功能状态
		DriversStatus b1Status_Relay_MAIN   :1;		//同口主继电器功能状态
		
		UINT8 b1_FuncOFF_OV					:1;		//连续过压保护导致IO功能关闭，仅用于接触器驱动方案
		UINT8 b1_FuncOFF_UV					:1;		//连续低压保护导致IO功能关闭，仅用于接触器驱动方案
		UINT8 b1_FuncOFF_Ocp_Ichg			:1;		//连续充电过流保护导致IO功能关闭
		UINT8 b1_FuncOFF_Ocp_Idsg			:1;		//连续放电过流保护导致IO功能关闭
		
		UINT8 b1_FuncOFF_Ocp_Imain			:1;		//连续过流保护(只有一个主接触器版本使用)导致IO功能关闭
													//被取消，严格按照充放电电流过流处理
		UINT8 b1_FuncOFF_Vdelta				:1;		//压差过大保护导致IO功能关闭
		UINT8 Res   						:2;		//
    }bits;
}Driver_OnOFF_Etc;

typedef union _DRIVER_FORCE_EXTERNAL {
    UINT16 all;
    struct Force_Result {
		FORCE_STATUS b2_DriverOFF_Flag		:2; 	//外部强制关闭驱动标志位
		FORCE_STATUS b2_Force_MOS_PRE		:2;		//外部强制控制预充MOS管，能强制OPEN，和强制CLOSE，默认解除控制为FORCE_KEEP_MODE
		FORCE_STATUS b2_Force_MOS_CHG      	:2;		//外部强制控制充电MOS管，能强制OPEN，和强制CLOSE，默认解除控制为FORCE_KEEP_MODE
		FORCE_STATUS b2_Force_MOS_DSG      	:2;		//外部强制控制放电MOS管，能强制OPEN，和强制CLOSE，默认解除控制为FORCE_KEEP_MODE
		
		FORCE_STATUS b2_Force_Relay_PRE    	:2;		//外部强制控制预充继电器，能强制OPEN，和强制CLOSE，默认解除控制为FORCE_KEEP_MODE
		FORCE_STATUS b2_Force_Relay_CHG    	:2;		//外部强制控制分口充电继电器，能强制OPEN，和强制CLOSE，默认解除控制为FORCE_KEEP_MODE
		FORCE_STATUS b2_Force_Relay_DSG    	:2;		//外部强制控制分口放电继电器，能强制OPEN，和强制CLOSE，默认解除控制为FORCE_KEEP_MODE
		FORCE_STATUS b2_Force_Relay_MAIN   	:2;		//外部强制控制同口主继电器，能强制OPEN，和强制CLOSE，默认解除控制为FORCE_KEEP_MODE
    }bits;
}DRIVER_FORCE_EXT;

typedef union FAULT_FLAG {
    UINT16 all;
    struct Fault_Flag_UNION {
		UINT8 b1CellOvp 		:1; 	//
		UINT8 b1CellUvp 		:1; 	//
		UINT8 b1BatOvp			:1; 	//
		UINT8 b1BatUvp			:1; 	//
		
		UINT8 b1IchgOcp 		:1; 	//
		UINT8 b1IdischgOcp		:1; 	//
		UINT8 b1CellChgOtp		:1; 	//
		UINT8 b1CellDischgOtp	:1; 	//
		
		UINT8 b1CellChgUtp		:1; 	//
		UINT8 b1CellDischgUtp	:1; 	//
		UINT8 b1VcellDeltaBig	:1; 	//
		UINT8 b1TempDeltaBig	:1; 	//这个没有，Res可用
		
		UINT8 b1SocLow			:1; 	//
		UINT8 b1TmosOtp 		:1; 	//
		UINT8 b1PackOvp 		:1; 	//串并机需要
		UINT8 b1PackUvp 		:1; 	//
     }bits;
}Fault_Flag;


typedef struct DRIVER_ELEMENT {
	//只需要赋值一次的参数
	UINT16 u16_VirCur_Chg;				//A*10
	UINT16 u16_VirCur_Dsg;				//A*10
	UINT16 u16_PreChg_Time;				//次数，多少个周期
	UINT16 u16_PreChg_Duty;				//占空比，10,20,30，到100。
	UINT16 u16_PreChg_Period;			//周期，100ms为单位，1为100ms，10为1s
	UINT8 u8_DriverCtrl_Right;			//驱动控制权，如果为0，则由lib文件控制IO的开关动作，如果为1，则lib文件只提供IO开关的信息，外部自行开关
										//备用，默认不管0就好。有局限性。如果外部延时操作，太长，会造成时序紊乱(预放功能所致等等)

	//需要不间断赋值的参数
	Fault_Flag Fault_Flag;
	UINT16 u16_CurChg;					//A*10
	UINT16 u16_CurDsg;					//A*10


	//能获取结果信息的参数
	Driver_OnOFF_Etc MosRelay_Status;	//还是决定省空间，目标是8L和103均能用


	//信息交换区
	UINT8 u8_FuncOFF_Flag; 				//要求关闭功能标志位，1，相关功能标志位需要关闭; 0，不关闭。
										//这个标志位用if(!=)则赋值的方式？
	//UINT8 u8_DriverOFF_Flag;			//外部强制关闭驱动标志位，转移到以下方位
	DRIVER_FORCE_EXT DriverForceExt;	//外部强制控制驱动标志位，全局控制和局部单个控制都有
}DriverElement;


typedef enum DRIVER_SELECT {
	DRIVER_RELAY_SAME_DOOR_NO_PRECHG = 0,
	DRIVER_RELAY_SAME_DOOR_HAVE_PRECHG,
	DRIVER_RELAY_DIFF_DOOR_NO_PRECHG,
	DRIVER_RELAY_DIFF_DOOR_HAVE_PRECHG,
	
	DRIVER_MOS_SAME_DOOR_NO_PRECHG,
	DRIVER_MOS_SAME_DOOR_HAVE_PRECHG,
	DRIVER_MOS_BOOTSTRAP_CIR,			//自举电路，这个驱动方案很有问题，先不作优化，没有外部强制控制功能
										//可能淘汰不用？
}Driver_Select;

extern DriverElement Driver_Element;

//GPIO均为推挽输出，2MHz，GPIO_Type有4种形式
void InitDrivers_GPIO(GPIO_Module* GPIOx, UINT16 GPIO_Pin_x, GPIO_Type GpioType);
void Drivers_Ctrl(UINT8 TimeBase_10ms, UINT8 OnOFF_Ctrl, Driver_Select DriverSelect);

#endif	/* IODRIVERS_H */

