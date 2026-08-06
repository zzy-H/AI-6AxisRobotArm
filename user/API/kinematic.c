/*
 * 逆运动学 + 自动抓取模块
 *
 * 数据流：
 *   OpenCV 像素帧 → Pixel_To_Phys() → 物理坐标(dist, alpha)
 *   → Arm_Inverse() → 三个关节角
 *   → 写入 servo_target[]（由 Task_ServoCtrl 执行）
 *
 * 注意：本模块只计算和写目标数组，不直接驱动舵机。
 *       实际执行由 servo_target / stepper_target 的轮询任务完成，
 *       与手动模式共用一套控制链路。
 */

/*****************************************************************
 *						机械臂 几何解析法 函数
 *	功能：根据末端执行器的轴坐标 以及末端执行器与x轴之间的夹角
 *		  来求出各个关节的角度
 *	传参：x,y末端执行器关节节点的坐标（轴的坐标）
 *	传参：phi,φ末端执行器与x轴之间的夹角
 *	整体思路：先求θ2：有几何关系可得：
 *					x^2 + y^2 = l1^2 + l2^2 - 2*l1*l2*cos(180-θ2)
 *			  再根据余弦定理求中间变量ψ  ，再求出θ1，继而求出θ3
 *****************************************************************
 */

#include "kinematic.h"
#include "main.h"       // Servo_IsMoving 到位检测
#include "uart.h"       // servo_target[] / stepper_target
#include "auto_disp.h"  // g_target_color（颜色门控判断）
#include "math.h"
#include "delay.h"     // Delay_() 延时函数
#include "FreeRTOS.h"  // vTaskDelay / pdMS_TO_TICKS
#include "task.h"
#include "stdio.h"     // printf
/* ==================== 像素 → 物理坐标 ==================== */
/**
 * @brief 像素坐标转换为物理坐标
 * @param px_w 框宽
 * @param px_h 框高
 * @param pos_x 像素x偏移
 * @param pos_y 像素y偏移
 * @param out 输出结构体指针，包含物理坐标(dist, alpha)
 */
void Pixel_To_Phys(uint16_t px_w, uint16_t px_h,
                   int16_t pos_x, int16_t pos_y,
                   Target_t *out)
{
    float x, y;

    /*
     * 坐标对应关系（对齐例程）：
     *   create_buffer 第4参数(x3=水平偏移) → buf[5:7] → 解析为 pos_y
     *   create_buffer 第5参数(y3=垂直偏移) → buf[7:9] → 解析为 pos_x
     *
     *   所以：
     *     pos_x = 物块垂直偏移（决定"距离"）
     *     pos_y = 物块水平偏移（决定"横向位置/角度"）
     *
     * 例程公式：
     *   x = (像素垂直偏移 / 框高) × 识别区高 + 基座到识别区底边距离
     *   y = (框宽/2 - 像素水平偏移) / 框宽 × 识别区宽
     */
    x = ((float)pos_x / px_h) * BG_HIGH + BASE_LEN;   // 距离方向
    y = ((float)px_w / 2.0f - pos_y) / px_w * BG_WIDTH; // 横向偏移

    /* 直线距离 + 偏转角 */
    out->dist  = sqrtf(x * x + y * y);// 计算直线距离
    out->alpha = atan2f(y, x) * 180.0f / 3.14159265f;// 计算偏转角
    /* TODO: color 由调用者填充 */
}

/* ==================== 逆运动学（几何解析法） ==================== */

/**
 * @brief 逆运动学计算
 * @param x 末端执行器x坐标
 * @param y 末端执行器y坐标
 * @param phi 末端执行器与x轴夹角
 * @return 计算得到的关节角度结构体
 */
