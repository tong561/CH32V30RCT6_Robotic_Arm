#ifndef __LCD_H
#define __LCD_H

#include "debug.h"
#include "stdint.h"
// 外部声明8个全局变量
extern volatile uint32_t n0;
extern volatile uint32_t n1;
extern volatile uint32_t n2;
extern volatile uint32_t n3;
extern volatile uint32_t n4;
extern volatile uint32_t n5;
extern volatile uint32_t n6;
extern volatile uint32_t n7;

void LCD_SendTask(void);
uint8_t LCD_ReceiveTask(void);
#endif





