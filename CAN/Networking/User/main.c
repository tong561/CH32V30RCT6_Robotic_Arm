
#include "debug.h"
#include "can.h"
#include "arm.h"
#include "uart_dma.h"
#include "uart.h"
#include "sw.h"
#include "bldc.h"
#include "step.h"
#include "lcd.h"
#include "rb.h"
#include "tim.h"
#include "exti.h"
#include "wt61.h"
int filter_distance_data(u16* distances, int length);
// 全局数组用于存储有效距离数据
u16 distance_buffer[5] = {0};  // 存储5次有效距离
u8 buffer_count = 0;           // 当前存储的数据个数
/*********************************************************************
  * @fn      main
  *
  * @brief   Main program.
  *
  * @return  none
  */
 int main(void)
 {
	SystemCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(115200);	
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID()) ;

	//bsp初始化部分
	USARTx_CFG(115200);
	USART23_CFG();
	DMA_INIT();
    TIM1_INT_Init(240-1,0);
    EXTI0_INT_INIT();
    TIM_Cmd(TIM1, ENABLE);
	CAN_Mode_Init(CAN_SJW_1tq, CAN_BS2_5tq, CAN_BS1_6tq, 16, CAN_Mode_Normal);/* Bps = 250Kbps */
    //rb_test04();
	// 锁住两个无刷电机
      //CAN_MOTOR_MODE_SET();
      //CAN_BLDC_POS_CONTROL(-97,2);
      //CAN_BLDC_POS_CONTROL(-100,3);
    // Control_Motor(0,4);
    // Control_Motor(0,5);//需要先校准方向，跑初始姿态的时候对一下细分和减速比
    // Control_Motor(0,6);//此处注意，模型是缺了一个关节的，所以45->56
    // Control_Motor(0,1);
    while(1)
	{
        // if (ring_buffer.RemainCount > 0)
        // {
        //     g_rx_data_len = ring_buffer.RemainCount;
        //     //printf("recv %d >>>\n", g_rx_data_len);
        //     static u8 count;
        //     count = 0;
        //     while (ring_buffer.RemainCount > 0)
        //     {
        //         g_rx_data_buffer[count] = ring_buffer_pop();
        //         //printf("%02x ", g_rx_data_buffer[count]);
        //         count++;
        //     }
        //     //printf("\n<<<\n");
        //         // 解析数据
        //     uint8_t result = parse_wt_protocol(g_rx_data_buffer, g_rx_data_len);
            
        //     if(result == 0)
        //     {
        //         // 解析成功，可以使用全局变量
        //         printf("%.2f,%.2f,%.2f\r\n", 
        //             Roll_angle, Pitch_angle, Yaw_angle);
        //     }
        //     else
        //     {
        //         printf("解析错误，错误码: %d\n", result);
        //     }
        //}
            // if(frame_ready == 1)
            // {
            //     // 提取距离值
            //     u16 distance = extract_distance(rx_buffer_uart2, rx_len_uart2);
                
            //     if(distance > 0)
            //     {
            //         printf("距离: %d\r\n", distance);
            //         // 在这里可以进行你需要的其他处理
            //     }
            //     else
            //     {
            //         printf("无效数据\n");
            //     }
                
            //     // 清空缓冲区，准备接收下一帧
            //     rx_len_uart2 = 0;
            //     frame_ready = 0;
            //     memset(rx_buffer_uart2, 0, sizeof(rx_buffer_uart2));
            // }
        // 修改后的数据处理代码
    if(frame_ready == 1)
    {
    // 提取距离值
    u16 distance = extract_distance(rx_buffer_uart2, rx_len_uart2);
    
    if(distance > 0)
    {
        //printf("距离: %d\r\n", distance);
        
        // 将有效数据存储到缓冲区
        distance_buffer[buffer_count] = distance;
        buffer_count++;
        
        // 检查是否已经存够5次数据
        if(buffer_count >= 3)
        {
            // 调用滤波函数
            int filter_result = filter_distance_data(distance_buffer, 3);
            
            // 打印滤波结果
            if(filter_result == -1)
            {
                //printf("滤波结果: 数据变化过大，丢弃\r\n");
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
    
    // 如果最大值和最小值的差超过10，返回-1
    if ((max_val - min_val) > 10) {
        return -1;
    }
    
    // 否则返回平均值
    return sum / length;
}