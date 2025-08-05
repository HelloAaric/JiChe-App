#ifndef _FLASH_H
#define _FLASH_H

#include "basic_data_type.h"
#include "n32h47x_48x_flash.h"


/*
N32H47x����512K��flash��һҳ8K��0x2000
������С��λ��һҳ��8K��
д����С��λ��64bits(8���ֽڣ�2����)
*/
#define FLASH_ADDR_IAP_START 			        0x08000000              // �ݶ�24K
#define FLASH_ADDR_IAP_END 			            0x08005FFF

#define FLASH_ADDR_APP_START 			        0x08006000		        // �ݶ�256K
#define APP_MAX_LEN				                0x00040000              // App file max length��Ŀǰ����APP���256K
#define FLASH_ADDR_APP_END 			            0x08045FFF		        // 512-24-8-8=472K

#define FLASH_ADDR_RUNTIME_LOG_START 	        0x08074000		        // ʵʱ�洢��̬��������־��¼
#define FLASH_ADDR_RUNTIME_SOC_START 	        0x08076000		        // ʵʱ�洢��̬����
#define FLASH_ADDR_PARA_SAVE_START 		        0x08078000		        // �������ò����洢��һ��λ��
#define FLASH_ADDR_END_PAGE_START 			    0x0807E000		        // 504K��ʼ���ο�ֵ
#define FLASH_ADDR_END 			                0x08080000		        // 512Kĩ��ǰһ���ֽڣ��ο�ֵ


// #define FLASH_ADDR_APP_VALID		            0x08026000              // ��־λ����128K����ط�
#define FLASH_ADDR_APP_VALID		            0x08010000              // �����ã�TODO���������÷����֡
#define FLASH_VALUE_APP_VALID		            0x12ABCDEF              // ��־λ����128K����ط�

//  BKP-SRAM   ��Χ��0x4002 3000~0x4002 3FFF  4K
#define BKP_SRAM_ADDR_START                     0x40023000
#define BKP_SRAM_ADDR_UPDATE_FLAG               0x40023000              // ������־λ
#define BKP_SRAM_TO_IAP_VALUE			        (0x00AB)
#define BKP_SRAM_TO_APP_VALUE			        (0x00CD)


#define BKP_SRAM_ADDR_SLEEP_FLAG                0x40023010	            // ���߹ؼ���־λ
#define BKP_SRAM_NORMAL_SLEEP_VALUE    	        (0x1234)
#define BKP_SRAM_DEEP_SLEEP_VALUE    		    (0x1235)
#define BKP_SRAM_HICCUP_SLEEP_VALUE    	        (0x1236)
#define BKP_SRAM_SLEEP_RESET_VALUE    		    (0x0000)


#if 0
#define FLASH_ADDR_SOC_VALUE_STORE 		0x0803E000			//����SOCֵ��λ��
#define FLASH_ADDR_SOC_EVENT_FLAG 		0x0803E800			//��ȷ���ش���󱣴滹�����߱����λ��
#define FLASH_VALUE_SLEEP_FLAG			((UINT16)0x1345)
#define FLASH_VALUE_POWEROFF_FLAG		((UINT16)0x1234)
#define FLASH_VALUE_STORE_RESET			((UINT16)0xFFFF)

#define FLASH_ADDR_SYS_LIMITS_ONCE 		0x0803D000			//ϵͳ����һ�ε����뱣��λ��
#define FLASH_ADDR_SYS_LIMITS_ALWAYS 	0x0803D800			//ϵͳ���ô�һ�ε����뱣��λ��
#define FLASH_VALUE_SYS_LIMITS_ONCE		((UINT16)12345)
#define FLASH_VALUE_SYS_LIMITS_ALWAYS	((UINT16)59137)
#define FLASH_VALUE_SYS_LIMITS_RESET	((UINT16)0xFFFF)

#define FLASH_ADDR_RTC_WK_TIME 			0x0803C000			//RTC���߻���ʱ�����
#define FLASH_VALUE_RTC_WK_TIME_RESET	((UINT16)0xFFFF)
#define FLASH_ADDR_SLEEP_ELEMENT 		0x0803C800			//������ز���
#define FLASH_VALUE_SLEEP_ELEMENT_RESET	((UINT16)0xFFFF)
#endif


UINT16 FlashRead_OneHalfWord(UINT32 faddr);
UINT32 FlashRead_OneWord(UINT32 faddr);
BOOL FlashWrite_DoubleWord(UINT32 address, UINT32 data0, UINT32 data1);
BOOL FlashWrite_DoubleWord_NoErase(UINT32 address, UINT32 data0, UINT32 data1);
BOOL FlashErase_OnePage(UINT32 Page_Address);


#endif	/* _FLASH_H */
