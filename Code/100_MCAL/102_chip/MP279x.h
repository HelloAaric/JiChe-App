/************************************************************************************
*         File: MP279x.h																														*
*					Description: Contains BMS registers address										 					 	*
*					CPU: STM32F03xx Medium density devices																		*
*					Project:	MPS BCN, EVKT-BMU1xx MP279x Main MCU														*
*					Version:	1.0																															*
*					Author:		Albert Corbella & Miguel A. Sanchez															*
*					Date:			01/09/2021																											*
*************************************************************************************/
#include "stdint.h"

//MP279x Registers MAP
#define REG_CELL_CONFIG 					0x00    //0d000
#define REG_PWR_STATUS						0x01		//0d001
#define REG_STB_STATUS						0x02		//0d002
#define REG_LOAD_CHGR_STATUS			0x03		//0d003
//																...
#define REG_ACT_CONFIG					 	0x05		//0d005
#define REG_STB_CONFIG						0x06		//0d006
#define REG_SAFE_CONFIG						0x07		//0d007
#define REG_REG_CONFIG						0x08		//0d008
#define REG_LOAD_CHGR_CONFIG			0x09		//0d009
#define REG_GPIO_STATUS						0x0A		//0d010
#define REG_GPIO_OUT							0x0B		//0d011
#define REG_GPIO_CONFIG						0x0C		//0d012
#define REG_PINS_CONFIG						0x0D		//0d013
#define REG_WDT_STATUS						0x0E		//0d014
#define REG_WDT_RST								0x0F		//0d015
#define REG_WDT_CONFIG						0x10		//0d016
#define REG_FET_STATUS						0x11		//0d017
#define REG_FET_CTRL							0x12		//0d018
#define REG_FET_MODE							0x13		//0d019
#define REG_FET_CONFIG						0x14		//0d020
#define REG_RD_INT0								0x15		//0d021
#define REG_RD_INT1								0x16		//0d022
#define REG_INT0_CLR				  		0x17		//0d023
#define REG_INT1_CLR 							0x18		//0d024
#define REG_INT0_EN							 	0x19		//0d025
#define REG_INT1_EN								0x1A		//0d026
#define REG_INT_TYPE0							0x1B		//0d027
#define REG_INT_TYPE1							0x1C		//0d028
#define REG_INT_TYPE2							0x1D		//0d029
//#define REG_INT_TYPE3				  		0x1E	(R0)
//#define REG_INT_TYPE4							0x1F	(R0)
#define REG_MASK_INT0				  		0x1E		//0d030
#define REG_MASK_INT1							0x1F		//0d031
#define REG_OC_STATUS						 	0x20		//0d032
//																...
//#define REG_OC_MASK								0x22	//(R0)
#define REG_OCFT_CTRL						 	0x23		//0d035
#define REG_DSGOC_LIM							0x24		//0d036
#define REG_DSGOC_DEG							0x25		//0d037
#define REG_CHGOC_DEG							0x26		//0d038
#define REG_SC_STATUS							0x27		//0d039
//#define REG_SC_MASK								0x28	//(R0)
//																...
#define REG_SCFT_CTRL							0x2A		//0d042
#define REG_DSGSC_CONFIG					0x2B		//0d043
#define REG_CHGSC_CONFIG					0x2C		//0d044
#define REG_RD_CELL_UV						0x2D		//0d045
#define REG_RD_CELL_OV						0x2E		//0d046
#define REG_RD_CELL_MSMT					0x2F		//0d047
#define REG_RD_CELL_DEAD					0x30		//0d048
//#define REG_CELLUV_MASK						0x31	//(R0)
//#define REG_CELLOV_MASK						0x32	//(R0)
//#define REG_CELLMSMT_MASK					0x33	//(R0)
#define REG_CELL_MSMT_STS					0x33		//0d051
#define REG_PACKFT_CTRL						0x34		//0d052
#define REG_CELLFT_CTRL						0x35		//0d053
#define REG_CELL_HYST							0x36		//0d054
//#define REG_PACK_HYST							0x37	//(R0)
#define REG_PACK_UV_OV						0x37		//0d055
#define REG_CELL_UV								0x38		//0d056
#define REG_CELL_OV							 	0x39		//0d057
#define REG_PACK_UV								0x3A		//0d058
#define REG_PACK_OV								0x3B		//0d059
#define REG_CELL_DEAD_THR					0x3C		//0d060
#define REG_CELL_MSMT							0x3D		//0d061
#define REG_RD_NTC_DIE						0x3E		//0d062
#define REG_RD_VANTC4							0x3F		//0d063
#define REG_RD_VANTC3							0x40		//0d064
#define REG_RD_VANTC2							0x41		//0d065
#define REG_RD_VANTC1							0x42		//0d066
#define REG_RD_T_DIE							0x43		//0d067
#define REG_NTC_CLR								0x44		//0d068
//#define REG_NTC_MASK							0x45	//(R0)
#define REG_DIE_CONFIG						0x46		//0d070
#define REG_NTC_CONFIG						0x47		//0d071
#define REG_OTHR_DSG							0x48		//0d072
#define REG_UTHR_DSG						 	0x49		//0d073
#define REG_OTHR_CHG							0x4A		//0d074
#define REG_UTHR_CHG							0x4B		//0d075
#define REG_NTCM_OTHR							0x4C		//0d076
#define REG_DIE_OT								0x4D		//0d077
#define REG_SELF_STS							0x4E		//0d078
#define REG_RD_VA1P8							0x4F		//0d079
#define REG_RD_VA3P3							0x50		//0d080
#define REG_RD_VA5								0x51		//0d081
#define REG_RD_VASELF							0x52		//0d082
#define REG_RD_OPENH							0x53		//0d083
#define REG_RD_OPENL							0x54		//0d084
#define REG_SFT_GO								0x55		//0d085
#define REG_SELF_CFG							0x56		//0d086
#define REG_OPEN_CFG							0x57		//0d087
#define REG_REGIN_UV							0x58		//0d088
#define REG_V3P3_UV							 	0x59		//0d089
#define REG_VDD_UV								0x5A		//0d090
#define REG_SELF_THR							0x5B		//0d091
//																...
#define REG_FT_STS1								0x5D		//0d093
#define REG_FT_STS2								0x5E		//0d094
#define REG_FT_CLR								0x5F		//0d095
#define REG_FT_REC								0x60		//0d096
#define REG_FT0_CONFIG						0x61		//0d097
#define REG_FT1_CONFIG						0x62		//0d098
#define REG_FT2_CONFIG						0x63		//0d099
//																...
#define REG_RD_CCIRQL							0x65		//0d101
#define REG_RD_CCIRQH							0x66		//0d102
#define REG_RD_CCACCQL						0x67		//0d103
#define REG_RD_CCACCQH						0x68		//0d104
#define REG_RD_VPACKP						 	0x69		//0d105
#define REG_RD_VTOP								0x6A		//0d106
#define REG_RD_ITOP								0x6B		//0d107
#define REG_RD_VCELL1							0x6C		//0d108
#define REG_RD_ICELL1							0x6D		//0d109
#define REG_RD_VCELL2							0x6E		//0d110
#define REG_RD_ICELL2							0x6F		//0d111
#define REG_RD_VCELL3							0x70		//0d112
#define REG_RD_ICELL3							0x71		//0d113
#define REG_RD_VCELL4							0x72		//0d114
#define REG_RD_ICELL4							0x73		//0d115
#define REG_RD_VCELL5							0x74		//0d116
#define REG_RD_ICELL5							0x75		//0d117
#define REG_RD_VCELL6							0x76		//0d118
#define REG_RD_ICELL6							0x77		//0d119
#define REG_RD_VCELL7							0x78		//0d120
#define REG_RD_ICELL7						 	0x79		//0d121
#define REG_RD_VCELL8							0x7A		//0d122
#define REG_RD_ICELL8							0x7B		//0d123
#define REG_RD_VCELL9							0x7C		//0d124
#define REG_RD_ICELL9							0x7D		//0d125
#define REG_RD_VCELL10						0x7E		//0d126
#define REG_RD_ICELL10						0x7F		//0d127
#define REG_RD_VCELL11						0x80		//0d128
#define REG_RD_ICELL11						0x81		//0d129
#define REG_RD_VCELL12						0x82		//0d130
#define REG_RD_ICELL12						0x83		//0d131
#define REG_RD_VCELL13						0x84		//0d132
#define REG_RD_ICELL13						0x85		//0d133
#define REG_RD_VCELL14						0x86		//0d134
#define REG_RD_ICELL14						0x87		//0d135
#define REG_RD_VCELL15						0x88		//0d136
#define REG_RD_ICELL15					 	0x89		//0d137
#define REG_RD_VCELL16						0x8A		//0d138
#define REG_RD_ICELL16						0x8B		//0d139
#define REG_RD_VNTC4							0x8C		//0d140
#define REG_RD_VNTC3							0x8D		//0d141
#define REG_RD_VNTC2							0x8E		//0d142
#define REG_RD_VNTC1							0x8F		//0d143
#define REG_RD_VGPIO3							0x90		//0d144
#define REG_RD_VGPIO2							0x91		//0d145
#define REG_RD_VGPIO1							0x92		//0d146
#define REG_RD_TDIE								0x93		//0d147
#define REG_RD_V1P8								0x94		//0d148
#define REG_RD_V3P3								0x95		//0d149
#define REG_RD_V5									0x96		//0d150
#define REG_CC_STS								0x97		//0d151
#define REG_ADC_STS								0x98		//0d152
#define REG_ADC_CTRL						 	0x99		//0d153
#define REG_CC_CONFIG							0x9A		//0d154
#define REG_TRIMG_IPCB						0x9B		//0d155
#define REG_HRSCAN0								0x9C		//0d156
#define REG_HRSCAN1								0x9D		//0d157
#define REG_HRSCAN2								0x9E		//0d158
#define REG_COMM_STS							0x9F		//0d159
#define REG_SILC_INFO1						0xA0		//0d160
#define REG_SILC_INFO2						0xA1		//0d161
#define REG_TEST_REV							0xA2		//0d162
#define REG_COMM_CONFIG						0xA3		//0d163
#define REG_BAL_STS								0xA4		//0d164
#define REG_BAL_LIST							0xA5		//0d165
#define REG_BAL_CTRL							0xA6		//0d166
#define REG_BAL_CONFIG						0xA7		//0d167
#define REG_BAL_THR								0xA8		//0d168

