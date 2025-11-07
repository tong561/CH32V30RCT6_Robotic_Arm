// #include "rb.h"
// #include "lcd.h"
// #include "bldc.h"
// #include "step.h"
// #include "arm.h"
// #include "ir.h"
// #include "tof.h"
// #include "wt61.h"
// #include "tim.h"
// /*
// 此处先做机械臂的电机标定和机械臂在理想坐标系下的一个最优初始状态
// */



// void rb_test05(void)
// {
//     // 初始XYZ坐标
//     float X = 0, Y = 40, Z = 49;
//     float prev_X = X, prev_Y = Y, prev_Z = Z; // 用于记录上一次的目标值
//     float cebai_angle = 0;
//     unsigned char start_flag=0;//开始运动标志1开始
//     unsigned char tree_mode_flag=0;
//     //锁住两个无刷电机
//     //  CAN_MOTOR_MODE_SET();
//     //  CAN_BLDC_POS_CONTROL(0,2);
//     //  CAN_BLDC_POS_CONTROL(0,3);
//      Control_Motor(0,4);
//      Control_Motor(-40,5);//需要先校准方向，跑初始姿态的时候对一下细分和减速比
//     Control_Motor(-95,6);//此处注意，模型是缺了一个关节的，所以45->56
//     Control_Motor(0,1);

//     while (1)
//     {
//         // 调用LCD接收任务，获取按键值
//         uint8_t cmd = LCD_ReceiveTask();

//         // 根据接收到的按键值修改 X、Y、Z 的值
//         switch (cmd)
//         {
//             case 0xF1: Z += 5; break; // 上按键，Z+5//单位cm
//             case 0xF2: Z -= 5; break; // 下按键，Z-5
//             //
//             case 0xF3: X -= 5; break; // 左按键，X-5
//             case 0xF4: X += 5; break; // 右按键，X+5
//             //
//             case 0xF5: Y += 5; break; // 前按键，Y+5
//             case 0xF6: Y -= 5; break; // 后按键，Y-5
//             case 0xF7: cebai_angle-=10;Control_Motor(cebai_angle,4); break; // 摇摆按键，Y-5
//             case 0xF8: cebai_angle+=10;Control_Motor(cebai_angle,4); break; // 摇摆按键，Y-5
//             case 0xF9:  //归位
//                         CAN_BLDC_POS_CONTROL(0,2);
//                         CAN_BLDC_POS_CONTROL(0,3);
//                         Control_Motor(0,4);
//                         Control_Motor(0,5);//需要先校准方向，跑初始姿态的时候对一下细分和减速比
//                         Control_Motor(0,6);//此处注意，模型是缺了一个关节的，所以45->56
//                         Control_Motor(0,1);
//                         break;
//             case 0xFA:  //起始状态
//                         CAN_BLDC_POS_CONTROL(-98,2);
//                         CAN_BLDC_POS_CONTROL(170,3);
//                         Control_Motor(0,4);
//                         Control_Motor(0,5);//需要先校准方向，跑初始姿态的时候对一下细分和减速比
//                         Control_Motor(0,6);//此处注意，模型是缺了一个关节的，所以45->56
//                         Control_Motor(0,1);
//                         break;
//             case 0xFB: //启动前标记，启动后模式选择
//                         if (!start_flag) ;//design_five_point();
//                         else tree_mode_flag=(tree_mode_flag+1)%3;
//             case 0xFC: //启动暂停
//                         start_flag=!start_flag;
                        
//                         //design_five_point();  
                      
//             default: break;; // 无效按键，跳过本次循环
//         }

//         switch(ir_command)
//         {
//             case 0x1c:  Y += 5;break;
//             case 0x55:  Y -= 5;break;
//             case 0x59:  X -= 5;break;
//             case 0x18:  X += 5;break;
//             case 0x15:  break;
//             case 0x47:  Z += 5;break;
//             case 0x5d:  Z -= 5;break;
//             case 0x45:  break;
//             case 0x44:  cebai_angle-=10;Control_Motor(cebai_angle,4);break;
//             case 0x5c:  cebai_angle+=10;Control_Motor(cebai_angle,4);break;
//             default: break;; // 无效按键，跳过本次循环
//         }
//         ir_command = 0;
//         tof_run(); //获取距离
//         get_angle();//末端角度
//         // 检查目标值是否发生变化
//         if (X != prev_X || Y != prev_Y || Z != prev_Z)
//         {
//             // 更新记录的目标值
//             prev_X = X;
//             prev_Y = Y;
//             prev_Z = Z;

//             // 调用机械臂运动学计算函数
//             robot_arm_5dof_method2(X, Y, Z);

//             // 控制电机位置
//             Control_Motor(90-J1_global, 1);
//             CAN_BLDC_POS_CONTROL(J2_global, 2);
//             CAN_BLDC_POS_CONTROL(J3_global, 3);
//             Control_Motor(J4_global, 5); // 校准方向，跑初始姿态时对细分和减速比进行调整
//             Control_Motor(J5_global, 6); // 模型缺少一个关节，45->56
            
//             if(J2_global<0)
//                 J2_global = -J2_global * 100;
//             // if(cebai_angle<0)
//             //     J2_global = -J2_global * 100;
//             // 更新LCD显示的通道数据
//             n0 = (volatile uint32_t)X_IN;
//             n1 = (volatile uint32_t)Y_IN;
//             n2 = (volatile uint32_t)Z_IN;
//             n3 = (volatile uint32_t)J1_global;
//             n4 = (volatile uint32_t)J2_global;
//             n5 = (volatile uint32_t)J3_global;
//             n6 = (volatile uint32_t)J4_global;
//             n7 = (volatile uint32_t)J5_global;
//             n8 = 0;
//             n9 = (volatile uint32_t)filter_result;
//             n10 = (volatile uint32_t)Pitch_angle;

//             // 调用LCD发送任务
//             LCD_SendTask();
//         }

//         // 模拟100Hz刷新频率
//         Delay_Ms(10);
//     }
// }




// ///////////////////////////////////////////////////////////////////////////////////////////////
// // 轨迹规划相关定义
// #define MAX_POINTS 5

