#include "flash.h"

// 这个是APP有效标志位，要放到最后一页，不然中途写一半中断就完了
const UINT32 g_LastLine __at(FLASH_ADDR_APP_VALID) = FLASH_VALUE_APP_VALID;


UINT16 FlashRead_OneHalfWord(UINT32 faddr)
{
	return *(UINT16*)faddr; 
}

UINT32 FlashRead_OneWord(UINT32 faddr)
{
	return *(UINT32*)faddr; 
}

BOOL FlashWrite_DoubleWord(UINT32 address, UINT32 data0, UINT32 data1)
{
	FLASH_STS result;
	FLASH_Unlock();

	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
	while(FLASH_EraseOnePage(address) != FLASH_EOP);
	result = FLASH_ProgramdoubleWord(address, data0, data1);

	FLASH_Lock();

    if(result == FLASH_EOP) {
        return TRUE;
    } else {
        return FALSE;
    }
}


BOOL FlashWrite_DoubleWord_NoErase(UINT32 address, UINT32 data0, UINT32 data1)
{
	FLASH_STS result;
	FLASH_Unlock();

	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
	// while(FLASH_EraseOnePage(address) != FLASH_EOP);
	result = FLASH_ProgramdoubleWord(address, data0, data1);

	FLASH_Lock();

    if(result == FLASH_EOP) {
        return TRUE;
    } else {
        return FALSE;
    }
}


BOOL FlashErase_OnePage(UINT32 Page_Address)
{
	FLASH_STS result;
	FLASH_Unlock();

	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
	while(FLASH_EraseOnePage(Page_Address) != FLASH_EOP);

	FLASH_Lock();

    if(result == FLASH_EOP) {
        return TRUE;
    } else {
        return FALSE;
    }
}

