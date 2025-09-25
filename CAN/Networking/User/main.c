
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
#include "tof.h"

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
    rb_test05();
    //rb_test04();
	// 锁住两个无刷电机
      //CAN_MOTOR_MODE_SET();
        //CAN_BLDC_POS_CONTROL(-97-98,2);
      //CAN_BLDC_POS_CONTROL(-100,3);
    // Control_Motor(0,4);
    //Control_Motor(45,5);//需要先校准方向，跑初始姿态的时候对一下细分和减速比
    // Control_Motor(0,6);//此处注意，模型是缺了一个关节的，所以45->56
    // Control_Motor(0,1);
    while(1)
	{
        
    }
 }
 
 
