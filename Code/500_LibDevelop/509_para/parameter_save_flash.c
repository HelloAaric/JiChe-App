#include "parameter_save_flash.h"
#include "flash.h"
#include "data_deal.h"
#include "soc_user.h"
#include "Fault.h"
#include "heat_cool.h"
#include "parameter_save.h"
#include "ProductionID.h"
#include <string.h>

// 是否自动更新标志位，如果修改，则自动刷新最新默认参数
#define FLASH_VALUE_BEGIN_FLAG				        0x166F

// 两个字位，8个字节为单位。
typedef struct _FLASH_PARA_HEAD {
    UINT16 u16_IsFirstStartUp;                      // 是否开机更新为默认参数
    UINT16 u16_EraseCnt;                            // 擦除次数
    UINT16 u16_Usage;                               // 使用：K
    UINT16 u16_PageSize;                            // 单位：K

    UINT16 u16_res1;
    UINT16 u16_res2;
    UINT16 u16_res3;
    UINT16 u16_res4;
}FLASH_PARA_HEAD;

typedef struct _FLASH_PARA_MANAGE {
    UINT16 u16_isDataUpdate;                        // 运行过程是否更新数据
    UINT16 u16_IsRefreshToDefault;	                // 是否重新初始化所有参数
    FLASH_PARA_HEAD FlashHead_Info;
}FLASH_PARA_MANAGE;


FLASH_PARA_MANAGE FlashPara_Manage;


// UINT16为一个单位，参数实际数量
#define FLASH_PARA_NUM_FLASH_CTRL_HEAD 		 		8                                       // 后面可以添加
#define FLASH_PARA_NUM_PROTECT 		 		        65
// #define FLASH_PARA_NUM_RTC		 			    12                                      // 暂时不要
#define FLASH_PARA_NUM_CALIB_K 		 		        KB_NUM			                        // 47
#define FLASH_PARA_NUM_CALIB_B 		 		        KB_NUM
#define FLASH_PARA_NUM_SOC_TABLE 		 	        SOC_TABLE_SIZE	                        // 42
#define FLASH_PARA_NUM_COPPERLOSS 		 	        CompensateNUM	                        // 16
#define FLASH_PARA_NUM_COPPERLOSS_NUM 		        CompensateNUM
// #define FLASH_PARA_NUM_FAULT_RECORD 	            (3*Record_len+ 3 +Record_len*6)         // 暂时不要
#define FLASH_PARA_NUM_OTHER_ELEMENT1 		        32
#define FLASH_PARA_NUM_HEAT_COOL			        24
#define FLASH_PARA_NUM_PRODUCT_ID_LENGTH_MAX        PRODUCT_ID_LENGTH_MAX


// 一个flash字最小单位是2个字，8字节。所以只能设置200字节一个区，100字节一个区有问题
// 地址必须是8的倍数
#define FLASH_PARA_ADDR_HEAD_INFO                   FLASH_ADDR_PARA_SAVE_START              // 存储管理头文件信息。
#define FLASH_PARA_ADDR_PROTECT                     (FLASH_PARA_ADDR_HEAD_INFO + 200)       // 实际 13*5*2 = 130个字节
#define FLASH_PARA_ADDR_CALIB_K                     (FLASH_PARA_ADDR_HEAD_INFO + 400)
#define FLASH_PARA_ADDR_CALIB_B                     (FLASH_PARA_ADDR_HEAD_INFO + 600)
#define FLASH_PARA_ADDR_SOC_TABLE                   (FLASH_PARA_ADDR_HEAD_INFO + 800)
#define FLASH_PARA_ADDR_COPPERLOSS                  (FLASH_PARA_ADDR_HEAD_INFO + 1000)
#define FLASH_PARA_ADDR_COPPERLOSS_NUM              (FLASH_PARA_ADDR_HEAD_INFO + 1200)
#define FLASH_PARA_ADDR_OTHER_ELEMENT1              (FLASH_PARA_ADDR_HEAD_INFO + 1400)
#define FLASH_PARA_ADDR_HEAT_COOL                   (FLASH_PARA_ADDR_HEAD_INFO + 1600)

