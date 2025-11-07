#ifndef __SW_H
#define __SW_H

#include "debug.h"
#include "stdint.h"

void GPIO_SW_INIT(void);
void SW_Release(void);
void SW_Catch(void);
void delay_us(unsigned int i);
void GPIO_Init2812();
void WS2812B_Reset(void);          //¸´Î»º¯Êý
void WS2812B_WriteColor( unsigned char C_R,unsigned char C_G,unsigned char C_B);


void yuyin_gpio();
void RGB_huxi(unsigned char mode);
void GPIO_fan_flame_light();
#define fan_on GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_SET)
#define fan_off GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_RESET)

#define flame_on GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_SET)
#define flame_off GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_RESET)

#define light_on GPIO_WriteBit(GPIOC, GPIO_Pin_5, Bit_SET)
#define light_off GPIO_WriteBit(GPIOC, GPIO_Pin_5, Bit_RESET)

#endif






