#include "uart.h"
#include <stdio.h>
void USART3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));


u8 RxBuffer2[10] = {0};                                             
volatile u8 RxCnt2 = 0;
volatile u8  Rxfinish2 = 0;

volatile uint8_t lcd_cmd = 0x00;


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
void USART3_CFG(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef  NVIC_InitStructure = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

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

    USART_Init(USART3, &USART_InitStructure);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
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