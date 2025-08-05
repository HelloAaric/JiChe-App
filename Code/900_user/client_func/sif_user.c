#include "sif_user.h"
#include "sif_com.h"
#include "basic_data_type.h"
#include "string.h"
#include "data_deal.h"
#include "sci_com.h"
#include "System_Monitor.h"

#define PACK_ONLY_SN "3472DZ48N-24EM/CPBCAJ16A4Y00201"

typedef union
{
    UINT32 all;
    struct Fault_Flag {
        UINT8 b1CellChgOtp              :1;
        UINT8 b1ShortErr                :1;
        UINT8 b1IdischgOcp		        :1; 	//
		UINT8 b1IchgOcp 		        :1; 	//

		UINT8 b1TmosOtp 		        :1; 	//
		UINT8 b1TempFault 		        :1; 	// 温度异常
		UINT8 b1CellUvp 		        :1; 	//
		UINT8 b1CellOvp 		        :1; 	//

		UINT8 b1AFE_Err 		        :1; 	// AFE异常
        UINT8 b1CellDischgUtp	        :1; 	//
        UINT8 b1CellChgUtp		        :1; 	//
        UINT8 b1CellDischgOtp		    :1; 	//

        UINT8 b1Mos_Err		            :1; 	// MOS异常
        UINT8 b1PchgMos_Err		        :1; 	// 预放MOS异常
        UINT8 b1ChgMos_Err		        :1; 	// 充电MOS异常
        UINT8 b1ChgPcs_Err              :1; 	// 充电器不匹配异常

        UINT8 b1IdsgOcp_warn		    :1; 	//
		UINT8 b1IchgOcp_warn 		    :1; 	//
		UINT8 b1CellUvp_warn  		    :1; 	//
		UINT8 b1CellOvp_warn  		    :1; 	//
        
        UINT8 b1CelldsgUtp_warn	        :1; 	//
        UINT8 b1CellChgUtp_warn		    :1; 	//
        UINT8 b1CelldsgOtp_warn	        :1; 	//
        UINT8 b1CellChgOtp_warn		    :1; 	//

        UINT8 bres		                :8; 	//
     }bits;
}SIF_FAULT_WARN;

typedef union
{
    UINT16 all;
    struct _PACK_STATUS {
        UINT8 b1status_chg              :1;
        UINT8 b1status_dsg              :1;
        UINT8 b1status_idle		        :1; 	//
		UINT8 b1status_mos_dsg 	        :1; 	//

		UINT8 b1status_mos_chg          :1; 	//
		UINT8 b1status_mos_pchg         :1; 	//
		UINT8 b1status_chgOver 		    :1; 	//
		UINT8 b1status_chgConnected     :1; 	//

		UINT8 b1status_chg_need         :1; 	//
        UINT8 bres		                :7; 	//
     }bits;
}SIF_PACK_STATUS;


/*
1. 全部都是u8类型
2. 小端模式，低字节在前
3. 
*/
typedef struct {
    UINT8 postID;
    UINT8 cell_Material;
    UINT8 cellType;
    UINT8 cell_Supplier;
    UINT8 bms_Supplier;
    UINT8 basicVbat;                    // V
    UINT8 basicCap;                     // Ah
    UINT8 maxDsgCur;                    // A
    UINT8 maxChgCur[2];                 // A*100
    UINT8 maxChgVbat[2];                // V*100
    UINT8 nowVbat[2];                   // V*10
    UINT8 nowCur[2];                    // A*10，-500 TODO，不懂
    UINT8 SOC;                          // 
    UINT8 cycle[2];                     // 
    UINT8 SOH;                          // 
    UINT8 remainCap[2];                 // Ah*100
    UINT8 maxVcell[2];                  // mV
    UINT8 minVcell[2];                  // mV
    UINT8 MaxMinPosi;                   // 高4位为最高电压位置，低4位为最低电压位置。
    INT8 maxTemp;                       // 有符号数，℃，0xFF无效
    INT8 minTemp;                       // 有符号数，℃，0xFF无效
    INT8 MosTemp;                       // 
    SIF_FAULT_WARN sif_fault;
    UINT8 packType;                     // 前四位软件版本，后四位硬件版本
    UINT8 FaultLevel_DC;                // 
    SIF_PACK_STATUS PackStatus;         // 
    UINT8 PackID[2];                    // 
    UINT8 res;                          // 
    UINT8 CRC_ID;                       // 
}ST_SIF_CLIENT_DATA;


