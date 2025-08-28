
#include "debug.h"
#include "can.h"
#include "arm.h"
#include "uart_dma.h"
#include "uart.h"
#include "sw.h"
#include "bldc.h"
#include "step.h"

float test_angle;
float step_angle = 45;
float angle90_to_pulse = 50*50*32;//减速比*90度对应的脉冲*细分
float step_pulse;
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

	//setup部分代码
	SW_Release();
	//CAN_MOTOR_MODE_SET();
	step_pulse = step_angle/90.0f*angle90_to_pulse;
	CAN_Send_Position_Mode(0x600,1,1000,200,(u32)step_pulse,1,0);
	while(1)
	{		
		
	}
 }
 
 
