#include "can_com.h"
#include "uart_upper.h"
#include "sys_timer.h"
#include "System_Monitor.h"
#include "sleep_user.h"
#include "pub_func.h"

// 接收ID
#define CAN_ADRESS_STD_ID						0x01

//发送ID
#define CANID_TX_Test      						((UINT32)0x001)

#define CANID_CHECK_0x00   					 	((UINT16)0x00)
#define CANID_CHECK_0x01                        ((UINT16)0x01)
#define CANID_CHECK_0x02                        ((UINT16)0x02)
#define CANID_CHECK_0x03                        ((UINT16)0x03)
#define CANID_CHECK_0x04                        ((UINT16)0x04)
#define CANID_CHECK_0x05                        ((UINT16)0x05)
#define CANID_CHECK_0x06                        ((UINT16)0x06)
#define CANID_CHECK_0x07                        ((UINT16)0x07)
#define CANID_CHECK_0x08                        ((UINT16)0x08)
#define CANID_CHECK_0x09                        ((UINT16)0x09)
#define CANID_CHECK_0x0A                        ((UINT16)0x0A)
#define CANID_CHECK_0x0B                        ((UINT16)0x0B)
#define CANID_CHECK_0x0C                        ((UINT16)0x0C)
#define CANID_CHECK_0x0D                        ((UINT16)0x0D)
#define CANID_CHECK_0x0E                        ((UINT16)0x0E)
#define CANID_CHECK_0x0F                        ((UINT16)0x0F)
#define CANID_CHECK_0x10                        ((UINT16)0x10)
#define CANID_CHECK_0x11                        ((UINT16)0x11)

union CanTxType_Status {
    UINT32   all;
    struct CanTxType_StatusBit {
	    UINT8 b1CanTx_Test     	:1;
		UINT8 b1CanTx_0x00		:1;
		UINT8 b1CanTx_0x01		:1;
    	UINT8 b1CanTx_0x02		:1;
		
        UINT8 b1CanTx_0x03     	:1;
        UINT8 b1CanTx_0x04     	:1;
        UINT8 b1CanTx_0x05     	:1;
        UINT8 b1CanTx_0x06     	:1;
		
        UINT8 b1CanTx_0x07     	:1;
        UINT8 b1CanTx_0x08    	:1;
        UINT8 b1CanTx_0x09    	:1;
        UINT8 b1CanTx_0x0A    	:1;
		
        UINT8 b1CanTx_0x0B    	:1;
        UINT8 b1CanTx_0x0C     	:1;
        UINT8 b1CanTx_0x0D    	:1;
        UINT8 b1CanTx_0x0E    	:1;

        UINT8 b1CanTx_0x0F    	:1;
        UINT8 b1CanTx_0x10     	:1;
        UINT8 b1CanTx_0x11    	:1;
        UINT8 b1CanTx_0x12    	:1;
		
        UINT8 b1CanTx_Res1      :4;
		UINT8 b1CanTx_Res2      :8;
     }bits;	
};


volatile union Can_Status Can_Status_Flag;
volatile union CanTxType_Status CanTxType_Flag;
CanTxMsg TxMessage; 
CanRxMsg RxMessage;

UINT16 g_u16BusOff_InitTestCnt = 0;             //CAN总线关闭计时
UINT16 g_u16BusOff_RecoverCnt = 0;          	//5s计时标志位


void CAN_Tx_Data(CanTxMsg* pTxFrame)
{
	CanSendMsg(ID_CAN1, pTxFrame);
}