// // 简单直线模式参数
// #define LINE_STEPS 2               // 直线运动步数
// #define LINE_MOVE_AMPLITUDE 10.0f   // 直线运动幅度(cm)

// // 圆形模式参数
// #define CIRCLE_TRAJECTORY_POINTS 72  // 圆形模式：每段轨迹的插值点数
// #define CIRCLE_MOVE_AMPLITUDE 6.0f   // 圆形运动半径(cm)

// // 三个模式的插值参数（区分开来）
// #define INTERP_STEPS_MODE0 3  // 模式0(X方向)：每两个轨迹点之间插值步数
// #define INTERP_STEPS_MODE1 4  // 模式1(Z方向)：每两个轨迹点之间插值步数  
// #define INTERP_STEPS_MODE2 2  // 模式2(圆形)：每两个轨迹点之间插值步数

// typedef struct {
//     float x, y, z;
// } Point;

// // 为每个点的每种模式存储预先计算的关节角度
// typedef struct {
//     float j1[CIRCLE_TRAJECTORY_POINTS];  // 使用最大的点数
//     float j2[CIRCLE_TRAJECTORY_POINTS];
//     float j3[CIRCLE_TRAJECTORY_POINTS];
//     float j4[CIRCLE_TRAJECTORY_POINTS];
//     float j5[CIRCLE_TRAJECTORY_POINTS];
//     int point_count;  // 实际使用的点数
// } TrajectoryBuffer;

// Point points[MAX_POINTS];
// TrajectoryBuffer trajectories[MAX_POINTS][3];

// unsigned char point_count = 0;
// unsigned char current_point = 0;
// unsigned char cycle_complete = 0;

// /*******************************************************/
// float circle_angle = 0;
// int move_direction = -1;
// unsigned int move_step = 0;

// #define MIN_INTERVAL 30000
// #define MAX_INTERVAL 360000
// uint32_t move_interval = 30000;
// uint32_t timer2_counter = 0;
// uint8_t timer_running = 0;

// unsigned char time_delay=0;
// /***********************************************/

// // S曲线函数
// float smooth_s_curve(float t) 
// {
//     return 0.5f * (1.0f - cosf(M_PI * t));
// }

// // 为某个标记点的某个模式预先计算轨迹
// void plan_trajectory(int point_index, int mode) 
// {
//     if(point_index >= MAX_POINTS) return;
    
//     Point home = points[point_index];
//     TrajectoryBuffer* traj = &trajectories[point_index][mode];
    
//     printf("规划点%d(%.1f,%.1f,%.1f)模式%d的轨迹...\n", 
//            point_index, home.x, home.y, home.z, mode);
    
//     int traj_points;
//     float amplitude;
    
//     if(mode == 2) {
//         traj_points = CIRCLE_TRAJECTORY_POINTS;
//         amplitude = CIRCLE_MOVE_AMPLITUDE;
//         printf("  圆形运动：半径=%.1fcm, 轨迹点数=%d\n", amplitude, traj_points);
//     } else {
//         traj_points = LINE_TRAJECTORY_POINTS;
//         amplitude = LINE_MOVE_AMPLITUDE;
//         printf("  直线运动：幅度=%.1fcm, 轨迹点数=%d\n", amplitude, traj_points);
//     }
    
//     traj->point_count = traj_points;
    
//     for(int i = 0; i < traj_points; i++) 
//     {
//         float t = (float)i / (traj_points - 1);
//         float x, y, z;
        
//         switch(mode) 
//         {
//             case 0:  // X方向左右往返
//             {
//                 float s = smooth_s_curve(t);
//                 float move_ratio = 2.0f * s - 1.0f;
//                 x = home.x + amplitude * move_ratio;
//                 y = home.y;
//                 z = home.z;
//                 break;
//             }
                
//             case 1:  // Z方向上下往返
//             {
//                 float s = smooth_s_curve(t);
//                 float move_ratio = 2.0f * s - 1.0f;
//                 x = home.x;
//                 y = home.y;
//                 z = home.z + amplitude * move_ratio;
//                 break;
//             }
                
//             case 2:  // 圆形运动
//             {
//                 float angle = t * 2.0f * M_PI;
//                 x = home.x + amplitude * cosf(angle);
//                 y = home.y + amplitude * sinf(angle);
//                 z = home.z;
//                 break;
//             }
                
//             default:
//                 x = home.x;
//                 y = home.y;
//                 z = home.z;
//                 break;
//         }
        
//         robot_arm_5dof_method2(x, y, z);
        
//         traj->j1[i] = J1_global;
//         traj->j2[i] = J2_global;
//         traj->j3[i] = J3_global;
//         traj->j4[i] = J4_global;
//         traj->j5[i] = J5_global;
        
//         if(i == 0 || i == traj_points - 1) {
//             printf("  轨迹点[%d]: J1=%.2f°, 位置(%.1f,%.1f,%.1f)\n", 
//                    i, J1_global, x, y, z);
//         }
//     }
    
//     printf("轨迹规划完成：%d个点\n\n", traj_points);
// }

// //////////////////////////////////////////////////////////////////////////////////////////
// // ===== 左右移动模式的参数定义 =====
// #define LR_MOVE_DISTANCE 8.0f        // 左右移动距离(cm)
// #define LR_MOVE_INTERVAL 3000        // 左右移动间隔时间(ms)，2秒
// #define LR_POINT_DURATION_MIN 5      // 每个标记点运动最小时间(分钟)
// #define LR_POINT_DURATION_MAX 60     // 每个标记点运动最大时间(分钟)
// #define MAX_POINTS 5

// typedef struct {
//     float x, y, z;                   // 标记点坐标
//     unsigned char position;          // 当前位置: 0=中心(X), 1=左侧(X-5), 2=右侧(X+5)
//     uint32_t point_timer;            // 该标记点已运行时间(ms)
//     uint32_t point_duration;         // 该标记点总运行时间(ms)
// } SimpleLRPoint;

