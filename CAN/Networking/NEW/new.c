#include "new.h"
#include "stdio.h"
#include "arm.h"
#include "lcd.h"
#include "bldc.h"
#include "step.h"
#include "ir.h"
#include "tof.h"
#include "wt61.h"
#include "tim.h"
#include "sw.h"
float cebai_angle = 0;

/****************参数定义*******************************************/
#define RGB_DURATION 500 //LED闪烁周期
unsigned char flame_flag=0;
unsigned char  RGB_ON_OFF_FLAG=1;
unsigned char  RGB_LED_MODE=255;
unsigned char  YUYIN_FLAG=0;
unsigned char  RGB_LED_MODE_BACKUP=255;  // 新增：备份RGB_LED_MODE
unsigned char  out_of_range_flag=0;    // 新增：超出范围标志

#define MAX_POINTS 5                // 最多标记5个点
#define POINT_DURATION_MIN 2        // 每个标记点运动最小时间(分钟)
#define POINT_DURATION_MAX 10       // 每个标记点运动最大时间(分钟)

// 坐标范围限制
#define X_MIN -25.0f
#define X_MAX  25.0f
#define Y_MIN  10.0f
#define Y_MAX  41.0f
#define Z_MIN  10.0f
#define Z_MAX  50.0f

// 左右移动参数
#define LR_MOVE_DISTANCE   3.0f     // X±5mm移动
#define LR_MOVE_INTERVAL   1200     // 左右移动间隔(ms)

// 圆形模式参数
#define CIRCLE_RADIUS      8.0f     // 圆形运动半径
#define CIRCLE_INTERVAL    700     // 圆形运动间隔(ms)
#define CIRCLE_POINTS 8  // 圆形运动分多少个点，可修改（建议4-360之间）

// 上下移动参数
#define UD_MOVE_INTERVAL   300     // 上下移动间隔(ms)
#define UD_TRAJECTORY_POINTS 8      // 上下运动分段数（用于实时计算插值进度）
#define UD_MOVE_AMPLITUDE 8.0f     // 上下运动幅度(cm)
#define INTERP_STEPS_MODE_UD 3      // 每段运动的插值步数

// 回位延时参数
#define RETURN_DELAY_MS 6000        // 回位延时3秒

// 上下模式运行时变量（仅存储实时计算所需状态）
static int ud_traj_step = 0;               // 实时运动分段索引（0~UD_TRAJECTORY_POINTS-1）
static int ud_interp_count = 0;            // 实时插值计数（0~INTERP_STEPS_MODE_UD-1）
static float ud_current_t = 0.0f;          // 实时运动进度（0~1，用于S曲线计算）

//================= 运行点结构 =================
//================= 运行点结构 =================
typedef struct {
    float x, y, z;                  // 标记点坐标
    float j1, j2, j3, j4, j5;
    uint32_t point_timer;           // 该标记点已运行时间(ms)
    uint32_t point_duration;        // 该标记点总运行时间(ms)
    uint8_t  position;              // 位置标记(模式专用)
    float jiuli_baseline;           // 新增：标记点时的基准距离(cm)
} SimplePoint;

//================= 全局变量 =================
SimplePoint points[MAX_POINTS];    // 标记点数组
unsigned char point_count = 0;     // 标记点计数
uint32_t point_duration = 5 * 60000*1.4; // 默认5分钟(ms)

// 模式标志(四选一) - 默认悬停模式
static uint8_t mode_hover  = 1;    // 悬停模式（默认）
static uint8_t mode_circle = 0;    // 圆形模式
static uint8_t mode_lr     = 0;    // 左右模式
static uint8_t mode_ud     = 0;    // 上下模式

// 周期标志
static volatile uint8_t circle_move_flag = 0;
static volatile uint8_t lr_move_flag     = 0;
static volatile uint8_t ud_move_flag     = 0;
static volatile uint8_t time_delay_100ms  = 0;

// 计时器控制
static volatile uint8_t timer_running_circle = 0;
static volatile uint8_t timer_running_lr     = 0;
static volatile uint8_t timer_running_ud     = 0;

// 回位控制
static volatile uint8_t return_delay_flag = 0;      // 3秒延时完成标志
static volatile uint8_t return_in_progress = 0;     // 回位进行中标志
static uint8_t return_type = 0;                     // 回位类型：0=无，1=模式切换，2=循环结束，3=重新开始

// 运行状态控制
static uint8_t start_flag = 0;     // 运动启动标志
static uint8_t cycle_complete = 0; // 一轮运行完成标志
static uint8_t current_point = 0;  // 当前运行点索引

// 模式专用角度记录（保留不清除）
float LR_angle[MAX_POINTS][3][6];  // [点][位置][轴]
float CIRCLE_angle[MAX_POINTS][36][6]; // 圆形36个角度点
float UD_angle[MAX_POINTS][3][6];  // 上下位置角度

/****************新增：坐标钳制函数*******************************************/
// 将坐标限制在合法范围内
void clamp_coordinates(float* x, float* y, float* z)
{
    // 限制X坐标
    if (*x < X_MIN) *x = X_MIN;
    if (*x > X_MAX) *x = X_MAX;
    
    // 限制Y坐标
    if (*y < Y_MIN) *y = Y_MIN;
    if (*y > Y_MAX) *y = Y_MAX;
    
    // 限制Z坐标
    if (*z < Z_MIN) *z = Z_MIN;
    if (*z > Z_MAX) *z = Z_MAX;
}
/****************新增：坐标范围检查函数*******************************************/
// 检查坐标是否在合法范围内
uint8_t check_coordinate_range(float x, float y, float z)
{
    if (x <= X_MIN || x >= X_MAX || y <= Y_MIN || y >= Y_MAX || z <= Z_MIN || z >= Z_MAX) {
        return 0;  // 超出范围
    }
    return 1;  // 在范围内
}

// 更新范围状态和LED模式
void update_range_status(float x, float y, float z)
{
    uint8_t in_range = check_coordinate_range(x, y, z);
    
    if (!in_range && !out_of_range_flag) {
        // 刚超出范围
        out_of_range_flag = 1;
        RGB_LED_MODE_BACKUP = RGB_LED_MODE;  // 备份当前模式
        RGB_LED_MODE = 5;  // 设置为模式5
    }
    else if (in_range && out_of_range_flag) {
        // 从超出范围恢复到范围内
        out_of_range_flag = 0;
        RGB_LED_MODE = RGB_LED_MODE_BACKUP;  // 恢复原模式
    }
}

