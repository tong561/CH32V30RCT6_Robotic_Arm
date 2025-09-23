#include "step.h"
#include "can.h"
#include "math.h"

/*
这里是步进电机的module层代码，can在前面调用BLDC时候已经用过，此处只需要调用api即可控制电机
    CAN_Send_Position_Mode(0x600,1,1000,200,(u32)step_pulse,1,0);
    变量定义如下
        float step_angle = 45;
        float angle90_to_pulse = 50*50*32;//减速比*90度对应的脉冲*细分
        float step_pulse;
*/
#define MAX_FRAME_LEN 8

// 全局变量定义
float angle_to_pulse_1 = 200 * 50 * 32; //90度对应的脉冲，减速比-90度需要的脉冲-细分
float angle_to_pulse_4 = 50 * 50 * 32; // 4号电机的角度换算
float angle_to_pulse_5 = 30 * 50 * 32; // 5号电机的角度换算
float angle_to_pulse_6 = 100 * 50 * 32; // 6号电机的角度换算

u8 direction_1 = 1; // 1号电机的方向
u8 direction_4 = 1; // 4号电机的方向
u8 direction_5 = 1; // 5号电机的方向
u8 direction_6 = 1; // 6号电机的方向

// 函数：位置模式控制命令发送
u8 CAN_Send_Position_Mode(u32 address, u8 direction, u16 speed, u8 acceleration, u32 pulse_count, u8 position_mode, u8 sync_mode) 
{
    u8 msg[10];  // 12 字节固定数据（不包括功能码和校验字节）
    u8 i = 0;

    // 构建命令数据（不包括功能码和校验字节）
    msg[0] = direction;                      // 方向 (0: CW, 1: CCW)
    msg[1] = (speed >> 8) & 0xFF;            // 速度高字节
    msg[2] = speed & 0xFF;                   // 速度低字节
    msg[3] = acceleration;                   // 加速度档位
    msg[4] = (pulse_count >> 24) & 0xFF;     // 脉冲数高字节
    msg[5] = (pulse_count >> 16) & 0xFF;     // 脉冲数中字节
    msg[6] = (pulse_count >> 8) & 0xFF;      // 脉冲数低字节
    msg[7] = pulse_count & 0xFF;             // 脉冲数低字节
    msg[8] = position_mode;                  // 相对/绝对模式标志 (0: 相对, 1: 绝对)
    msg[9] = sync_mode;                      // 多机同步标志 (0: 不启用, 1: 启用)

    // 分两帧发送，校验字节固定为 0x6B
    u8 frame_data[MAX_FRAME_LEN];
    u32 can_id = address;  // 第一帧的CAN ID为地址

    // 第一帧：发送功能码 + 7字节数据
    frame_data[0] = 0xFD;  // 功能码
    for (i = 0; i < 7; i++) {
        frame_data[i + 1] = msg[i];  // 复制前7个数据字节
    }

    // 调用 CAN_Send_Msg 函数发送第一帧
    u8 result = CAN_Send_Msg(frame_data, 8, 0, can_id);
    if (result != 0) {
        return 1;  // 发送失败
    }

    // 第二帧：发送功能码 + 剩余的5字节数据 + 校验字节
    frame_data[0] = 0xFD;  // 功能码
    for (i = 0; i < 3; i++) {
        frame_data[i + 1] = msg[i + 7];  // 复制后3个数据字节
    }
    frame_data[4] = 0x6B;  // 校验字节固定为 0x6B

    // 第二帧的CAN ID为地址 + 1
    can_id++;

    // 调用 CAN_Send_Msg 函数发送第二帧
    result = CAN_Send_Msg(frame_data, 5, 0, can_id);
    if (result != 0) {
        return 1;  // 发送失败
    }

    // 如果所有帧都成功发送，返回 0
    return 0;
}

// 控制电机的通用函数
void Control_Motor(float angle, u8 id) 
{
    u32 pulse_count;
    u8 direction;
    u32 address;
    float angle_ori = angle;
    if(angle <0)
        angle = -angle;

    // 根据电机ID设置对应的参数
    switch (id) {
        case 1:
            pulse_count = (u32)(angle * angle_to_pulse_1 / 90.0);
            direction = direction_1;
            address = 0x100;
            break;
        case 4:
            pulse_count = (u32)(angle * angle_to_pulse_4 / 90.0);
            direction = direction_4;
            address = 0x400;
            break;
        case 5:
            pulse_count = (u32)(angle * angle_to_pulse_5 / 90.0);
            direction = direction_5;
            address = 0x500;
            break;
        case 6:
            pulse_count = (u32)(angle * angle_to_pulse_6 / 90.0);
            direction = direction_6;
            address = 0x600;
            break;
        default:
            return; // 无效ID，直接返回
    }
        // 如果角度为负值，取反方向
    if (angle_ori < 0) {
        direction = !direction;
    }

    // 调用 CAN_Send_Position_Mode 发送控制命令
    CAN_Send_Position_Mode(address, direction, 1000, 200, pulse_count, 1, 0);
}
