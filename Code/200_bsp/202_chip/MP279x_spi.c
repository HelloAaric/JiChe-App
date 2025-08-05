#include "MP279x_spi.h"
#include "spi.h"
#include "sys_timer.h"
#include "io_define.h"

#define Current_Offset		        0
#define I_SENSE_R					1

unsigned char DataRxTemp[2]={0,0};
uint8_t gu8_AFE_Select = 0;

uint8_t ByteCRC(uint16_t CRC8, uint8_t data_in)
{
    uint16_t genPoly = 0x107;
    uint8_t i=0;
    CRC8 ^= data_in;
    for (i = 0; i < 8; i++) {
        if ((CRC8 & 0x80) == 0x80) {
            CRC8 = (CRC8 << 1) ^ genPoly;
        } else {
            CRC8 <<= 1;
        }
    }
    CRC8 &= 0xff;
    i = CRC8;
    return i;
}

uint8_t BlockCRC_check(uint8_t* Block, uint8_t BlockLen)
{
    uint16_t CRC8 = 0;
    uint8_t i=0;

    for (i = 0; i < BlockLen; i++) {
        CRC8 = ByteCRC(CRC8, *Block);
        Block++;
    }
    i = CRC8;
    return i;
}

//This function Writes a register of the MP2797. Error flag return if there is any problem.*****************************************************
uint8_t SPI_MP279x_Write(SPI_Module* SPIx, uint8_t Address, uint8_t Register, uint8_t DataL, uint8_t DataH, uint8_t usecrc)
{
	uint8_t count=4; 
	uint8_t dataTx[5]={0,0,0,0,0}, dataRx[5]={0,0,0,0,0};
    uint8_t ErrFlag;

	dataTx[0]=(Address<<1);
	dataTx[1]=Register;
	dataTx[2]=DataL;
	dataTx[3]=DataH;
	count=4;
	if (usecrc) {
		dataTx[4]=BlockCRC_check(dataTx, 4);
		count=5;
	}
	ErrFlag=SPI_TransmitReceive(SPIx, dataTx, dataRx, count, 0xFFFF);

	return ErrFlag;
}

//This function Reads a register from MP2797 BMS chip. Error flag return if there is any problem.***************************************************
uint8_t SPI_MP279x_Read(SPI_Module* SPIx, uint8_t Address, uint8_t Register, uint8_t* DataL, uint8_t* DataH, uint8_t usecrc)
{
	uint8_t count=4; 
	uint8_t dataTx[5]={0,0,0,0,0}, dataRx[5]={0,0,0,0,0};
	uint8_t ErrFlag;

	dataTx[0]=((Address<<1)|0x01);
	dataTx[1]=Register;
	dataTx[2]=0xFF;
	dataTx[3]=0xFF;
	dataTx[4]=0xFF;
	count=4;
	if (usecrc) {
		count = 5;
	}

	ErrFlag=SPI_TransmitReceive(SPIx, dataTx, dataRx, count, 0xFFFF);

	if (usecrc) {
        dataTx[2]=dataRx[2];
        dataTx[3]=dataRx[3];
        dataTx[4]=BlockCRC_check(dataTx, 4);
        if(dataTx[4]!=dataRx[4]) {
            ErrFlag=1;
        }
	}

    *DataL=dataRx[2];
    *DataH=dataRx[3];

	return ErrFlag;
}

CFG_TypeDef MP2797_CFG_CellNum(SPI_Module* SPIx, uint8_t Address, uint8_t cellNums)
{
    uint8_t ErrFlag = 0;
	uint8_t cellNumSet = 0;
    if(cellNums > 16) cellNumSet = 16;
    if(cellNums < 7) cellNumSet = 7;
    ErrFlag |= SPI_MP279x_Write(SPIx, Address, REG_CELL_CONFIG, (cellNumSet - 1)&0x0F, 0, USE_CRC);
    return (CFG_TypeDef)ErrFlag;
}