// SimpleLRPoint lr_points[MAX_POINTS];
// unsigned char lr_point_count = 0;
// unsigned char lr_current_point = 0;
// unsigned char lr_cycle_complete = 0;
// unsigned int lr_move_timer = 0;      // 左右移动计时器
// uint32_t lr_point_duration = 20000;//300000; // 默认5分钟 (5*60*1000ms)
// unsigned char lr_move_flag = 0;      // 2秒到达标志位，在定时器中置位

// unsigned char time_delay_lr = 0;     // 左右移动模式的40ms延时标志
// uint8_t timer_running_lr = 0;        // 左右移动模式的定时器运行标志



// /**************************************************************/


// // // ===== 简单画圆模式的参数定义 =====
// // #define CIRCLE_RADIUS 3.0f           // 圆的半径(cm)
// // #define CIRCLE_POINTS 16              // 圆周上的点数（可修改，角度会自动调整）
// // #define CIRCLE_POINT_INTERVAL 1002    // 每个点的停留时间(ms)
// // #define CIRCLE_POINT_DURATION_MIN 5  // 单个标记点运行最小时间(分钟)
// // #define CIRCLE_POINT_DURATION_MAX 60 // 单个标记点运行最大时间(分钟)
// // #define MAX_CIRCLE_MARKS 5           // 最多标记点数

// // typedef struct {
// //     float center_x, center_y, center_z;  // 标记的中心点坐标
// //     unsigned char current_position;       // 当前在圆周的哪个点 (0 到 CIRCLE_POINTS-1)
// //     uint32_t point_timer;                // 该标记点已运行时间(ms)
// //     uint32_t point_duration;             // 该标记点总运行时间(ms)
// // } CircleMarkPoint;

// // CircleMarkPoint circle_marks[MAX_CIRCLE_MARKS];
// // unsigned char circle_mark_count = 0;
// // unsigned char circle_current_mark = 0;
// // unsigned char circle_cycle_complete = 0;
// // unsigned int circle_move_timer = 0;      // 圆周点移动计时器
// // uint32_t circle_duration = 300000;       // 默认5分钟 (5*60*1000ms)
// // unsigned char circle_move_flag = 0;      // 点切换标志位，在定时器中置位
// // unsigned char time_delay_circle = 0;     // 40ms延时标志
// // uint8_t timer_running_circle = 0;        // 定时器运行标志

// // // ===== 计算圆周上第n个点的坐标 =====
// // // 角度会根据CIRCLE_POINTS自动细分：
// // // 8个点 -> 45°间隔, 16个点 -> 22.5°间隔, 32个点 -> 11.25°间隔
// // void calculate_circle_point(float center_x, float center_y, float center_z, 
// //                             int point_index, float* out_x, float* out_y, float* out_z)
// // {
// //     // 根据点数自动计算角度步进：360° / CIRCLE_POINTS
// //     // 例如：8个点时每个点间隔45°，16个点时每个点间隔22.5°
// //     float angle = point_index * (2.0f * M_PI / CIRCLE_POINTS);  // 转换为弧度
    
// //     *out_x = center_x + CIRCLE_RADIUS * cosf(angle);
// //     *out_y = center_y + CIRCLE_RADIUS * sinf(angle);
// //     *out_z = center_z;
// // }

// // // ===== rb_test_circle: 简单画圆模式主函数 =====
// // void rb_test08(void)
// // {
// //     float X = 0, Y = 40, Z = 49;
// //     float prev_X = X, prev_Y = Y, prev_Z = Z;
// //     float cebai_angle = 0;
// //     unsigned char start_flag = 0;
// //     float add = 0.0f;
    
// //     // 初始化电机
// //     CAN_MOTOR_MODE_SET();
// //     CAN_BLDC_POS_CONTROL(0, 2);
// //     CAN_BLDC_POS_CONTROL(0, 3);
// //     Control_Motor(0, 4);
// //     Control_Motor(0, 5);
// //     Control_Motor(0, 6);
// //     Control_Motor(0, 1);
    
// //     printf("=== 简单画圆模式 rb_test_circle 启动 ===\n");
// //     printf("圆半径: %.1fcm, 圆周点数: %d (角度间隔: %.1f°)\n", 
// //            CIRCLE_RADIUS, CIRCLE_POINTS, 360.0f / CIRCLE_POINTS);
// //     printf("默认运行时间: %d分钟\n", (int)(circle_duration/60000));
    
// //     while (1)
// //     {
// //         uint8_t cmd = LCD_ReceiveTask();

// //         // 按键处理
// //         switch (cmd)
// //         {
// //             case 0xF1: Z += 5; break;  // Z轴上
// //             case 0xF2: Z -= 5; break;  // Z轴下
// //             case 0xF3: X -= 5; break;  // X轴左
// //             case 0xF4: X += 5; break;  // X轴右
// //             case 0xF5: Y += 5; break;  // Y轴前
// //             case 0xF6: Y -= 5; break;  // Y轴后
// //             case 0xF7: 
// //                 cebai_angle -= 10; 
// //                 Control_Motor(cebai_angle, 4); 
// //                 break;
// //             case 0xF8: 
// //                 cebai_angle += 10; 
// //                 Control_Motor(cebai_angle, 4); 
// //                 break;
                
// //             case 0xF9:  // 归位
// //                 CAN_BLDC_POS_CONTROL(0, 2);
// //                 CAN_BLDC_POS_CONTROL(0, 3);
// //                 Control_Motor(0, 4);
// //                 Control_Motor(0, 5);
// //                 Control_Motor(0, 6);
// //                 Control_Motor(0, 1);
// //                 printf("电机归位\n");
// //                 break;
                
// //             case 0xFA:  // 起始状态
// //                 CAN_BLDC_POS_CONTROL(-98, 2);
// //                 CAN_BLDC_POS_CONTROL(170, 3);
// //                 Control_Motor(0, 4);
// //                 Control_Motor(0, 5);
// //                 Control_Motor(0, 6);
// //                 Control_Motor(0, 1);
// //                 printf("电机起始状态\n");
// //                 break;
                