ST_SIF_CLIENT_DATA sif_clientData;

const char sif_PackOnlySN[32] = PACK_ONLY_SN;

void sif_DataFill_msg1(void)
{
    UINT16 u16_temp = 0;
    INT16 i16_temp = 0;

    sif_clientData.postID = 0xAA;
    sif_clientData.cell_Material = 0x03;
    sif_clientData.cellType = (1 << 4) | 2;     // 同口，脚踏
    sif_clientData.cell_Supplier = 0;            // 0x00
    sif_clientData.bms_Supplier = 0;
    sif_clientData.basicVbat = 72;
    sif_clientData.basicCap = OtherElement.u16Soc_Ah/10;
    sif_clientData.maxDsgCur = ;
    sif_clientData.maxChgCur[0] = ;
    sif_clientData.maxChgCur[1] = ;
    sif_clientData.maxChgVbat[0] = ;
    sif_clientData.maxChgVbat[1] = ;

    sif_clientData.nowVbat[0] = g_stCellInfoReport.u16VCellTotle/10;
    sif_clientData.nowVbat[1] = (g_stCellInfoReport.u16VCellTotle/10)>>8;
    sif_clientData.nowCur[0] = ;
    sif_clientData.nowCur[1] = ;
    sif_clientData.SOC = g_stCellInfoReport.SocElement.u16Soc;
    sif_clientData.cycle[0] = g_stCellInfoReport.SocElement.u16Cycle_times;
    sif_clientData.cycle[1] = g_stCellInfoReport.SocElement.u16Cycle_times >> 8;
    sif_clientData.SOH = g_stCellInfoReport.SocElement.u16Soh;
    sif_clientData.remainCap[0] = g_stCellInfoReport.SocElement.u16CapacityNow;
    sif_clientData.remainCap[1] = g_stCellInfoReport.SocElement.u16CapacityNow >> 8;
    sif_clientData.maxVcell[0] = g_stCellInfoReport.u16VCellMax;
    sif_clientData.maxVcell[1] = g_stCellInfoReport.u16VCellMax >> 8;
    sif_clientData.minVcell[0] = g_stCellInfoReport.u16VCellMin;
    sif_clientData.minVcell[1] = g_stCellInfoReport.u16VCellMin >> 8;

    sif_clientData.MaxMinPosi = ;
    // i16_temp = ((INT16)g_stCellInfoReport.u16TempMax)/10 - 40;
    sif_clientData.maxTemp = ((INT16)g_stCellInfoReport.u16TempMax)/10 - 40;
    sif_clientData.minTemp = ((INT16)g_stCellInfoReport.u16TempMin)/10 - 40;
    sif_clientData.MosTemp = ((INT16)g_stCellInfoReport.u16Temperature[MOS_TEMP1])/10 - 40;

    sif_clientData.sif_fault.bits.b1CellChgOtp = g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgOtp;
    sif_clientData.sif_fault.bits.b1ShortErr = System_ERROR_UserCallback(ERROR_STATUS_CBC_DSG) != 0 ? 1 : 0;
    sif_clientData.sif_fault.bits.b1IdischgOcp = g_stCellInfoReport.unMdlFault_Third.bits.b1IdischgOcp;
    sif_clientData.sif_fault.bits.b1IchgOcp = g_stCellInfoReport.unMdlFault_Third.bits.b1IchgOcp;
    sif_clientData.sif_fault.bits.b1TmosOtp = g_stCellInfoReport.unMdlFault_Third.bits.b1TmosOtp;
    u16_temp |= g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgOtp;
    u16_temp |= g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgOtp;
    u16_temp |= g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgUtp;
    u16_temp |= g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgUtp;
    sif_clientData.sif_fault.bits.b1TempFault = u16_temp;
    sif_clientData.sif_fault.bits.b1CellUvp = g_stCellInfoReport.unMdlFault_Third.bits.b1CellUvp;
    sif_clientData.sif_fault.bits.b1CellOvp = g_stCellInfoReport.unMdlFault_Third.bits.b1CellOvp;
    sif_clientData.sif_fault.bits.b1AFE_Err = System_ERROR_UserCallback(ERROR_STATUS_AFE1) != 0 ? 1 : 0;
    sif_clientData.sif_fault.bits.b1CellDischgUtp = g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgUtp;
    sif_clientData.sif_fault.bits.b1CellDischgOtp = g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgOtp;
    sif_clientData.sif_fault.bits.b1Mos_Err = 0;
    sif_clientData.sif_fault.bits.b1PchgMos_Err = 0;
    sif_clientData.sif_fault.bits.b1ChgMos_Err = 0;
    sif_clientData.sif_fault.bits.b1ChgPcs_Err = 0;
    sif_clientData.sif_fault.bits.b1IdsgOcp_warn = g_stCellInfoReport.unMdlFault_Second.bits.b1IdischgOcp;
    sif_clientData.sif_fault.bits.b1IchgOcp_warn = g_stCellInfoReport.unMdlFault_Second.bits.b1IchgOcp;
    sif_clientData.sif_fault.bits.b1CellUvp_warn = g_stCellInfoReport.unMdlFault_Second.bits.b1CellUvp;
    sif_clientData.sif_fault.bits.b1CellOvp_warn = g_stCellInfoReport.unMdlFault_Second.bits.b1CellOvp;
    sif_clientData.sif_fault.bits.b1CelldsgUtp_warn = g_stCellInfoReport.unMdlFault_Second.bits.b1CellDischgUtp;
    sif_clientData.sif_fault.bits.b1CellChgUtp_warn = g_stCellInfoReport.unMdlFault_Second.bits.b1CellChgUtp;
    sif_clientData.sif_fault.bits.b1CelldsgOtp_warn = g_stCellInfoReport.unMdlFault_Second.bits.b1CellDischgOtp;
    sif_clientData.sif_fault.bits.b1CellChgOtp_warn = g_stCellInfoReport.unMdlFault_Second.bits.b1CellChgOtp;

    sif_clientData.packType = ;
    sif_clientData.FaultLevel_DC = ;

    sif_clientData.PackStatus.bits.b1status_chg = g_stCellInfoReport.u16Ichg > 20 ? 1 : 0;
    sif_clientData.PackStatus.bits.b1status_dsg = g_stCellInfoReport.u16IDischg > 20 ? 1 : 0;
    sif_clientData.PackStatus.bits.b1status_idle = !(sif_clientData.PackStatus.bits.b1status_chg | sif_clientData.PackStatus.bits.b1status_dsg);
    sif_clientData.PackStatus.bits.b1status_mos_dsg = SystemStatus.bits.b1Status_MOS_DSG;
    sif_clientData.PackStatus.bits.b1status_mos_chg = SystemStatus.bits.b1Status_MOS_CHG;
    sif_clientData.PackStatus.bits.b1status_mos_pchg = SystemStatus.bits.b1Status_MOS_PRE;
    sif_clientData.PackStatus.bits.b1status_chgOver = ;
    sif_clientData.PackStatus.bits.b1status_chgConnected = ;
    sif_clientData.PackStatus.bits.b1status_chg_need = ;


    sif_clientData.PackID[0] = ;
    sif_clientData.PackID[1] = ;
    sif_clientData.res = ;
    sif_clientData.CRC_ID = ;

    gst_SifFrame.TxLength = sizeof(sif_clientData);
    memcpy(gst_SifFrame.u8_TxData, &sif_clientData.postID, gst_SifFrame.TxLength);
}

void sif_DataFill_msg2(void)
{
    gst_SifFrame.TxLength = sizeof(sif_PackOnlySN);
    memcpy(gst_SifFrame.u8_TxData, sif_PackOnlySN, gst_SifFrame.TxLength);
}
