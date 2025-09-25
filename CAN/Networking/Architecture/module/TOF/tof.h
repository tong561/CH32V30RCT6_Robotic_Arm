#ifndef __TOF_H
#define __TOF_H

#include "debug.h"
#include "math.h"
#include "stdint.h"

// 外部变量声明
extern u16 distance_buffer[5];
extern u8 buffer_count;

// 外部函数声明
u16 extract_distance(char* buffer, int length);
int filter_distance_data(u16* distances, int length);
void tof_run(void);
#endif