#define FLASH_PARA_ADDR_PRO_ID_SERIAL_NUM           (FLASH_PARA_ADDR_HEAD_INFO + 1800)
#define FLASH_PARA_ADDR_PRO_ID_HARDWARE_VER         (FLASH_PARA_ADDR_PRO_ID_SERIAL_NUM + 64)    // 内部小区间
#define FLASH_PARA_ADDR_PRO_ID_SOFTWARE_VER         (FLASH_PARA_ADDR_PRO_ID_SERIAL_NUM + 128)

#define FLASH_PARA_ADDR_NEXT_ONE                    (FLASH_PARA_ADDR_HEAD_INFO + 2000)

void FlashErase_AllRegion(void)
{
    FlashErase_OnePage(FLASH_ADDR_PARA_SAVE_START);
}

UINT16 FlashRead_u16Array(UINT32 faddr)
{
    return FlashRead_OneHalfWord(faddr);
}

void FlashWrite_2Word_NoErase(uint32_t StartAddr, uint32_t Buffer1, uint32_t Buffer2)
{
    FlashWrite_DoubleWord_NoErase(StartAddr, Buffer1, Buffer2);
}

// 地址必须是8的倍数
void FlashWrite_u16Array_NoErase(uint32_t StartAddr, uint16_t* buffer, uint16_t length)
{
    UINT16 i = 0;
    UINT16 TwoWord_Length = 0;
    UINT16 res_Length = 0;
    TwoWord_Length = length / 4;
    res_Length = length % 4;

    if((StartAddr % 8) != 0) {
        return;
    }
    for(i = 0; i < TwoWord_Length; ++i) {
        FlashWrite_2Word_NoErase(StartAddr + i*8, *((UINT32*)buffer + 2*i), *((UINT32*)buffer + 2*i+1));
    }

    switch(res_Length) {
        case 0:
            break;
        case 1:
            // 下面这个写法虽然没问题，但是还是不要越界吧
            // FlashWrite_2Word_NoErase(StartAddr + i*8, 0xFFFF0000|*((UINT32*)buffer + 2*i), 0xFFFFFFFF);
            FlashWrite_2Word_NoErase(StartAddr + i*8, 0xFFFF0000|(UINT32)buffer[length - 1], 0xFFFFFFFF);
            break;
        case 2:
            // 这样写看起来没多大问题。TODO
            FlashWrite_2Word_NoErase(StartAddr + i*8, *((UINT32*)buffer + 2*i), 0xFFFFFFFF);
            break;
        case 3:
            FlashWrite_2Word_NoErase(StartAddr + i*8, *((UINT32*)buffer + 2*i), 0xFFFF0000|(UINT32)buffer[length - 1]);
            break;
        default:
            break;
    }
}

