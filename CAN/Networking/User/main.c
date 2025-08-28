
#include "debug.h"
#include "can.h"
#include "arm.h"
#include "uart_dma.h"
#include "uart.h"
#include "sw.h"
#include "bldc.h"
float test_angle;
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
	CAN_MOTOR_MODE_SET();
	test_angle = 10;
	//CAN_BLDC_POS_CONTROL(50*8.0f,2);
	//CAN_BLDC_POS_CONTROL(test_angle*8.0f,3);
	Delay_Ms(100);

	while(1)
	{		
		//CAN_MOTOR_MODE_SET();
		CAN_BLDC_POS_CONTROL(0*8.0f,2);
		CAN_BLDC_POS_CONTROL(0*8.0f,3);
		Delay_Ms(100);
	}
 }
 
 
