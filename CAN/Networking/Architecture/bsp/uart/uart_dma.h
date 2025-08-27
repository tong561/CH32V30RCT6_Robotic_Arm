#ifndef __UART_DMA_H
#define __UART_DMA_H

#include "debug.h"
#include "stdint.h"

// The length of a single buffer used by DMA
#define RX_BUFFER_LEN       (128u)
#define RING_BUFFER_LEN     (1024u)

extern uint8_t g_rx_data_buffer[RX_BUFFER_LEN];  // 全局接收数据数组
extern volatile uint8_t g_rx_data_ready;     // 数据就绪标志位
extern volatile uint16_t g_rx_data_len;      // 接收到的数据长度

typedef struct
{
    uint8_t           buffer[RING_BUFFER_LEN];
    volatile uint16_t RecvPos;
    volatile uint16_t SendPos;
    volatile uint16_t RemainCount;
} RingBuffer;

extern RingBuffer ring_buffer;

void USARTx_CFG(uint32_t baudrate);
void DMA_INIT(void);
uint8_t ring_buffer_pop();
#endif