CFG_TypeDef MP2797_CFG_WDT(SPI_Module* SPIx, uint8_t Address, FUNC_EN wdt_en, uint16_t bark_timeout, uint16_t bite_timeout)
{
    uint8_t ErrFlag = 0;
	uint8_t bark, bite;
	uint16_t WDT;
	if((bark_timeout>=25)&&(bark_timeout<=3200)) bark=(bark_timeout-25)/25;
	else if(bark_timeout<25) bark=0;
	else bark=0x7F;
	if((bite_timeout>=25)&&(bite_timeout<=3200)) bite=(bite_timeout-25)/25;
	else if(bite_timeout<25) bite=0;
	else bite=0x7F;	
	WDT=wdt_en|(bark<<2)|(bite<<9);
	ErrFlag|= SPI_MP279x_Write(SPIx, Address, REG_WDT_CONFIG, WDT&0x00FF, ((WDT&0xFF00)>>8), USE_CRC);
	return (CFG_TypeDef) ErrFlag;	
}

CFG_TypeDef MP2797_CFG_CC(SPI_Module* SPIx, uint8_t Address, FUNC_EN cc_en_safe,
						  FUNC_EN cc_pwr_save, uint8_t cc_int_cnt, FUNC_EN cc_b2b_acc_ctrl)
{
    uint8_t ErrFlag = 0;
	uint16_t CC_CFG;
	if(cc_int_cnt>64) cc_int_cnt=64;
	CC_CFG = (cc_en_safe<<1)|(cc_pwr_save<<3)|((cc_int_cnt-1)<<8)|(cc_b2b_acc_ctrl<<14);
	ErrFlag |= SPI_MP279x_Write(SPIx, Address, REG_CC_CONFIG, CC_CFG&0x00FF, ((CC_CFG&0xFF00)>>8), USE_CRC);
	return (CFG_TypeDef) ErrFlag;
}

CFG_TypeDef MP2797_NTC_EN(SPI_Module* SPIx, uint8_t Address, FUNC_EN ntc1, FUNC_EN ntc2, FUNC_EN ntc3, FUNC_EN ntc4)
{
	uint8_t ErrFlag = 0;
	uint16_t NTC_EN;
	ErrFlag |= SPI_MP279x_Read(SPIx, Address,REG_NTC_CONFIG, (uint8_t *)&NTC_EN, (uint8_t *)&NTC_EN + 1, USE_CRC);
	NTC_EN &= (1|(1<<2)|(1<<4)|(1<<6));
	NTC_EN |= ntc1|(ntc2<<2)|(ntc3<<4)|(ntc4<<6);
  	ErrFlag |= SPI_MP279x_Write(SPIx, Address, REG_NTC_CONFIG, NTC_EN&0x00FF, ((NTC_EN&0xFF00)>>8), USE_CRC);
	return (CFG_TypeDef) ErrFlag;
}

CFG_TypeDef MP2797_NTC_FT_NTCB(SPI_Module* SPIx, uint8_t Address, FUNC_EN cell_cold, FUNC_EN cell_hot,
							   FUNC_EN pcb_hot, FUNC_EN ntcb_dyon)
{
	uint8_t ErrFlag = 0;
	uint16_t NTC_FT;
	ErrFlag |= SPI_MP279x_Read(SPIx, Address, REG_NTC_CONFIG, (uint8_t *)&NTC_FT, (uint8_t *)&NTC_FT + 1, USE_CRC);
	NTC_FT &= ~((1<<13)|(1<<14)|(1<<15)|(1<<10));
	NTC_FT |= (cell_hot<<13)|(cell_cold<<14)|(pcb_hot<<15)|(ntcb_dyon<<10);
  	ErrFlag |= SPI_MP279x_Write(SPIx, Address, REG_NTC_CONFIG, NTC_FT&0x00FF, ((NTC_FT&0xFF00)>>8), USE_CRC);
	return (CFG_TypeDef) ErrFlag;
}



