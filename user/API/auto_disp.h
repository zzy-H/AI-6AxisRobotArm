#ifndef __AUTO_DISP_H_
#define __AUTO_DISP_H_

#include "stm32f10x.h"

/* 显示区域尺寸（示意区在屏幕上的大小） */
#define DISP_AREA_W   320    // 示意区宽
#define DISP_AREA_H   240    // 示意区高

/* 颜色定义：与 OpenCV 端一致（0=红 1=绿 2=蓝） */
#define COLOR_RED    0
#define COLOR_GREEN  1
#define COLOR_BLUE   2

/* 目标颜色（例程逻辑）：255=未选择不抓取，0=红，1=绿，2=蓝 */
extern volatile uint8_t g_target_color;

/**
 * @brief 创建示意控件（进入自动模式页时调用一次）
 *
 * 只创建控件不更新内容，之后每次收到新帧调 AutoDisp_Update 更新。
 * 注意：必须在 screen_2 已创建（setup_scr_screen_2 执行后）再调用，
 *       否则控件会创建到错误的活动屏幕下。
 */
void AutoDisp_Init(void);

/**
 * @brief 更新示意显示（收到新帧后调用）
 * @param width      AruCo 框原始宽度（像素，<=0 视为无效数据）
 * @param height     AruCo 框原始高度（像素，<=0 视为无效数据）
 * @param pos_x      物块相对框左上角的 X 偏移（像素，可为负）
 * @param pos_y      物块相对框左上角的 Y 偏移（像素，可为负）
 * @param color_type 物块颜色：0=红 1=绿 2=蓝
 */
void AutoDisp_Update(int width, int height, int pos_x, int pos_y, uint8_t color_type);
void AutoDisp_Reset(void);   // 控件指针置 NULL（离开页面时调用）
#endif
