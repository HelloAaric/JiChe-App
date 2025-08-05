/******************************************************************************
;  *   	@MCU				 STM32F103C8T6
;  *   	@Create Date         2023.09.20
;  *    @Official website		 http://www.devechip.com/
;  *----------------------Abstract Description---------------------------------
;  *			    								FET��������
**************************************************************************************/
#include "FETControl.h"

/**
	* @˵��	Ԥ����������
	* @����	bool ��/��
	* @����ֵ	�ɹ�/ʧ��
	* @ע	
*/
bool PCHG_FETControl(bool abel){

	g_AfeRegs.R81.PCHGC=abel;

	return DVC11XX_WriteRegs(AFE_ADDR_R(81),1);
}
/**
	* @˵��	Ԥ����������
	* @����	bool ��/��
	* @����ֵ	�ɹ�/ʧ��
	* @ע	
*/
bool PDSG_FETControl(bool abel){

	g_AfeRegs.R81.PDSGC=abel;

	return DVC11XX_WriteRegs(AFE_ADDR_R(81),1);
}

/**
	* @˵��	�����������
	* @����	u8 ģʽ
	* @����ֵ	�ɹ�/ʧ��
	* @ע	
*/
bool CHG_FETControl(u8 mode){

	if(mode==2)
	g_AfeRegs.R84.CBDM=0;

	g_AfeRegs.R81.CHGC=mode;

	return DVC11XX_WriteRegs(AFE_ADDR_R(81),4);
}

/**
	* @˵��	�ŵ���������
	* @����	u8 ģʽ
	* @����ֵ	�ɹ�/ʧ��
	* @ע	
*/
bool DSG_FETControl(u8 mode){

	if(mode==2)
	g_AfeRegs.R83.DBDM=0;

	g_AfeRegs.R81.DSGC=mode;

	return DVC11XX_WriteRegs(AFE_ADDR_R(81),3);
}

/**
	* @˵��	�߱�DSG���ģʽ����
	* @����	bool ģʽ
	* @����ֵ	�ɹ�/ʧ��
	* @ע	
*/
bool DSGM_Control(bool mode){

	g_AfeRegs.R81.DSGM=mode;

	return DVC11XX_WriteRegs(AFE_ADDR_R(81),1);
}

/**
	* @˵��	�߱�DSG����ǿ������
	* @����	u8 ǿ�ȵȼ� 0~30
	* @����ֵ	�ɹ�/ʧ��
	* @ע	
*/
bool DPC_Config(u8 DPC){

	if(DPC<31){
	g_AfeRegs.R82.DPC=DPC;

	return DVC11XX_WriteRegs(AFE_ADDR_R(82),1);
	}
	return ERROR;
}
/**
	* @˵��	����������ֵ����
	* @����	u16 ������ֵ ��λ uV
	* @����ֵ	�ɹ�/ʧ��
	* @ע	������ֵ=BDPT*40uV
*/
bool BDPT_Config(u16 value){
	
	if(value<=10200){
	g_AfeRegs.R102.BDPT=value/40;

	return DVC11XX_WriteRegs(AFE_ADDR_R(102),1);
	}
	else
	return ERROR;
}
/**
	* @˵��	��ɱ��������
	* @����	u8 ģʽ
	* @����ֵ	�ɹ�/ʧ��
	* @ע	
*/
bool ChargePump_Control(u8 mode){
	
	g_AfeRegs.R109.CPVS=mode;

	return DVC11XX_WriteRegs(AFE_ADDR_R(109),1);
}

/**
	* @˵��	�߱�������������
	* @����	bool ��/��
	* @����ֵ	�ɹ�/ʧ��
	* @ע	������ֵ=BDPT*40uV
*/
bool HSFM_Control(bool mode){
	
	g_AfeRegs.R85.HSFM=mode;

	return DVC11XX_WriteRegs(AFE_ADDR_R(85),1);
}

