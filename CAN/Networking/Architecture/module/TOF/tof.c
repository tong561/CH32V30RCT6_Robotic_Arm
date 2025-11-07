#include "tof.h"
#include "uart.h"
#include "string.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * 计算CRC-16/MODBUS校验值
 * @param data 输入的数据缓冲区
 * @param length 数据长度（字节数）
 * @return 计算得到的16位CRC校验值
 */
/**
 * 计算CRC-16/MODBUS校验值（使用反向多项式0xA001）
 * @param data 输入的数据缓冲区
 * @param length 数据长度（字节数）
 * @return 计算得到的16位CRC校验值
 */
uint16_t crc16_modbus(uint8_t *data, uint16_t length) {
    uint16_t crc = 0xFFFF;          // 初始值保持0xFFFF
    const uint16_t polynomial = 0xA001;  // 使用反向多项式0xA001（0x8005的按位反转）
    
    if (data == NULL || length == 0) {
        return 0;  // 无效输入处理
    }
    
    for (uint16_t i = 0; i < length; i++) {
        crc ^= data[i];  // 与当前字节异或
        
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x0001) {  // 检查最低位
                crc >>= 1;       // 右移一位
                crc ^= polynomial;  // 与多项式异或
            } else {
                crc >>= 1;       // 单纯右移
            }
        }
    }
    
    // 注意：使用0xA001多项式时不需要取反操作
    return (crc << 8) | (crc >> 8);
}

/**
 * 验证包含CRC校验值的数据是否正确
 * @param data_with_crc 包含2字节CRC校验值的完整数据
 * @param total_length 完整数据的总长度（包括2字节CRC）
 * @return 验证通过返回true，否则返回false
 */
bool verify_crc16_modbus(unsigned char *data_with_crc, uint32_t total_length) {
    // 数据长度至少需要2字节（CRC部分）
    if (data_with_crc == NULL || total_length < 2) {
        return false;
    }
    
    // 原始数据长度 = 总长度 - 2字节CRC
    uint32_t data_length = total_length - 2;
    
    // 计算原始数据的CRC
    uint16_t calculated_crc = crc16_modbus(data_with_crc, data_length);
    
    // 从数据末尾提取接收到的CRC（大端模式）
    uint16_t received_crc = (uint16_t)(data_with_crc[data_length] << 8 | 
                                       data_with_crc[data_length + 1]);
    
    // 比较计算得到的CRC和接收到的CRC
    return calculated_crc == received_crc;
}
// 全局数组用于存储有效距离数据
u16 distance_buffer[5] = {0};  // 存储5次有效距离
u8 buffer_count = 0;           // 当前存储的数据个数
int filter_result;
/**
 * 从数据中提取距离值
 * @param buffer 数据缓冲区
 * @param length 数据长度
 * @return 提取到的距离值，失败返回0
 */
u16 extract_distance(char* buffer, int length)
{
    // 确保字符串结尾
    buffer[length] = '\0';
    
    // 查找目标字符串
    char* found = strstr(buffer, "ID:61 1 55 4c,dTOF(mm):");
    if (found == NULL) {
        return 0;  // 没找到目标字符串
    }
    
    // 跳过固定部分，指向数字
    found += strlen("ID:61 1 55 4c,dTOF(mm):");

    // 提取数字
    //u16 distance = (u16)atoi(found);
    // u16 distance = 0;
    // sscanf(found, "%d", &distance);
    u16 distance = 0;
    while(*found >= '0' && *found <= '9')
    {
        distance = distance * 10 + (*found - '0');
        found++;
    }
    
    return distance;
}

/**
 * 距离数据滤波函数
 * @param distances 距离数据数组
 * @param length 数组长度
 * @return 平均值(正数) / -1(数据变化过大)
 */
int filter_distance_data(u16* distances, int length)
{
    if (distances == NULL || length <= 0) {
        return -1;  // 无效输入
    }
    
    if (length == 1) {
        return distances[0];  // 单个数据直接返回
    }
    
    // 找出最大值和最小值
    u16 min_val = distances[0];
    u16 max_val = distances[0];
    u32 sum = distances[0];
    
    for (int i = 1; i < length; i++) {
        if (distances[i] < min_val) {
            min_val = distances[i];
        }
        if (distances[i] > max_val) {
            max_val = distances[i];
        }
        sum += distances[i];
    }
    
    // 如果最大值和最小值的差超过10，检查是否是丢位数的情况
    if ((max_val - min_val) > 10) {
        // 检查最小值是否可能是其他值丢失一位数字的情况
        u16 expected_min_range_start = min_val * 10;
        u16 expected_min_range_end = min_val * 10 + 9;
        
        // 统计有多少个值在期望范围内
        int matches = 0;
        u32 valid_sum = 0;
        int valid_count = 0;
        
        for (int i = 0; i < length; i++) {
            if (distances[i] >= expected_min_range_start && 
                distances[i] <= expected_min_range_end) {
                matches++;
            }
            
            // 收集非最小值的数据用于计算平均值
            if (distances[i] != min_val) {
                valid_sum += distances[i];
                valid_count++;
            }
        }
        
        // 如果大部分值都在期望范围内，说明最小值确实是丢了一位数
        // 这里使用一个阈值：至少要有length-1个值匹配（除了最小值本身）
        if (matches >= length - 1 && valid_count > 0) {
            // 返回除最小值外其他值的平均数
            return valid_sum / valid_count;
        }
        
        // 如果不是丢位数的情况，返回-1
        return -1;
    }
    
    // 否则返回所有值的平均值
    return sum / length;
}

