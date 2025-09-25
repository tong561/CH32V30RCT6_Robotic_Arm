#include "rb.h"
#include "lcd.h"
#include "bldc.h"
#include "step.h"
#include "arm.h"
#include "ir.h"
#include "tof.h"
#include "wt61.h"
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

//可以用UI做没有X轴的遥控任务
void rb_test02(void)
{
    // 初始XYZ坐标
    float X = 0, Y = 40, Z = 49;
    float prev_X = X, prev_Y = Y, prev_Z = Z; // 用于记录上一次的目标值

    // 锁住两个无刷电机
    CAN_MOTOR_MODE_SET();

    while (1)
    {
        // 调用LCD接收任务，获取按键值
        uint8_t cmd = LCD_ReceiveTask();

        // 根据接收到的按键值修改 X、Y、Z 的值
        switch (cmd)
        {
            case 0xF1: Z += 5; break; // 上按键，Z+5
            case 0xF2: Z -= 5; break; // 下按键，Z-5
            case 0xF3: X -= 5; break; // 左按键，X-5
            case 0xF4: X += 5; break; // 右按键，X+5
            case 0xF5: Y += 5; break; // 前按键，Y+5
            case 0xF6: Y -= 5; break; // 后按键，Y-5
            default: continue; // 无效按键，跳过本次循环
        }

        // 检查目标值是否发生变化
        if (X != prev_X || Y != prev_Y || Z != prev_Z)
        {
            // 更新记录的目标值
            prev_X = X;
            prev_Y = Y;
            prev_Z = Z;

            // 调用机械臂运动学计算函数
            robot_arm_5dof_method2(X, Y, Z);

            // 控制电机位置
            // Control_Motor(J1_global, 1);
            CAN_BLDC_POS_CONTROL(J2_global, 2);
            CAN_BLDC_POS_CONTROL(J3_global, 3);
            Control_Motor(J4_global, 5); // 校准方向，跑初始姿态时对细分和减速比进行调整
            Control_Motor(J5_global, 6); // 模型缺少一个关节，45->56

            // 更新LCD显示的通道数据
            n0 = (volatile uint32_t)X_IN;
            n1 = (volatile uint32_t)Y_IN;
            n2 = (volatile uint32_t)Z_IN;
            n3 = (volatile uint32_t)J1_global;
            n4 = (volatile uint32_t)J2_global;
            n5 = (volatile uint32_t)J3_global;
            n6 = (volatile uint32_t)J4_global;
            n7 = (volatile uint32_t)J5_global;

            // 调用LCD发送任务
            LCD_SendTask();
        }

        // 模拟100Hz刷新频率
        Delay_Ms(10);
    }
}