// //             case 0xFB:  // 标记圆心点
// //                 if (!start_flag && circle_mark_count < MAX_CIRCLE_MARKS) 
// //                 {
// //                     circle_marks[circle_mark_count].center_x = X;
// //                     circle_marks[circle_mark_count].center_y = Y;
// //                     circle_marks[circle_mark_count].center_z = Z;
// //                     circle_marks[circle_mark_count].current_position = 0;
// //                     circle_marks[circle_mark_count].point_timer = 0;
// //                     circle_marks[circle_mark_count].point_duration = circle_duration;
// //                     circle_mark_count++;
// //                     printf("标记圆心点%d: (%.1f, %.1f, %.1f), 运行时间=%d分钟\n", 
// //                            circle_mark_count, X, Y, Z, (int)(circle_duration/60000));
// //                 }
// //                 break;
                
// //             case 0xFC:  // 启动/暂停
// //                 if (start_flag) 
// //                 {
// //                     start_flag = 0;
// //                     timer_running_circle = 0;
// //                     printf("暂停画圆\n");
// //                 } 
// //                 else 
// //                 {
// //                     if (circle_mark_count > 0) 
// //                     {
// //                         if (circle_cycle_complete) 
// //                         {
// //                             circle_current_mark = 0;
// //                             circle_cycle_complete = 0;
// //                             // 重置所有标记点的计时器
// //                             for(int i = 0; i < circle_mark_count; i++) {
// //                                 circle_marks[i].point_timer = 0;
// //                                 circle_marks[i].current_position = 0;
// //                             }
// //                         }
// //                         start_flag = 1;
// //                         timer_running_circle = 1;
// //                         circle_move_timer = 0;
// //                         printf("开始画圆运动，标记点数=%d\n", circle_mark_count);
                        
// //                         // 移动到第一个标记点的第一个圆周点
// //                         CircleMarkPoint* first_mark = &circle_marks[0];
// //                         float target_x, target_y, target_z;
// //                         calculate_circle_point(first_mark->center_x, first_mark->center_y, 
// //                                              first_mark->center_z, 0, 
// //                                              &target_x, &target_y, &target_z);
                        
// //                         robot_arm_5dof_method2(target_x, target_y, target_z);
// //                         Control_Motor(90 - J1_global, 1);
// //                         Control_Motor(J4_global, 5);
// //                         Control_Motor(J5_global, 6);
// //                         CAN_BLDC_POS_CONTROL(J2_global, 2);
// //                         CAN_BLDC_POS_CONTROL(J3_global, 3);
// //                     }
// //                 }
// //                 break;
                
// //             case 0xFD:  // 增加运行时间
// //                 if (circle_duration < CIRCLE_POINT_DURATION_MAX * 60000) {
// //                     circle_duration += 60000;  // 增加1分钟
// //                     printf("单个标记点运行时间: %d分钟\n", (int)(circle_duration/60000));
// //                 }
// //                 break;
                
// //             case 0xFE:  // 减少运行时间
// //                 if (circle_duration > CIRCLE_POINT_DURATION_MIN * 60000) {
// //                     circle_duration -= 60000;  // 减少1分钟
// //                     printf("单个标记点运行时间: %d分钟\n", (int)(circle_duration/60000));
// //                 }
// //                 break;
                
// //             default: 
// //                 break;
// //         }

// //         // 红外遥控处理
// //         switch(ir_command)
// //         {
// //             case 0x1c:  Y += 5; break;
// //             case 0x55:  Y -= 5; break;
// //             case 0x59:  X -= 5; break;
// //             case 0x18:  X += 5; break;
// //             case 0x47:  Z += 5; break;
// //             case 0x5d:  Z -= 5; break;
// //             case 0x44:  
// //                 cebai_angle -= 10; 
// //                 Control_Motor(cebai_angle, 4); 
// //                 break;
// //             case 0x5c:  
// //                 cebai_angle += 10; 
// //                 Control_Motor(cebai_angle, 4); 
// //                 break;
                
// //             case 0x15:  // 红外标记圆心点
// //                 if (!start_flag && circle_mark_count < MAX_CIRCLE_MARKS) 
// //                 {
// //                     circle_marks[circle_mark_count].center_x = X;
// //                     circle_marks[circle_mark_count].center_y = Y;
// //                     circle_marks[circle_mark_count].center_z = Z;
// //                     circle_marks[circle_mark_count].current_position = 0;
// //                     circle_marks[circle_mark_count].point_timer = 0;
// //                     circle_marks[circle_mark_count].point_duration = circle_duration;
// //                     circle_mark_count++;
// //                     printf("标记圆心点%d: (%.1f, %.1f, %.1f)\n", 
// //                            circle_mark_count, X, Y, Z);
// //                 }
// //                 break;
                
// //             case 0x45:  // 红外启动/暂停
// //                 if (start_flag) {
// //                     start_flag = 0;
// //                     timer_running_circle = 0;
// //                     printf("暂停画圆\n");
// //                 } else {
// //                     if (circle_mark_count > 0) {
// //                         if (circle_cycle_complete) {
// //                             circle_current_mark = 0;
// //                             circle_cycle_complete = 0;
// //                             for(int i = 0; i < circle_mark_count; i++) {
// //                                 circle_marks[i].point_timer = 0;
// //                                 circle_marks[i].current_position = 0;
// //                             }
// //                         }
// //                         start_flag = 1;
// //                         timer_running_circle = 1;
// //                         circle_move_timer = 0;
// //                         printf("开始画圆运动\n");
                        
// //                         CircleMarkPoint* first_mark = &circle_marks[0];
// //                         float target_x, target_y, target_z;
// //                         calculate_circle_point(first_mark->center_x, first_mark->center_y, 
// //                                              first_mark->center_z, 0, 
// //                                              &target_x, &target_y, &target_z);
                        
// //                         robot_arm_5dof_method2(target_x, target_y, target_z);
// //                         Control_Motor(90 - J1_global, 1);
// //                         Control_Motor(J4_global, 5);
// //                         Control_Motor(J5_global, 6);
// //                         CAN_BLDC_POS_CONTROL(J2_global, 2);
// //                         CAN_BLDC_POS_CONTROL(J3_global, 3);
// //                     }
// //                 }
// //                 break;
                
