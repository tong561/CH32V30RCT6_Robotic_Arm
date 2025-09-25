#ifndef __UART_H
#define __UART_H

#include "debug.h"
#include "stdint.h"

typedef void (*recv_callback_func)(uint8_t* data, int len);

extern uint8_t RxBuffer2[10];
extern volatile uint8_t RxCnt2;
extern volatile uint8_t Rxfinish2;
extern volatile uint8_t lcd_cmd;
// 外部声明（放在头文件中或其他源文件中使用）
extern volatile char rx_buffer_uart2[60];             // 串口接收缓冲区外部声明
extern volatile int rx_len_uart2;                     // 当前接收数据长度外部声明  
extern volatile int frame_ready;                      // 帧接收完成标志外部声明
void USART23_CFG(void);
u16 extract_distance(char* buffer, int length);
#endif


