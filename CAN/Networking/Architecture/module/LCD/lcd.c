#include "lcd.h"
#include "uart.h"
#include "uart_dma.h"
#include <stdio.h>
#include <string.h>

// 声明12个全局变量，对应显示通道 n0~n11，添加 volatile 属性
volatile uint32_t n0 = 0;
volatile uint32_t n1 = 0;
volatile uint32_t n2 = 0;
volatile uint32_t n3 = 0;
volatile uint32_t n4 = 0;
volatile uint32_t n5 = 0;
volatile uint32_t n6 = 0;
volatile uint32_t n7 = 0;
volatile uint32_t n8 = 0;
volatile uint32_t n9 = 0;
volatile uint32_t n10 = 0;
volatile uint32_t n11 = 0;

// LCD发送任务
void LCD_SendTask(void) 
{
    char data_frame[32]; // 用于存储数据帧字符串
    uint8_t send_buffer[40]; // 最终发送的字节数组
    UART_Status_t status;

    // 定义一个数组存储全局变量的指针
    uint32_t *channels[] = {&n0, &n1, &n2, &n3, &n4, &n5, &n6, &n7, 
                           &n8, &n9, &n10, &n11};

    for (int i = 0; i < 12; i++) 
    {
        // 构造数据帧字符串，例如 "page0.n0.val=3\xFF\xFF\xFF"
        snprintf(data_frame, sizeof(data_frame), "page0.n%d.val=%d", i, *channels[i]);

        // 将字符串拷贝到发送缓冲区，并在末尾添加三个0xFF
        size_t len = strlen(data_frame);
        memcpy(send_buffer, data_frame, len);
        send_buffer[len] = 0xFF;
        send_buffer[len + 1] = 0xFF;
        send_buffer[len + 2] = 0xFF;

        // 调用UART_SendBytes函数发送数据
        status = UART_SendBytes(USART3, send_buffer, len + 3, 1000);
        if (status != UART_OK) 
        {
            printf("发送通道 n%d 数据失败\n", i);
        }
    }
}

uint8_t LCD_ReceiveTask(void) 
{
    uint8_t return_cmd = 0;
    switch (lcd_cmd) 
    {
        case 0xF1:
            printf("按下上按键\n");
            break;
        case 0xF2:
            printf("按下下按键\n");
            break;
        case 0xF3:
            printf("按下左按键\n");
            break;
        case 0xF4:
            printf("按下右按键\n");
            break;
        case 0xF5:
            printf("按下前按键\n");
            break;
        case 0xF6:
            printf("按下后按键\n");
            break;
        default:
            //printf("未定义的按键: 0x%02X\n", lcd_cmd);
            break;
    }
    if(lcd_cmd!=0x00)//单次按键单次调用
    {
        return_cmd = lcd_cmd;
        lcd_cmd = 0x00;
    }
    return return_cmd;
}