// //             default: 
// //                 break;
// //         }
// //         ir_command = 0;
        
// //         // ===== 画圆运动控制 =====
// //         if (circle_move_flag)  // 定时器置位的标志
// //         {
// //             circle_move_flag = 0;  // 清除标志位
            
// //             if (start_flag && circle_mark_count > 0 && !circle_cycle_complete) 
// //             {
// //                 CircleMarkPoint* mark = &circle_marks[circle_current_mark];
                
// //                 // 移动到下一个圆周点
// //                 mark->current_position = (mark->current_position + 1) % CIRCLE_POINTS;
                
// //                 float target_x, target_y, target_z;
// //                 calculate_circle_point(mark->center_x, mark->center_y, mark->center_z, 
// //                                       mark->current_position, 
// //                                       &target_x, &target_y, &target_z);
                
// //                 // 计算目标位置的关节角
// //                 robot_arm_5dof_method2(target_x, target_y, target_z);
                
// //                 // 控制电机到目标位置
// //                 Control_Motor(90 - J1_global, 1);
// //                 Control_Motor(J4_global, 5);
// //                 Control_Motor(J5_global, 6);
// //                 CAN_BLDC_POS_CONTROL(J2_global, 2);
// //                 CAN_BLDC_POS_CONTROL(J3_global, 3);
                
// //                 // 计算当前角度位置
// //                 float current_angle = mark->current_position * (360.0f / CIRCLE_POINTS);
                
// //                 printf("标记点%d: 圆周点%d/%d (角度%.1f°), 已运行%.1f分钟/总%.1f分钟\n", 
// //                        circle_current_mark, 
// //                        mark->current_position + 1, 
// //                        CIRCLE_POINTS,
// //                        current_angle,
// //                        mark->point_timer / 60000.0f,
// //                        mark->point_duration / 60000.0f);
                
// //                 // 检查当前标记点是否运行时间已到
// //                 if (mark->point_timer >= mark->point_duration) 
// //                 {
// //                     printf("标记点%d画圆完成，切换到下一个标记点\n", circle_current_mark);
                    
// //                     // 移动到下一个标记点
// //                     circle_current_mark++;
                    
// //                     if (circle_current_mark >= circle_mark_count) 
// //                     {
// //                         // 所有标记点都完成了
// //                         circle_cycle_complete = 1;
// //                         start_flag = 0;
// //                         timer_running_circle = 0;
// //                         circle_current_mark = 0;
// //                         printf("========== 所有标记点画圆完成！==========\n");
// //                     }
// //                     else 
// //                     {
// //                         // 重置下一个标记点的计时器和位置
// //                         circle_move_timer = 0;
// //                         circle_marks[circle_current_mark].point_timer = 0;
// //                         circle_marks[circle_current_mark].current_position = 0;
                        
// //                         // 移动到下一个标记点的第一个圆周点
// //                         CircleMarkPoint* next_mark = &circle_marks[circle_current_mark];
// //                         float next_x, next_y, next_z;
// //                         calculate_circle_point(next_mark->center_x, next_mark->center_y, 
// //                                              next_mark->center_z, 0, 
// //                                              &next_x, &next_y, &next_z);
                        
// //                         robot_arm_5dof_method2(next_x, next_y, next_z);
// //                         Control_Motor(90 - J1_global, 1);
// //                         Control_Motor(J4_global, 5);
// //                         Control_Motor(J5_global, 6);
// //                         CAN_BLDC_POS_CONTROL(J2_global, 2);
// //                         CAN_BLDC_POS_CONTROL(J3_global, 3);
                        
// //                         printf("====== 开始标记点%d画圆 ======\n", circle_current_mark);
// //                     }
// //                 }
// //             }
// //         }
        
// //         // TOF和角度传感器
// //         tof_run();
// //         get_angle();
        
// //         // 姿态补偿
// //         if(Pitch_angle < 30 && Pitch_angle > -30)
// //         {
// //             if(Pitch_angle > 0.6)
// //             {
// //                 add += 2;
// //                 if(add > 10) add = 10;
// //             }
// //             else if (Pitch_angle < -1)
// //             {
// //                 add -= 2;
// //                 if(add < -10) add = -10;
// //             }
// //         }
        
// //         // 手动控制时更新位置
// //         if ((X != prev_X || Y != prev_Y || Z != prev_Z) && !start_flag)
// //         {
// //             prev_X = X;
// //             prev_Y = Y;
// //             prev_Z = Z;

// //             robot_arm_5dof_method2(X, Y, Z);

// //             Control_Motor(90 - J1_global, 1);
// //             CAN_BLDC_POS_CONTROL(J2_global, 2);
// //             CAN_BLDC_POS_CONTROL(J3_global, 3);
// //             Control_Motor(J4_global, 5);
// //             Control_Motor(J5_global, 6);
            
// //             if (J2_global < 0)
// //                 J2_global = -J2_global * 100;
            
// //             n0 = (volatile uint32_t)X;
// //             n1 = (volatile uint32_t)Y;
// //             n2 = (volatile uint32_t)Z;
// //             n3 = (volatile uint32_t)J1_global;
// //             n4 = (volatile uint32_t)J2_global;
// //             n5 = (volatile uint32_t)J3_global;
// //             n6 = (volatile uint32_t)J4_global;
// //             n7 = (volatile uint32_t)J5_global;

// //             LCD_SendTask();
// //         }

// //         Delay_Ms(10);
// //     }
// // }

// // // ===== 画圆模式专用定时器中断 =====
// // void TIM3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
// // void TIM3_IRQHandler(void)
// // {
// //     static unsigned int time_ses = 0;
    
// //     if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
// //     {
// //         // 40ms延时标志（用于主循环刷新）
// //         time_ses++;
// //         if (time_ses > 40)
// //         {
// //             time_delay_circle = 1;
// //             time_ses = 0; 
// //         }

// //         // 只有定时器运行时才计时
// //         if (timer_running_circle)
// //         {
// //             // 圆周点切换间隔计时
// //             circle_move_timer += 10;  // 每10ms累加
            