CFG_TypeDef MP2797_CFG_STB(SPI_Module* SPIx, uint8_t Address, FUNC_EN stby_en, uint8_t stby_cur_th,
						   FUNC_EN stby_hys, uint8_t stby_mon_cfg, FUNC_EN stby_pfet_en)
{
    uint8_t ErrFlag = 0;
	uint16_t STB_CFG;
	STB_CFG = stby_en|(stby_cur_th<<1)|(stby_hys<<3)|(stby_mon_cfg<<4)|(stby_pfet_en<<6);
	ErrFlag|= SPI_MP279x_Write(SPIx,Address,REG_STB_CONFIG, STB_CFG&0x00FF, ((STB_CFG&0xFF00)>>8), USE_CRC);	 
	return (CFG_TypeDef) ErrFlag;		
}

CFG_TypeDef MP2797_CFG_Safe(SPI_Module* SPIx, uint8_t Address, FUNC_EN prot_in_safe, FUNC_EN safe_scoc_en, uint8_t act_mon_cfg)
{
    uint8_t ErrFlag = 0;
    uint16_t SAFE_CFG;
	ErrFlag |= SPI_MP279x_Read(SPIx,Address,REG_SAFE_CONFIG, (uint8_t*)&SAFE_CFG, (uint8_t*)&SAFE_CFG + 1, USE_CRC);
	SAFE_CFG &= ~(1|(1<<1)|(1<<3));
	SAFE_CFG |= prot_in_safe|(safe_scoc_en<<1)|(act_mon_cfg<<3);
	ErrFlag |= SPI_MP279x_Write(SPIx,Address,REG_SAFE_CONFIG, SAFE_CFG&0x00FF, ((SAFE_CFG&0xFF00)>>8), USE_CRC);	 
	return (CFG_TypeDef) ErrFlag;	
}

CFG_TypeDef MP2797_CFG_ADC_Go(SPI_Module* SPIx, uint8_t Address)
{
    uint8_t ErrFlag = 0;
    ErrFlag|=SPI_MP279x_Write(SPIx, Address, REG_ADC_CTRL, 0x01, 0x00, USE_CRC);
    return (CFG_TypeDef) ErrFlag;
}
 
CFG_TypeDef MP2797_CFG_ADC_Stop(SPI_Module* SPIx, uint8_t Address)
{
    uint8_t ErrFlag = 0;
    ErrFlag|=SPI_MP279x_Write(SPIx, Address, REG_ADC_CTRL, 0x00, 0x00, USE_CRC);
    return (CFG_TypeDef) ErrFlag;
}

AFE_TypeDef MP2797_GetData_Vcell(SPI_Module* SPIx, uint8_t Address, Meas_REG* Measurements)
{
	unsigned char i;
    uint8_t ErrFlag = 0;

	// 全部获取
	for (i = 0; i < 16; i++) {
		ErrFlag |= SPI_MP279x_Read(SPIx, Address, REG_RD_VCELL1+2*i,
					(uint8_t *)(Measurements->Cells + i), ((uint8_t *)(Measurements->Cells + i)) + 1, USE_CRC);
		// Measurements->Cells[i] = (uint16_t)(((uint32_t)(Measurements->Cells[i]*50000))/32768);
		Measurements->Cells[i] = (uint16_t)(((uint32_t)(Measurements->Cells[i]*5000))>>15);

	}
		
	ErrFlag |= SPI_MP279x_Read(SPIx, Address, REG_RD_VTOP, 
               (uint8_t *)&Measurements->VTOP, ((uint8_t *) &Measurements->VTOP) + 1, USE_CRC);
	Measurements->VTOP = ((Measurements->VTOP*80*100)/32768);

	ErrFlag |= SPI_MP279x_Read(SPIx ,Address, REG_RD_VPACKP,
               (uint8_t *)&Measurements->VPack, ((uint8_t *) &Measurements->VPack) + 1, USE_CRC);
	Measurements->VPack = ((Measurements->VPack*80*100)/32768);

	return (AFE_TypeDef) ErrFlag;
}


