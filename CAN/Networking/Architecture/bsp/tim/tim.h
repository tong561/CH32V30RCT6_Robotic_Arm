#ifndef __TIM_H
#define __TIM_H

#include "debug.h"
#include "stdint.h"

extern volatile u32 time_125us;
void TIM1_INT_Init( u16 arr, u16 psc);
#endif