void Para_ResetData_AllToDefault(void)
{
	const struct PRT_E2ROM_PARAS PrtE2PARAS_Default = E2P_PROTECT_DEFAULT_PRT;
	// const struct RTC_ELEMENT RTC_element_Default = RTC_element_default;
	const struct OTHER_ELEMENT OtherElement_Default = OtherElement_default;
	const struct HEAT_COOL_ELEMENT HeatCoolEle_Default 	= HeatCoolElement_Default;
	UINT16 i = 0;
 
    FlashPara_Manage.FlashHead_Info.u16_IsFirstStartUp = FLASH_VALUE_BEGIN_FLAG;
    FlashPara_Manage.FlashHead_Info.u16_EraseCnt = 6;       // 默认来个6次吧
    FlashPara_Manage.FlashHead_Info.u16_Usage = 2;
    FlashPara_Manage.FlashHead_Info.u16_PageSize = 8;
    FlashPara_Manage.FlashHead_Info.u16_res1 = 0;
    FlashPara_Manage.FlashHead_Info.u16_res2 = 0;
    FlashPara_Manage.FlashHead_Info.u16_res3 = 0;
    FlashPara_Manage.FlashHead_Info.u16_res4 = 0;

	//KB
	for(i = 0;i < KB_NUM; ++i) {
		g_u16CalibCoefK[i] = SYSKDEFAULT;
	   	g_i16CalibCoefB[i] = SYSBDEFAULT;
	}

	//Protect
	for(i = 0;i < FLASH_PARA_NUM_PROTECT; ++i) {
		*(&PRT_E2ROMParas.u16VcellOvp_First+i) = *(&PrtE2PARAS_Default.u16VcellOvp_First+i);
	}

	//SocTable
	for(i = 0;i < FLASH_PARA_NUM_SOC_TABLE; ++i) {
		SOC_Table_Set[i] = SOC_Table_Default[i];
	}

	//CopperLoss
	for(i = 0; i < FLASH_PARA_NUM_COPPERLOSS; ++i) {
		CopperLoss[i] = CopperLoss_Default;
		CopperLoss_Num[i] = CopperLossNum_Default;
	}

	#if 0
	//RTC_element
	for(i = 0; i < E2P_PARA_NUM_RTC; ++i) {
		*(&RTC_time.RTC_Time_Year+i) = *(&RTC_element_Default.RTC_Time_Year+i);
	}
	u32E2P_RTC_Element_WriteFlag = E2P_PARA_ALL_RTC_ELEMENT;
	#endif

	//Other_CanAdd_element
	for(i = 0; i < FLASH_PARA_NUM_OTHER_ELEMENT1; ++i) {
		*(&OtherElement.u16Balance_OpenVoltage+i) = *(&OtherElement_Default.u16Balance_OpenVoltage+i);
	}

	//HeatCool_element
	for(i = 0; i < FLASH_PARA_NUM_HEAT_COOL; ++i) {
		*(&Heat_Cool_Element.u16Heat_OpenTemp+i) = *(&HeatCoolEle_Default.u16Heat_OpenTemp+i);
	}


	UINT8 harewareCount = sizeof(BMS_HARDWARE_VERDION_DEFAULT) >32 ? 32 : sizeof(BMS_HARDWARE_VERDION_DEFAULT);
	UINT8 softwareCount = sizeof(BMS_SOFTWARE_VERDION_DEFAULT) >32 ? 32 : sizeof(BMS_SOFTWARE_VERDION_DEFAULT);
	UINT8 serialNumberCount = sizeof(BMS_SERIAL_NUMBER_DEFAULT) >32 ? 32 : sizeof(BMS_SERIAL_NUMBER_DEFAULT);

	memset(&ProductionInfor, 0, sizeof(PRODUCTION_ID_INFO));
	ProductionInfor.BMS_SerialNumberHeadAdress = FLASH_PARA_ADDR_PRO_ID_SERIAL_NUM;
	ProductionInfor.BMS_HardWareVersionHeadAdress = FLASH_PARA_ADDR_PRO_ID_HARDWARE_VER;
	ProductionInfor.BMS_SoftWareVersionHeadAdress = FLASH_PARA_ADDR_PRO_ID_SOFTWARE_VER;
    #if 1
	memcpy(ProductionInfor.BMS_HardWareVersion, BMS_HARDWARE_VERDION_DEFAULT, harewareCount);
	memcpy(ProductionInfor.BMS_SoftWareVersion, BMS_SOFTWARE_VERDION_DEFAULT, softwareCount);
	memcpy(ProductionInfor.BMS_SerialNumber, BMS_SERIAL_NUMBER_DEFAULT, serialNumberCount);
    #endif
}