int32_t ITOP_Temp2 = 0;
int16_t DataTemp2 = 0;
AFE_TypeDef MP2797_GetData_Current(SPI_Module* SPIx, uint8_t Address, Meas_REG* Measurements)
{
    uint8_t ErrFlag = 0;
	int16_t CellsI = 0;
	int32_t ITOP_Temp = 0;

	// 全部获取
	for (unsigned char i=0; i<16; i++) {
		ErrFlag |= SPI_MP279x_Read(SPIx, Address, REG_RD_ICELL1+2*i,
					(uint8_t*)&CellsI, (uint8_t*)&CellsI + 1, USE_CRC);
		Measurements->Current[i] = ((CellsI*100*1000/32768)/I_SENSE_R) + Current_Offset; 	// uV
	}

	ErrFlag |= SPI_MP279x_Read(SPIx, Address, REG_RD_ITOP, (uint8_t*)&DataTemp2, ((uint8_t*)&DataTemp2)+1, USE_CRC);
	ITOP_Temp = (-(DataTemp2*100*1000/32768)/I_SENSE_R);
	ITOP_Temp2 = ITOP_Temp;
	if(ErrFlag!= 0) {
		return AFE_ERR;
	}
	Measurements->PackCurrent =- ITOP_Temp + Current_Offset;
	return (AFE_TypeDef)ErrFlag;
}

AFE_TypeDef MP2797_GetData_NTC(SPI_Module* SPIx, uint8_t Address, Meas_REG* Measurements)
{
    uint8_t ErrFlag = 0;
	uint16_t VTemp=0;
    unsigned char i = 0;
	
    //Read Cell temperature
	for (i = 0; i < 4; i++) {
		ErrFlag |= SPI_MP279x_Read(SPIx, Address, REG_RD_VNTC1 - i,
                   (uint8_t *)&VTemp, (uint8_t *)&VTemp + 1, USE_CRC);
        Measurements->TempVect[i] = (VTemp*100/32768);
	}

	return (AFE_TypeDef) ErrFlag;
}

AFE_TypeDef MP2797_GetData_Status(SPI_Module* SPIx, uint8_t Address, Meas_REG* Measurements)
{
    uint8_t ErrFlag = 0;
	uint16_t DataTemp;

	ErrFlag |= SPI_MP279x_Read(SPIx, Address, REG_PWR_STATUS,
               (uint8_t *) &DataTemp, ((uint8_t *) &DataTemp) + 1, USE_CRC);
	return (AFE_TypeDef) ErrFlag;
}

AFE_TypeDef MP2797_Ctrl_Balance(SPI_Module* SPIx, uint8_t Address, Meas_REG* Measurements)
{
	return AFE_OK;
}

void MP279x_WakeUpAFE(void)
{
	// MCUO_AFE_WAKE_BOTH_SHDN = 1;
	__delay_ms(100);
	// MCUO_AFE_WAKE_BOTH_SHDN = 0;
}