// 打印关节角度的函数
static void print_joint_angles()
{
    printf("角度: J1=%.1f, J2=%.1f, J3=%.1f, J4=%.1f, J5=%.1f, 侧摆=%.1f\r\n",
           J1_global, J2_global, J3_global, J4_global, J5_global, cebai_angle);
}
// 高度补偿函数:根据当前距离和基准距离计算Z补偿值
static float calculate_z_compensation(float current_jiuli, float baseline_jiuli) {
    static float last_compensated_jiuli = 0.0f;  // 记录上次补偿时的距离
    static unsigned is_first_call = 1;            // 是否首次调用
    
    // 如果基准距离无效,不进行补偿
    if (baseline_jiuli <= 10 || baseline_jiuli >= 50 || 
        current_jiuli >= 50 || current_jiuli <= 0) {
        return 0.0f;
    }
    
    // 首次调用时,初始化上次补偿位置为基准值
    if (is_first_call) {
        last_compensated_jiuli = baseline_jiuli;
        is_first_call = 0;
    }
    
    // **条件1: 必须距离基准超过2cm**
    float diff_from_baseline = fabs(current_jiuli - baseline_jiuli);
    if (diff_from_baseline <= 2.0f) {
        return 0.0f;  // 未超过基准2cm,不补偿
    }
    
    // **条件2: 必须距离上次补偿位置变化超过1cm**
    float diff_from_last = fabs(current_jiuli - last_compensated_jiuli);
    if (diff_from_last < 1.0f) {
        return 0.0f;  // 变化不足1cm,不补偿
    }
    
    // 两个条件都满足,执行补偿
    // 计算补偿值(传感器读数变小=地面升高=需要减小Z,反之亦然)
    float compensation = baseline_jiuli - current_jiuli;
    
    // 限制补偿范围,避免过大的调整(例如±10cm)
    if (compensation > 10.0f) compensation = 10.0f;
    if (compensation < -10.0f) compensation = -10.0f;
    
    // **更新上次补偿的位置**
    last_compensated_jiuli = current_jiuli;
    return compensation;
}