Arm_Angle_t Arm_Inverse(float x, float y, float phi)
{
    Arm_Angle_t angle = {0};// 初始化关节角度结构体
    float l1 = ARM_L1, l2 = ARM_L2;// 机械臂大臂和小臂长度
    float psi, theta1, theta2, theta3;// 中间变量和三个关节角度
    float phi_rad = phi * 3.14159265f / 180.0f;// 角度转弧度

    float dist_sq = x * x + y * y;// 末端执行器到基座的平方距离
    float l1_sq = l1 * l1;// 机械臂大臂长度的平方
    float l2_sq = l2 * l2;// 机械臂小臂长度的平方

    /* TODO: 按例程公式实现
     * 1. theta2 = PI - acos((l1²+l2²-x²-y²) / (2·l1·l2))
     * 2. psi = acos((l2²-x²-y²-l1²) / (-2·l1·sqrt(x²+y²)))
     * 3. theta1 = atan2(y,x) - psi
     * 4. theta3 = phi - theta1 - theta2
     * 5. 弧度转角度
     */
    //先求theta2
    /* acos 参数必须限制在 [-1, 1]：
     * 当物块距离超出机械臂可达范围（dist > l1+l2 或 dist < |l1-l2|）时，
     * 参数会越界，acosf 返回 NaN，导致后续角度全变 NaN、舵机乱转。
     * 钳位后角度会停在极限位置，至少不会产生非法值。
     */
    float cos_theta2 = (l1_sq + l2_sq - dist_sq) / (2.0f * l1 * l2);
    if (cos_theta2 > 1.0f)  cos_theta2 = 1.0f;
    if (cos_theta2 < -1.0f) cos_theta2 = -1.0f;
    theta2 = 3.14159265f - acosf(cos_theta2);

    //再求psi（同样钳位）
    float cos_psi = (l2_sq - dist_sq - l1_sq) / (-2.0f * l1 * sqrtf(dist_sq));
    if (cos_psi > 1.0f)  cos_psi = 1.0f;
    if (cos_psi < -1.0f) cos_psi = -1.0f;
    psi = acosf(cos_psi);
    //再求theta1
    if (theta2 >= 0)
    {
        theta1 = atan2(y, x) - psi;	
    }
	else
    {
        theta1 = atan2(y, x) + psi;
    }
		
    //我们的手臂考虑不到theta1小于零的情况
	if (theta1 <= 0)
	{
		theta2 = -theta2;
		theta1 = atan2(y, x) + psi;
	}
    //再求theta3
    theta3 = phi_rad - theta1 - theta2;

    //弧度转角度
    theta1 = theta1 * 180.0f / 3.14159265f;
    theta2 = theta2 * 180.0f / 3.14159265f;
    theta3 = theta3 * 180.0f / 3.14159265f;

    angle.theta1 = theta1;
    angle.theta2 = theta2;
    angle.theta3 = theta3;
    return angle;
}

/**
 * @brief 按距离分段修正的逆运动学（参考六轴视觉AI机械臂工程）
 *
 * 纯数学逆运动学在真实机械臂上不落地（重力、安装偏差、舵机零位等），
 * 六轴工程在数学基础上按距离区间手工标定了 y 偏移和 phi 角，
 * 每个区间用不同的修正参数，这是实际调试出来的结果。
 *
 * @param x 物块距离 cm
 * @param y 高度（本函数忽略，用分段表内的高度修正）
 * @return 修正后的三关节角（theta1 已做 180- 转换，可直接写 servo_target）
 */
Arm_Angle_t Arm_Kinematic_2_Angle(float x, float y)
{
    Arm_Angle_t arm_angle = {0};
    (void)y;   // 用分段表内的高度修正，忽略传入值

    /* 距离修正：近处减 1cm（视觉/机械偏差补偿） */
    if (x < 23) x = x - 1;

    /* 分段查表（六轴工程实测参数）：
     * 每个区间：(距离修正, 高度y, 末端姿态角phi) */
    if (x < 12)
        arm_angle = Arm_Inverse(x, -9.0f, -70);
    else if (x < 14)
        arm_angle = Arm_Inverse(x - 1.05f, -8.0f, -95);
    else if (x < 15)
        arm_angle = Arm_Inverse(x - 1.2f, -6.75f, -95);
    else if (x < 16)
        arm_angle = Arm_Inverse(x - 1.5f, -5.5f, -95);
    else if (x < 17)
        arm_angle = Arm_Inverse(x, -3.75f, -95);
    else if (x < 19)
        arm_angle = Arm_Inverse(x - 1.0f, -2.0f, -60);
    else if (x <= 20)
        arm_angle = Arm_Inverse(x - 1.0f, 5.0f, -85);
    else if (x < 21)
        arm_angle = Arm_Inverse(x - 11.5f, -9.0f, -50);
    else if (x <= 23)
        arm_angle = Arm_Inverse(x - 13.5f, -9.0f, -50);
    else if (x <= 25)
        arm_angle = Arm_Inverse(x - 15.5f, -9.0f, -40);
    else
        arm_angle = Arm_Inverse(x - 6.0f, 1.0f, -100);

    /* 大臂角度取反（六轴工程：theta1 = 180 - theta1） */
    arm_angle.theta1 = 180.0f - arm_angle.theta1;

    return arm_angle;
}