void rb_test03(void)
{
    // 初始XYZ坐标
    float X = 0, Y = 40, Z = 49;
    float prev_X = X, prev_Y = Y, prev_Z = Z; // 用于记录上一次的目标值
    float cebai_angle = 0;
    // 锁住两个无刷电机
    CAN_MOTOR_MODE_SET();
    CAN_BLDC_POS_CONTROL(0,2);
    CAN_BLDC_POS_CONTROL(0,3);
    Control_Motor(0,4);
    Control_Motor(0,5);//需要先校准方向，跑初始姿态的时候对一下细分和减速比
    Control_Motor(0,6);//此处注意，模型是缺了一个关节的，所以45->56
    Control_Motor(0,1);

    while (1)
    {
        // 调用LCD接收任务，获取按键值
        uint8_t cmd = LCD_ReceiveTask();

        // 根据接收到的按键值修改 X、Y、Z 的值
        switch (cmd)
        {
            case 0xF1: Z += 5; break; // 上按键，Z+5
            case 0xF2: Z -= 5; break; // 下按键，Z-5
            case 0xF3: X -= 5; break; // 左按键，X-5
            case 0xF4: X += 5; break; // 右按键，X+5
            case 0xF5: Y += 5; break; // 前按键，Y+5
            case 0xF6: Y -= 5; break; // 后按键，Y-5
            case 0xF7: cebai_angle-=10;Control_Motor(cebai_angle,4); break; // 后按键，Y-5
            case 0xF8: cebai_angle+=10;Control_Motor(cebai_angle,4); break; // 后按键，Y-5
            case 0xF9:  //归位
                        CAN_BLDC_POS_CONTROL(0,2);
                        CAN_BLDC_POS_CONTROL(0,3);
                        Control_Motor(0,4);
                        Control_Motor(0,5);//需要先校准方向，跑初始姿态的时候对一下细分和减速比
                        Control_Motor(0,6);//此处注意，模型是缺了一个关节的，所以45->56
                        Control_Motor(0,1);
                        break;
            default: break;; // 无效按键，跳过本次循环
        }

        // 检查目标值是否发生变化
        if (X != prev_X || Y != prev_Y || Z != prev_Z)
        {
            // 更新记录的目标值
            prev_X = X;
            prev_Y = Y;
            prev_Z = Z;

            // 调用机械臂运动学计算函数
            robot_arm_5dof_method2(X, Y, Z);

            // 控制电机位置
            Control_Motor(90-J1_global, 1);
            CAN_BLDC_POS_CONTROL(J2_global, 2);
            CAN_BLDC_POS_CONTROL(J3_global, 3);
            Control_Motor(J4_global, 5); // 校准方向，跑初始姿态时对细分和减速比进行调整
            Control_Motor(J5_global, 6); // 模型缺少一个关节，45->56
            
            if(J2_global<0)
                J2_global = -J2_global * 100;
            // 更新LCD显示的通道数据
            n0 = (volatile uint32_t)X_IN;
            n1 = (volatile uint32_t)Y_IN;
            n2 = (volatile uint32_t)Z_IN;
            n3 = (volatile uint32_t)J1_global;
            n4 = (volatile uint32_t)J2_global;
            n5 = (volatile uint32_t)J3_global;
            n6 = (volatile uint32_t)J4_global;
            n7 = (volatile uint32_t)J5_global;

            // 调用LCD发送任务
            LCD_SendTask();
        }

        // 模拟100Hz刷新频率
        Delay_Ms(10);
    }
}

