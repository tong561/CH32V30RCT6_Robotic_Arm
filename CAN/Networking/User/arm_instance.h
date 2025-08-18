#ifndef __ARM_INSTANCE_H
#define __ARM_INSTANCE_H

#include "debug.h"
#include "stdint.h"

void CAN_MOTOR_MODE_SET(void);
void CAN_BLDC_POS_CONTROL(float angle,u8 motor_id);
void GPIO_SW_INIT(void);
void SW_Release(void);
void SW_Catch(void);
#endif