#define USE_CRC										1
#define NO_CRC										0

typedef struct
{
	uint8_t SAFE_CONFIG;
	uint16_t FET_MODE;
	uint16_t CELL_UV;
	uint16_t CELL_OV;
	uint16_t CELL_HYST;
	uint16_t CELL_DEAD_THR;
	uint16_t CELLFT_CTRL;
	uint16_t CELL_MSMT;
	uint16_t PACK_UV;
	uint16_t PACK_OV;
	uint16_t PACK_HYST;
	uint16_t PACKFT_CTRL;
	uint8_t DSGOC1_LIM;
	uint8_t DSGOC1_DEG;
	uint8_t DSGOC2_LIM;
	uint8_t DSGOC2_DEG;
	uint8_t CHGOC_LIM;
	uint8_t CHGOC_DEG;
	uint16_t OCFT_CTRL;
	uint8_t DSGSC_LIM;
	uint8_t DSGSC_DEG;
	uint8_t CHGSC_LIM;
	uint8_t CHGSC_DEG;
	uint16_t SCFT_CTRL;
	uint16_t FT0_CFG;
	uint16_t FT_REC;
	uint16_t CELLS_MASK;
	uint16_t OTHR_DSG;
	uint16_t UTHR_DSG;
	uint16_t OTHR_CHG;
	uint16_t UTHR_CHG;
	uint16_t NTCM_OTHR;
	uint16_t NTC_CONFIG;
	uint16_t DIE_OT;
	uint8_t SELF_CFG;
	uint8_t PINS_CONFIG;
	int16_t TRIMG_IPCB;
	uint16_t BAL_THR;
	uint16_t BAL_LIST;
} MP279xConfig_REGs;



