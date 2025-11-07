#ifndef __STEP_H
#define __STEP_H

#include "debug.h"
#include "stdint.h"
// 全局变量定义 - 角度转脉冲系数
extern float angle_to_pulse_1 ;  // 1号电机：90度对应的脉冲，减速比-90度需要的脉冲-细分//200
extern float angle_to_pulse_4  ;   // 4号电机的角度换算
extern float angle_to_pulse_5 ;   // 5号电机的角度换算
extern float angle_to_pulse_6;  // 6号电机的角度换算 
 // 全局变量定义 - 方向
extern u8 direction_1 ;// 1号电机的方向
extern u8 direction_4 ; // 4号电机的方向
extern u8 direction_5 ; // 5号电机的方向
extern u8 direction_6 ; // 6号电机的方向
void Control_Motor_new(float angle, u8 id,unsigned int speed, unsigned char acceleration);
void Control_Motor_Single(float angle, u8 id, u16 speed, u8 acceleration) ;
u8 CAN_Send_Position_Mode(u32 address, u8 direction, u16 speed, u8 acceleration, u32 pulse_count, u8 position_mode, u8 sync_mode);
void Control_Motor(float angle, u8 id);
#endif





