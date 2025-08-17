/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/03/05
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 *FPU hardware floating point operation routine:
 *This example demonstrates hardware floating-point arithmetic.
 *
 *Enable hardware floating point M-RS configuration reference This example configuration
 *Specific configuration-Properties -> C/C++ Build -> Setting -> Target Processor
 *-> The Floating point option is configured as Single precision extension (RVF)
 *Floating point ABI option configured as Single precision(f)
 *
 */

#include "debug.h"
#include "table.h"
// 关节角度变量
float J1 = 0, J2 = 0, J3 = 0, J4 = 0;  // 4个关节角度

// 搜索参数
float tolerance = 2.5;     // 位置容差
float bu_chang = 0.1;      // 搜索步长
// 机械臂参数 - 4自由度版本
float A1 = 16.0;  // 基座高度
float A2 = 24.0;  // 第二关节连杆长度
float A3 = 40.0;  // 第三关节连杆长度  
float A4 = 9.5;   // 末端执行器长度
float P = 10.0;    // 底部盲区半径
void robot_arm_4dof_method2(float X, float Y, float Z) ;
float val1=33.14;

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    int t,t1;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	USART_Printf_Init(115200);
	SystemCoreClockUpdate();
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

	val1 = (val1/2+11.12)*2;
    t=(int)(val1*10)%10;
    t1=(int)(val1*100)%10;

	printf("%d.%d%d\n", (int)val1, t, t1);

	robot_arm_4dof_method2(30,30,30);
	while(1);
}


void robot_arm_4dof_method2(float X, float Y, float Z) 
{
    // Variable definitions
    float target_height, horizontal_distance;
    float c2, c23, c234, s2, s23, s234;
    float cur_height, cur_horizontal;
    
    // 最优解变量
    float best_J1, best_J2, best_J3, best_J4;
    float best_error = INFINITY;
    int solution_count = 0;
    
    // Calculate base rotation angle J1
    float J1 = atan2f(P + Y, X);
    if (J1 < 0) {
        J1 += 2.0f * M_PI;  // Convert to [0, 2π] range
    }
    
    // Calculate target position parameters
    target_height = Z;
    horizontal_distance = sqrtf(X * X + (P + Y) * (P + Y));
    
    printf("开始搜索所有可能的解...\r\n");
    
    // 遍历所有可能的角度组合
    for (float J2 = 0; J2 < M_PI/2; J2 += bu_chang)
    {
        for (float J3 = 0; J3 < M_PI; J3 += bu_chang)
        {
            for (float J4 = 0; J4 < M_PI; J4 += bu_chang)
            {
                c2 = lookup_cos_any(J2);
                c23 = lookup_cos_any(J2 + J3);
                c234 = lookup_cos_any(J2 + J3 + J4);

                s2 = lookup_sin_any(J2);
                s23 = lookup_sin_any(J2 + J3);
                s234 = lookup_sin_any(J2 + J3 + J4);
                
                cur_height = A1 + A2 * c2 + A3 * c23 + A4 * c234;
                cur_horizontal = A2 * s2 + A3 * s23 + A4 * s234;
                
                // 计算误差
                float height_error = fabsf(cur_height - target_height);
                float horizontal_error = fabsf(horizontal_distance - cur_horizontal);
                float total_error = sqrtf(height_error * height_error + horizontal_error * horizontal_error);
                
                // 如果在容忍范围内，立即打印并检查是否为更优解
                if (height_error < tolerance && horizontal_error < tolerance)
                {
                    solution_count++;
                    // printf("解 %d: J1=%.3f, J2=%.3f, J3=%.3f, J4=%.3f (误差=%.6f)", 
                    //        solution_count, J1, J2, J3, J4, total_error);
                    
                    // // 检查是否为最优解
                    // if (total_error < best_error) {
                    //     best_J1 = J1;
                    //     best_J2 = J2;
                    //     best_J3 = J3;
                    //     best_J4 = J4;
                    //     best_error = total_error;
                    //     printf(" [新的最优解]");
                    // }
                    // printf("\r\n");
                }
            }
        }
    }
    
    // 在所有计算完成后打印总结
    printf("搜索完成！\r\n");
    if (solution_count > 0) {
        printf("总共找到 %d 个解决方案\r\n", solution_count);
        printf("执行最优解: J1=%.3f, J2=%.3f, J3=%.3f, J4=%.3f (误差=%.6f)\r\n", 
               best_J1, best_J2, best_J3, best_J4, best_error);
        
        // 设置全局变量
        J1 = best_J1;
        J2 = best_J2;
        J3 = best_J3;
        J4 = best_J4;
        
    } else {
        printf("未找到满足条件的解\r\n");
    }
}