void CAN_TX_Test(void) { 
	TxMessage.TxHeader.ID             = CANID_TX_Test;		//标准帧ID
    TxMessage.TxHeader.IdType         = FDCAN_STANDARD_ID;	//标准标识符
    TxMessage.TxHeader.TxFrameType    = FDCAN_DATA_FRAME;	//为数据帧
    TxMessage.TxHeader.DataLength     = FDCAN_DLC_BYTES_8;	//消息的数据长度为8个字节

	TxMessage.TxData[0] = 1; 				//数据
	TxMessage.TxData[1] = 2;
	TxMessage.TxData[2] = 3;
	TxMessage.TxData[3] = 4;
	TxMessage.TxData[4] = 5;
	TxMessage.TxData[5] = 6;
	TxMessage.TxData[6] = 7;
	TxMessage.TxData[7] = 8;

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x00(void) {
	UINT16 u16_tmp16a;

	TxMessage.TxHeader.ID             = CANID_CHECK_0x00;		//标准帧ID
    TxMessage.TxHeader.IdType         = FDCAN_STANDARD_ID;	//标准标识符
    TxMessage.TxHeader.TxFrameType    = FDCAN_DATA_FRAME;	//为数据帧
    TxMessage.TxHeader.DataLength     = FDCAN_DLC_BYTES_8;	//消息的数据长度为8个字节


    u16_tmp16a = g_stCellInfoReport.u16VCellTotle;			//总压，10mV
 	TxMessage.TxData[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[1] = (UINT8)(u16_tmp16a & 0xFF);

	
    if(g_stCellInfoReport.u16IDischg > 0) {					//总电流，符号型，充电为正，放电为负
		u16_tmp16a = g_stCellInfoReport.u16IDischg*10;		//10mA
		u16_tmp16a = (0x7FFF - u16_tmp16a + 1) | 0x8000;		
	}
	else {
		u16_tmp16a = g_stCellInfoReport.u16Ichg*10; 		// 电池总电流
	}
 	TxMessage.TxData[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[3] = (UINT8)(u16_tmp16a & 0xFF);


	u16_tmp16a = g_stCellInfoReport.SocElement.u16CapacityNow;//剩余容量10mAh
 	TxMessage.TxData[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[5] = (UINT8)(u16_tmp16a & 0xFF);


	u16_tmp16a = Sci_CRC16RTU(TxMessage.TxData, 6);
 	TxMessage.TxData[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x01(void) {
	UINT16 u16_tmp16a;

	TxMessage.TxHeader.ID             = CANID_CHECK_0x01;		//标准帧ID
    TxMessage.TxHeader.IdType         = FDCAN_STANDARD_ID;		//标准标识符
    TxMessage.TxHeader.TxFrameType    = FDCAN_DATA_FRAME;		//为数据帧
    TxMessage.TxHeader.DataLength     = FDCAN_DLC_BYTES_8;		//消息的数据长度为8个字节


    u16_tmp16a = g_stCellInfoReport.SocElement.u16CapacityFull;	//满电容量10mAh
 	TxMessage.TxData[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[1] = (UINT8)(u16_tmp16a & 0xFF);

    u16_tmp16a = g_stCellInfoReport.SocElement.u16Cycle_times;	//循环次数
 	TxMessage.TxData[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.SocElement.u16Soc;
 	TxMessage.TxData[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.TxData, 6);
 	TxMessage.TxData[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x02(void) {
	UINT16 u16_tmp16a;

	TxMessage.TxHeader.ID             = CANID_CHECK_0x02;		//标准帧ID
    TxMessage.TxHeader.IdType         = FDCAN_STANDARD_ID;	//标准标识符
    TxMessage.TxHeader.TxFrameType    = FDCAN_DATA_FRAME;	//为数据帧
    TxMessage.TxHeader.DataLength     = FDCAN_DLC_BYTES_8;	//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16BalanceFlag1;
 	TxMessage.TxData[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);
    TxMessage.TxData[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16BalanceFlag2;
 	TxMessage.TxData[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);
    TxMessage.TxData[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.unMdlFault_Third.all;
 	TxMessage.TxData[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);
    TxMessage.TxData[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.TxData, 6);
 	TxMessage.TxData[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);
    TxMessage.TxData[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x03(void) {
	UINT16 u16_tmp16a;

	TxMessage.TxHeader.ID             = CANID_CHECK_0x03;		//标准帧ID
    TxMessage.TxHeader.IdType         = FDCAN_STANDARD_ID;	//标准标识符
    TxMessage.TxHeader.TxFrameType    = FDCAN_DATA_FRAME;	//为数据帧
    TxMessage.TxHeader.DataLength     = FDCAN_DLC_BYTES_8;	//消息的数据长度为8个字节

	switch(OPEN) {
		case 0:
			u16_tmp16a = ((~                        ((UINT16)(SystemStatus.all&0x000003FF)))&0x00FE)|(                        ((UINT16)(SystemStatus.all&0x000003FF))&0xFF01);
			break;
		case 1:
			u16_tmp16a = (UINT16)(SystemStatus.all&0x000003FF);
			break;
		default:
			u16_tmp16a = (UINT16)(SystemStatus.all&0x000003FF);
			break;
	}
 	TxMessage.TxData[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = 0x1234;				//生产日期
 	TxMessage.TxData[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = 0x1234;				//软件版本
 	TxMessage.TxData[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.TxData, 6);
 	TxMessage.TxData[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x04(void) {
	UINT16 u16_tmp16a;

	TxMessage.TxHeader.ID             = CANID_CHECK_0x04;		//标准帧ID
    TxMessage.TxHeader.IdType         = FDCAN_STANDARD_ID;	//标准标识符
    TxMessage.TxHeader.TxFrameType    = FDCAN_DATA_FRAME;	//为数据帧
    TxMessage.TxHeader.DataLength     = FDCAN_DLC_BYTES_8;	//消息的数据长度为8个字节

    TxMessage.TxData[0] = SeriesNum;			//电池串数
	TxMessage.TxData[1] = System_ErrFlag.u8ErrFlag_CBC_DSG > 0 ? 1 : 0;

	u16_tmp16a = g_stCellInfoReport.u16TempMax;
 	TxMessage.TxData[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16TempMin;
 	TxMessage.TxData[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.TxData, 6);
 	TxMessage.TxData[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x05(void) {
	UINT16 u16_tmp16a;

	TxMessage.TxHeader.ID             = CANID_CHECK_0x05;		//标准帧ID
    TxMessage.TxHeader.IdType         = FDCAN_STANDARD_ID;	//标准标识符
    TxMessage.TxHeader.TxFrameType    = FDCAN_DATA_FRAME;	//为数据帧
    TxMessage.TxHeader.DataLength     = FDCAN_DLC_BYTES_8;	//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.SocElement.u16Soh;
 	TxMessage.TxData[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = 0;
 	TxMessage.TxData[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = 0;
 	TxMessage.TxData[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.TxData, 6);
 	TxMessage.TxData[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x06(void) {
	UINT16 u16_tmp16a;

	TxMessage.TxHeader.ID             = CANID_CHECK_0x06;		//标准帧ID
    TxMessage.TxHeader.IdType         = FDCAN_STANDARD_ID;	//标准标识符
    TxMessage.TxHeader.TxFrameType    = FDCAN_DATA_FRAME;	//为数据帧
    TxMessage.TxHeader.DataLength     = FDCAN_DLC_BYTES_8;	//消息的数据长度为8个字节

    u16_tmp16a = 0;
 	TxMessage.TxData[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = 0;
 	TxMessage.TxData[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = 0;
 	TxMessage.TxData[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.TxData, 6);
 	TxMessage.TxData[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x07(void) {
	UINT16 u16_tmp16a;

	TxMessage.TxHeader.ID             = CANID_CHECK_0x07;		//标准帧ID
    TxMessage.TxHeader.IdType         = FDCAN_STANDARD_ID;	//标准标识符
    TxMessage.TxHeader.TxFrameType    = FDCAN_DATA_FRAME;	//为数据帧
    TxMessage.TxHeader.DataLength     = FDCAN_DLC_BYTES_8;	//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16VCell[0];
 	TxMessage.TxData[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[1];
 	TxMessage.TxData[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[2];
 	TxMessage.TxData[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.TxData, 6);
 	TxMessage.TxData[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x08(void) {
	UINT16 u16_tmp16a;

	TxMessage.TxHeader.ID             = CANID_CHECK_0x08;		//标准帧ID
    TxMessage.TxHeader.IdType         = FDCAN_STANDARD_ID;	//标准标识符
    TxMessage.TxHeader.TxFrameType    = FDCAN_DATA_FRAME;	//为数据帧
    TxMessage.TxHeader.DataLength     = FDCAN_DLC_BYTES_8;	//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16VCell[3];
 	TxMessage.TxData[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[4];
 	TxMessage.TxData[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[5];
 	TxMessage.TxData[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.TxData, 6);
 	TxMessage.TxData[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x09(void) {
	UINT16 u16_tmp16a;

	TxMessage.TxHeader.ID             = CANID_CHECK_0x09;		//标准帧ID
    TxMessage.TxHeader.IdType         = FDCAN_STANDARD_ID;	//标准标识符
    TxMessage.TxHeader.TxFrameType    = FDCAN_DATA_FRAME;	//为数据帧
    TxMessage.TxHeader.DataLength     = FDCAN_DLC_BYTES_8;	//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16VCell[6];
 	TxMessage.TxData[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[7];
 	TxMessage.TxData[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[8];
 	TxMessage.TxData[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.TxData, 6);
 	TxMessage.TxData[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x0A(void) {
	UINT16 u16_tmp16a;

	TxMessage.TxHeader.ID             = CANID_CHECK_0x0A;		//标准帧ID
    TxMessage.TxHeader.IdType         = FDCAN_STANDARD_ID;	//标准标识符
    TxMessage.TxHeader.TxFrameType    = FDCAN_DATA_FRAME;	//为数据帧
    TxMessage.TxHeader.DataLength     = FDCAN_DLC_BYTES_8;	//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16VCell[9];
 	TxMessage.TxData[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[10];
 	TxMessage.TxData[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[11];
 	TxMessage.TxData[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.TxData, 6);
 	TxMessage.TxData[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x0B(void) {
	UINT16 u16_tmp16a;

	TxMessage.TxHeader.ID             = CANID_CHECK_0x0B;		//标准帧ID
    TxMessage.TxHeader.IdType         = FDCAN_STANDARD_ID;	//标准标识符
    TxMessage.TxHeader.TxFrameType    = FDCAN_DATA_FRAME;	//为数据帧
    TxMessage.TxHeader.DataLength     = FDCAN_DLC_BYTES_8;	//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16VCell[12];
 	TxMessage.TxData[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[13];
 	TxMessage.TxData[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[14];
 	TxMessage.TxData[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.TxData, 6);
 	TxMessage.TxData[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x0C(void) {
	UINT16 u16_tmp16a;

	TxMessage.TxHeader.ID             = CANID_CHECK_0x0C;		//标准帧ID
    TxMessage.TxHeader.IdType         = FDCAN_STANDARD_ID;	//标准标识符
    TxMessage.TxHeader.TxFrameType    = FDCAN_DATA_FRAME;	//为数据帧
    TxMessage.TxHeader.DataLength     = FDCAN_DLC_BYTES_8;	//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16VCell[15];
 	TxMessage.TxData[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[16];
 	TxMessage.TxData[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[17];
 	TxMessage.TxData[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.TxData, 6);
 	TxMessage.TxData[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x0D(void) {
	UINT16 u16_tmp16a;

	TxMessage.TxHeader.ID             = CANID_CHECK_0x0D;		//标准帧ID
    TxMessage.TxHeader.IdType         = FDCAN_STANDARD_ID;	//标准标识符
    TxMessage.TxHeader.TxFrameType    = FDCAN_DATA_FRAME;	//为数据帧
    TxMessage.TxHeader.DataLength     = FDCAN_DLC_BYTES_8;	//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16VCell[18];
 	TxMessage.TxData[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[19];
 	TxMessage.TxData[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[20];
 	TxMessage.TxData[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.TxData, 6);
 	TxMessage.TxData[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x0E(void) {
	UINT16 u16_tmp16a;

	TxMessage.TxHeader.ID             = CANID_CHECK_0x0E;		//标准帧ID
    TxMessage.TxHeader.IdType         = FDCAN_STANDARD_ID;	//标准标识符
    TxMessage.TxHeader.TxFrameType    = FDCAN_DATA_FRAME;	//为数据帧
    TxMessage.TxHeader.DataLength     = FDCAN_DLC_BYTES_8;	//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16VCell[21];
 	TxMessage.TxData[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[22];
 	TxMessage.TxData[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[23];
 	TxMessage.TxData[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.TxData, 6);
 	TxMessage.TxData[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x0F(void) {
	UINT16 u16_tmp16a;

	TxMessage.TxHeader.ID             = CANID_CHECK_0x0F;		//标准帧ID
    TxMessage.TxHeader.IdType         = FDCAN_STANDARD_ID;	//标准标识符
    TxMessage.TxHeader.TxFrameType    = FDCAN_DATA_FRAME;	//为数据帧
    TxMessage.TxHeader.DataLength     = FDCAN_DLC_BYTES_8;	//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16VCell[24];
 	TxMessage.TxData[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[25];
 	TxMessage.TxData[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[26];
 	TxMessage.TxData[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.TxData, 6);
 	TxMessage.TxData[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x10(void) {
	UINT16 u16_tmp16a;

	TxMessage.TxHeader.ID             = CANID_CHECK_0x10;		//标准帧ID
    TxMessage.TxHeader.IdType         = FDCAN_STANDARD_ID;	//标准标识符
    TxMessage.TxHeader.TxFrameType    = FDCAN_DATA_FRAME;	//为数据帧
    TxMessage.TxHeader.DataLength     = FDCAN_DLC_BYTES_8;	//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16VCell[27];
 	TxMessage.TxData[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[28];
 	TxMessage.TxData[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[29];
 	TxMessage.TxData[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.TxData, 6);
 	TxMessage.TxData[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x11(void) {
	UINT16 u16_tmp16a;

	TxMessage.TxHeader.ID             = CANID_CHECK_0x11;		//标准帧ID
    TxMessage.TxHeader.IdType         = FDCAN_STANDARD_ID;	//标准标识符
    TxMessage.TxHeader.TxFrameType    = FDCAN_DATA_FRAME;	//为数据帧
    TxMessage.TxHeader.DataLength     = FDCAN_DLC_BYTES_8;	//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16VCell[30];
 	TxMessage.TxData[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[31];
 	TxMessage.TxData[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = 0;
 	TxMessage.TxData[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.TxData, 6);
 	TxMessage.TxData[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}

void CAN_TX_0x1F0(void)
{
	UINT16 u16_tmp16a;
	static UINT8 su8_changeStatus = 0;

	TxMessage.TxHeader.ID             = 0x1F0;				//标准帧ID
    TxMessage.TxHeader.IdType         = FDCAN_STANDARD_ID;	//标准标识符
    TxMessage.TxHeader.TxFrameType    = FDCAN_DATA_FRAME;	//为数据帧
    TxMessage.TxHeader.DataLength     = FDCAN_DLC_BYTES_8;	//消息的数据长度为8个字节

    u16_tmp16a = 2000;
 	TxMessage.TxData[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = 2000;
 	TxMessage.TxData[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.SocElement.u16Soc * 10;
 	TxMessage.TxData[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = su8_changeStatus;
	su8_changeStatus = !su8_changeStatus;
 	TxMessage.TxData[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.TxData[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}



void Can_BusOFF_FaultTimeCtrl(void) {

}


void Can_BusOFF_FaultChk(void) {

}


void Can_BusOFF_Recover(void) {

}


void Can_BusOFF_Monitor(void) {
	Can_BusOFF_FaultChk();
	Can_BusOFF_FaultTimeCtrl();
	Can_BusOFF_Recover();
}


void Can_ReceiveDeal(void) {
	if(!Can_Status_Flag.bits.b1Can_Received) {
		return;
	}

	RTC_ExtComCnt++;

	//ID已经过滤掉，不需要再判断ID是否为本机
	if((RxMessage.RxHeader.ID>>7) != CAN_ADRESS_STD_ID) {
		return;
	}
	
	switch(RxMessage.RxHeader.ID&0x007F) {	//按照逻辑，是这个函数处理接受数据，然后处理数据，然后赋值标志位返回才对
		case CANID_CHECK_0x00:
			CanTxType_Flag.bits.b1CanTx_0x00 = 1;
			break;
		case CANID_CHECK_0x01:
			CanTxType_Flag.bits.b1CanTx_0x01 = 1;
			break;
		case CANID_CHECK_0x02:
			CanTxType_Flag.bits.b1CanTx_0x02 = 1;
			break;
		case CANID_CHECK_0x03:
			CanTxType_Flag.bits.b1CanTx_0x03 = 1;
			break;
		case CANID_CHECK_0x04:
			CanTxType_Flag.bits.b1CanTx_0x04 = 1;
			break;
		case CANID_CHECK_0x05:
			CanTxType_Flag.bits.b1CanTx_0x05 = 1;
			break;
		case CANID_CHECK_0x06:
			CanTxType_Flag.bits.b1CanTx_0x06 = 1;
			break;
		case CANID_CHECK_0x07:
			CanTxType_Flag.bits.b1CanTx_0x07 = 1;
			break;
		case CANID_CHECK_0x08:
			CanTxType_Flag.bits.b1CanTx_0x08 = 1;
			break;
		case CANID_CHECK_0x09:
			CanTxType_Flag.bits.b1CanTx_0x09 = 1;
			break;
		case CANID_CHECK_0x0A:
			CanTxType_Flag.bits.b1CanTx_0x0A = 1;
			break;
		case CANID_CHECK_0x0B:
			CanTxType_Flag.bits.b1CanTx_0x0B = 1;
			break;
		case CANID_CHECK_0x0C:
			CanTxType_Flag.bits.b1CanTx_0x0C = 1;
			break;
		case CANID_CHECK_0x0D:
			CanTxType_Flag.bits.b1CanTx_0x0D = 1;
			break;
		case CANID_CHECK_0x0E:
			CanTxType_Flag.bits.b1CanTx_0x0E = 1;
			break;
		case CANID_CHECK_0x0F:
			CanTxType_Flag.bits.b1CanTx_0x0F = 1;
			break;
		case CANID_CHECK_0x10:
			CanTxType_Flag.bits.b1CanTx_0x10 = 1;
			break;
		case CANID_CHECK_0x11:
			CanTxType_Flag.bits.b1CanTx_0x11 = 1;
			break;

		default:
			break;
	}
	
	Can_Status_Flag.bits.b1Can_Received = 0;
}


void Can_TransmitDeal(void) {
	if(CanTxType_Flag.all) {
		// 改成接收那里
		// RTC_ExtComCnt++;
	}

    if(CanTxType_Flag.bits.b1CanTx_0x00) {
        CanTxType_Flag.bits.b1CanTx_0x00 = 0;
		CAN_TX_0x00();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x01) {
        CanTxType_Flag.bits.b1CanTx_0x01 = 0;
		CAN_TX_0x01();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x02) {
       	CanTxType_Flag.bits.b1CanTx_0x02 = 0;
		CAN_TX_0x02();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x03) {
        CanTxType_Flag.bits.b1CanTx_0x03 = 0;
		CAN_TX_0x03();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x04) {
        CanTxType_Flag.bits.b1CanTx_0x04 = 0;
		CAN_TX_0x04();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x05) {
        CanTxType_Flag.bits.b1CanTx_0x05 = 0;
		CAN_TX_0x05();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x06) {
        CanTxType_Flag.bits.b1CanTx_0x06 = 0;
		CAN_TX_0x06();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x07) {
        CanTxType_Flag.bits.b1CanTx_0x07 = 0;
		CAN_TX_0x07();
    }
	else if(CanTxType_Flag.bits.b1CanTx_0x08) {
        CanTxType_Flag.bits.b1CanTx_0x08 = 0;
		CAN_TX_0x08();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x09) {
       	CanTxType_Flag.bits.b1CanTx_0x09 = 0;
		CAN_TX_0x09();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x0A) {
        CanTxType_Flag.bits.b1CanTx_0x0A = 0;
		CAN_TX_0x0A();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x0B) {
        CanTxType_Flag.bits.b1CanTx_0x0B = 0;
		CAN_TX_0x0B();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x0C) {
        CanTxType_Flag.bits.b1CanTx_0x0C = 0;
		CAN_TX_0x0C();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x0D) {
        CanTxType_Flag.bits.b1CanTx_0x0D = 0;
		CAN_TX_0x0D();
    }
	else if(CanTxType_Flag.bits.b1CanTx_0x0E) {
        CanTxType_Flag.bits.b1CanTx_0x0E = 0;
		CAN_TX_0x0E();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x0F) {
       	CanTxType_Flag.bits.b1CanTx_0x0F = 0;
		CAN_TX_0x0F();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x10) {
        CanTxType_Flag.bits.b1CanTx_0x10 = 0;
		CAN_TX_0x10();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x11) {
        CanTxType_Flag.bits.b1CanTx_0x11 = 0;
		CAN_TX_0x11();
    }
	else if(CanTxType_Flag.bits.b1CanTx_Test) {
        CanTxType_Flag.bits.b1CanTx_Test = 0;
        CAN_TX_Test();
    }
}

void Can_TransmiT_0x1F0(void)
{
	static UINT16 su16_Tcnt = 0;

	if(++su16_Tcnt >= 50) {
		su16_Tcnt= 0;
		CAN_TX_0x1F0();
	}
}

void Init_CanCom(void)
{
	InitCan();

	TxMessage.TxHeader.ErrorState     = FDCAN_ESI_PASSIVE;
    TxMessage.TxHeader.BitRateSwitch  = FDCAN_BRS_OFF;
    TxMessage.TxHeader.FDFormat       = FDCAN_CLASSIC_CAN;
    TxMessage.TxHeader.TxEventFifo    = FDCAN_NO_TX_EVENTS;
    TxMessage.TxHeader.MsgMarker      = 0x55;
}

//这个函数不能用Switch架构来解决，因为这个都是并行任务，不是串行。
void App_CanCom(void)
{
	if(STARTUP_CONT == System_FUNC_StartUp(SYSTEM_FUNC_STARTUP_CAN)) {
		// return;
	}

	if(0 == g_st_SysTimeFlag.bits.b1Sys10msFlag2) {
		return;
	}
	
	Can_BusOFF_Monitor();
	Can_ReceiveDeal();
	Can_TransmitDeal();

	Can_TransmiT_0x1F0();

}


void App_CanTest(void) {
	if(0 == g_st_SysTimeFlag.bits.b1Sys200msFlag1) {
		return;
    }
	CAN_TX_Test();
}


