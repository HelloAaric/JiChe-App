/******************************************************************************
;  *   	@MCU				 STM32F103C8T6
;  *   	@Create Date         2023.03.22
;  *    @Official website		 http://www.devechip.com/
;  *----------------------Abstract Description---------------------------------
;  *			    								˯�߿��ƴ���
**************************************************************************************/
//#include "AlertInterruptMask.h"
#include "Sleep.h"
/**
	* @˵��	�򿪶�ʱ˯��
	* @����	
	* @����ֵ	
	* @ע	����ֵ����ʵ�����д��
*/
void Sleep_config(void){

	DVC11XX_ReadRegs(AFE_ADDR_R(85), 17);
#ifdef TWK	//��ʱ����
	g_AfeRegs.R120.TIWK=1;//������ʱ����
	g_AfeRegs.R120.TWSE=1;//10��
	DVC11XX_WriteRegs(AFE_ADDR_R(120),1);
#endif
#ifdef CWK //��������
	g_AfeRegs.R85.CAES=1;//����CADCʹ��
	g_AfeRegs.R86.C1OS=1;//8ms
	DVC11XX_WriteRegs(AFE_ADDR_R(85),2);
	//������ŵ��������
	g_AfeRegs.R94.OCD2E=OCD2_E;//�����ŵ����ʹ��
	g_AfeRegs.R94.OCD2T=2;//8mV	�����ŵ������ֵ
	g_AfeRegs.R95.OCC2E=OCC2_E;//����������ʹ��
	g_AfeRegs.R95.OCC2T=2;//8mV	������������ֵ
	g_AfeRegs.R96.OCD2D=4;//�����ŵ����ʱ�� 16ms
	g_AfeRegs.R97.OCC2D=4;//����������ʱ�� 16ms
	//��·����
	g_AfeRegs.R98.SCDE=SCD_E;//�ŵ��·����ʹ��
	g_AfeRegs.R98.SCDT=2;//20mV	�ŵ��·��ֵ
	g_AfeRegs.R99.SCDD=1;//7.81us ��·������ʱ
	//
	g_AfeRegs.R101.CWT=50;//0.5mV ����������ֵ
	DVC11XX_WriteRegs(AFE_ADDR_R(94),8);
#endif
//	GP2_ModeConfig(0x06);//INT
//	InterruptShield_Config(0x7F);//GP�жϴ���ѡ��
	
}