/* ==================== 自动抓取状态机 ==================== */

/* 状态定义（参考例程 Arm_AutoMove） */
enum {
    ST_INIT,      // 0: 初始化姿态（回零）
    ST_ALIGN,     // 1: 对准目标（逆运动学 + 爪子张开）
    ST_GRAB,      // 2: 夹取（爪子闭合）
    ST_LIFT,      // 3: 拿起（大臂抬起，带物块转向放物区）
    ST_RELEASE,   // 4: 松开（爪子张开）
    ST_HOME       // 5: 回位（回到初始姿态）
};

/*
 * 例程姿态参数（Arm_AllAnge_Update 的角度）：
 *   初始姿态：大臂90 小臂90 腕部90 舵机2=100 爪子85(张开) 步进800(中心)
 *   对准姿态：大臂θ1' 小臂θ2' 腕部θ3' 舵机2=100 爪子85(张开) 步进800
 *   夹取姿态：爪子172(闭合)
 *   拿起姿态：大臂120 爪子170 步进400-(100×颜色)  ← 按颜色转放物区！
 *   松开姿态：爪子85(张开)
 *   回位姿态：同初始
 *
 * 适配本工程架构：
 *   舵机写 servo_target[]（Task_ServoCtrl 执行）
 *   步进写 stepper_target（Task_StepperCtrl 执行，相对步数）
 *   步进绝对位置 = 目标位置 - g_stepper_pos
 */

/* 全局停止标志：1=请求停止抓取（由"停止"按钮置位） */
volatile uint8_t g_auto_stop = 0;

/* 停止锁存：1=已完全停止，状态机停住不动，直到重新选择颜色才解除 */
volatile uint8_t g_auto_halt = 0;

/*
 * 等待所有舵机到位（带超时保护）
 * 取代固定 vTaskDelay：角度差小就快进，角度差大就等完，卡住不死等
 * timeout_ms：超时上限（默认 3000ms）
 */
static void Wait_Servo_Arrive(uint32_t timeout_ms)
{
    uint16_t timeout = 0;
    while (Servo_IsMoving() && timeout < timeout_ms / 20)
    {
        vTaskDelay(pdMS_TO_TICKS(20));
        timeout++;
    }
}