void Para_WriteAllCycle(void)
{
    FlashWrite_u16Array_NoErase(FLASH_PARA_ADDR_HEAD_INFO, &FlashPara_Manage.FlashHead_Info.u16_IsFirstStartUp,
        FLASH_PARA_NUM_FLASH_CTRL_HEAD);
    FlashWrite_u16Array_NoErase(FLASH_PARA_ADDR_PROTECT, &PRT_E2ROMParas.u16VcellOvp_First, FLASH_PARA_NUM_PROTECT);
    FlashWrite_u16Array_NoErase(FLASH_PARA_ADDR_CALIB_K, g_u16CalibCoefK, FLASH_PARA_NUM_CALIB_K);
    FlashWrite_u16Array_NoErase(FLASH_PARA_ADDR_CALIB_B, (UINT16*)g_i16CalibCoefB, FLASH_PARA_NUM_CALIB_B);
    FlashWrite_u16Array_NoErase(FLASH_PARA_ADDR_SOC_TABLE, SOC_Table_Set, FLASH_PARA_NUM_SOC_TABLE);
    FlashWrite_u16Array_NoErase(FLASH_PARA_ADDR_COPPERLOSS, CopperLoss, FLASH_PARA_NUM_COPPERLOSS);
    FlashWrite_u16Array_NoErase(FLASH_PARA_ADDR_COPPERLOSS_NUM, CopperLoss_Num, FLASH_PARA_NUM_COPPERLOSS_NUM);
    FlashWrite_u16Array_NoErase(FLASH_PARA_ADDR_OTHER_ELEMENT1, &OtherElement.u16Balance_OpenVoltage, FLASH_PARA_NUM_OTHER_ELEMENT1);
    FlashWrite_u16Array_NoErase(FLASH_PARA_ADDR_HEAT_COOL, &Heat_Cool_Element.u16Heat_OpenTemp, FLASH_PARA_NUM_HEAT_COOL);


	ProductionInfor.BMS_SerialNumberHeadAdress = FLASH_PARA_ADDR_PRO_ID_SERIAL_NUM;
	ProductionInfor.BMS_HardWareVersionHeadAdress = FLASH_PARA_ADDR_PRO_ID_HARDWARE_VER;
	ProductionInfor.BMS_SoftWareVersionHeadAdress = FLASH_PARA_ADDR_PRO_ID_SOFTWARE_VER;
    FlashWrite_u16Array_NoErase(ProductionInfor.BMS_SerialNumberHeadAdress, &ProductionInfor.BMS_SerialNumberLength, 1);
    FlashWrite_u16Array_NoErase(ProductionInfor.BMS_HardWareVersionHeadAdress, &ProductionInfor.BMS_HardWareVersionLength, 1);
    FlashWrite_u16Array_NoErase(ProductionInfor.BMS_SoftWareVersionHeadAdress, &ProductionInfor.BMS_SoftWareVersionLength, 1);

    // 前八个当头
    FlashWrite_u16Array_NoErase(ProductionInfor.BMS_SerialNumberHeadAdress + 16,\
        (UINT16*)ProductionInfor.BMS_SerialNumber, PRODUCT_ID_LENGTH_MAX>>1);

    FlashWrite_u16Array_NoErase(ProductionInfor.BMS_HardWareVersionHeadAdress + 16,\
        (UINT16*)ProductionInfor.BMS_HardWareVersion, PRODUCT_ID_LENGTH_MAX>>1);

    FlashWrite_u16Array_NoErase(ProductionInfor.BMS_SoftWareVersionHeadAdress + 16,\
        (UINT16*)ProductionInfor.BMS_SoftWareVersion, PRODUCT_ID_LENGTH_MAX>>1);
}

void Para_WriteAll_DataDefault(void)
{
    Para_ResetData_AllToDefault();
    FlashErase_AllRegion();
    Para_WriteAllCycle();
}

void Para_WriteAll_DataRefresh(void)
{
    FlashPara_Manage.FlashHead_Info.u16_EraseCnt++;
    FlashErase_AllRegion();
    Para_WriteAllCycle();
}

