
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
	USART3_CFG();
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
        //memcpy(g_rx_data_buffer, USART_DMA_CTRL.Rx_Buffer[oldbuffer], rxlen);
        
        //g_rx_data_ready = 1;  // 置位数据就绪标志

        if (ring_buffer.RemainCount > 0)
        {
            g_rx_data_len = ring_buffer.RemainCount;
            //printf("recv %d >>>\n", g_rx_data_len);
            static u8 count;
            count = 0;
            while (ring_buffer.RemainCount > 0)
            {
                g_rx_data_buffer[count] = ring_buffer_pop();
                //printf("%02x ", g_rx_data_buffer[count]);
                count++;
            }
            //printf("\n<<<\n");
                // 解析数据
            uint8_t result = parse_wt_protocol(g_rx_data_buffer, g_rx_data_len);
            
            if(result == 0)
            {
                // 解析成功，可以使用全局变量
                printf("%.2f,%.2f,%.2f\r\n", 
                    Roll_angle, Pitch_angle, Yaw_angle);
            }
            else
            {
                printf("解析错误，错误码: %d\n", result);
            }
        }
    
        // 主循环中简单检查和处理
        // if (g_rx_data_ready) 
        // {
        //     printf("收到数据包，长度: %d\n", g_rx_data_len);
            
        //     // 直接处理整个数据包
        //     for (int i = 0; i < g_rx_data_len; i++) 
        //     {
        //         printf("%c", g_rx_data_buffer[i]);
        //     }
            
        //     g_rx_data_ready = 0;  // 处理完成，清除标志
        // }
    }

	
 }
 
 