void MP279x_Init(SPI_Module* SPIx, uint8_t Address, uint8_t cellnums)
{
	uint8_t reset_num = 0;
	static uint8_t su8_Tcnt = 0;
	
	//Enable CRC and configure I2C Address; wait until everything is correct:
	do {
		if(reset_num > 5) {
			reset_num = 0;
			MP279x_WakeUpAFE();
			if(++su8_Tcnt >= 5) {
				su8_Tcnt = 0;
				break;
			}
		}
		SPI_MP279x_Write(SPIx, 1, REG_COMM_CONFIG, 0x04, Address, USE_CRC);
		reset_num++;
	} while (SPI_MP279x_Read(SPIx, Address,REG_COMM_CONFIG, &DataRxTemp[0], &DataRxTemp[1], USE_CRC));

	MP2797_CFG_CellNum(SPIx, Address, cellnums);                  	//0x00; Configue cell series in use
    //WDT configuration，看门狗
	MP2797_CFG_WDT(SPIx, Address, Disable, 2000, 3000);                 	//0x10;
	//CC configuration，电流
    MP2797_CFG_CC(SPIx, Address, Enable, Disable, 15, Disable);    	//0x9A
	MP2797_CFG_STB(SPIx, Address, Disable, 0, Disable, 2, Disable);	//0x06，主要是这个，disables standby mode.
	MP2797_CFG_Safe(SPIx, Address, Disable, Disable, 0);           	//0x07; Enable Slot A in Safe; 1 Slot A each 4 Slot Bs in active
	
	// 下面这两句话看看删掉是否可行。
	MP2797_NTC_EN(SPIx, Address, Enable, Enable, Enable, Enable);
	MP2797_NTC_FT_NTCB(SPIx, Address, Enable, Enable, Enable, Enable);

	// 加了这个才能采集温度。REG_HRSCAN0关键的寄存器默认是开启的， 不用管。
	SPI_MP279x_Write(SPIx, Address, REG_HRSCAN2, 0xE0, 0x01, USE_CRC);


    //Interrupt clear and Fault clear
	SPI_MP279x_Write(SPIx, Address, REG_INT0_CLR, 0xFF, 0xFF, USE_CRC);    // 0x17
	SPI_MP279x_Write(SPIx, Address, REG_INT1_CLR, 0xFF, 0xFF, USE_CRC);    // 0x18
	SPI_MP279x_Write(SPIx, Address, REG_FT_CLR, 0xFF, 0xFF, USE_CRC);      // 0x5F
}

/*
0：ON-ing
1：start
2：end
3：error
*/
uint8_t MP279x_GetDataOnce(SPI_Module* SPIx, uint8_t Address, Meas_REG* Measurements)
{
	uint8_t result = 0;
    uint16_t DataTemp = 0;

    // Read ADC Status
    SPI_MP279x_Read(SPIx,Address, REG_ADC_STS, (uint8_t *)&Measurements->ADC_STS,
                    ((uint8_t *)&Measurements->ADC_STS) + 1, USE_CRC);
    // Read ADC Command 
    SPI_MP279x_Read(SPIx, Address, REG_ADC_CTRL, (uint8_t *)&DataTemp, ((uint8_t *)&DataTemp)+1, USE_CRC);

	// ADC error
    if (DataTemp&0x04) {
        SPI_MP279x_Write(SPIx, Address, REG_BAL_CTRL, 0x00, 0x00, USE_CRC);		// Cell Balance stop
        SPI_MP279x_Write(SPIx, Address, REG_SFT_GO, 0x00, 0x00, USE_CRC);     	// Open wire stop
        SPI_MP279x_Write(SPIx, Address, REG_ADC_CTRL, 0x00, 0x00, USE_CRC);		// ADC Stop
		result = 3;
		// 不执行下面了
		return result;
    }

    // no ADC_Go command and the feature command scheduler status is Free
    if ((!(DataTemp&0x01)) && (!(Measurements->ADC_STS&0x300))) {
        MP2797_CFG_ADC_Go(SPIx, Address);
		result = 1;
		// 不执行下面了
		return result;
    }

    // ADC finished
    if (DataTemp&0x02) {
        MP2797_CFG_ADC_Stop(SPIx, Address);
        // Read ADC Results
        MP2797_GetData_Vcell(SPIx, Address, Measurements);
        MP2797_GetData_Current(SPIx, Address, Measurements);
        MP2797_GetData_NTC(SPIx, Address, Measurements);
        // MP2797_GetData_Status(SPIx, Address, Measurements);
		result = 2;
		// 不执行下面了
		return result;
    }

	return result;
}
