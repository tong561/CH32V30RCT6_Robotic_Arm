#include "arm.h"
#include "math.h"
#include "fast_sin.h"

// 关节角度变量
float J1 = 0, J2 = 0, J3 = 0, J4 = 0;  // 4个关节角度

// 5自由度机械臂参数
float A1 = 16.0;  // 基座高度
float A2 = 24.0;  // 第二关节连杆长度
float A3 = 40.0;  // 第三关节连杆长度  
float A4 = 14.5;  // 第四关节连杆长度 (原来的A4增加到14.5)
float A5 = 9.5;   // 第五关节连杆长度 (末端执行器长度)
float P = 10.0;   // 底部盲区半径

// 搜索参数
float tolerance = 1.0;     // 位置容差
float bu_chang = 0.1;      // 搜索步长

// 关节角度变量 - 增加J5
float J1_global = 0, J2_global = 0, J3_global = 0, J4_global = 0, J5_global = 0; 
void robot_arm_5dof_method2(float X, float Y, float Z);

void robot_arm_5dof_method2(float X, float Y, float Z) 
{
    // 输入验证：检查是否在盲区内
    if (X < 0) {
        printf("错误:目标位置在盲区内,X必须 >= 0\r\n");
        printf("盲区半径P = %.1fmm,机械臂无法到达X < 0的位置\r\n", P);
        return;
    }
    
    // Variable definitions
    float target_height, horizontal_distance;
    float c2, c23, c234, s2, s23, s234;
    float cur_height, cur_horizontal;
    
    // 最优解变量
    float best_J1=0, best_J2=0, best_J3=0, best_J4=0, best_J5=0;
    float best_error = INFINITY;
    int solution_count = 0;
    
    // Calculate base rotation angle J1
    float J1 = atan2f(Y+P, X);  // X+P因为坐标系原点在盲区边界，基座在-P位置
    
    // 限制J1在±π/2范围内
    if (J1 > M_PI) {
        J1 = M_PI;   // 限制最大角度 +90°
    } else if (J1 < 0) {
        J1 = 0;  // 限制最小角度 -90°
    }
    
    // Calculate target position parameters - 考虑末端执行器垂直向下的偏移
    target_height = Z + A5;  // 目标高度需要加上A5，因为末端执行器垂直向下
    horizontal_distance = sqrtf((X + P) * (X + P) + Y * Y);  // 计算到基座的实际距离
    
    printf("开始搜索所有可能的解 (5自由度-末端垂直)...\r\n");
    printf("目标位置: X=%.1f, Y=%.1f, Z=%.1f\r\n", X, Y, Z);
    printf("距基座距离: %.1fmm, J1角度: %.1f°\r\n", 
           horizontal_distance, J1 * 180.0f / M_PI);
    
    // 遍历前4个关节的角度组合，第5关节保持垂直向下
    for (float J2 = -M_PI/2; J2 < M_PI/2; J2 += bu_chang)
    {
        for (float J3 = 0; J3 < M_PI; J3 += bu_chang)
        {
            for (float J4 = 0; J4 < M_PI; J4 += bu_chang)
            {
                // 计算三角函数值 (只需要前4个关节)
                fast_sin_cos(J2, &s2, &c2);
                fast_sin_cos(J2 + J3, &s23, &c23);
                fast_sin_cos(J2 + J3 + J4, &s234, &c234);
                
                // 计算第5关节角度，使末端执行器垂直向下
                // 要使末端垂直向下，需要: J2 + J3 + J4 + J5 = π/2 (或其他使cos=0的角度)
                // 这里选择使末端严格垂直向下: J5 = π/2 - (J2 + J3 + J4)
                float J5 = M_PI - (J2 + J3 + J4);
                
                // 限制J5角度范围，保护机械结构 (±135° = ±3π/4)
                const float J5_MAX = 3.0f * M_PI / 4.0f;   // +135°
                //const float J5_MIN = -3.0f * M_PI / 4.0f;  // -135°
                
                if (J5 < 0) {
                    J5 = 0;  // 限制最小角度 -135°
                } else if (J5 > J5_MAX) {
                    J5 = J5_MAX;  // 限制最大角度 +135°
                }
                
                // 计算当前位置 - A5项垂直向下，只影响高度
                cur_height = A1 + A2 * c2 + A3 * c23 + A4 * c234 - A5;  // A5垂直向下，所以是减法
                cur_horizontal = A2 * s2 + A3 * s23 + A4 * s234;        // A5不影响水平距离
                
                // 计算误差
                float height_error = fabsf(cur_height - target_height);
                float horizontal_error = fabsf(horizontal_distance - cur_horizontal);
                
                // 计算总误差用于寻找最优解
                float total_error = height_error + horizontal_error;
                
                // 如果在容忍范围内
                if (height_error < tolerance && horizontal_error < tolerance)
                {
                    solution_count++;
                    
                    // 检查是否为更优解
                    if (total_error < best_error)
                    {
                        best_error = total_error;
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
    
    // 在所有计算完成后打印总结
    printf("搜索完成！\r\n");
    if (solution_count > 0) {
        printf("总共找到 %d 个解决方案\r\n", solution_count);
        printf("最优解误差: %.3f\r\n", best_error);
        
        // 设置全局变量
        J1_global = best_J1;
        J2_global = best_J2;
        J3_global = best_J3;
        J4_global = best_J4;
        J5_global = best_J5;
        
        // 打印最优解
        printf("最优解角度 (度): J1=%.1f, J2=%.1f, J3=%.1f, J4=%.1f, J5=%.1f\r\n", 
               best_J1 * 180.0f / M_PI,
               best_J2 * 180.0f / M_PI, 
               best_J3 * 180.0f / M_PI,
               best_J4 * 180.0f / M_PI,
               best_J5 * 180.0f / M_PI);
        
    } else {
        printf("未找到满足条件的解\r\n");
    }
}
