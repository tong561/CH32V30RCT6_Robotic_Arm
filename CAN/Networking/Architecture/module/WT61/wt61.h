#ifndef __WT61_H
#define __WT61_H

#include "debug.h"
#include "math.h"
#include "stdint.h"

// 外部变量声明 - 用于其他源文件中引用
extern float Roll_angle;    // 滚转角
extern float Pitch_angle;   // 俯仰角  
extern float Yaw_angle;     // 偏航角

uint8_t parse_wt_protocol(uint8_t *data_array, uint8_t data_length);

#endif





