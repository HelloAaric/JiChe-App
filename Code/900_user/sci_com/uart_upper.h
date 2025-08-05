#ifndef _UART_UPPER_H
#define _UART_UPPER_H

#include "basic_data_type.h"
#include "data_deal.h"

#define	SCI_TX_BUF_LEN			            255   	//��־��¼��������Ϊ250
#define RS485_MAX_BUFFER_SIZE 	            255		//


#define	RS485_ACK_POS			        	0x00	// ����Ӧ
#define	RS485_ACK_NEG			        	0x01	// ����Ӧ
//Error type
#define	RS485_ERROR_ADDR_INVALID	    	0x01	// ��ַ���Ϸ�
#define	RS485_ERROR_CRC_ERROR				0x02	// CRCУ�����
#define	RS485_ERROR_DATA_INVALID	    	0x03	// �������Ϸ�
#define	RS485_ERROR_CMD_INVALID				0x04	// ��ǰ״̬��������Ч
#define	RS485_ERROR_RONLY_NO_W				0x05	// ֻ�������ܾ�д��
#define	RS485_ERROR_WONLY_NO_R				0x06	// ֻд�����ܾ���ȡ
#define	RS485_ERROR_NO_PERMISSION			0x07	// ��Ȩ��
#define	RS485_ERROR_NULL			    	0x08	// δ֪����


struct SOC_CAL_ELEMENT_UPPER {
	UINT16 u16Soc;                 	//��ǰ���SOC     0��100 Ϊ��������ٷֱ�
	UINT16 u16Soh;                 	//Ϊ���������ٷֱ�0����100
	UINT16 u16CapacityNow;        	//��ǰ����	Ah*100
	UINT16 u16CapacityFull;        	//��ǰ��������	Ah*100		//Ϊʲô*100Ϊ��λ�أ���Ϊ��λ����mAh�������������ʾ����
	UINT16 u16CapacityFactory;     	//������������	Ah*100		//�����Ľ����650Ah���
	UINT16 u16Cycle_times;     		//ѭ������
};

//485 cmd type
enum RS485_CMD_E {
	RS485_CMD_READ_REGS = 3,
	RS485_CMD_WRITE_REG = 6,
	RS485_CMD_WRITE_REGS = 16,
	
	//UART_CLIENT_CMD_0x01 = 0xA1,	//�ͻ���
	//UART_CLIENT_CMD_0x02 = 0xA2,	
};

struct MDLCHGFAULT_BITS     {    	// bits  description
	UINT8 b1CellOvp			:1;   	//
	UINT8 b1CellUvp			:1;   	//
	UINT8 b1BatOvp			:1;   	//
	UINT8 b1BatUvp			:1;   	//
	
	UINT8 b1IchgOcp			:1;   	//
	UINT8 b1IdischgOcp		:1;   	//
	UINT8 b1CellChgOtp		:1;   	//
	UINT8 b1CellDischgOtp 	:1;   	//

	UINT8 b1CellChgUtp		:1;   	//
	UINT8 b1CellDischgUtp 	:1;   	//
	UINT8 b1VcellDeltaBig	:1;   	//
	UINT8 b1TempDeltaBig 	:1;   	//���û�У�Res����

	UINT8 b1SocLow			:1;   	//
	UINT8 b1TmosOtp			:1;   	//
	UINT8 b1Rcved1	 		:1;   	//
	UINT8 b1Rcved2  		:1;   	//
};

union  MDLCHGFAULT_REG
{
	UINT16   all;
	struct MDLCHGFAULT_BITS	bits;
};


struct stCell_Info {
	UINT16	u16VCell[32];
	UINT16	u16VCellMax;                    // mv
	UINT16	u16VCellMin;                    // mv
    UINT16	u16VCellMaxPosition;
    UINT16	u16VCellMinPosition;
	UINT16	u16VCellDelta;                  // mv
	UINT16	u16VCellTotle;                  // v *100
    UINT16	u16Temperature[TEMP_NUM];       // +40��C *10
    UINT16	u16TempMax;                     // +40��C *10
	UINT16	u16TempMin;                     // +40��C *10
	UINT16	u16Ichg;                        // A *10
    UINT16	u16IDischg;                     // A *10
    //UINT16	u16Soc;							// %
    struct SOC_CAL_ELEMENT_UPPER SocElement;
    union MDLCHGFAULT_REG unMdlFault_First;
    union MDLCHGFAULT_REG unMdlFault_Second;
	union MDLCHGFAULT_REG unMdlFault_Third;
	UINT16	u16BalanceFlag1;                 //��ؾ����־λ1
	UINT16	u16BalanceFlag2;                 //��ؾ����־λ2

	UINT16	u16VCellpack;                   // V *100��ֻ�ܷ������
};


// SCI_485 Message Structure
struct RS485MSG {
	UINT8	ptr_no;          	// Word stating what state msg is in
	UINT8	csr;          		// I2C address of slave msg is intended for
	UINT16	u16RdRegStartAddr;	// read reg start addr
	UINT16	u16RdRegStartAddrActure;	//�Զ����ַ����
	UINT8	u16RdRegByteNum;    // read byte lenth
	UINT8	AckLenth;			// ack byte lenth
	UINT8	AckType;			// ack type
	UINT8	ErrorType;			// error type
	UINT8 	u16Buffer[RS485_MAX_BUFFER_SIZE];    // Array holding msg data - max that
	enum RS485_CMD_E enRs485CmdType;
};

extern struct RS485MSG g_stCurrentMsgPtr_SCI1;
extern struct RS485MSG g_stCurrentMsgPtr_SCI2;
extern struct RS485MSG g_stCurrentMsgPtr_SCI3;

extern struct stCell_Info g_stCellInfoReport;
extern UINT8 gu8_TxEnable_SCI1;
extern UINT8 gu8_TxEnable_SCI2;
extern UINT8 gu8_TxEnable_SCI3;
extern UINT8 u8FlashUpdateFlag;

void Sci1_CommonUpper_Rx_Deal(struct RS485MSG *s);
void Sci1_CommonUpper_FaultChk(void);
void Sci2_CommonUpper_Rx_Deal(struct RS485MSG *s);
void Sci2_CommonUpper_FaultChk(void);
void Sci3_CommonUpper_Rx_Deal(struct RS485MSG *s);
void Sci3_CommonUpper_FaultChk(void);



void InitData_CommonUpper(void);
void App_CommonUpper(void);

#endif	/* _UART_UPPER_H */
