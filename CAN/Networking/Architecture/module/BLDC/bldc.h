#ifndef __BLDC_H
#define __BLDC_H

#include "debug.h"
#include "stdint.h"

void CAN_MOTOR_MODE_SET(void);
void CAN_BLDC_POS_CONTROL(float angle,u8 motor_id);
#endif





