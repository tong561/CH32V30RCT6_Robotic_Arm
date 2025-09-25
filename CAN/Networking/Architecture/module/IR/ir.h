#ifndef __IR_H
#define __IR_H

#include "debug.h"
#include "stdint.h"
// 外部声明8个全局变量
extern volatile uint8_t ir_command;
void IR_EXTI_Callback(void);

#endif