void Para_GetAll_StoreData(void)
{
    UINT16 i = 0;

    // FlashRead_u16Array(FLASH_PARA_ADDR_PROTECT);
	//Protect
	for(i = 0; i < FLASH_PARA_NUM_PROTECT; ++i) {
		*(&PRT_E2ROMParas.u16VcellOvp_First + i) = FlashRead_u16Array(FLASH_PARA_ADDR_PROTECT + 2 * i);
	}

	for(i = 0; i < FLASH_PARA_NUM_CALIB_K; ++i) {
		g_u16CalibCoefK[i] = FlashRead_u16Array(FLASH_PARA_ADDR_CALIB_K + 2 * i);
	}

    for(i = 0; i < FLASH_PARA_NUM_CALIB_B; ++i) {
		g_i16CalibCoefB[i] = FlashRead_u16Array(FLASH_PARA_ADDR_CALIB_B + 2 * i);
	}

    for(i = 0; i < FLASH_PARA_NUM_SOC_TABLE; ++i) {
		SOC_Table_Set[i]= FlashRead_u16Array(FLASH_PARA_ADDR_SOC_TABLE + 2 * i);
	}

    for(i = 0; i < FLASH_PARA_NUM_COPPERLOSS; ++i) {
		CopperLoss[i] = FlashRead_u16Array(FLASH_PARA_ADDR_COPPERLOSS + 2 * i);
	}

    for(i = 0; i < FLASH_PARA_NUM_COPPERLOSS_NUM; ++i) {
		CopperLoss_Num[i] = FlashRead_u16Array(FLASH_PARA_ADDR_COPPERLOSS_NUM + 2 * i);
	}

    for(i = 0; i < FLASH_PARA_NUM_OTHER_ELEMENT1; ++i) {
		*(&OtherElement.u16Balance_OpenVoltage + i) = FlashRead_u16Array(FLASH_PARA_ADDR_OTHER_ELEMENT1 + 2 * i);
	}

    for(i = 0; i < FLASH_PARA_NUM_HEAT_COOL; ++i) {
		*(&Heat_Cool_Element.u16Heat_OpenTemp + i) = FlashRead_u16Array(FLASH_PARA_ADDR_HEAT_COOL + 2 * i);
	}


	ProductionInfor.BMS_SerialNumberHeadAdress = FLASH_PARA_ADDR_PRO_ID_SERIAL_NUM;
	ProductionInfor.BMS_HardWareVersionHeadAdress = FLASH_PARA_ADDR_PRO_ID_HARDWARE_VER;
	ProductionInfor.BMS_SoftWareVersionHeadAdress = FLASH_PARA_ADDR_PRO_ID_SOFTWARE_VER;
	ProductionInfor.BMS_SerialNumberLength = FlashRead_u16Array(ProductionInfor.BMS_SerialNumberHeadAdress);
	ProductionInfor.BMS_HardWareVersionLength = FlashRead_u16Array(ProductionInfor.BMS_HardWareVersionHeadAdress);
	ProductionInfor.BMS_SoftWareVersionLength = FlashRead_u16Array(ProductionInfor.BMS_SoftWareVersionHeadAdress);

	for(i = 0; i < (PRODUCT_ID_LENGTH_MAX>>1); i++) {
		*((UINT16*)(&ProductionInfor.BMS_SerialNumber[i*2])) = \
			FlashRead_u16Array(ProductionInfor.BMS_SerialNumberHeadAdress + 16 + i*2);		//往后挪一位

		*((UINT16*)(&ProductionInfor.BMS_HardWareVersion[i*2])) = \
			FlashRead_u16Array(ProductionInfor.BMS_HardWareVersionHeadAdress + 16 + i*2);
		
		*((UINT16*)(&ProductionInfor.BMS_SoftWareVersion[i*2])) = \
			FlashRead_u16Array(ProductionInfor.BMS_SoftWareVersionHeadAdress + 16 + i*2);
	}
}

