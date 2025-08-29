
#include "debug.h"
#include "can.h"
#include "arm.h"
#include "uart_dma.h"
#include "uart.h"
#include "sw.h"
#include "bldc.h"
#include "step.h"
#include "lcd.h"
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
	CAN_Mode_Init( CAN_SJW_1tq, CAN_BS2_5tq, CAN_BS1_6tq, 16, CAN_Mode_Normal );/* Bps = 250Kbps */
	GPIO_SW_INIT();
	SW_Release();
	robot_arm_5dof_method2(0,40,49);

	// 初始化LCD显示通道的测试数据
    n0 = 0;
    n1 = 0;
    n2 = 0;
    n3 = 0;
    n4 = 0;
    n5 = 0;
    n6 = 0;
    n7 = 10000;

	while(1)
	{		
        // n0 自增，模拟动态变化
        n0++;
		n7--;

        // 调用LCD发送任务
        LCD_SendTask();

        // 调用LCD接收任务，获取按键值
        uint8_t cmd = LCD_ReceiveTask();

        // 根据接收到的按键值修改 n1~n7 的值
        switch (cmd) 
        {
            case 0xF1: n1 += 10; break; // 上按键
            case 0xF2: n2 += 10; break; // 下按键
            case 0xF3: n3 += 10; break; // 左按键
            case 0xF4: n4 += 10; break; // 右按键
            case 0xF5: n5 += 10; break; // 前按键
            case 0xF6: n6 += 10; break; // 后按键
            default: break;
        }

        // 模拟100Hz刷新频率
        Delay_Ms(10);
    }
		
	
 }
 
 