// S曲线函数（运行时实时计算运动比例）
float smooth_s_curve(float t) 
{
    // t范围[0,1]，输出[0,1]，实现平滑启停
    return 0.5f * (1.0f - cosf((float)M_PI * t));
}
void RGB_RUN()
{
    static uint32_t acc_LED     = 0;
    static unsigned char lan_jici=0;
    if(RGB_LED_MODE==4)
        {
            RGB_huxi(1);
        }
        else if(RGB_LED_MODE==6)
        {
            RGB_huxi(0);
        }
        else
        {
            acc_LED+=100;
            if (acc_LED==500)
            {
                if(RGB_LED_MODE==0)
                {
                    WS2812B_Reset();
                    WS2812B_WriteColor(50,50,50);//白
                }
                else if(RGB_LED_MODE==1)
                {
                    WS2812B_Reset();
                    WS2812B_WriteColor(0,50,0);//绿
                    //WS2812B_WriteColor(60,40,0);
                }
                else if(RGB_LED_MODE==3)
                {
                    WS2812B_Reset();
                    WS2812B_WriteColor(60,40,0);//黄色
                }
                else if (RGB_LED_MODE==5)
                {
                    WS2812B_Reset();
                    WS2812B_WriteColor(60,0,0);  // 红色
                }
                else if(RGB_LED_MODE==7)
                {
                    WS2812B_Reset();
                    WS2812B_WriteColor(0,0,60);  // 蓝色
                    lan_jici++;
                    if(lan_jici>=3)
                    {
                        lan_jici=0;
                        RGB_LED_MODE=RGB_LED_MODE_BACKUP;
                    }
                }
            }
            if(acc_LED==1000)
            {
                acc_LED=0;
                WS2812B_Reset();
                WS2812B_WriteColor(0,0,0);
            }
        }
}
void yunyin_run()
{
    static uint32_t acc_yuyin=0,OK_flag=1;
    static unsigned char last_yuyin_flag=255;
    if (last_yuyin_flag!=YUYIN_FLAG&&OK_flag)
    {
        
        last_yuyin_flag=YUYIN_FLAG;
        acc_yuyin=0;
        OK_flag=0;
        switch (YUYIN_FLAG) 
        {
            case 0: GPIO_WriteBit(GPIOA,GPIO_Pin_8,Bit_RESET); break;
            case 1: GPIO_WriteBit(GPIOC,GPIO_Pin_9,Bit_RESET); break;
            case 2: GPIO_WriteBit(GPIOC,GPIO_Pin_8,Bit_RESET); break;
            case 3: GPIO_WriteBit(GPIOC,GPIO_Pin_7,Bit_RESET); break;
            case 4: GPIO_WriteBit(GPIOC,GPIO_Pin_6,Bit_RESET); break;
            case 5: GPIO_WriteBit(GPIOB,GPIO_Pin_15,Bit_RESET); break;
            case 6: GPIO_WriteBit(GPIOB,GPIO_Pin_14,Bit_RESET); break;
            case 7: GPIO_WriteBit(GPIOB,GPIO_Pin_13,Bit_RESET); break;
            case 8: GPIO_WriteBit(GPIOB,GPIO_Pin_12,Bit_RESET); break;
            default:break;
        }
        if(YUYIN_FLAG==6||YUYIN_FLAG==7)
        {
            YUYIN_FLAG=255;
        }

    }
    else if(acc_yuyin<3000)
    {
        acc_yuyin+=100;
    }
    else if (acc_yuyin==3000)
    {
        acc_yuyin+=100;
        OK_flag=1;
        if(YUYIN_FLAG==1)
        {
            if (mode_hover) {YUYIN_FLAG=4;}
            else if (mode_lr) {YUYIN_FLAG=3;}
            else if (mode_ud){YUYIN_FLAG=2;}
            else if (mode_circle) {YUYIN_FLAG=5;}
        }
        GPIO_WriteBit(GPIOA,GPIO_Pin_8,Bit_SET); 
        GPIO_WriteBit(GPIOC,GPIO_Pin_9,Bit_SET); 
        GPIO_WriteBit(GPIOC,GPIO_Pin_8,Bit_SET); 
        GPIO_WriteBit(GPIOC,GPIO_Pin_7,Bit_SET); 
        GPIO_WriteBit(GPIOC,GPIO_Pin_6,Bit_SET); 
        GPIO_WriteBit(GPIOB,GPIO_Pin_15,Bit_SET);
        GPIO_WriteBit(GPIOB,GPIO_Pin_14,Bit_SET);
        GPIO_WriteBit(GPIOB,GPIO_Pin_13,Bit_SET);
        GPIO_WriteBit(GPIOB,GPIO_Pin_12,Bit_SET);
    }
}
/****************定时器中断处理100ms*******************************************/
void TIM3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM3_IRQHandler(void)
{
    static uint32_t t_10ms = 0;
    static uint32_t acc_circle = 0;
    static uint32_t acc_lr     = 0;
    static uint32_t acc_ud     = 0;
    static uint32_t acc_flame     = 0;
    static uint32_t acc_return = 0;  // 回位延时累加器
   

    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {

        // 100ms节拍
        t_10ms += 100;//原10ms,没改名称
        if (t_10ms >= 100) 
        {
            time_delay_100ms = 1;
            t_10ms = 0;
            
        }
        if(flame_flag)
        {
            acc_flame+=100;
            if(acc_flame>=252000)
            {
                flame_flag=0;
                acc_flame=0;
                light_on;
                fan_on;
                flame_off;

            }
        
        }
        RGB_RUN();
        yunyin_run();
        
        // 模式周期计时
        if (timer_running_circle) {
            acc_circle += 100;
            if (acc_circle >= CIRCLE_INTERVAL) {
                acc_circle = 0;
                circle_move_flag = 1;
            }
        } else acc_circle = 0;

        if (timer_running_lr) {
            acc_lr += 100;
            if (acc_lr >= LR_MOVE_INTERVAL) {
                acc_lr = 0;
                lr_move_flag = 1;
            }
        } else acc_lr = 0;

        if (timer_running_ud) {
            acc_ud += 100;
            if (acc_ud >= UD_MOVE_INTERVAL) {
                acc_ud = 0;
                ud_move_flag = 1;
            }
        } else acc_ud = 0;

        // 回位延时计时
        if (return_in_progress) {
            acc_return += 100;
            if (acc_return >= RETURN_DELAY_MS) {
                acc_return = 0;
                return_delay_flag = 1;  // 3秒到，设置标志
            }
        } else {
            acc_return = 0;
        }

        // 标记点计时(所有模式共用，悬停模式也计时)
        if (start_flag && !cycle_complete && point_count > 0) {
            points[current_point].point_timer += 100;
        }

        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}

/****************基础动作函数*******************************************/
//原版回零
// void RUN_ZERO()//回零
// {
//     CAN_MOTOR_MODE_SET();
//     CAN_BLDC_POS_CONTROL(0, 2);
//     CAN_BLDC_POS_CONTROL(0, 3);
//     Control_Motor(0, 4);
//     Control_Motor(0, 5);
//     Control_Motor(0, 6);
//     Control_Motor(0, 1);
//     printf("电机归位\r\n");
//     // 打印归位后的角度
//     J1_global = 0; J2_global = 0; J3_global = 0; J4_global = 0; J5_global = 0;
//     print_joint_angles();
// }



// void RUN_START()//回初始位置
// {
//     CAN_BLDC_POS_CONTROL(-98, 2);
//     CAN_BLDC_POS_CONTROL(170, 3);
//     Control_Motor(0, 4);
//     Control_Motor(0, 5);
//     Control_Motor(90, 6);
//     Control_Motor(0, 1);
//     printf("电机起始状态\r\n");
//     // 设置初始角度并打印
//     J1_global = 0; J2_global = -98; J3_global = 170; J4_global = 0; J5_global = 0;
//     print_joint_angles();
// }
void RUN_START()//回初始位置
{
    CAN_BLDC_POS_CONTROL(-10, 2);
    CAN_BLDC_POS_CONTROL(95, 3);
    Control_Motor(0, 4);
    Control_Motor(45, 5);
    Control_Motor(60, 6);
    Control_Motor(0, 1);
    printf("电机起始状态\r\n");
    // 设置初始角度并打印
    J1_global = 0; J2_global = -10; J3_global = 95; J4_global = 45; J5_global = 60;
    print_joint_angles();
}
void RUN_ZERO()//回零
{
    CAN_MOTOR_MODE_SET();
    CAN_BLDC_POS_CONTROL(-98, 2);
    CAN_BLDC_POS_CONTROL(170, 3);
    Control_Motor(0, 4);
    Control_Motor(0, 5);
    Control_Motor(90, 6);
    Control_Motor(0, 1);
    printf("电机归位\r\n");
    // 打印归位后的角度
    J1_global = 0; J2_global = -98; J3_global = 170; J4_global = 0; J5_global = 90;
    print_joint_angles();
}

void RUN_TIME_ADD()//增加单点运行时间
{
    if (point_duration < POINT_DURATION_MAX * 60000) 
    {
        point_duration += 60000;  // 增加1分钟  
        for(unsigned char i=0;i<point_count;i++)
        {
            points[i].point_duration+=60000;
        }


        printf("单点运行时间: %d分钟\r\n", (int)(point_duration/60000));
    }
}

void RUN_TIME_MINUS()//减少单点运行时间
{
    if (point_duration > POINT_DURATION_MIN * 60000) {
        point_duration -= 60000;  // 减少1分钟
        printf("单点运行时间: %d分钟\r\n", (int)(point_duration/60000));
        for(unsigned char i=0;i<point_count;i++)
        {
            points[i].point_duration-=60000;
        }

    }
}

/****************回位函数*******************************************/
// 移动到指定标记点
static void move_to_point(uint8_t point_idx)
{
    if (point_idx >= point_count) return;
    
    SimplePoint* pt = &points[point_idx];
    if(pt->j1==0&&pt->j2==0&&pt->j3==0&&pt->j4==0&&pt->j5==0)
    {
        robot_arm_5dof_method2(pt->x, pt->y, pt->z);
        points[point_idx].j1 = J1_global;
        points[point_idx].j2 = J2_global;
        points[point_idx].j3 = J3_global;
        points[point_idx].j4 = J4_global;
        points[point_idx].j5 = J5_global;
        pt = &points[point_idx];
    }
    // 直接使用保存的关节角度，无需重新逆解
    Control_Motor(90 - pt->j1, 1);
    CAN_BLDC_POS_CONTROL(pt->j2, 2);
    CAN_BLDC_POS_CONTROL(pt->j3, 3);
    Control_Motor(pt->j4, 5);
    Control_Motor(pt->j5, 6);
    
    printf("移动到标记点%d: (%.1f, %.1f, %.1f)\r\n", 
           point_idx + 1, pt->x, pt->y, pt->z);
    printf("关节角度: J1=%.1f, J2=%.1f, J3=%.1f, J4=%.1f, J5=%.1f\r\n",
           pt->j1, pt->j2, pt->j3, pt->j4, pt->j5);
}

// 启动回位延时（先移动再延时）
static void start_return_delay(uint8_t type, uint8_t target_point_idx)
{
    return_type = type;
    
    const char* type_names[] = {"无", "模式切换", "循环结束", "重新开始", "首次启动"};
    printf("======= 开始回位流程 - 类型: %s =======\r\n", type_names[type]);
    
    // 先移动到目标点
    if (target_point_idx < point_count) {
        move_to_point(target_point_idx);
    }
    
    // 然后启动延时
    return_in_progress = 1;
    return_delay_flag = 0;
    printf("移动完成，开始延时%d秒...\r\n", RETURN_DELAY_MS/1000);
}

/****************模式切换函数*******************************************/
static void switch_mode(uint8_t new_mode)
{
    // 停止当前模式计时器
    timer_running_circle = 0;
    timer_running_lr = 0;
    timer_running_ud = 0;
    
    // 更新模式标志（0=悬停, 1=圆形, 2=左右, 3=上下）
    mode_hover  = (new_mode == 0) ? 1 : 0;
    mode_circle = (new_mode == 1) ? 1 : 0;
    mode_lr     = (new_mode == 2) ? 1 : 0;
    mode_ud     = (new_mode == 3) ? 1 : 0;
    
    // 模式切换提示
    const char* mode_names[] = {"悬停", "圆形", "左右", "上下"};
    printf("已切换到%s模式\r\n", mode_names[new_mode]);
    
    // 如果正在运行，先移动到当前标记点再延时
    if (start_flag && point_count > 0) {
        start_return_delay(1, current_point);  // 类型1：模式切换，移动到当前点
    } else {
        // 不在运行状态，直接启动新模式计时器（悬停模式不需要计时器）
        if (start_flag) {
            timer_running_circle = mode_circle;
            timer_running_lr = mode_lr;
            timer_running_ud = mode_ud;
        }
    }
}

/****************模式运动函数*******************************************/
// 悬停模式运动（保持在标记点不动）
static void run_hover() {
    static uint32_t last_print_time = 0;
    SimplePoint* pt = &points[current_point];
    
    // 获取当前距离并计算补偿
    float current_jiuli = filtered_tof();
    float z_compensation = calculate_z_compensation(current_jiuli, pt->jiuli_baseline);
    float compensated_z = pt->z + z_compensation;
    
    // 钳制坐标
    float temp_x = pt->x, temp_y = pt->y, temp_z = compensated_z;
    clamp_coordinates(&temp_x, &temp_y, &temp_z);
    
    // 只要有补偿值就更新位置(因为calculate_z_compensation已经做了所有判断)
    if (fabs(z_compensation) > 0.01f) {  // 浮点数比较用小阈值
        robot_arm_5dof_method2(temp_x, temp_y, temp_z);
        CAN_BLDC_POS_CONTROL(J2_global, 2);
        CAN_BLDC_POS_CONTROL(J3_global, 3);
        Control_Motor(J4_global, 5);
        Control_Motor(90 - J1_global, 1);
    }
}

// 左右模式运动
static void run_lr()
{
    if (!lr_move_flag || !start_flag || cycle_complete || return_in_progress) return;
    
    SimplePoint* pt = &points[current_point];
    float target_x;
    const char* pos_name;

    // 确定下一个位置
    switch (pt->position)
    {
        case 0: target_x = pt->x + LR_MOVE_DISTANCE; pos_name = "X+5"; pt->position = 1; break;
        case 1: target_x = pt->x - LR_MOVE_DISTANCE; pos_name = "X-5"; pt->position = 2; break;
        case 2: target_x = pt->x + LR_MOVE_DISTANCE; pos_name = "X+5"; pt->position = 1; break;
        default: target_x = pt->x; pos_name = "中心"; pt->position = 0; break;
    }

    // 新增：高度补偿
    float current_jiuli = filtered_tof();
    float z_compensation = calculate_z_compensation(current_jiuli, pt->jiuli_baseline);
    float compensated_z = pt->z + z_compensation;
    
    // 钳制坐标
    clamp_coordinates(&target_x, &(float){pt->y}, &compensated_z);

    // 实时逆解（使用补偿后的Z）
    robot_arm_5dof_method2(target_x, pt->y, compensated_z);
    
    // 控制电机
    Control_Motor_new(90 - J1_global, 1, 2500, 220);
    CAN_BLDC_POS_CONTROL(J2_global, 2);
    CAN_BLDC_POS_CONTROL(J3_global, 3);

    lr_move_flag = 0;
}

// 圆形模式运动（实时逆解版）
static void run_circle()
{
    if (!circle_move_flag || !start_flag || cycle_complete || return_in_progress) return;
    
    SimplePoint* pt = &points[current_point];
    static uint8_t angle_idx = 0;
    
    // 计算当前角度
    float angle_step = 360.0f / CIRCLE_POINTS;
    float current_angle = angle_idx * angle_step;
    float rad = current_angle * 3.1415926f / 180.0f;
    
    // 计算目标位置
    float target_x = pt->x + CIRCLE_RADIUS * cos(rad);
    float target_y = pt->y + CIRCLE_RADIUS * sin(rad);
    
    // 新增：高度补偿
    float current_jiuli = filtered_tof();
    float z_compensation = calculate_z_compensation(current_jiuli, pt->jiuli_baseline);
    float compensated_z = pt->z + z_compensation;
    
    // 钳制坐标
    clamp_coordinates(&target_x, &target_y, &compensated_z);
    
    // 实时逆解并控制（使用补偿后的Z）
    robot_arm_5dof_method2(target_x, target_y, compensated_z);
    Control_Motor_new(90 - J1_global, 1, 150, 0);
    CAN_BLDC_POS_CONTROL(J2_global, 2);
    CAN_BLDC_POS_CONTROL(J3_global, 3);
    Control_Motor(cebai_angle, 4);
    Control_Motor(J4_global, 5);
    angle_idx = (angle_idx + 1) % CIRCLE_POINTS;
    circle_move_flag = 0;
}
// 上下模式运动
static void run_ud()
{
    if (!ud_move_flag || !start_flag || cycle_complete || return_in_progress) return;
    
    SimplePoint* pt = &points[current_point];
    
    // 实时计算运动进度
    float total_steps = (float)(UD_TRAJECTORY_POINTS * INTERP_STEPS_MODE_UD);
    float current_total_step = (float)(ud_traj_step * INTERP_STEPS_MODE_UD + ud_interp_count);
    ud_current_t = current_total_step / total_steps;

    // 实时计算Z方向目标位置（S曲线平滑）
    float s = smooth_s_curve(ud_current_t);
    float move_ratio = 2.0f * s - 1.0f;
    float target_z_base = pt->z + UD_MOVE_AMPLITUDE * move_ratio;
    
    // 新增：高度补偿
    float current_jiuli = filtered_tof();
    float z_compensation = calculate_z_compensation(current_jiuli, pt->jiuli_baseline);
    float compensated_z = target_z_base + z_compensation;
    
    // 钳制坐标
    float temp_x = pt->x, temp_y = pt->y;
    clamp_coordinates(&temp_x, &temp_y, &compensated_z);

    // 实时逆解计算关节角度（使用补偿后的Z）
    robot_arm_5dof_method2(temp_x, temp_y, compensated_z);
    
    // 实时执行电机控制
    Control_Motor_new(90 - J1_global, 1, 2000, 200);
    CAN_BLDC_POS_CONTROL(J2_global, 2);
    CAN_BLDC_POS_CONTROL(J3_global, 3);
    Control_Motor_new(J4_global, 5, 200, 80);
    // 更新运行时状态
    ud_interp_count++;
    if (ud_interp_count >= INTERP_STEPS_MODE_UD) {
        ud_interp_count = 0;
        ud_traj_step++;
        if (ud_traj_step >= UD_TRAJECTORY_POINTS) {
            ud_traj_step = 0;
        }
    }

    ud_move_flag = 0;
}

static float P =0.5f ,I=0.02f,D=0.0f;
static float last_error=0.0f,last_last_error=0.0f,error=0.0f;
static float run_angle=0;
float PID_five_motor(float angle)
{
    error=angle;
    run_angle+= P*(error-last_error)+I*error+D*(error-2*last_error+last_last_error);
    last_error=error;
    last_last_error=last_error;
   // if (run_angle>200) run_angle=200;
   // if (run_angle<-200) run_angle=-200;
    return run_angle;
}

/****************主控制函数*******************************************/
void rb_test08(void)
{
    float X = 0, Y = 30, Z = 25;
    float prev_X = X, prev_Y = Y, prev_Z = Z;
    float add = 0.0f;
    unsigned char pitch_on_flag=1;
    float first_anglg=0,first_imu_flag=1,first_imu_flag2=0,imu_time=0;
    float jiuli=0;
    float correct_angle6=0,correct_angle1=0;
    unsigned char biji_flag=1; //允许标记标志
    RGB_LED_MODE=6;//白色呼吸

    // 初始化回起始位置
    // CAN_BLDC_POS_CONTROL(0, 2);
    // CAN_BLDC_POS_CONTROL(0, 3);
    // Control_Motor(0, 4);
    // Control_Motor(0, 5);
    // Control_Motor(0, 6);
    // Control_Motor(0, 1);
    //RUN_ZERO();
    //RUN_START();
    SW_Release();
    printf("默认运行时间: %d分钟\r\n", (int)(point_duration/60000));
    printf("当前模式: 悬停(默认)\r\n");
    printf("坐标范围限制: X[%.1f~%.1f], Y[%.1f~%.1f], Z[%.1f~%.1f]\r\n", 
           X_MIN, X_MAX, Y_MIN, Y_MAX, Z_MIN, Z_MAX);

    while (1)
    {
        // 按键处理
        uint8_t cmd = LCD_ReceiveTask();
        switch (cmd)
        {
             case 0xf1:  // Z上升
            {
                
                
                // 判断是否在运行状态
                if (start_flag && !cycle_complete && point_count > 0) 
                {
                    // 运行中：调整当前运行点的Z
                    if((points[current_point].z +5)<Z_MAX)
                    {
                        points[current_point].z += 5;
                        robot_arm_5dof_method2(points[current_point].x, points[current_point].y, points[current_point].z);
                        points[current_point].j1=J1_global;
                        points[current_point].j2=J2_global;
                        points[current_point].j3=J3_global;
                        points[current_point].j4=J4_global;
                        points[current_point].j5=J5_global;
                        points[current_point].jiuli_baseline+=5;
                        YUYIN_FLAG = 6;
                    }
                } 
                else
                {
                    // 未运行：调整全局Z
                    Z += 5;
                    clamp_coordinates(&X, &Y, &Z);
                    update_range_status(X, Y, Z);

                }
                
                pitch_on_flag = 1;
            }
            break;
            case 0xf2:  // Z下降
                {
                    float old_Z;
                    
                    // 判断是否在运行状态
                    if (start_flag && !cycle_complete && point_count > 0) {
                        // 运行中：调整当前运行点的Z
                        // 运行中：调整当前运行点的Z
                        if((points[current_point].z -5)>Z_MIN)
                        {

                            points[current_point].z -= 5;
                            robot_arm_5dof_method2(points[current_point].x, points[current_point].y, points[current_point].z);
                            points[current_point].j1=J1_global;
                            points[current_point].j2=J2_global;
                            points[current_point].j3=J3_global;
                            points[current_point].j4=J4_global;
                            points[current_point].j5=J5_global;
                            points[current_point].jiuli_baseline-=5;
                            YUYIN_FLAG = 7;
                        }
                    } else {
                        // 未运行：调整全局Z
                        old_Z = Z;
                        Z -= 5;
                        clamp_coordinates(&X, &Y, &Z);
                        update_range_status(X, Y, Z);
                    }
                    
                    pitch_on_flag = 1;
                }
                break;
            case 0xf4: 
                X -= 5; 
                clamp_coordinates(&X, &Y, &Z);  // 新增
                update_range_status(X, Y, Z);
                pitch_on_flag=1; 
                break;
            case 0xf3: 
                X += 5; 
                clamp_coordinates(&X, &Y, &Z);  // 新增
                update_range_status(X, Y, Z);
                pitch_on_flag=1; 
                break;
            case 0xf5: 
                Y += 5; 
                clamp_coordinates(&X, &Y, &Z);  // 新增：限制坐标
                update_range_status(X, Y, Z);   // 已有：更新LED状态
                pitch_on_flag=1; 
                break;
            case 0xf6: 
                Y -= 5; 
                clamp_coordinates(&X, &Y, &Z);  // 新增
                update_range_status(X, Y, Z);
                pitch_on_flag=1; 
                break;
            case 0xF7: cebai_angle -= 10; Control_Motor(cebai_angle, 4); pitch_on_flag=1; break;
            case 0xf8: cebai_angle += 10; Control_Motor(cebai_angle, 4); pitch_on_flag=1; break;
            case 0xF9: RUN_ZERO();  pitch_on_flag=1; first_imu_flag=1; break;
            case 0xFA: RUN_START(); pitch_on_flag=0; break;
            case 0xFB:  // 标记点
                if (!start_flag && point_count < MAX_POINTS && first_imu_flag2&&biji_flag)
                {
                    // 检查坐标是否在范围内
                    if (!check_coordinate_range(X, Y, Z)) {
                        break;
                    }
                    
                    RGB_LED_MODE = 7;
                    
                    points[point_count].x = X;
                    points[point_count].y = Y;
                    points[point_count].z = Z;
                    points[point_count].position = 0;
                    points[point_count].point_timer = 0;
                    points[point_count].point_duration = point_duration;
                    
                    // 新增：记录标记时的基准距离
                    points[point_count].jiuli_baseline = filtered_tof();
                    
                    // 保存当前的关节角度
                    points[point_count].j1 = J1_global;
                    points[point_count].j2 = J2_global;
                    points[point_count].j3 = J3_global;
                    points[point_count].j4 = J4_global;
                    points[point_count].j5 = J5_global;
                    
                    point_count++;
                    
                    printf("标记点%d: (%.1f, %.1f, %.1f), 运行时间=%d分钟, 基准距离=%.1fcm\r\n",
                        point_count, X, Y, Z, (int)(point_duration/60000), 
                        points[point_count-1].jiuli_baseline);
                    
                    // 计算并打印标记点的角度
                    robot_arm_5dof_method2(X, Y, Z);
                    printf("标记点%d初始角度: ", point_count);
                    print_joint_angles();
                }
                else if(point_count!=0&&first_imu_flag2&&biji_flag==0)
                {
                    // 模式切换 (悬停->左右->上下->圆形->悬停)
                    if (mode_hover) {switch_mode(2);YUYIN_FLAG=3;}
                    else if (mode_lr) {switch_mode(3);YUYIN_FLAG=2;}
                    else if (mode_ud){switch_mode(1);YUYIN_FLAG=5;}
                    else if (mode_circle) {switch_mode(0);YUYIN_FLAG=4;}
                }
                break;

            case 0xfc:  // 启动/暂停按键
                if (point_count > 0)
                {
                    if (start_flag)  // 当前正在运行 → 执行暂停
                    {
                        start_flag = 0;
                        timer_running_circle = 0;
                        timer_running_lr = 0;
                        timer_running_ud = 0;
                        circle_move_flag = 0;
                        lr_move_flag = 0;
                        ud_move_flag = 0;
                        
                        RGB_LED_MODE = 1;  // 绿色闪烁表示暂停
                        RGB_LED_MODE_BACKUP = 1;
                        
                        printf("========== 运行已暂停 ==========\r\n");
                        printf("当前停留在标记点%d，已运行%.1f秒\r\n", 
                            current_point + 1, 
                            points[current_point].point_timer / 1000.0f);
                        YUYIN_FLAG = 8;  // 播放暂停提示音
                    }
                    else  // 当前未运行 → 执行启动或恢复
                    {
                        if (cycle_complete && first_imu_flag2)
                        {
                            // 情况1：循环结束后重新开始
                            biji_flag = 0;
                            flame_on;
                            flame_flag = 1;
                            YUYIN_FLAG = 1;
                            RGB_LED_MODE = 3;
                            RGB_LED_MODE_BACKUP = 3;
                            
                            printf("重新开始运行，准备回到第一个标记点...\r\n");
                            current_point = 0;
                            cycle_complete = 0;
                            for (int i = 0; i < point_count; i++) {
                                points[i].point_timer = 0;
                                points[i].position = 0;
                            }
                            ud_traj_step = 0;
                            ud_interp_count = 0;
                            ud_current_t = 0.0f;
                            
                            start_return_delay(3, 0);  // 类型3：重新开始
                        }
                        else if (current_point == 0 && points[0].point_timer == 0)
                        {
                            // 情况2：首次启动
                            biji_flag = 0;
                            flame_on;
                            flame_flag = 1;
                            YUYIN_FLAG = 1;
                            RGB_LED_MODE = 3;
                            RGB_LED_MODE_BACKUP = 3;
                            
                            printf("首次启动运行，准备回到第一个标记点...\r\n");
                            start_return_delay(4, 0);  // 类型4：首次启动
                        }
                        else
                        {
                            // 情况3：从暂停恢复运行（新增）
                            RGB_LED_MODE = 3;
                            RGB_LED_MODE_BACKUP = 3;
                            
                            printf("========== 从暂停恢复运行 ==========\r\n");
                            printf("继续标记点%d，剩余时间%.1f秒\r\n", 
                                current_point + 1,
                                (points[current_point].point_duration - points[current_point].point_timer) / 1000.0f);
                            
                            start_flag = 1;
                            timer_running_circle = mode_circle;
                            timer_running_lr = mode_lr;
                            timer_running_ud = mode_ud;
                            YUYIN_FLAG = 1;  // 播放启动提示音
                        }
                    }
                }
                break;

            case 0xFD: RUN_TIME_ADD(); break;//时间加
            case 0xFE: RUN_TIME_MINUS(); break;
            case 0xE1: 
                start_flag=0;
                biji_flag=1; //(允许重新标记)
                /********** 1. 停止所有运动与计时器 **********/
                timer_running_circle = 0;  // 停止圆形模式计时器
                timer_running_lr = 0;      // 停止左右模式计时器
                timer_running_ud = 0;      // 停止上下模式计时器
                circle_move_flag = 0;      // 清除圆形运动触发
                lr_move_flag = 0;          // 清除左右运动触发
                ud_move_flag = 0;          // 清除上下运动触发
                return_in_progress = 0;    // 停止回位延时

                /********** 2. 重置标记点核心数据 **********/
                point_count = 0;  // 已标记点数量清零
                // 遍历重置所有标记点数据（避免残留脏数据）
                for (int i = 0; i < MAX_POINTS; i++) {
                    points[i].x = 0.0f;
                    points[i].y = 0.0f;
                    points[i].z = 0.0f;
                    points[i].j1 = 0.0f;
                    points[i].j2 = 0.0f;
                    points[i].j3 = 0.0f;
                    points[i].j4 = 0.0f;
                    points[i].j5 = 0.0f;
                    points[i].point_timer = 0;
                    points[i].point_duration = point_duration;  // 保留默认时长
                    points[i].position = 0;
                    points[i].jiuli_baseline = 0.0f;  // 基准距离清零
                }
                /********** 4. 恢复硬件与提示状态 **********/
                // 关闭点火、风扇、灯光（若运行中）
                flame_off;
                fan_off;
                light_off;
                // LED恢复初始模式（呼吸模式），同步备份
                RGB_LED_MODE = 6;
                RGB_LED_MODE_BACKUP = 6;
                printf("========== 所有标记点已清空！==========\r\n");

             break;//清空标记点
            case 0xE2: correct_angle6+=10; Control_Motor(correct_angle6, 6);break;//电机6+10度
            case 0xE3: correct_angle6-=10; Control_Motor(correct_angle6, 6);break;//电机6-10度
            case 0xE4: correct_angle6+=1; Control_Motor(correct_angle6, 6);break;//电机6+1度
            case 0xE5: correct_angle6-=1; Control_Motor(correct_angle6, 6);break;//电机6-1度
            case 0xE6: correct_angle1+=10; Control_Motor(correct_angle1, 1);break;//电机6+10度
            case 0xE7: correct_angle1-=10; Control_Motor(correct_angle1, 1);break;//电机6-10度
            case 0xE8: correct_angle1+=1; Control_Motor(correct_angle1, 1);break;//电机6+1度
            case 0xE9: correct_angle1-=1; Control_Motor(correct_angle1, 1);break;//电机6-1度
            default: break;
        }

        // 红外遥控处理
        switch (ir_command)
        {
            case 0x1c: 
                Y += 5; 
                clamp_coordinates(&X, &Y, &Z);  // 新增：限制坐标
                update_range_status(X, Y, Z);   // 已有：更新LED状态
                pitch_on_flag=1; 
                break;
                
            case 0x55: 
                Y -= 5; 
                clamp_coordinates(&X, &Y, &Z);  // 新增
                update_range_status(X, Y, Z);
                pitch_on_flag=1; 
                break;
                
            case 0x59: 
                X -= 5; 
                clamp_coordinates(&X, &Y, &Z);  // 新增
                update_range_status(X, Y, Z);
                pitch_on_flag=1; 
                break;
                
            case 0x18: 
                X += 5; 
                clamp_coordinates(&X, &Y, &Z);  // 新增
                update_range_status(X, Y, Z);
                pitch_on_flag=1; 
                break;
                
            case 0x47:  // Z上升
            {
                
                
                // 判断是否在运行状态
                if (start_flag && !cycle_complete && point_count > 0) 
                {
                    // 运行中：调整当前运行点的Z
                    if((points[current_point].z +5)<Z_MAX)
                    {
                        points[current_point].z += 5;
                        robot_arm_5dof_method2(points[current_point].x, points[current_point].y, points[current_point].z);
                        points[current_point].j1=J1_global;
                        points[current_point].j2=J2_global;
                        points[current_point].j3=J3_global;
                        points[current_point].j4=J4_global;
                        points[current_point].j5=J5_global;
                        points[current_point].jiuli_baseline+=5;
                        YUYIN_FLAG = 6;
                    }
                } 
                else
                {
                    // 未运行：调整全局Z
                    Z += 5;
                    clamp_coordinates(&X, &Y, &Z);
                    update_range_status(X, Y, Z);

                }
                
                pitch_on_flag = 1;
            }
            break;
            
        case 0x5d:  // Z下降
            {
                // 判断是否在运行状态
                if (start_flag && !cycle_complete && point_count > 0) {
                    // 运行中：调整当前运行点的Z
                    // 运行中：调整当前运行点的Z
                    if((points[current_point].z -5)>Z_MIN)
                    {

                        points[current_point].z -= 5;
                        robot_arm_5dof_method2(points[current_point].x, points[current_point].y, points[current_point].z);
                        points[current_point].j1=J1_global;
                        points[current_point].j2=J2_global;
                        points[current_point].j3=J3_global;
                        points[current_point].j4=J4_global;
                        points[current_point].j5=J5_global;
                        points[current_point].jiuli_baseline-=5;
                        YUYIN_FLAG = 7;
                    }
                } else {
                    // 未运行：调整全局Z
     
                    Z -= 5;
                    clamp_coordinates(&X, &Y, &Z);
                    update_range_status(X, Y, Z);
                }
                
                pitch_on_flag = 1;
            }
            break;
                
            case 0x44: cebai_angle -= 10; Control_Motor(cebai_angle, 4); pitch_on_flag=1; break;
            case 0x5c: cebai_angle += 10; Control_Motor(cebai_angle, 4); pitch_on_flag=1; break;

            case 0x15:  // 标记点
                if (!start_flag && point_count < MAX_POINTS && first_imu_flag2&&biji_flag)
                {
                    // 检查坐标是否在范围内
                    if (!check_coordinate_range(X, Y, Z)) {
                        break;
                    }
                    
                    RGB_LED_MODE = 7;
                    
                    points[point_count].x = X;
                    points[point_count].y = Y;
                    points[point_count].z = Z;
                    points[point_count].position = 0;
                    points[point_count].point_timer = 0;
                    points[point_count].point_duration = point_duration;
                    
                    // 新增：记录标记时的基准距离
                    points[point_count].jiuli_baseline = filtered_tof();
                    
                    // 保存当前的关节角度
                    points[point_count].j1 = J1_global;
                    points[point_count].j2 = J2_global;
                    points[point_count].j3 = J3_global;
                    points[point_count].j4 = J4_global;
                    points[point_count].j5 = J5_global;
                    
                    point_count++;
                    
                    printf("标记点%d: (%.1f, %.1f, %.1f), 运行时间=%d分钟, 基准距离=%.1fcm\r\n",
                        point_count, X, Y, Z, (int)(point_duration/60000), 
                        points[point_count-1].jiuli_baseline);
                    
                    // 计算并打印标记点的角度
                    robot_arm_5dof_method2(X, Y, Z);
                    printf("标记点%d初始角度: ", point_count);
                    print_joint_angles();
                }
                else if(point_count!=0&&first_imu_flag2&&biji_flag==0)
                {
                    // 模式切换 (悬停->左右->上下->圆形->悬停)
                    if (mode_hover) {switch_mode(2);YUYIN_FLAG=3;}
                    else if (mode_lr) {switch_mode(3);YUYIN_FLAG=2;}
                    else if (mode_ud){switch_mode(1);YUYIN_FLAG=5;}
                    else if (mode_circle) {switch_mode(0);YUYIN_FLAG=4;}
                }
                break;

            case 0x45:  // 启动/暂停按键
                if (point_count > 0)
                {
                    if (start_flag)  // 当前正在运行 → 执行暂停
                    {
                        start_flag = 0;
                        timer_running_circle = 0;
                        timer_running_lr = 0;
                        timer_running_ud = 0;
                        circle_move_flag = 0;
                        lr_move_flag = 0;
                        ud_move_flag = 0;
                        
                        RGB_LED_MODE = 1;
                        RGB_LED_MODE_BACKUP = 1;
                        
                        printf("========== 运行已暂停 ==========\r\n");
                        printf("当前停留在标记点%d，已运行%.1f秒\r\n", 
                            current_point + 1, 
                            points[current_point].point_timer / 1000.0f);
                        YUYIN_FLAG = 8;
                    }
                    else  // 当前未运行 → 执行启动或恢复
                    {
                        if (cycle_complete && first_imu_flag2)
                        {
                            biji_flag = 0;
                            flame_on;
                            flame_flag = 1;
                            YUYIN_FLAG = 1;
                            RGB_LED_MODE = 3;
                            RGB_LED_MODE_BACKUP = 3;
                            
                            printf("重新开始运行，准备回到第一个标记点...\r\n");
                            current_point = 0;
                            cycle_complete = 0;
                            for (int i = 0; i < point_count; i++) {
                                points[i].point_timer = 0;
                                points[i].position = 0;
                            }
                            ud_traj_step = 0;
                            ud_interp_count = 0;
                            ud_current_t = 0.0f;
                            
                            start_return_delay(3, 0);
                        }
                        else if (current_point == 0 && points[0].point_timer == 0)
                        {
                            biji_flag = 0;
                            flame_on;
                            flame_flag = 1;
                            YUYIN_FLAG = 1;
                            RGB_LED_MODE = 3;
                            RGB_LED_MODE_BACKUP = 3;
                            
                            printf("首次启动运行，准备回到第一个标记点...\r\n");
                            start_return_delay(4, 0);
                        }
                        else
                        {
                            // 从暂停恢复运行
                            RGB_LED_MODE = 3;
                            RGB_LED_MODE_BACKUP = 3;
                            
                            printf("========== 从暂停恢复运行 ==========\r\n");
                            printf("继续标记点%d，剩余时间%.1f秒\r\n", 
                                current_point + 1,
                                (points[current_point].point_duration - points[current_point].point_timer) / 1000.0f);
                            
                            start_flag = 1;
                            timer_running_circle = mode_circle;
                            timer_running_lr = mode_lr;
                            timer_running_ud = mode_ud;
                            YUYIN_FLAG = 1;
                        }
                    }
                }
                break;


            default: break;
        }
        ir_command = 0;

        // 回位延时处理
        if (return_in_progress && return_delay_flag)
        {
            return_delay_flag = 0;
            return_in_progress = 0;
            
            printf("延时完成！\r\n");
            
            switch (return_type)
            {
                case 1:  // 模式切换
                    printf("模式切换完成，重置当前标记点%d位置状态\r\n", current_point + 1);
                    points[current_point].position = 0;
                    // 重置上下模式状态
                    ud_traj_step = 0;
                    ud_interp_count = 0;
                    ud_current_t = 0.0f;
                    // 启动新模式计时器（悬停模式不需要计时器）
                    timer_running_circle = mode_circle;
                    timer_running_lr = mode_lr;
                    timer_running_ud = mode_ud;
                    break;
                    
                case 2:  // 循环结束
                    flame_off;
                    fan_off;
                    light_off;
                    printf("循环结束，已停留在最后标记点%d\r\n", point_count);
                    break;
                    
                case 3:  // 重新开始
                    printf("重新开始完成，从第一个标记点开始运行\r\n");
                    start_flag = 1;
                    timer_running_circle = mode_circle;
                    timer_running_lr = mode_lr;
                    timer_running_ud = mode_ud;
                    break;
                    
                case 4:  // 首次启动
                    printf("首次启动完成，从第一个标记点开始运行\r\n");
                    start_flag = 1;
                    timer_running_circle = mode_circle;
                    timer_running_lr = mode_lr;
                    timer_running_ud = mode_ud;
                    break;
                    
                case 5:  // 点切换
                    printf("点切换完成，开始标记点%d运行\r\n", current_point + 1);
                    points[current_point].position = 0;
                    points[current_point].point_timer = 0;
                    // 重置上下模式状态
                    ud_traj_step = 0;
                    ud_interp_count = 0;
                    ud_current_t = 0.0f;
                    // 继续运行
                    start_flag = 1;
                    timer_running_circle = mode_circle;
                    timer_running_lr = mode_lr;
                    timer_running_ud = mode_ud;
                    break;
            }
            
            return_type = 0;
        }

        // 主运行逻辑
        if (start_flag && !cycle_complete && point_count > 0)
        {
            // 检查当前点是否运行时间到
            if (points[current_point].point_timer >= points[current_point].point_duration)
            {
                current_point++;
                if (current_point >= point_count)
                {
                    flame_off;
                    fan_off;
                    light_off;
                    RGB_LED_MODE=0;
                    // 所有点运行完成
                    cycle_complete = 1;
                    start_flag = 0;
                    timer_running_circle = 0;
                    timer_running_lr = 0;
                    timer_running_ud = 0;
                    printf("========== 所有标记点运行完成！==========\r\n");
                    YUYIN_FLAG=8;
                    start_return_delay(2, point_count - 1);  // 类型2：循环结束
                }
                else
                {
                    // 切换到下一个点
                    printf("====== 标记点%d运行时间到，准备切换到点%d ======\r\n", 
                        current_point, current_point + 1);
                    
                    // 停止当前模式运动
                    start_flag = 0;
                    timer_running_circle = 0;
                    timer_running_lr = 0;
                    timer_running_ud = 0;
                    
                    start_return_delay(5, current_point);  // 类型5：点切换
                }
            }
            else
            {
                // 执行当前模式运动
                if (mode_hover) {
                    run_hover();  // 悬停模式
                }
                else if (mode_lr && lr_move_flag) {
                    run_lr();
                }
                else if (mode_circle && circle_move_flag) {
                    run_circle();
                }
                else if (mode_ud && ud_move_flag) {
                    run_ud();
                }
            }
        }

        // 传感器处理
        jiuli=filtered_tof();
        get_angle();
        // if(jiuli)
        // printf("picth:%f,%f,%f,%f,%f,%f\n",Pitch_angle,run_angle,jiuli,X,Y,Z);
        if(first_imu_flag)
            first_anglg=J5_global;
        
        if(time_delay_100ms&&pitch_on_flag&&first_imu_flag2)
        {
            time_delay_100ms=0;
            
            if(imu_time>4000)
                Control_Motor_new(first_anglg+PID_five_motor(Pitch_angle-6.5),6,220,0);
            else 
            {
                if(imu_time<100)
                    Control_Motor(first_anglg,6);
                imu_time+=100;
            }
        }

        // 手动控制
        if ((X != prev_X || Y != prev_Y || Z != prev_Z) && !start_flag)
        {
            if (first_imu_flag2)
            {
                first_imu_flag=0;
            }
            if(RGB_LED_MODE!=5)
            {
                RGB_LED_MODE=1;
                RGB_LED_MODE_BACKUP=1;  // 同步备份
            }
            first_imu_flag2=1;
            prev_X = X; prev_Y = Y; prev_Z = Z;
            robot_arm_5dof_method2(X, Y, Z);
            Control_Motor(90 - J1_global, 1);
            CAN_BLDC_POS_CONTROL(J2_global, 2);
            CAN_BLDC_POS_CONTROL(J3_global, 3);
            Control_Motor(J4_global, 5);

            n0 = (uint32_t)X;
            n1 = (uint32_t)Y;
            n2 = (uint32_t)Z;
            n3 = (uint32_t)J1_global;
            n4 = (uint32_t)J2_global;
            n5 = (uint32_t)J3_global;
            n6 = (uint32_t)J4_global;
            n7 = (uint32_t)J5_global;

            LCD_SendTask();
            // 打印手动控制时的角度
            printf("手动控制位置更新: ");
            print_joint_angles();
        }
        
    }
}