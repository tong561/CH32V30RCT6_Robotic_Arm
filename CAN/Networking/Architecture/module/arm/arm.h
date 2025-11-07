#ifndef __ARM_H
#define __ARM_H

#include "debug.h"
#include "stdint.h"

// 外部声明全局变量
extern float J1, J2, J3, J4;//new
extern float J1_global;
extern float J2_global;
extern float J3_global;
extern float J4_global;
extern float J5_global;
extern volatile float X_IN,Y_IN,Z_IN;
void robot_arm_5dof_method2(float X, float Y, float Z) ;
#endif