void Arm_AutoGrab(Target_t *target)
{
    static uint8_t state = ST_INIT;

    /* 颜色门控：没选颜色（255）→ 状态机停在初始，不执行
     * （独立任务上电就轮询，没有这个门控机械臂会开机自动抓） */
    if (g_target_color == 255)
    {
        state = ST_INIT;   // 保持初始状态
        return;
    }

    /* 停止锁存检查：完全停止状态时，什么都不做 */
    if (g_auto_halt)
    {
        /* 只有重新选择颜色（g_target_color 从 255 变为 0/1/2）才解除 */
        if (g_target_color == 255)
        {
            return;   // 仍处于停止状态，不执行
        }
        g_auto_halt = 0;   // 重新选了颜色 → 解除停止，继续执行
        state = ST_INIT;   // 从头开始
    }

    /* 停止检查：检测到停止请求 → 回初始姿态并完全停止 */
    if (g_auto_stop)
    {
        g_auto_stop = 0;    // 清除标志
        g_auto_halt = 1;    // 锁存停止状态
        state = ST_INIT;    // 状态机回到起点

        /* 回初始姿态（安全位置） */
        servo_target[J_SHOULDER] = 90;
        servo_target[J_ELBOW]    = 90;
        servo_target[J_WRIST]    = 90;
        servo_target[2]          = 100;
        servo_target[J_CLAW]     = 85;

        /* 步进回零点 */
        if (g_stepper_pos != 0)
        {
            stepper_target = -g_stepper_pos;
        }

        printf("已停止\r\n");
        return;             // 停住，不执行状态机
    }

    switch (state)
    {
    case ST_INIT:
        /* 初始姿态：所有关节回零，爪子张开 */
        servo_target[J_SHOULDER] = 90;
        servo_target[J_ELBOW]    = 90;
        servo_target[J_WRIST]    = 90;
        servo_target[2]          = 100;   // 腕部旋转舵机（例程固定 100）
        servo_target[J_CLAW]     = 85;    // 爪子张开

        /*
         * 步进回零点：上电时 g_stepper_pos=0 即正前方（校准点），
         * 已在零点则一步不走；不在零点才反向走回。
         * 注意：不能用例程的 800 中心，那是例程自己的行程定义。
         */
        if (g_stepper_pos != 0)
        {
            stepper_target = -g_stepper_pos;
        }

        Wait_Servo_Arrive(3000);   // 等舵机回初始姿态（替代固定延时）
        state = ST_ALIGN;
        break;

    case ST_ALIGN:
        /* 三道检查：颜色匹配 + 数据新鲜 + 距离范围，任一不满足回初始等待 */

        /* 检查1：颜色是否仍匹配（物块可能被换成别的颜色） */
        if (target->color != g_target_color)
        {
            state = ST_INIT;
            break;
        }

        /* 检查2：数据是否新鲜（超过2秒没收到新帧 → 物块可能被移走） */
        if (xTaskGetTickCount() - g_last_frame_tick > pdMS_TO_TICKS(2000))
        {
            state = ST_INIT;
            break;
        }

        /* 检查3：距离是否在可达范围（防垃圾角度） */
        if (target->dist < 5.0f || target->dist > 35.0f)
        {
            state = ST_INIT;
            break;
        }

        /* 逆运动学：按距离分段修正（六轴工程实测参数） */
        {
            Arm_Angle_t a = Arm_Kinematic_2_Angle(target->dist, 3.0f);

            /* 写入目标角度（分段函数已做 180-θ1 转换） */
            servo_target[J_SHOULDER] = (uint16_t)a.theta1;
            servo_target[J_ELBOW]    = (uint16_t)a.theta2;
            servo_target[J_WRIST]    = (uint16_t)a.theta3;
            servo_target[2]          = 100;
            servo_target[J_CLAW]     = 85;   // 爪子保持张开

            printf("对准: 大臂%d 小臂%d 腕部%d\r\n",
                   (int)servo_target[J_SHOULDER],
                   (int)servo_target[J_ELBOW],
                   (int)servo_target[J_WRIST]);
        }

        /*
         * 步进对准偏转角（alpha→步数）
         * 标定：4 细分下每度 = 200×4/360 = 2.22 步
         * 负号：步进方向与 alpha 正负相反（实测修正）
         */
        stepper_target = (int32_t)(-target->alpha * 2.22f);

        Wait_Servo_Arrive(5000);   // 等舵机转到目标角度（替代固定延时）
        state = ST_GRAB;
        break;

    case ST_GRAB:
        /* 爪子闭合夹取 */
        servo_target[J_CLAW] = 172;   // 例程夹取角度
        Wait_Servo_Arrive(2000);   // 等爪子闭合（替代固定延时）
        state = ST_LIFT;
        break;

    case ST_LIFT:
        /* 拿起：大臂抬起，步进转向放物区（按颜色不同位置） */
        servo_target[J_SHOULDER] = 120;
        servo_target[J_CLAW]     = 170;   // 保持夹紧

        /*
         * 放物区位置（例程假设行程 0~800：红→400 绿→300 蓝→200）。
         * 你的零点=正前方，放物区实际位置需按机械臂结构标定，
         * 例如放物区在左侧 300 步 → 目标 = -300。
         * 负号：步进方向与例程相反（实测修正，同 ST_ALIGN）。
         */
        stepper_target = -((400 - (int32_t)(100 * target->color)) - g_stepper_pos);

        Wait_Servo_Arrive(3000);   // 等大臂抬起（替代固定延时）
        state = ST_RELEASE;
        break;

    case ST_RELEASE:
        /* 松开：爪子张开 */
        servo_target[J_CLAW] = 85;
        Wait_Servo_Arrive(2000);   // 等爪子张开（替代固定延时）
        state = ST_HOME;
        break;

    case ST_HOME:
        /* 回位：回到初始姿态 */
        servo_target[J_SHOULDER] = 90;
        servo_target[J_ELBOW]    = 90;
        servo_target[J_WRIST]    = 90;
        servo_target[J_CLAW]     = 85;

        /* 步进回零点（同 ST_INIT 逻辑） */
        if (g_stepper_pos != 0)
        {
            stepper_target = -g_stepper_pos;
        }

        Wait_Servo_Arrive(3000);   // 等舵机回位（替代固定延时）
        state = ST_INIT;   // 回到初始，等待下一次识别
        break;
    }
}
