/******************************************************************************
;  *   	@MCU				 STM32F103C8T6
;  *   	@Create Date         2023.03.16
;  *    @Official website		 http://www.devechip.com/
;  *----------------------Abstract Description---------------------------------
;  *			    						 		均衡驱动
**************************************************************************************/
#include "CellBalance.h"

u8 bOTC;//过温标志位
int uiCellVmin,uiCellVmax;
int	uiCellvotage[AFE_MAX_CELL_CNT];
static u32	uiBalMaskFlags=0,uiBalMaskFlags_Prepared=0;

/**
	* @说明	均衡控制
	* @参数	u32 vlaue 均衡位
	* @返回值	
	* @注 
*/
void Balance_Contrl(u32 vlaue) {
	if(vlaue<0xffffff)
	{
		g_AfeRegs.R103_R105.CB[0]=vlaue>>16;
		g_AfeRegs.R103_R105.CB[1]=vlaue>>8;
		g_AfeRegs.R103_R105.CB[2]=vlaue&0xff;
		
		DVC11XX_WriteRegs(AFE_ADDR_R(103),3);//打开对应均衡位
	}
}


void OverTempProtect(u8 temp){
float t;
	t=DVC11XX_Calc_ChipTemp();//获取芯片温度
	
	if(t>=temp)
	bOTC=1;		//过温置位
	else
	bOTC=0;
}

/**
	* @说明	电芯最大最小电压计算
	* @参数	
	* @返回值	
	* @注	
*/
void CalcuVolMaxMin(void){
	s8 cellIndex;
	u8 i;
  int cellVoltage;

		for(i=0;i<AFE_MAX_CELL_CNT;i++)
		uiCellvotage[i]=DVC11XX_Calc_VCell(i);//各串电压实际值还原

		uiCellVmin = uiCellVmax = uiCellvotage[0];//初始化

  for(cellIndex=AFE_MAX_CELL_CNT-1;cellIndex>=0;cellIndex--){

		cellVoltage=uiCellvotage[cellIndex];
		if( cellVoltage>uiCellVmax){
		  uiCellVmax = cellVoltage;
		  if(uiCellVmin==0) uiCellVmin=uiCellVmax;
	  }
	  else if(cellVoltage<uiCellVmin && cellVoltage>0){//防0
		  uiCellVmin = cellVoltage;
			}
	}
}

/**
	* @说明	自动均衡处理
	* @参数	
	* @返回值	
	* @注	
*/

//void BalanceProcess(void){
// u32 newBals = 0;

//		uiBalMaskFlags=(g_AfeRegs.R103_R105.CB[0]<<16)+(g_AfeRegs.R103_R105.CB[1]<<8)+g_AfeRegs.R103_R105.CB[2];//刷新当前均衡位

//	  if(!bOTC && (uiCellVmin>uiBalanceVol)&&(uiCellVmax-uiCellVmin)>=uiBalanceVolDiff){//未过温、最低电压高于均衡开启最小电压、压差高于均衡开启阈值
//      u8 i;
//			int cellVoltage=0;
//      for(i=0;i<AFE_MAX_CELL_CNT;i++){
//				cellVoltage=uiCellvotage[i];
//		    if( (cellVoltage-uiCellVmin)>=uiBalanceVolDiff){
//#ifdef DVC1117
//					if(i>9)
//					newBals |= (1<<(2*i-9));
//					else
//#endif
//#ifdef DVC1114
//					if(i>3)
//					newBals |= (1<<(2*i-3));
//					else
//#endif
//#ifdef DVC1110
//					if(i>3)
//					newBals |= (1<<(3*i-6));
//					else
//#endif
//          newBals |= (1<<i);	//计算出需要开启均衡的各个位
//        }
//      }
//	  }
//	  if(newBals!=uiBalMaskFlags){ //均衡位变化
//		  if(newBals==uiBalMaskFlags_Prepared){
//		    u8 i;
//					uiBalMaskFlags=uiBalMaskFlags_Prepared;

//					Balance_Contrl(uiBalMaskFlags);
//					
//					for(i=0;i<AFE_MAX_CELL_CNT;i++){

//#ifdef DVC1117
//					if((i>9)	&&	((uiBalMaskFlags>>(2*i-9)&0x01)==1))
//					PRINT_AFE_INFO("CELL %d balance open\r\n",i+1);
//					else
//#endif
//#ifdef DVC1114
//					if((i>3)&&	((uiBalMaskFlags>>(2*i-3)&0x01)==1))
//					PRINT_AFE_INFO("CELL %d balance open\r\n",i+1);
//					else					
//#endif
//#ifdef DVC1110
//					if((i>3)&&	((uiBalMaskFlags>>(3*i-6)&0x01)==1))
//					PRINT_AFE_INFO("CELL %d balance open\r\n",i+1);
//					else				
//#endif				
//					if((uiBalMaskFlags>>i&0x01)==1)
//					PRINT_AFE_INFO("CELL %d balance open\r\n",i+1);
//					}
//		  }	
//      else
//			  uiBalMaskFlags_Prepared=newBals; 
//	  }
//	  else 
//		  uiBalMaskFlags_Prepared=uiBalMaskFlags;
//		  
//}
/**
	* @说明	自动均衡处理
	* @参数	
	* @返回值	
	* @注	
*/

void BalanceProcess(void){
 u32 newBals = 0;

		uiBalMaskFlags=(g_AfeRegs.R103_R105.CB[0]<<16)+(g_AfeRegs.R103_R105.CB[1]<<8)+g_AfeRegs.R103_R105.CB[2];//刷新当前均衡位

	  if(!bOTC && (uiCellVmin>uiBalanceVol)&&(uiCellVmax-uiCellVmin)>=uiBalanceVolDiff){//未过温、最低电压高于均衡开启最小电压、压差高于均衡开启阈值
      u8 i;
			int cellVoltage=0;
      for(i=0;i<AFE_MAX_CELL_CNT;i++){
				cellVoltage=uiCellvotage[i];
		    if( (cellVoltage-uiCellVmin)>=uiBalanceVolDiff){
#ifndef DVC1124
					newBals	|=(1<<CellMask[i]);
#else
					newBals	|=(1<<i);
#endif
        }
      }
	  }
	  if(newBals!=uiBalMaskFlags){ //均衡位变化
		  if(newBals==uiBalMaskFlags_Prepared){
		    u8 i;
					uiBalMaskFlags=uiBalMaskFlags_Prepared;

					Balance_Contrl(uiBalMaskFlags);
					
					for(i=0;i<24;i++){
						if(((uiBalMaskFlags>>i)&0x01)==1)
						{
#ifndef DVC1124
							if(CellTrue[i]<AFE_MAX_CELL_CNT)
							PRINT_AFE_INFO("CELL %d balance open\r\n",CellTrue[i]+1);
#else
							PRINT_AFE_INFO("CELL %d balance open\r\n",i+1);
#endif
						}
					}
		  }	
      else
			  uiBalMaskFlags_Prepared=newBals; 
	  }
	  else 
		  uiBalMaskFlags_Prepared=uiBalMaskFlags;
		  
}