// //             // 当前标记点运行时间累加
// //             if (circle_current_mark < circle_mark_count) 
// //             {
// //                 circle_marks[circle_current_mark].point_timer += 10;
// //             }
            
// //             // 每2秒置位移动标志（切换到下一个圆周点）
// //             if (circle_move_timer >= CIRCLE_POINT_INTERVAL) 
// //             {
// //                 circle_move_timer = 0;     // 重置计时器
// //                 circle_move_flag = 1;      // 置位移动标志
// //             }
// //         }
// //     }
    
// //     TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
// // }

// /*
// =====================================================================
// 使用说明：rb_test_circle - 简单画圆模式
// =====================================================================

// 【功能概述】
// 机械臂在标记的圆心点周围画圆，依次经过8个均匀分布的圆周点。
// 每个标记点画圆运行指定时间后，自动切换到下一个标记点。

// 【圆周点分布】
// 8个点均匀分布在圆周上：
// 点1: 0°   (正右方)
// 点2: 45°  (右上方)
// 点3: 90°  (正上方)
// 点4: 135° (左上方)
// 点5: 180° (正左方)
// 点6: 225° (左下方)
// 点7: 270° (正下方)
// 点8: 315° (右下方)

// 【运动流程】
// 标记点1的圆心 → 点1(0°) → 点2(45°) → ... → 点8(315°) → 点1(0°) → ...
//                     (循环运行指定时间)
// ↓
// 标记点2的圆心 → 点1(0°) → 点2(45°) → ... → 点8(315°) → ...
//                     (循环运行指定时间)
// ↓
// ...

// 【按键说明】
// - 0xF1/F2: Z轴上下 (±5cm)
// - 0xF3/F4: X轴左右 (±5cm)
// - 0xF5/F6: Y轴前后 (±5cm)
// - 0xF7/F8: 侧摆角度调整 (±10°)
// - 0xF9: 电机归位
// - 0xFA: 电机起始状态
// - 0xFB: 标记当前位置为圆心点
// - 0xFC: 启动/暂停画圆运动
// - 0xFD: 增加运行时间 (+1分钟)
// - 0xFE: 减少运行时间 (-1分钟)

// 【红外遥控】
// - 0x15: 标记圆心点
// - 0x45: 启动/暂停运动
// - 其他: 同按键功能

// 【操作步骤】
// 1. 手动移动机械臂到第一个目标圆心位置
// 2. 按 0xFB 标记该圆心点
// 3. 重复步骤1-2，标记更多圆心点（最多5个）
// 4. 按 0xFD/0xFE 调整每个圆心点的画圆时间（默认5分钟）
// 5. 按 0xFC 启动，机械臂开始自动画圆
// 6. 按 0xFC 可以随时暂停

// 【参数设置】
// - 圆的半径: 8cm (可修改 CIRCLE_RADIUS)
// - 圆周点数: 8个 (可修改 CIRCLE_POINTS)
// - 点停留时间: 2秒 (可修改 CIRCLE_POINT_INTERVAL)
// - 运行时间: 5-60分钟可调
// - 最多标记点: 5个

// 【定时器】
// 使用 TIM4，10ms中断周期
// - 累计每个标记点的运行时间
// - 每2秒置位 circle_move_flag，切换到下一个圆周点
// - 独立于其他定时器

// 【注意事项】
// 1. 确保圆周点不会超出机械臂的工作范围
// 2. 圆的半径可以根据实际需求调整
// 3. 如需更密集或更稀疏的圆周点，修改 CIRCLE_POINTS
// 4. 点停留时间越短，画圆越快，但运动会更频繁

// =====================================================================
// // */
// // // 定时器2中断服务函数，每10ms触发一次
// // void TIM3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
// // void TIM3_IRQHandler(void)
// // {
// //     static unsigned int time_ses = 0;
// //     if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
// //     {
// //         time_ses++;
// //         if (time_ses > 40)
// //         {
// //             time_delay = 1;
// //             time_ses = 0; 
// //         }

// //         if (timer_running)
// //         {
// //             timer2_counter++;
// // #if SIMPLE_LR_MODE
// //             lr_timer_counter++;  // 左右移动模式的计时器
// // #endif
// //             if (timer2_counter >= 0xFFFFFFFF)
// //             {
// //                 timer2_counter = move_interval;
// //             }
// //         }
// //     }
// //     TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
// // }


// // void rb_test08(void)
// // {
// //     float X = 0, Y = 40, Z = 49;
// //     float home_X = X, home_Y = Y, home_Z = Z;
// //     float prev_X = X, prev_Y = Y, prev_Z = Z;
// //     float cebai_angle = 0;
// //     unsigned char start_flag = 0;
// //     unsigned char tree_mode_flag = 0;
// //     float add=0.0f;
// //     char moduan_tim=0;
// //     // 插值相关变量
// //     int traj_index = 0;
// //     int interp_count = 0;
// //     int current_interp_steps = INTERP_STEPS_MODE0;  // 当前模式的插值步数
    
// //     // 存储插值起点和终点
// //     float j1_start, j2_start, j3_start, j4_start, j5_start;
// //     float j1_end, j2_end, j3_end, j4_end, j5_end;
    
// //     CAN_MOTOR_MODE_SET();
// //     CAN_BLDC_POS_CONTROL(0, 2);
// //     CAN_BLDC_POS_CONTROL(0, 3);
// //     Control_Motor(0, 4);
// //     Control_Motor(0, 5);
// //     Control_Motor(0, 6);
// //     Control_Motor(0, 1);
    
// //     while (1)
// //     {
// //         uint8_t cmd = LCD_ReceiveTask();

