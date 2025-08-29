#ifndef __UART_H
#define __UART_H

#include "debug.h"
#include "stdint.h"

typedef void (*recv_callback_func)(uint8_t* data, int len);

extern uint8_t RxBuffer2[10];
extern volatile uint8_t RxCnt2;
extern volatile uint8_t Rxfinish2;
extern volatile uint8_t lcd_cmd;
void USART3_CFG(void);

#endif


