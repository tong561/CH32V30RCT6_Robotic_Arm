#ifndef __CAN_H
#define __CAN_H

#include "debug.h"
#include "stdint.h"

void CAN_Mode_Init( u8 tsjw, u8 tbs2, u8 tbs1, u16 brp, u8 mode );
u8 CAN_Send_Msg(u8 *msg, u8 len, u8 isstd, u32 id);
#endif





