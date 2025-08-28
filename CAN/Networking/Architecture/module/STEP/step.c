#include "step.h"
#include "can.h"

#define MAX_FRAME_LEN 8

// 函数：位置模式控制命令发送
u8 CAN_Send_Position_Mode(u32 address, u8 direction, u16 speed, u8 acceleration, u32 pulse_count, u8 position_mode, u8 sync_mode) {
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
