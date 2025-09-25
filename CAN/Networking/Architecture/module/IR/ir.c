#include "ir.h"
#include "tim.h"
#include "exti.h"

volatile uint8_t nec_leader_detected = 0;
volatile uint8_t nec_space_detected  = 0;
volatile uint32_t nec_leader_count   = 0;
volatile uint32_t nec_space_count    = 0;
volatile uint32_t nec_zero_count = 0;
volatile uint32_t nec_one_count  = 0;
volatile uint8_t bits[32];
volatile uint8_t bitCount;
volatile uint8_t ir_address;
volatile uint8_t ir_address_inv;
volatile uint8_t ir_command;
volatile uint8_t ir_command_inv;
volatile uint8_t ir_frame_ready;  // 0 = none, 1 = full frame, 2 = repeat

void IR_EXTI_Callback(void)
{
    static uint8_t state = 0;
    uint32_t d = (uint32_t)time_125us;

    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == Bit_SET)
    {
        // 上升沿，开始计算时间准备
        time_125us = 0;
    }
    else
    {
        // 下降沿，根据 state 状态和 d 时间判断
        if (state == 0)
        {
            if (d >= 34 && d <= 40)
            {
                // 9 ms 低电平，开始接收数据
                state = 1;
                bitCount = 0;
            }
            else if (d >= 16 && d <= 20)
            {
                // 重复码
                ir_frame_ready = 2;
            }
        }
        else if (state == 1)
        {
            // 数据位的高电平时间判断 "0" / "1"
            if (d >= 3 && d <= 7)
            {
                bits[bitCount++] = 0;
            }
            else if (d >= 11 && d <= 17)
            {
                bits[bitCount++] = 1;
            }
            else
            {
                // 异常，重置
                state = 0;
            }

            // 接收 32 位，校验并解析
            if (bitCount == 32)
            {
                uint8_t a  = 0, ai = 0, c = 0, ci = 0;
                for (uint8_t i = 0; i < 8; i++)
                {
                    a  |= bits[i]       << i;
                    ai |= bits[i + 8]   << i;
                    c  |= bits[i + 16]  << i;
                    ci |= bits[i + 24]  << i;
                }
                if ((a ^ ai) == 0xFF && (c ^ ci) == 0xFF)
                {
                    ir_address      = a;
                    ir_address_inv  = ai;
                    ir_command      = c;
                    ir_command_inv  = ci;
                    ir_frame_ready  = 1;
                    // 打印接收到的四个字节（十六进制）
                    // printf("ADDR=0x%02X, ~ADDR=0x%02X, CMD=0x%02X, ~CMD=0x%02X\r\n",
                    //        a, ai, c, ci);
                    printf("cmd is 0x%02X\r\n",ir_command);
                }
                state = 0;
            }
        }
    }
}