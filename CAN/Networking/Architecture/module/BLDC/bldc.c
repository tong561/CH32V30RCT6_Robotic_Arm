#include "bldc.h"
#include "can.h"
#include "sw.h"
/*
这里是BLDC8108-48电机的module文件，以下为示例
    CAN_MOTOR_MODE_SET();
    //此处开启刹车电源
    CAN_BLDC_POS_CONTROL(0*8.0f,2);
	CAN_BLDC_POS_CONTROL(0*8.0f,3);
*/

// 将浮点数转换为 4 字节 IEEE 754 单精度浮点数编码
void float_to_bytes(float num, uint8_t *bytes) 
{
    // 将浮点数按位存储为 32 位整数
    uint32_t bits = *((uint32_t*)&num);

    // 将 32 位整数的每个字节提取出来并存入 bytes 数组
    bytes[0] = (bits >> 24) & 0xFF;  // 提取最高字节
    bytes[1] = (bits >> 16) & 0xFF;  // 提取第二字节
    bytes[2] = (bits >> 8) & 0xFF;   // 提取第三字节
    bytes[3] = bits & 0xFF;          // 提取最低字节
}

void CAN_MOTOR_MODE_SET(void)
{
    u8 tx_buffer[8];
    //设置为位置模式
    tx_buffer[0] = 0x03;//低位在前
    tx_buffer[1] = 0x00;//低位在前
    tx_buffer[2] = 0x00;//低位在前
    tx_buffer[3] = 0x00;//低位在前
    tx_buffer[4] = 0x03;
    tx_buffer[5] = 0x00;
    tx_buffer[6] = 0x00;
    tx_buffer[7] = 0x00;
    CAN_Send_Msg(tx_buffer,8,1,(u32)0x02<<5 | 0x0B);
    CAN_Send_Msg(tx_buffer,8,1,(u32)0x03<<5 | 0x0B);
    //进入闭环模式
    tx_buffer[0] = 0x08;//低位在前
    tx_buffer[1] = 0x00;//低位在前
    tx_buffer[2] = 0x00;//低位在前
    tx_buffer[3] = 0x00;//低位在前
    tx_buffer[4] = 0x00;
    tx_buffer[5] = 0x00;
    tx_buffer[6] = 0x00;
    tx_buffer[7] = 0x00;
    CAN_Send_Msg(tx_buffer,8,1,(u32)0x03<<5 | 0x07);
    CAN_Send_Msg(tx_buffer,8,1,(u32)0x02<<5 | 0x07);
    delay_us(10000);
}

// 在文件头部添加偏置开关宏定义
//#define ENABLE_MOTOR_OFFSET  // 注释掉这行就禁用偏置
void  CAN_BLDC_POS_CONTROL(float angle,u8 motor_id)//angle是角度，id是1~5,这里有单圈绝对值
{
    u8 tx_buffer[8];
    u8 ieee_tx[4];
    float rad;
    
#ifdef ENABLE_MOTOR_OFFSET
    // 根据电机ID添加偏置
    if(motor_id == 0x02) // J2电机
    {
        angle += (-1.0f) * (3.1415926f/2) / 6.28f * 360 * 8; // 添加J2偏置：-90°*8 = -720°
    }
    else if(motor_id == 0x03) // J3电机  
    {
        angle = -angle + 3.1415926f / 6.28f * 360 * 8; // J3偏置处理：反号+180°*8 = 1440°
    }
#else
    // 不使用偏置，保持原始angle
    // angle = angle; // 实际上不需要做任何处理
#endif
    
    if(motor_id == 0x02)
        angle+=98;
    else if(motor_id == 0X03)
    {
        angle = -(170 - angle);
    }
    rad = angle*8.0f/360 * 3.1415926f *2;
    //rad = fmodf(rad,6.28f);
    float_to_bytes(rad,ieee_tx);
    //设置为位置模式
    tx_buffer[0] = ieee_tx[3];//低位在前
    tx_buffer[1] = ieee_tx[2];//低位在前
    tx_buffer[2] = ieee_tx[1];//低位在前
    tx_buffer[3] = ieee_tx[0];//低位在前
    tx_buffer[4] = 0x00;
    tx_buffer[5] = 0x00;
    tx_buffer[6] = 0x00;
    tx_buffer[7] = 0x00;
    if(motor_id == 0x02)
    {
        CAN_Send_Msg(tx_buffer,8,1,(u32)0x02<<5 | 0x0C);
    }
    else if(motor_id == 0x03)
    {
        CAN_Send_Msg(tx_buffer,8,1,(u32)0x03<<5 | 0x0C);
    }
}