//CC Status:
typedef struct __attribute__((packed))
{
	unsigned CC_Cntr: 30;
	unsigned CC_Running: 1;
	unsigned CC_OverFlow: 1;
} Def_CC_Status;

//BMS states definition
typedef enum
{
	WAKE_UP,
	WAKE_UP_CHG,
	CHG,
	DCHG,
	STAND_BY,
	ALARM,
	SAFE,
	RESET_ALARM,
	AUTO_RECOVERY
}states;

//Alarms Indication
typedef struct __attribute__((packed))
{
	unsigned OVC: 1;
	unsigned UVC: 1;
	unsigned DeadC: 1;
	unsigned OVP: 1;
	unsigned UVP: 1;
	unsigned COC: 1;
	unsigned DOC: 1;
	unsigned CSC: 1;
	unsigned DSC: 1;
	unsigned Tchip:1;
	unsigned BatOT:1;
	unsigned BatUT:1;
	unsigned TPCB:1;
	unsigned OW:1;
	unsigned MSMT:1;
	unsigned FET_Drv:1;
	unsigned ChipSystemError:1;
	unsigned ChipSenseError:1;
} AlarmHandlerBit;

//BMSData Registers
typedef struct __attribute__((packed))
{
	uint16_t VTOP;										//2
	int32_t ITOP;											//4
	int32_t CC;												//4
	Def_CC_Status CC_Status;					//4
	uint16_t PACKV;										//2
	int16_t TempPCB;									//2
	int16_t TempIC;										//2
	states BMSstatus;									//1
	uint8_t AFE_Status;								//1
	AlarmHandlerBit BMSAlarms;				//3
	AlarmHandlerBit RT_Alarms;				//3
	uint16_t CellsUVReg;							//2
	uint16_t CellsOVReg;							//2
	uint16_t CellsDeadReg; 						//2
	uint16_t CellsMSMTReg; 						//2
	uint16_t CellsMSMT_Status;				//2
	uint8_t NTC_Status;								//1
	uint8_t OpenWireDet[3];						//3
	uint8_t FETs_Status; 							//1
	uint8_t SelfStatus;								//1
	uint16_t LC_Status;								//2
	uint16_t Bal_Status;							//2
	uint32_t VScan_Int_Counter;				//4
	uint16_t WDT_Int_Counter;					//2
	uint16_t AFE_Mode_Int_Counter;		//2
	uint16_t Pack_I_Int_Counter;			//2
	uint16_t Conn_Int_Counter;				//2
	
	uint16_t V5;
	uint16_t V3P3;
  uint16_t V1P8;
} BMS_REG;


typedef struct __attribute__((packed))
{
	uint16_t 		Cells[16];		// mV，单节电池电压值
	uint16_t    	VTOP; 			// V*100，VB+
	uint16_t 		VPack;			// V*100，VP+
	int32_t 		Current[16];	// 单节电池电流值，不管
	int32_t 		PackCurrent;	// uV，总电流，充电为正，放电为负
	int32_t 		I_CC;
	uint16_t 		TempVect[4];	// 100%，NTC
	uint16_t 		CellsBalancing;
	uint16_t    	ADC_STS;		// ADC状态
} Meas_REG;

