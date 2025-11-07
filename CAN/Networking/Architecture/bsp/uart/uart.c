#include "uart.h"
#include <stdio.h>
#include "string.h"

void USART3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
u8 RxBuffer2[10] = {0};                                             
volatile u8 RxCnt2 = 0;
volatile u8  Rxfinish2 = 0;
volatile uint8_t lcd_cmd = 0x00;
//以下为串口2相关变量
// 全局变量
volatile unsigned char rx_buffer_uart2[60];             // 串口接收缓冲区（50字节+余量）
volatile int rx_len_uart2 = 0;                 // 当前接收数据长度
volatile int frame_ready = 0;                  // 帧接收完成标志


// 第1步：定义函数指针类型和全局函数指针变量

recv_callback_func uart_recv_callback = NULL;  // 全局函数指针，初始为空

// BSP层提供注册函数给上层调用
void bsp_register_callback(recv_callback_func callback) 
{
    uart_recv_callback = callback;  // 保存上层传来的函数地址
    printf("BSP: 回调函数已注册\n");
}


/*********************************************************************
 * @fn      USARTx_CFG
 *
 * @brief   Initializes the USART2 & USART3 peripheral.
 *
 * @return  none
 */
void USART23_CFG(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef  NVIC_InitStructure = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    /* USART2 TX-->A.2   RX-->A.3 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* USART3 TX-->B.10  RX-->B.11 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART2, &USART_InitStructure);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    USART_Init(USART3, &USART_InitStructure);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(USART2, ENABLE);
    USART_Cmd(USART3, ENABLE);
}


void USART3_IRQHandler(void)
{
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        lcd_cmd = USART_ReceiveData(USART3);
        // RxBuffer2[RxCnt2++] = USART_ReceiveData(USART3);

        // if(RxCnt2 >5)
        // {
        //     if (uart_recv_callback != NULL) {
        //         printf("BSP: 准备调用上层函数...\n");
        //         uart_recv_callback(RxBuffer2, RxCnt2);  // 实际调用上层函数！
        //     } else {
        //         printf("BSP: 没有注册回调函数\n");
        //     }

        //     //USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
        //     Rxfinish2 = 1;
        // }
    }
}

u16 extract_distance(char* buffer, int length);
void USART2_IRQHandler(void) 
{     
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)     
    {         
        char received_char = USART_ReceiveData(USART2);   
        if(frame_ready == 0)
        {
            // 存储接收到的字符         
            rx_buffer_uart2[rx_len_uart2] = received_char;         
            rx_len_uart2++;                  
            
            // 检查是否达到50个数据         
            if(rx_len_uart2 >= 13)         
            {             
                frame_ready = 1;  // 标记帧接收完成，可以处理数据
                // 注意：这里不清空缓冲区，等待主程序处理完后再清空
            }     
        }
        //     return;

    } 
}