// //         switch (cmd)
// //         {
// //             case 0xF1: Z += 5; break;
// //             case 0xF2: Z -= 5; break;
// //             case 0xF3: X -= 5; break;
// //             case 0xF4: X += 5; break;
// //             case 0xF5: Y += 5; break;
// //             case 0xF6: Y -= 5; break;
// //             case 0xF7: cebai_angle -= 10; Control_Motor(cebai_angle, 4); break;
// //             case 0xF8: cebai_angle += 10; Control_Motor(cebai_angle, 4); break;
// //             case 0xF9:
// //                 CAN_BLDC_POS_CONTROL(0, 2);
// //                 CAN_BLDC_POS_CONTROL(0, 3);
// //                 Control_Motor(0, 4);
// //                 Control_Motor(0, 5);
// //                 Control_Motor(0, 6);
// //                 Control_Motor(0, 1);
// //                 break;
// //             case 0xFA:
// //                 CAN_BLDC_POS_CONTROL(-98, 2);
// //                 CAN_BLDC_POS_CONTROL(170, 3);
// //                 Control_Motor(0, 4);
// //                 Control_Motor(0, 5);
// //                 Control_Motor(0, 6);
// //                 Control_Motor(0, 1);
// //                 break;
// //             case 0xFB:
// //                 if (!start_flag) 
// //                 {
// //                     if (point_count < MAX_POINTS) 
// //                     {
// //                         points[point_count].x = X;
// //                         points[point_count].y = Y;
// //                         points[point_count].z = Z;
                        
// //                         plan_trajectory(point_count, 0);
// //                         plan_trajectory(point_count, 1);
// //                         plan_trajectory(point_count, 2);
                        
// //                         point_count++;
// //                         printf("标记点%d完成\n", point_count);
// //                     }
// //                 } 
// //                 else if (!cycle_complete) 
// //                 {
// //                     tree_mode_flag = (tree_mode_flag + 1) % 3;
// //                     traj_index = 0;
// //                     interp_count = 0;
                    
// //                     // 根据新模式设置插值步数
// //                     switch(tree_mode_flag) {
// //                         case 0: current_interp_steps = INTERP_STEPS_MODE0; break;
// //                         case 1: current_interp_steps = INTERP_STEPS_MODE1; break;
// //                         case 2: current_interp_steps = INTERP_STEPS_MODE2; break;
// //                     }
// //                     printf("切换到模式%d，插值步数=%d\n", tree_mode_flag, current_interp_steps);
// //                 }
// //                 break;
                
// //             case 0xFC:
// //                 if (start_flag) 
// //                 {
// //                     start_flag = 0;
// //                     timer_running = 0;
// //                 } 
// //                 else 
// //                 {
// //                     if (cycle_complete) 
// //                     {
// //                         current_point = 0;
// //                         cycle_complete = 0;
// //                         timer2_counter = 0;
// //                     }
// //                     start_flag = 1;
// //                     timer_running = 1;
// //                     traj_index = 0;
// //                     interp_count = 0;
                    
// //                     // 启动时设置初始插值步数
// //                     switch(tree_mode_flag) {
// //                         case 0: current_interp_steps = INTERP_STEPS_MODE0; break;
// //                         case 1: current_interp_steps = INTERP_STEPS_MODE1; break;
// //                         case 2: current_interp_steps = INTERP_STEPS_MODE2; break;
// //                     }
// //                 }
// //                 break;
// //             case 0xFD:
// //                 if (move_interval < MAX_INTERVAL) {
// //                     move_interval += 300000;
// //                 }
// //                 break;
// //             case 0xFE:
// //                 if (move_interval > MIN_INTERVAL) {
// //                     move_interval -= 300000;
// //                 }
// //                 break;
// //             default: break;
// //         }

// //         // 红外遥控处理
// //         switch(ir_command)
// //         {
// //             case 0x1c:  Y += 5; break;
// //             case 0x55:  Y -= 5; break;
// //             case 0x59:  X -= 5; break;
// //             case 0x18:  X += 5; break;
// //             case 0x47:  Z += 5; break;
// //             case 0x5d:  Z -= 5; break;
// //             case 0x44:  cebai_angle -= 10; Control_Motor(cebai_angle, 4); break;
// //             case 0x5c:  cebai_angle += 10; Control_Motor(cebai_angle, 4); break;
// //             case 0x15:
// //                 if (!start_flag && point_count < MAX_POINTS) 
// //                 {
// //                     points[point_count].x = X;
// //                     points[point_count].y = Y;
// //                     points[point_count].z = Z;
                    
// //                     plan_trajectory(point_count, 0);
// //                     plan_trajectory(point_count, 1);
// //                     plan_trajectory(point_count, 2);
                    
// //                     point_count++;
// //                 } 
// //                 else if (!cycle_complete) 
// //                 {
// //                     tree_mode_flag = (tree_mode_flag + 1) % 3;
// //                     traj_index = 0;
// //                     interp_count = 0;
                    
// //                     switch(tree_mode_flag) {
// //                         case 0: current_interp_steps = INTERP_STEPS_MODE0; break;
// //                         case 1: current_interp_steps = INTERP_STEPS_MODE1; break;
// //                         case 2: current_interp_steps = INTERP_STEPS_MODE2; break;
// //                     }
// //                 }
// //                 break;      
// //             case 0x45:
// //                 if (start_flag) {
// //                     start_flag = 0;
// //                     timer_running = 0;
// //                 } else {
// //                     if (cycle_complete) {
// //                         current_point = 0;
// //                         cycle_complete = 0;
// //                         timer2_counter = 0;
// //                     }
// //                     start_flag = 1;
// //                     timer_running = 1;
// //                     traj_index = 0;
// //                     interp_count = 0;
                    
// //                     switch(tree_mode_flag) {
// //                         case 0: current_interp_steps = INTERP_STEPS_MODE0; break;
// //                         case 1: current_interp_steps = INTERP_STEPS_MODE1; break;
// //                         case 2: current_interp_steps = INTERP_STEPS_MODE2; break;
// //                     }
// //                 } 
// //                 break;
// //             default: break;
// //         }
// //         ir_command = 0;
        
// //         // ===== 核心：区分插值方式的轨迹执行部分 =====
// //         if(time_delay)
// //         {
// //             time_delay = 0;
// //             if (start_flag && point_count > 0 && !cycle_complete) 
// //             {
// //                 if (timer2_counter < move_interval) 
// //                 {
// //                     TrajectoryBuffer* traj = &trajectories[current_point][tree_mode_flag];
                    
