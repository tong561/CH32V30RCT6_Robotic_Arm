
#include "debug.h"
#include "table.h"
#include "can.h"
#include "arm.h"
#include "arm_instance.h"
#include "uart_dma.h"
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

	USARTx_CFG(115200);
	DMA_INIT();
	CAN_Mode_Init( CAN_SJW_1tq, CAN_BS2_5tq, CAN_BS1_6tq, 16, CAN_Mode_Normal );/* Bps = 250Kbps */
	
	GPIO_SW_INIT();
	SW_Release();
	// Delay_Ms(3000);
	// SW_Catch();
	//SW_Catch();
	CAN_MOTOR_MODE_SET();
	//robot_arm_5dof_method2(0,30,50);
	//SW_Release();

	test_angle = 60;


	// GPIO_WriteBit(GPIOA, GPIO_Pin_11, Bit_RESET);
	// GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_RESET);




	while(1)
	{		
        if (ring_buffer.RemainCount > 0)
        {
            printf("recv %d >>>\n", ring_buffer.RemainCount);
            while (ring_buffer.RemainCount > 0)
            {
                printf("%c", ring_buffer_pop());
            }
            printf("\n<<<\n");
        }


		CAN_BLDC_POS_CONTROL(50*8.0f,2);
		CAN_BLDC_POS_CONTROL(test_angle*8.0f,3);
		Delay_Ms(100);
	}
 }
 
 
