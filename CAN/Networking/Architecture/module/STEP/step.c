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

// 全局变量定义 - 角度转脉冲系数
float angle_to_pulse_1 = 245 * 50 * 32;  // 1号电机：90度对应的脉冲，减速比-90度需要的脉冲-细分//200 49
float angle_to_pulse_4 = 50 * 50 * 32;   // 4号电机的角度换算 10
float angle_to_pulse_5 = 30 * 50 * 32;   // 5号电机的角度换算6
float angle_to_pulse_6 = 100 * 50 * 32;  // 6号电机的角度换算20

// 全局变量定义 - 方向
u8 direction_1 = 1;// 1号电机的方向
u8 direction_4 = 1; // 4号电机的方向
u8 direction_5 = 0; // 5号电机的方向
u8 direction_6 = 0; // 6号电机的方向

// 全局变量定义 - 速度（每个电机独立配置）
u16 speed_1 = 2000;  // 1号电机的速度
u16 speed_4 = 1500;  // 4号电机的速度
u16 speed_5 = 600;  // 5号电机的速度
u16 speed_6 = 2000;  // 6号电机的速度

// 全局变量定义 - 加速度（每个电机独立配置）
u8 acceleration_1 = 200;   // 1号电机的加速度
u8 acceleration_4 = 100;   // 4号电机的加速度
u8 acceleration_5 = 100;    // 5号电机的加速度
u8 acceleration_6 = 200;   // 6号电机的加速度

// // 全局变量定义 - 速度（每个电机独立配置）
// u16 speed_1 = 220;  // 1号电机的速度
// u16 speed_4 = 800;  // 4号电机的速度
// u16 speed_5 = 100;  // 5号电机的速度
// u16 speed_6 = 400;  // 6号电机的速度

// // 全局变量定义 - 加速度（每个电机独立配置）
// u8 acceleration_1 = 0;   // 1号电机的加速度
// u8 acceleration_4 = 0;   // 4号电机的加速度
// u8 acceleration_5 = 80;    // 5号电机的加速度
// u8 acceleration_6 = 220;   // 6号电机的加速度

//////////////////////////////////////////////////////////
// // 全局变量定义 - 速度（每个电机独立配置）
// u16 speed_1 = 200;  // 1号电机的速度
// u16 speed_4 = 1500;  // 4号电机的速度
// u16 speed_5 = 50;  // 5号电机的速度
// u16 speed_6 = 100;  // 6号电机的速度

// // 全局变量定义 - 加速度（每个电机独立配置）
// u8 acceleration_1 = 0;   // 1号电机的加速度
// u8 acceleration_4 = 0;   // 4号电机的加速度
// u8 acceleration_5 = 0;    // 5号电机的加速度
// u8 acceleration_6 = 0;   // 6号电机的加速度

// // 全局变量定义 - 速度（每个电机独立配置）
// u16 speed_1 = 2000;  // 1号电机的速度
// u16 speed_4 = 1500;  // 4号电机的速度
// u16 speed_5 = 1000;  // 5号电机的速度
// u16 speed_6 = 2000;  // 6号电机的速度

// // 全局变量定义 - 加速度（每个电机独立配置）
// u8 acceleration_1 = 200;   // 1号电机的加速度
// u8 acceleration_4 = 100;   // 4号电机的加速度
// u8 acceleration_5 = 100;    // 5号电机的加速度
// u8 acceleration_6 = 200;   // 6号电机的加速度
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

//更改库
/****************************************
加速度转化
acceleration=256-20000/a

*****************************************/
unsigned char get_acceleration(float a)
{
    unsigned char acceleration=(unsigned char) (256-20000.0f/a);
    if(acceleration<220)
        return acceleration;
    else
        return 220;
}

/****************************************
angle：运动角度
id:电机编号
speed：速度
acceleration：加速度
*****************************************/
void Control_Motor_new(float angle, u8 id,unsigned int speed, unsigned char acceleration)
{
     u32 pulse_count;
    u8 direction;
    u32 address;
    float angle_ori = angle;
    
    if(angle < 0)
        angle = -angle;

    // 根据电机ID设置对应的参数
    switch (id) {
        case 1:
            pulse_count = (u32)(angle * angle_to_pulse_1 / 90.0);
            direction = direction_1;
            address = 0x100;
            break;
        case 4://摇摆电机
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
            if (angle>150)angle=150;
            if(angle<-30)angle=-30;
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

    // 调用 CAN_Send_Position_Mode 发送控制命令，使用各电机独立的速度和加速度
    CAN_Send_Position_Mode(address, direction, speed, acceleration, pulse_count, 1, 0);
}
// 控制电机的通用函数

void Control_Motor(float angle, u8 id) 
{
    u32 pulse_count;
    u8 direction;
    u32 address;
    u16 speed;
    u8 acceleration;
    float angle_ori = angle;
    
    if(angle < 0)
        angle = -angle;

    // 根据电机ID设置对应的参数
    switch (id) {
        case 1:
            if (angle>100)angle=100;
            if(angle<-100)angle=-100;
            pulse_count = (u32)(angle * angle_to_pulse_1 / 90.0);
            direction = direction_1;
            address = 0x100;
            speed = speed_1;
            acceleration = acceleration_1;
            break;
        case 4:
            pulse_count = (u32)(angle * angle_to_pulse_4 / 90.0);
            direction = direction_4;
            address = 0x400;
            speed = speed_4;
            acceleration = acceleration_4;
            break;
        case 5:
            if (angle>120)angle=120;
            if(angle<-100)angle=-100;
            pulse_count = (u32)(angle * angle_to_pulse_5 / 90.0);
            direction = direction_5;
            address = 0x500;
            speed = speed_5;
            acceleration = acceleration_5;
            break;
        case 6:
            if (angle>120)angle=120;
            if(angle<-100)angle=-100;
            pulse_count = (u32)(angle * angle_to_pulse_6 / 90.0);
            direction = direction_6;
            address = 0x600;    
            speed = speed_6;
            acceleration = acceleration_6;
            break;
        default:
            return; // 无效ID，直接返回
    }
    
    // 如果角度为负值，取反方向
    if (angle_ori < 0) {
        direction = !direction;
    }

    // 调用 CAN_Send_Position_Mode 发送控制命令，使用各电机独立的速度和加速度
    CAN_Send_Position_Mode(address, direction, speed, acceleration, pulse_count, 1, 0);
}
/***************************************
// 全局变量定义 - 角度转脉冲系数90度对应的脉冲
float angle_to_pulse_1 = 245 * 50 * 32;  // 1号电机
float angle_to_pulse_4 = 50 * 50 * 32;   // 4号电机的角度换算
float angle_to_pulse_5 = 30 * 50 * 32;   // 5号电机的角度换算
float angle_to_pulse_6 = 100 * 50 * 32;  // 6号电机的角度换算
**************************************/

// 控制电机的通用函数，允许外部指定速度和加速度
void Control_Motor1(float angle, u8 id, u16 speed, u8 acceleration) 
{
    u32 pulse_count;
    u8 direction;
    u32 address;
    float angle_ori = angle;
    
    if(angle < 0)
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

    // 调用 CAN_Send_Position_Mode 发送控制命令，使用传入的速度和加速度
    CAN_Send_Position_Mode(address, direction, speed, acceleration, pulse_count, 1, 0);
}