void tof_run(void)
{
        if(frame_ready == 1)
        {
        // 提取距离值
            u16 distance = extract_distance(rx_buffer_uart2, rx_len_uart2);
    
            if(distance > 0)
            {
                printf("距离: %d\r\n", distance);
                
                // 将有效数据存储到缓冲区
                distance_buffer[buffer_count] = distance;
                buffer_count++;
            
            // 检查是否已经存够5次数据
                if(buffer_count >= 3)
                {
                    // 调用滤波函数
                    filter_result = filter_distance_data(distance_buffer, 3);
                    
                    // 打印滤波结果
                    if(filter_result == -1)
                    {
                        //printf("滤波结果: 数据变化过大，丢弃 %d,%d,%d\r\n",distance_buffer[0],distance_buffer[1],distance_buffer[2]);
                    }
                    else
                    {
                        printf("%d\r\n", filter_result);
                    }
                    
                    // 重置缓冲区计数
                    buffer_count = 0;
                    memset(distance_buffer, 0, sizeof(distance_buffer));
                }
            }
            else
            {
                //printf("无效数据\n");
            }
            
            // 清空缓冲区，准备接收下一帧
            rx_len_uart2 = 0;
            frame_ready = 0; 
            memset(rx_buffer_uart2, 0, sizeof(rx_buffer_uart2));
        }

}
// float new_tof()
// {
//     u16 tof_num[3]={0};
//     unsigned char jisu=0;
//       if(frame_ready == 1)
//         {
//             for (int i = 0; i < 15; i++) // 最多检查到 data[17]
//                 {
//                     if (rx_buffer_uart2[i] == 0x01 && rx_buffer_uart2[i + 1] == 0x03 && rx_buffer_uart2[i + 2] == 0x02)
//                     {
//                         if (verify_crc16_modbus(rx_buffer_uart2+i,7))
//                         tof_num[jisu]=(unsigned short)(rx_buffer_uart2[i + 3]<<8)+rx_buffer_uart2[i + 4];
//                         jisu++;
//                     }
//                 }
        
//         // 清空缓冲区，准备接收下一帧
//         rx_len_uart2 = 0;
//         frame_ready = 0; 
//         memset(rx_buffer_uart2, 0, sizeof(rx_buffer_uart2));
//         }
//     return (float)tof_num[0]/10;
// }
// TOF传感器组合滤波实现
// 采用"限幅滤波+滑动平均滤波"组合方式

// 滤波参数配置
#define FILTER_WINDOW 4         // 滑动平均窗口大小（3-10之间调整）
#define MAX_ALLOWED_CHANGE 10.0f // 最大允许变化量(cm)，根据传感器特性调整

// 静态变量 - 保存滤波状态
static float tof_buffer[FILTER_WINDOW] = {0}; // 滑动窗口缓冲区
static uint8_t buffer_index = 0;              // 缓冲区索引
static float last_valid_value = 0.0f;         // 上一次有效测量值
static uint8_t init_count = 0;                // 初始化计数

// 限幅滤波：过滤突变值
static float limit_filter(float new_value) {
    // 初始化阶段直接接受值
    if (init_count < FILTER_WINDOW) {
        last_valid_value = new_value;
        init_count++;
        return new_value;
    }
    
    // 计算与上一次有效值的差值
    float diff = new_value - last_valid_value;
    
    // 限制变化幅度
    if (diff > MAX_ALLOWED_CHANGE) {
        return last_valid_value + MAX_ALLOWED_CHANGE;
    } else if (diff < -MAX_ALLOWED_CHANGE) {
        return last_valid_value - MAX_ALLOWED_CHANGE;
    } else {
        return new_value; // 变化在允许范围内，直接使用新值
    }
}

// 滑动平均滤波：平滑数据
static float moving_average_filter(float value) {
    // 存储新值到缓冲区
    tof_buffer[buffer_index] = value;
    buffer_index = (buffer_index + 1) % FILTER_WINDOW;
    
    // 计算平均值
    float sum = 0.0f;
    for (uint8_t i = 0; i < FILTER_WINDOW; i++) {
        sum += tof_buffer[i];
    }
    
    return sum / FILTER_WINDOW;
}

// 组合滤波：先限幅再滑动平均
float filtered_tof() {
    // 获取原始TOF数据
    float raw_value = new_tof();
    
    // 异常值处理（如果传感器返回0或异常大值）
    if (raw_value <= 0 || raw_value > 50) { // 假设最大测量距离为1000cm
        return last_valid_value; // 返回上一次有效值
    }
    
    // 第一步：限幅滤波
    float limited_value = limit_filter(raw_value);
    
    // 第二步：滑动平均滤波
    float filtered_value = moving_average_filter(limited_value);
    
    // 更新上一次有效值
    last_valid_value = filtered_value;
    
    return filtered_value;
}

// 原始TOF数据读取函数（保持不变）
float new_tof()
{
    u16 tof_num[3]={0};
    unsigned char jisu=0;
    if(frame_ready == 1)
    {
        for (int i = 0; i < 15; i++) // 最多检查到 data[17]
        {
            if (rx_buffer_uart2[i] == 0x01 && rx_buffer_uart2[i + 1] == 0x03 && rx_buffer_uart2[i + 2] == 0x02)
            {
                if (verify_crc16_modbus(rx_buffer_uart2+i,7))
                    tof_num[jisu]=(unsigned short)(rx_buffer_uart2[i + 3]<<8)+rx_buffer_uart2[i + 4];
                jisu++;
            }
        }
    
        // 清空缓冲区，准备接收下一帧
        rx_len_uart2 = 0;
        frame_ready = 0; 
        memset(rx_buffer_uart2, 0, sizeof(rx_buffer_uart2));
    }
    return (float)tof_num[0]/10;
}
    