void rb_test04(void)
{
    // 初始XYZ坐标
    float X = 0, Y = 40, Z = 49;
    float prev_X = X, prev_Y = Y, prev_Z = Z; // 用于记录上一次的目标值
    float cebai_angle = 0;
    // 锁住两个无刷电机
    CAN_MOTOR_MODE_SET();
    CAN_BLDC_POS_CONTROL(0,2);
    CAN_BLDC_POS_CONTROL(0,3);
    Control_Motor(0,4);
    Control_Motor(0,5);//需要先校准方向，跑初始姿态的时候对一下细分和减速比
    Control_Motor(0,6);//此处注意，模型是缺了一个关节的，所以45->56
    Control_Motor(0,1);

    while (1)
    {
        // 调用LCD接收任务，获取按键值
        uint8_t cmd = LCD_ReceiveTask();

        // 根据接收到的按键值修改 X、Y、Z 的值
        switch (cmd)
        {
            case 0xF1: Z += 5; break; // 上按键，Z+5
            case 0xF2: Z -= 5; break; // 下按键，Z-5
            case 0xF3: X -= 5; break; // 左按键，X-5
            case 0xF4: X += 5; break; // 右按键，X+5
            case 0xF5: Y += 5; break; // 前按键，Y+5
            case 0xF6: Y -= 5; break; // 后按键，Y-5
            case 0xF7: cebai_angle-=10;Control_Motor(cebai_angle,4); break; // 后按键，Y-5
            case 0xF8: cebai_angle+=10;Control_Motor(cebai_angle,4); break; // 后按键，Y-5
            case 0xF9:  //归位
                        CAN_BLDC_POS_CONTROL(0,2);
                        CAN_BLDC_POS_CONTROL(0,3);
                        Control_Motor(0,4);
                        Control_Motor(0,5);//需要先校准方向，跑初始姿态的时候对一下细分和减速比
                        Control_Motor(0,6);//此处注意，模型是缺了一个关节的，所以45->56
                        Control_Motor(0,1);
                        break;
            case 0xFA:  //归位
                        CAN_BLDC_POS_CONTROL(-98,2);
                        CAN_BLDC_POS_CONTROL(180,3);
                        Control_Motor(0,4);
                        Control_Motor(0,5);//需要先校准方向，跑初始姿态的时候对一下细分和减速比
                        Control_Motor(0,6);//此处注意，模型是缺了一个关节的，所以45->56
                        Control_Motor(0,1);
                        break;
            default: break;; // 无效按键，跳过本次循环
        }

        switch(ir_command)
        {
            case 0x1c:  Y += 5;break;
            case 0x55:  Y -= 5;break;
            case 0x59:  X -= 5;break;
            case 0x18:  X += 5;break;
            case 0x15:  break;
            case 0x47:  Z += 5;break;
            case 0x5d:  Z -= 5;break;
            case 0x45:  break;
            case 0x44:  cebai_angle-=10;Control_Motor(cebai_angle,4);break;
            case 0x5c:  cebai_angle+=10;Control_Motor(cebai_angle,4);break;
            default: break;; // 无效按键，跳过本次循环
        }
        ir_command = 0;

        // 检查目标值是否发生变化
        if (X != prev_X || Y != prev_Y || Z != prev_Z)
        {
            // 更新记录的目标值
            prev_X = X;
            prev_Y = Y;
            prev_Z = Z;

            // 调用机械臂运动学计算函数
            robot_arm_5dof_method2(X, Y, Z);

            // 控制电机位置
            Control_Motor(90-J1_global, 1);
            CAN_BLDC_POS_CONTROL(J2_global, 2);
            CAN_BLDC_POS_CONTROL(J3_global, 3);
            Control_Motor(J4_global, 5); // 校准方向，跑初始姿态时对细分和减速比进行调整
            Control_Motor(J5_global, 6); // 模型缺少一个关节，45->56
            
            if(J2_global<0)
                J2_global = -J2_global * 100;
            // 更新LCD显示的通道数据
            n0 = (volatile uint32_t)X_IN;
            n1 = (volatile uint32_t)Y_IN;
            n2 = (volatile uint32_t)Z_IN;
            n3 = (volatile uint32_t)J1_global;
            n4 = (volatile uint32_t)J2_global;
            n5 = (volatile uint32_t)J3_global;
            n6 = (volatile uint32_t)J4_global;
            n7 = (volatile uint32_t)J5_global;

            // 调用LCD发送任务
            LCD_SendTask();
        }

        // 模拟100Hz刷新频率
        Delay_Ms(10);
    }
}

