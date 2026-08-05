#ifndef __KINEMATIC_H_
#define __KINEMATIC_H_

#include "stm32f10x.h"

/* ==================== 机械臂物理参数（cm） ==================== */
/* 根据实际机械臂量取，例程参考值：l1=10.5, l2=10, l3=16 */
#define ARM_L1   10.5f    // 大臂长度 cm
#define ARM_L2   10.0f    // 小臂长度 cm
#define ARM_L3   16.0f    // 末端（爪子）长度 cm

/* ==================== 识别区物理参数（cm） ==================== */
/* 例程参考值，实际需要根据定位纸尺寸重新标定 */
#define BG_WIDTH  24.0f   // 识别区宽 cm
#define BG_HIGH   16.0f   // 识别区高 cm
#define BASE_LEN  11.0f   // 基座到识别区底边的距离 cm

/* ==================== 舵机编号 ==================== */
#define J_SHOULDER  5     // 大臂舵机（对应 servo_target[5]）
#define J_ELBOW     4     // 小臂舵机（对应 servo_target[4]）
#define J_WRIST     3     // 腕部舵机（对应 servo_target[3]）
#define J_CLAW      1     // 爪子舵机（对应 servo_target[1]）

/* ==================== 逆运动学结果 ==================== */
typedef struct {
    float theta1;   // 大臂角（度）
    float theta2;   // 小臂角（度）
    float theta3;   // 腕部角（度）
} Arm_Angle_t;

/* ==================== 目标信息（物块） ==================== */
typedef struct {
    float dist;     // 物块到基座的直线距离 cm
    float alpha;    // 物块相对基座正前方的偏转角（度）
    uint8_t color;  // 颜色：0红 1绿 2蓝
} Target_t;

/* 停止标志：1=请求停止抓取（由"停止"按钮置位，Arm_AutoGrab 检测） */
extern volatile uint8_t g_auto_stop;

/* 停止锁存：1=已完全停止，状态机停住，重新选颜色才解除 */
extern volatile uint8_t g_auto_halt;

/* 最后收到有效帧的时刻（tick），用于判断物块数据是否新鲜 */
extern volatile uint32_t g_last_frame_tick;

/* ==================== 函数声明 ==================== */

/**
 * @brief 像素坐标 → 物理坐标
 * @param px_w     AruCo 框像素宽度
 * @param px_h     AruCo 框像素高度
 * @param pos_x    物块 X 偏移（像素）
 * @param pos_y    物块 Y 偏移（像素）
 * @param out      输出：距离 + 偏转角
 */
void Pixel_To_Phys(uint16_t px_w, uint16_t px_h,
                   int16_t pos_x, int16_t pos_y,
                   Target_t *out);

/**
 * @brief 逆运动学：物块坐标 → 三个关节角
 * @param x    物块水平距离 cm
 * @param y    物块高度 cm（可传 0）
 * @param phi  末端姿态角（度，例程用 -95 左右）
 * @return     三个关节角
 */
Arm_Angle_t Arm_Inverse(float x, float y, float phi);

/**
 * @brief 按距离分段修正的逆运动学（六轴工程实测参数）
 * @param x 物块距离 cm
 * @param y 高度（忽略，用分段表内修正）
 * @return 修正后的关节角（theta1 已做 180- 转换）
 */
Arm_Angle_t Arm_Kinematic_2_Angle(float x, float y);

/**
 * @brief 自动抓取状态机（在任务中循环调用）
 * @param target 目标信息
 */
void Arm_AutoGrab(Target_t *target);

#endif
