#include "tof.h"
#include "uart.h"
#include "string.h"

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