extern UINT16 gu16_Rs_uO;
void InitPara_Flash(void)
{
    FlashPara_Manage.u16_isDataUpdate = 0;
    FlashPara_Manage.FlashHead_Info.u16_IsFirstStartUp = FlashRead_u16Array(FLASH_PARA_ADDR_HEAD_INFO);

    if(FlashPara_Manage.FlashHead_Info.u16_IsFirstStartUp != FLASH_VALUE_BEGIN_FLAG) {
        Para_WriteAll_DataDefault();
    } else {
        FlashPara_Manage.FlashHead_Info.u16_EraseCnt = FlashRead_u16Array(FLASH_PARA_ADDR_HEAD_INFO + 2);
        FlashPara_Manage.FlashHead_Info.u16_Usage = FlashRead_u16Array(FLASH_PARA_ADDR_HEAD_INFO + 4);
        FlashPara_Manage.FlashHead_Info.u16_PageSize = FlashRead_u16Array(FLASH_PARA_ADDR_HEAD_INFO + 6);
        FlashPara_Manage.FlashHead_Info.u16_res1 = FlashRead_u16Array(FLASH_PARA_ADDR_HEAD_INFO + 8);
        FlashPara_Manage.FlashHead_Info.u16_res2 = FlashRead_u16Array(FLASH_PARA_ADDR_HEAD_INFO + 10);
        FlashPara_Manage.FlashHead_Info.u16_res3 = FlashRead_u16Array(FLASH_PARA_ADDR_HEAD_INFO + 12);
        FlashPara_Manage.FlashHead_Info.u16_res4 = FlashRead_u16Array(FLASH_PARA_ADDR_HEAD_INFO + 14);

        Para_GetAll_StoreData();
    }

    gu16_Rs_uO = OtherElement.u16Sys_CS_Res*1000 / OtherElement.u16Sys_CS_Res_Num;
}

/*
1. 先处理固件参数，
2. 日志记录先屏蔽，后续还是使用flash
3. 动态参数，2个lib文件要处理。
*/
void App_ParaSave_Flash(void)
{
    if(FlashPara_Manage.u16_isDataUpdate) {
        FlashPara_Manage.u16_isDataUpdate = 0;
        Para_WriteAll_DataRefresh();
    }

    if(FlashPara_Manage.u16_IsRefreshToDefault) {
        FlashPara_Manage.u16_IsRefreshToDefault = 0;
        Para_WriteAll_DataDefault();
    }

    if(u8E2P_KB_WriteFlag\
        || u32E2P_Pro_VolCur_WriteFlag || u32E2P_Pro_Temp_WriteFlag || u32E2P_Pro_Other_WriteFlag\
        || u8E2P_SocTable_WriteFlag|| u8E2P_CopperLoss_WriteFlag || u32E2P_RTC_Element_WriteFlag\
        || u32E2P_OtherElement1_WriteFlag || u32E2P_HeatCool_WriteFlag || ProductionInfor.BMS_SerialNumber_WriteFlag\
        || ProductionInfor.BMS_HardWareVersion_WriteFlag || ProductionInfor.BMS_SoftWareVersion_WriteFlag) { //0x2000,0x2100,0x2200,0x2300
        FlashPara_Manage.u16_isDataUpdate = 1;
        
		u32E2P_Pro_VolCur_WriteFlag = 0;
		u32E2P_Pro_Temp_WriteFlag = 0;
		u32E2P_Pro_Other_WriteFlag = 0;
		u32E2P_OtherElement1_WriteFlag = 0;
		u32E2P_RTC_Element_WriteFlag = 0;
		u8E2P_SocTable_WriteFlag = 0;
		u8E2P_CopperLoss_WriteFlag = 0;
		u8E2P_KB_WriteFlag = 0;
		u32E2P_HeatCool_WriteFlag = 0;
        ProductionInfor.BMS_SerialNumber_WriteFlag = 0;
        ProductionInfor.BMS_HardWareVersion_WriteFlag = 0;
        ProductionInfor.BMS_SoftWareVersion_WriteFlag = 0;

    }
}
