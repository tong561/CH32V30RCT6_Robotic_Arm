#include "rb.h"
#include "lcd.h"
#include "bldc.h"
#include "step.h"
#include "arm.h"
/*
此处先做机械臂的电机标定和机械臂在理想坐标系下的一个最优初始状态
*/

void rb_test01(void)
{
    CAN_MOTOR_MODE_SET();//锁住两个无刷电机
    robot_arm_5dof_method2(0,40,49);//J1=90.0, J2=18.8, J3=11.4, J4=80.2, J5=69.4,调用变量时调用
    //Control_Motor(J1_global,1);
    CAN_BLDC_POS_CONTROL(J2_global,2);
    CAN_BLDC_POS_CONTROL(J3_global,3);
    Control_Motor(J4_global,5);//需要先校准方向，跑初始姿态的时候对一下细分和减速比
    Control_Motor(J5_global,6);//此处注意，模型是缺了一个关节的，所以45->56
    // 调用LCD发送任务
    n0 = (volatile uint32_t)X_IN;
    n1 = (volatile uint32_t)Y_IN;
    n2 = (volatile uint32_t)Z_IN;
    n3 = (volatile uint32_t)J1_global;
    n4 = (volatile uint32_t)J2_global;
    n5 = (volatile uint32_t)J3_global;
    n6 = (volatile uint32_t)J4_global;
    n7 = (volatile uint32_t)J5_global;
    
    while(1)
	{		
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