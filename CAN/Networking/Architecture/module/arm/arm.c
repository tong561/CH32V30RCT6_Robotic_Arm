#include "arm.h"
#include "math.h"
#include "fast_sin.h"

// 关节角度变量
float J1 = 0, J2 = 0, J3 = 0, J4 = 0;  // 4个关节角度

// 5自由度机械臂参数
float A1 = 16.0;  // 基座高度
float A2 = 24.0;  // 第二关节连杆长度
float A3 = 35;//40.0;  // 第三关节连杆长度  
float A4 = 12.5;  // 第四关节连杆长度 (原来的A4增加到14.5)
float A5 = 9.5;   // 第五关节连杆长度 (末端执行器长度)
float P = 10.0;   // 底部盲区半径

// 搜索参数
float tolerance = 1.0;     // 位置容差
float bu_chang = 0.1;      // 搜索步长

volatile float X_IN=0,Y_IN=0,Z_IN=0;
// 关节角度变量 - 增加J5
float J1_global = 0, J2_global = 0, J3_global = 0, J4_global = 0, J5_global = 0; 
void robot_arm_5dof_method2(float X, float Y, float Z);

void robot_arm_5dof_method2(float X, float Y, float Z) 
{
    // 静态变量记录上一次的关节角度，用于计算运动幅度
    static float prev_J1 = 0, prev_J2 = 0, prev_J3 = 0, prev_J4 = 0, prev_J5 = 0;
    static unsigned char first_run = 1;  // 首次运行标志
    
    X_IN = X;
    Y_IN = Y;
    Z_IN = Z;
    
    // Variable definitions
    float target_height, horizontal_distance;
    float c2, c23, c234, s2, s23, s234;
    float cur_height, cur_horizontal;
    
    // 最优解变量
    float best_J1=0, best_J2=0, best_J3=0, best_J4=0, best_J5=0;
    float best_motion = INFINITY;  // 改为最小运动幅度
    int solution_count = 0;
    
    // Calculate base rotation angle J1
    float J1 = atan2f(Y+P, X);
    
    // 限制J1在±π/2范围内
    if (J1 > M_PI) {
        J1 = M_PI;
    } else if (J1 < 0) {
        J1 = 0;
    }
    
    // Calculate target position parameters
    target_height = Z + A5;
    horizontal_distance = sqrtf((X + P) * (X + P) + Y * Y);
    
    printf("开始搜索所有可能的解 (最小运动幅度)...\r\n");
    printf("目标位置: X=%.1f, Y=%.1f, Z=%.1f\r\n", X, Y, Z);
    
    // 遍历前4个关节的角度组合
    for (float J2 = -M_PI/2; J2 < M_PI/2; J2 += bu_chang)
    {
        for (float J3 = 0; J3 < M_PI; J3 += bu_chang)
        {
            for (float J4 = 0; J4 < 115.0f * M_PI / 180.0f; J4 += bu_chang)
            {
                // 计算三角函数值
                fast_sin_cos(J2, &s2, &c2);
                fast_sin_cos(J2 + J3, &s23, &c23);
                fast_sin_cos(J2 + J3 + J4, &s234, &c234);
                
                // 计算第5关节角度，使末端垂直向下
                float J5 = M_PI - (J2 + J3 + J4);
                
                // 限制J5角度范围
                const float J5_MAX = 3.0f * M_PI / 4.0f;
                
                if (J5 < 0) {
                    J5 = 0;
                } else if (J5 > J5_MAX) {
                    J5 = J5_MAX;
                }
                
                // 计算当前位置
                cur_height = A1 + A2 * c2 + A3 * c23 + A4 * c234 - A5;
                cur_horizontal = A2 * s2 + A3 * s23 + A4 * s234;
                
                // 计算位置误差
                float height_error = fabsf(cur_height - target_height);
                float horizontal_error = fabsf(horizontal_distance - cur_horizontal);
                
                // 如果在容忍范围内
                if (height_error < tolerance && horizontal_error < tolerance)
                {
                    solution_count++;
                    
                    // 计算运动幅度（各关节角度变化的绝对值之和）
                    float motion_amplitude;
                    if (first_run) {
                        // 首次运行，以零位为参考
                        motion_amplitude = fabsf(J1) + fabsf(J2) + fabsf(J3) + 
                                         fabsf(J4) + fabsf(J5);
                    } else {
                        // 后续运行，以上一次角度为参考
                        motion_amplitude = fabsf(J1 - prev_J1) + 
                                         fabsf(J2 - prev_J2) + 
                                         fabsf(J3 - prev_J3) + 
                                         fabsf(J4 - prev_J4) + 
                                         fabsf(J5 - prev_J5);
                    }
                    
                    // 选择运动幅度最小的解
                    if (motion_amplitude < best_motion)
                    {
                        best_motion = motion_amplitude;
                        best_J1 = J1;
                        best_J2 = J2;
                        best_J3 = J3;
                        best_J4 = J4;
                        best_J5 = J5;
                    }
                }
            }
        }
    }
    
    // 打印结果
    printf("搜索完成！\r\n");
    if (solution_count > 0) {
        printf("总共找到 %d 个解决方案\r\n", solution_count);
        printf("最优解运动幅度: %.3f°\r\n", best_motion * 180.0f / M_PI);

        // 更新全局变量
        J1_global = best_J1 * 180.0f / M_PI;
        J2_global = best_J2 * 180.0f / M_PI;
        J3_global = best_J3 * 180.0f / M_PI;
        J4_global = best_J4 * 180.0f / M_PI;
        J5_global = best_J5 * 180.0f / M_PI - 15;
        
        // 保存当前角度供下次计算使用
        prev_J1 = best_J1;
        prev_J2 = best_J2;
        prev_J3 = best_J3;
        prev_J4 = best_J4;
        prev_J5 = best_J5;
        first_run = 0;
        
        printf("最优解角度 (度): J1=%.1f, J2=%.1f, J3=%.1f, J4=%.1f, J5=%.1f\r\n", 
               J1_global, J2_global, J3_global, J4_global, J5_global);
        
    } else {
        printf("未找到满足条件的解\r\n");
    }
}