// //                     // 如果插值计数为0，获取新的起点和终点
// //                     if (interp_count == 0) 
// //                     {
// //                         j1_start = traj->j1[traj_index];
// //                         j2_start = traj->j2[traj_index];
// //                         j3_start = traj->j3[traj_index];
// //                         j4_start = traj->j4[traj_index];
// //                         j5_start = traj->j5[traj_index];
                        
// //                         int next_index = (traj_index + 1) % traj->point_count;
// //                         j1_end = traj->j1[next_index];
// //                         j2_end = traj->j2[next_index];
// //                         j3_end = traj->j3[next_index];
// //                         j4_end = traj->j4[next_index];
// //                         j5_end = traj->j5[next_index];
// //                     }
                    
// //                     float j1_current, j2_current, j3_current, j4_current, j5_current;
                    
// //                     // ===== 关键：根据模式选择不同的插值方式 =====
// //                     if (tree_mode_flag == 0 || tree_mode_flag == 2) 
// //                     {
// //                         // 模式0(X方向)和模式2(圆形)：以j1为基准均分插值
                        
// //                         // 计算j1的角度差
// //                         float j1_diff = j1_end - j1_start;
                        
// //                         // 处理角度跨越±180°的情况（选择最短路径）
// //                         if (j1_diff > 180.0f) {
// //                             j1_diff -= 360.0f;
// //                         } else if (j1_diff < -180.0f) {
// //                             j1_diff += 360.0f;
// //                         }
                        
// //                         // j1均分插值
// //                         float t_j1 = (float)interp_count / (float)current_interp_steps;
// //                         j1_current = j1_start + j1_diff * t_j1;
                        
// //                         // 归一化到 -180 ~ 180
// //                         while (j1_current > 180.0f) j1_current -= 360.0f;
// //                         while (j1_current < -180.0f) j1_current += 360.0f;
                        
// //                         // 其他关节角按j1的进度同步插值
// //                         j2_current = j2_start + (j2_end - j2_start) * t_j1;
// //                         j3_current = j3_start + (j3_end - j3_start) * t_j1;
// //                         j4_current = j4_start + (j4_end - j4_start) * t_j1;
// //                         j5_current = j5_start + (j5_end - j5_start) * t_j1;
                        
// //                         printf("模式%d[J1均分]: traj=%d, interp=%d/%d, J1=%.2f°,j2=%f,j3=%f\r\n",
// //                                tree_mode_flag, traj_index, interp_count, 
// //                                current_interp_steps, j1_current,j2_current,j3_current);
// //                     }
// //                     else if (tree_mode_flag == 1)
// //                     {
// //                         // 模式1(Z方向)：所有关节角线性插值
// //                         float t = (float)interp_count / (float)current_interp_steps;
                        
// //                         j1_current = j1_start + (j1_end - j1_start) * t;
// //                         j2_current = j2_start + (j2_end - j2_start) * t;
// //                         j3_current = j3_start + (j3_end - j3_start) * t;
// //                         j4_current = j4_start + (j4_end - j4_start) * t;
// //                         j5_current = j5_start + (j5_end - j5_start) * t;
                        
// //                         printf("模式1[线性]: traj=%d, interp=%d/%d\r\n",
// //                                traj_index, interp_count, current_interp_steps);
// //                     }
                    
// //                     // 执行插值后的角度
// //                     Control_Motor(90 - j1_current, 1);
// //                     Control_Motor(j4_current, 5);
// //                     Control_Motor(j5_current, 6); 
// //                     CAN_BLDC_POS_CONTROL(j2_current, 2);
// //                     CAN_BLDC_POS_CONTROL(j3_current, 3);
                    
// //                     // 增加插值计数
// //                     interp_count++;
                    
// //                     // 如果完成了当前插值段，移动到下一个轨迹点
// //                     if (interp_count >= current_interp_steps) 
// //                     {
// //                         interp_count = 0;
// //                         traj_index++;
                        
// //                         if (traj_index >= traj->point_count) 
// //                         {

// //                             traj_index = 0;
// //                         }
// //                     }
// //                 } 
// //                 else 
// //                 {
// //                     // 时间到，移动到下一个标记点
// //                     current_point++;
// //                     if (current_point >= point_count) 
// //                     {
// //                         cycle_complete = 1;
// //                         start_flag = 0;
// //                         timer_running = 0;
// //                         current_point = 0;
// //                     } 
// //                     else 
// //                     {
// //                         timer2_counter = 0;
// //                         traj_index = 0;
// //                         interp_count = 0;
// //                     }
// //                 }
// //             }
// //         }
        
// //         tof_run();
// //         get_angle();
        
// //         if(Pitch_angle<30&&Pitch_angle>-30)
// //         {
// //             if(Pitch_angle>0.6)
// //             {
// //                 add+=2;
// //                 if(add>10) add =10;
// //             }
// //             else if (Pitch_angle<-1)
// //             {
// //                 add-=2;
// //                 if(add<-10) add =-10;
// //             }
// //         }
        
// //         // 手动控制时更新位置
// //         if ((X != prev_X || Y != prev_Y || Z != prev_Z) && !start_flag)
// //         {
// //             prev_X = X;
// //             prev_Y = Y;
// //             prev_Z = Z;

// //             robot_arm_5dof_method2(X, Y, Z);

// //             Control_Motor(90 - J1_global, 1);
// //             CAN_BLDC_POS_CONTROL(J2_global, 2);
// //             CAN_BLDC_POS_CONTROL(J3_global, 3);
// //             Control_Motor(J4_global, 5);
// //             Control_Motor(J5_global, 6);
            
// //             if (J2_global < 0)
// //                 J2_global = -J2_global * 100;
            
// //             n0 = (volatile uint32_t)X;
// //             n1 = (volatile uint32_t)Y;
// //             n2 = (volatile uint32_t)Z;
// //             n3 = (volatile uint32_t)J1_global;
// //             n4 = (volatile uint32_t)J2_global;
// //             n5 = (volatile uint32_t)J3_global;
// //             n6 = (volatile uint32_t)J4_global;
// //             n7 = (volatile uint32_t)J5_global;

// //             LCD_SendTask();
// //         }

// //         Delay_Ms(10);
// //     }
// // }
