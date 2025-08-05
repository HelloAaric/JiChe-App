#ifndef _SYS_CFG_H
#define _SYS_CFG_H

#include "basic_data_type.h"

void BkpRam_WriteUINT16(UINT32 adress, UINT16 data);
UINT16 BkpRam_ReadUINT16(UINT32 adress);

#endif	/* _SYS_CFG_H */
