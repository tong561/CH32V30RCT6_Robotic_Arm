
#include "debug.h"
#include "table.h"
#include "can.h"
#include "arm.h"
#include "arm_instance.h"
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

	GPIO_SW_INIT();
	CAN_Mode_Init( CAN_SJW_1tq, CAN_BS2_5tq, CAN_BS1_6tq, 16, CAN_Mode_Normal );/* Bps = 250Kbps */
	//robot_arm_5dof_method2(0,30,50);
	
	SW_Release();
	// GPIO_WriteBit(GPIOA, GPIO_Pin_11, Bit_RESET);
	// GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_RESET);

	//Delay_Ms(4000);
	//SW_Catch();


	while(1)
	{		
		CAN_MOTOR_MODE_SET();
		CAN_BLDC_POS_CONTROL(60*8.0f,2);
		CAN_BLDC_POS_CONTROL(90*8.0f,3);
		Delay_Ms(100);
	}
 }
 
 
