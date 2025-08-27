#ifndef __UART_H
#define __UART_H

#include "debug.h"
#include "stdint.h"
// Íâ²¿ÉùÃ÷
extern uint8_t RxBuffer2[10];
extern volatile uint8_t RxCnt2;
extern volatile uint8_t Rxfinish2;

void USART3_CFG(void);

#endif


