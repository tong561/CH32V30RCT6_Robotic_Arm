/**
 * WT协议数据解析 - 单片机版本
 * 解析33字节数据包，提取角度信息
 */
#include "wt61.h"
#include <stdint.h>

// 全局变量定义
float Roll_angle = 0.0f;    // 滚转角
float Pitch_angle = 0.0f;   // 俯仰角
float Yaw_angle = 0.0f;     // 偏航角

/**
 * 将低字节和高字节组合成有符号16位整数
 * 按照WT协议手册要求：DATA1=(short)((short)DATA1H<<8|DATA1L)
 * @param low_byte  低字节
 * @param high_byte 高字节
 * @return 有符号16位整数
 */
static int16_t bytes_to_signed_short(uint8_t low_byte, uint8_t high_byte)
{
    return (int16_t)((int16_t)high_byte << 8 | low_byte);
}

/**
 * 计算校验和
 * @param data 数据数组指针
 * @param len  数据长度
 * @return 校验和（8位）
 */
static uint8_t calculate_checksum(uint8_t *data, uint8_t len)
{
    uint8_t sum = 0;
    for(uint8_t i = 0; i < len; i++)
    {
        sum += data[i];
    }
    return sum;
}

/**
 * WT协议数据解析主函数
 * @param data_array 33字节数据数组
 * @param data_length 数据长度（应为33）
 * @return 0-成功, 1-数据长度错误, 2-未找到角度数据, 3-校验和错误
 */
uint8_t parse_wt_protocol(uint8_t *data_array, uint8_t data_length)
{
    // 检查数据长度
    if(data_length != 33)
    {
        return 1; // 数据长度错误
    }
    
    // 查找角度数据帧 (TYPE = 0x53)
    for(uint8_t i = 0; i < 33; i += 11)
    {
        // 检查协议头和数据类型
        if(data_array[i] == 0x55 && data_array[i+1] == 0x53)
        {
            // 找到角度数据帧，验证校验和
            uint8_t calc_checksum = calculate_checksum(&data_array[i], 10);
            if(calc_checksum != data_array[i+10])
            {
                return 3; // 校验和错误
            }
            
            // 提取原始数据
            int16_t roll_raw = bytes_to_signed_short(data_array[i+2], data_array[i+3]);
            int16_t pitch_raw = bytes_to_signed_short(data_array[i+4], data_array[i+5]);
            int16_t yaw_raw = bytes_to_signed_short(data_array[i+6], data_array[i+7]);
            
            // 计算实际角度值
            Roll_angle = (float)roll_raw / 32768.0f * 180.0f;
            Pitch_angle = (float)pitch_raw / 32768.0f * 180.0f;
            Yaw_angle = (float)yaw_raw / 32768.0f * 180.0f;
            
            return 0; // 成功
        }
    }
    
    return 2; // 未找到角度数据
}
/* 使用示例：
int main()
{
    // 33字节数据数组（示例数据）
    uint8_t wt_data[33] = {
        0x55, 0x51, 0xeb, 0xff, 0xe9, 0x07, 0x9d, 0xff, 0xbe, 0xff, 0xd9,
        0x55, 0x52, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0xc9, 0x89, 0xfa,
        0x55, 0x53, 0xfe, 0x41, 0x6f, 0x00, 0x9f, 0xf1, 0x0d, 0x81, 0x74
    };
    
    // 解析数据
    uint8_t result = parse_wt_protocol(wt_data, 33);
    
    if(result == 0)
    {
        // 解析成功，可以使用全局变量
        printf("Roll: %.1f°, Pitch: %.1f°, Yaw: %.1f°\n", 
               Roll_angle, Pitch_angle, Yaw_angle);
    }
    else
    {
        printf("解析错误，错误码: %d\n", result);
    }
    
    return 0;
}
*/