void rb_test05(void)
{
    // 初始XYZ坐标
    float X = 0, Y = 40, Z = 49;
    float prev_X = X, prev_Y = Y, prev_Z = Z; // 用于记录上一次的目标值
    float cebai_angle = 0;
    // 锁住两个无刷电机
    CAN_MOTOR_MODE_SET();
    CAN_BLDC_POS_CONTROL(0,2);
    CAN_BLDC_POS_CONTROL(0,3);
    Control_Motor(0,4);
    Control_Motor(0,5);//需要先校准方向，跑初始姿态的时候对一下细分和减速比
    Control_Motor(0,6);//此处注意，模型是缺了一个关节的，所以45->56
    Control_Motor(0,1);

    while (1)
    {
        // 调用LCD接收任务，获取按键值
        uint8_t cmd = LCD_ReceiveTask();

        // 根据接收到的按键值修改 X、Y、Z 的值
        switch (cmd)
        {
            case 0xF1: Z += 5; break; // 上按键，Z+5
            case 0xF2: Z -= 5; break; // 下按键，Z-5
            case 0xF3: X -= 5; break; // 左按键，X-5
            case 0xF4: X += 5; break; // 右按键，X+5
            case 0xF5: Y += 5; break; // 前按键，Y+5
            case 0xF6: Y -= 5; break; // 后按键，Y-5
            case 0xF7: cebai_angle-=10;Control_Motor(cebai_angle,4); break; // 后按键，Y-5
            case 0xF8: cebai_angle+=10;Control_Motor(cebai_angle,4); break; // 后按键，Y-5
            case 0xF9:  //归位
                        CAN_BLDC_POS_CONTROL(0,2);
                        CAN_BLDC_POS_CONTROL(0,3);
                        Control_Motor(0,4);
                        Control_Motor(0,5);//需要先校准方向，跑初始姿态的时候对一下细分和减速比
                        Control_Motor(0,6);//此处注意，模型是缺了一个关节的，所以45->56
                        Control_Motor(0,1);
                        break;
            case 0xFA:  //归位
                        CAN_BLDC_POS_CONTROL(-98,2);
                        CAN_BLDC_POS_CONTROL(170,3);
                        Control_Motor(0,4);
                        Control_Motor(0,5);//需要先校准方向，跑初始姿态的时候对一下细分和减速比
                        Control_Motor(0,6);//此处注意，模型是缺了一个关节的，所以45->56
                        Control_Motor(0,1);
                        break;
            default: break;; // 无效按键，跳过本次循环
        }

        switch(ir_command)
        {
            case 0x1c:  Y += 5;break;
            case 0x55:  Y -= 5;break;
            case 0x59:  X -= 5;break;
            case 0x18:  X += 5;break;
            case 0x15:  break;
            case 0x47:  Z += 5;break;
            case 0x5d:  Z -= 5;break;
            case 0x45:  break;
            case 0x44:  cebai_angle-=10;Control_Motor(cebai_angle,4);break;
            case 0x5c:  cebai_angle+=10;Control_Motor(cebai_angle,4);break;
            default: break;; // 无效按键，跳过本次循环
        }
        ir_command = 0;
        tof_run(); 
        get_angle();
        // 检查目标值是否发生变化
        if (X != prev_X || Y != prev_Y || Z != prev_Z)
        {
            // 更新记录的目标值
            prev_X = X;
            prev_Y = Y;
            prev_Z = Z;

            // 调用机械臂运动学计算函数
            robot_arm_5dof_method2(X, Y, Z);

            // 控制电机位置
            Control_Motor(90-J1_global, 1);
            CAN_BLDC_POS_CONTROL(J2_global, 2);
            CAN_BLDC_POS_CONTROL(J3_global, 3);
            Control_Motor(J4_global, 5); // 校准方向，跑初始姿态时对细分和减速比进行调整
            Control_Motor(J5_global, 6); // 模型缺少一个关节，45->56
            
            if(J2_global<0)
                J2_global = -J2_global * 100;
            if(cebai_angle<0)
                J2_global = -J2_global * 100;
            // 更新LCD显示的通道数据
            n0 = (volatile uint32_t)X_IN;
            n1 = (volatile uint32_t)Y_IN;
            n2 = (volatile uint32_t)Z_IN;
            n3 = (volatile uint32_t)J1_global;
            n4 = (volatile uint32_t)J2_global;
            n5 = (volatile uint32_t)J3_global;
            n6 = (volatile uint32_t)J4_global;
            n7 = (volatile uint32_t)J5_global;
            n8 = (volatile uint32_t)cebai_angle;
            n9 = (volatile uint32_t)filter_result;
            n10 = (volatile uint32_t)Pitch_angle;

            // 调用LCD发送任务
            LCD_SendTask();
        }

        // 模拟100Hz刷新频率
        Delay_Ms(10);
    }
}