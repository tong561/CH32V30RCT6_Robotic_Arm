#ifndef __STEP_H
#define __STEP_H

#include "debug.h"
#include "stdint.h"

u8 CAN_Send_Position_Mode(u32 address, u8 direction, u16 speed, u8 acceleration, u32 pulse_count, u8 position_mode, u8 sync_mode);
void Control_Motor(float angle, u8 id);
#endif





