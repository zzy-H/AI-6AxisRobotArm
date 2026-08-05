/*
 * 自动模式 LCD 示意显示模块
 *
 * 功能：
 *   在自动模式页（screen_2）上用 LVGL 控件画出"摄像头识别结果"的示意：
 *     - 绿色边框矩形 = AruCo 定位框（按比例缩放到示意区）
 *     - 彩色小方块   = 物块位置（颜色 = OpenCV 识别的颜色）
 *     - 文字标签     = 显示框尺寸、缩放比例、物块偏移
 *
 * 为什么不用真实图像：
 *   9600bps 串口传不了图像（一帧 800×480 需 13 分钟），
 *   所以只传输 10 字节坐标帧，LCD 端按坐标绘制示意，这是工业视觉的常见做法。
 *
 * 坐标系说明：
 *   OpenCV 端发送的 pos_x/pos_y 是"物块相对 AruCo 框左上角"的像素偏移，
 *   可以为负（物块在框外）。示意区按 scale 缩放后同比例绘制。
 *
 * 注意：本文件使用 LVGL v7.11 API（lv_obj_set_style_local_xxx 带 part/state 参数），
 *       不要混用 v8 的 lv_obj_set_style_xxx(obj, val, 0) 写法，会编译报错。
 */

#include "auto_disp.h"
#include "lvgl.h"
#include "gui_guider.h"   // guider_ui 全局结构体（screen_2 控件挂载点）
#include "uart.h"
#include "stdio.h"
#include "kinematic.h"    // g_auto_stop 停止标志

/* ==================== 全局变量 ==================== */
/* 目标颜色（例程逻辑）：255=未选择不抓取，0=红，1=绿，2=蓝 */
volatile uint8_t g_target_color = 255;

/* ==================== 静态控件指针 ==================== */
static lv_obj_t *cont_bg;     // 示意区背景容器（浅灰底）
static lv_obj_t *box_frame;   // AruCo 框（绿色边框、背景透明）
static lv_obj_t *dot_block;   // 物块色块（纯色小方块）
static lv_obj_t *label_info;  // 信息文字标签

/* ==================== 颜色选择按钮回调 ==================== */

/**
 * @brief "红色"按钮：选择/取消抓取红色物块（再点一次取消）
 */
static void btn_red_handler(lv_obj_t * obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        if (g_target_color == COLOR_RED)
        {
            g_target_color = 255;      // 再点一次 = 取消选择
            printf("已取消抓取\r\n");
        }
        else
        {
            g_target_color = COLOR_RED;
            printf("目标颜色: 红色\r\n");
        }
    }
}

/**
 * @brief "绿色"按钮：选择/取消抓取绿色物块（再点一次取消）
 */
static void btn_green_handler(lv_obj_t * obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        if (g_target_color == COLOR_GREEN)
        {
            g_target_color = 255;      // 再点一次 = 取消选择
            printf("已取消抓取\r\n");
        }
        else
        {
            g_target_color = COLOR_GREEN;
            printf("目标颜色: 绿色\r\n");
        }
    }
}

/**
 * @brief "蓝色"按钮：选择/取消抓取蓝色物块（再点一次取消）
 */
static void btn_blue_handler(lv_obj_t * obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        if (g_target_color == COLOR_BLUE)
        {
            g_target_color = 255;      // 再点一次 = 取消选择
            printf("已取消抓取\r\n");
        }
        else
        {
            g_target_color = COLOR_BLUE;
            printf("目标颜色: 蓝色\r\n");
        }
    }
}

/**
 * @brief "停止"按钮：立即停止抓取（置停止标志）
 *
 * 停止后 Arm_AutoGrab 检测到标志会重置状态机并回初始姿态。
 * 同时取消颜色选择（g_target_color=255），避免停止后继续触发抓取。
 */
static void btn_stop_handler(lv_obj_t * obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        g_auto_stop = 1;        // 置停止标志（Arm_AutoGrab 检测）
        g_target_color = 255;   // 取消颜色选择，防止再触发
        printf("STOP请求停止抓取\r\n");
    }
}

/**
 * @brief 在指定位置创建一个颜色按钮
 * @param parent  父对象（screen_2）
 * @param x,y     按钮位置
 * @param text    按钮文字
 * @param cb      点击回调
 */
static void create_color_btn(lv_obj_t *parent, lv_coord_t x, lv_coord_t y,
                             const char *text, lv_event_cb_t cb)
{
    lv_obj_t *btn = lv_btn_create(parent, NULL);
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_size(btn, 100, 50);
    lv_obj_set_event_cb(btn, cb);

    lv_obj_t *label = lv_label_create(btn, NULL);
    lv_label_set_text(label, text);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
}

/**
 * @brief 创建示意控件（进入自动模式页时调用一次）
 *
 * 控件只在首次进入时创建，之后只更新位置/大小/颜色。
 * 不要每次收到帧都重新创建——LVGL 动态创建控件会消耗堆内存，
 * 反复创建会导致 LV_MEM_SIZE 耗尽、系统崩溃。
 */
void AutoDisp_Init(void)
{
	lv_mem_monitor_t mon;
	lv_mem_monitor(&mon);
	printf("LVGL堆剩余: %d\r\n", (int)mon.free_size);
	
	printf("AutoDisp_Init 执行\r\n");   // 看有没有执行
	if (cont_bg != NULL) return;   // 已创建（且未被删除）→ 跳过
	
    /*
     * 注意：不要用指针非空判断做防重复创建！
     * 返回主屏时 lv_scr_load_anim(..., true) 的 autodel 会删除 screen_2，
     * 旧控件随屏幕一起销毁，但 cont_bg 指针仍是旧地址（悬空指针）。
     * 再次进入时如果直接 return，控件永远不会重建 → 屏幕空白。
     * 所以这里每次都重新创建（旧屏幕已删，不会泄漏）。
     */
    lv_obj_t *scr = guider_ui.screen_2;   // 直接指定 screen_2

    /* ---- 1. 示意区背景容器：浅灰底，居中 ---- */
    cont_bg = lv_obj_create(scr, NULL);
    lv_obj_set_size(cont_bg, DISP_AREA_W, DISP_AREA_H);
    lv_obj_align(cont_bg, NULL, LV_ALIGN_CENTER, 0, 0);   // v7 写法：中间参数 NULL

    /* 浅灰色背景 */
    lv_obj_set_style_local_bg_color(cont_bg, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,
                                    lv_color_hex(0xE5E5E5));
    lv_obj_set_style_local_bg_opa(cont_bg, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,
                                  LV_OPA_COVER);
    /* 去掉默认边框，直角 */
    lv_obj_set_style_local_border_width(cont_bg, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_radius(cont_bg, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);

    /* ---- 2. AruCo 框：绿色边框矩形，背景透明，初始隐藏 ---- */
    box_frame = lv_obj_create(cont_bg, NULL);
    lv_obj_set_size(box_frame, 120, 120);            // 初始尺寸，Update 时按比例改
    lv_obj_align(box_frame, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);  // 左上角对齐容器原点

    /* 背景透明（只显示边框） */
    lv_obj_set_style_local_bg_opa(box_frame, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,
                                  LV_OPA_TRANSP);
    /* 绿色边框 3px */
    lv_obj_set_style_local_border_width(box_frame, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 3);
    lv_obj_set_style_local_border_color(box_frame, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,
                                        lv_color_hex(0x00CC00));
    lv_obj_set_style_local_radius(box_frame, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);

    lv_obj_set_hidden(box_frame, true);   // 初始隐藏（还没数据）

    /* ---- 3. 物块色块：纯色小方块，初始隐藏 ---- */
    dot_block = lv_obj_create(cont_bg, NULL);
    lv_obj_set_size(dot_block, 24, 24);              // 物块显示尺寸
    lv_obj_set_style_local_bg_color(dot_block, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,
                                    lv_color_hex(0x007AFF));  // 默认蓝色
    lv_obj_set_style_local_bg_opa(dot_block, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,
                                  LV_OPA_COVER);
    lv_obj_set_style_local_border_width(dot_block, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_radius(dot_block, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 2);

    lv_obj_set_hidden(dot_block, true);   // 初始隐藏

    /* ---- 4. 信息标签：顶部显示文字 ---- */
    label_info = lv_label_create(cont_bg, NULL);
    lv_label_set_text(label_info, "THINK...");
    lv_obj_align(label_info, NULL, LV_ALIGN_IN_TOP_MID, 0, 8);
    lv_obj_set_style_local_text_font(label_info, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT,
                                     &lv_font_montserrat_14);
    lv_obj_set_style_local_text_color(label_info, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT,
                                      lv_color_hex(0x333333));

    /* ---- 5. 颜色选择按钮：选择抓取目标颜色（例程逻辑） ---- */
    /* 屏幕下方并排 3 个按钮：红色 / 绿色 / 蓝色 */
    create_color_btn(scr, 180, 420, "RED", btn_red_handler);
    create_color_btn(scr, 350, 420, "GREEN", btn_green_handler);
    create_color_btn(scr, 520, 420, "BLUE", btn_blue_handler);

    /* 停止按钮：抓取执行中可随时停止 */
    create_color_btn(scr, 20, 420, "STOP", btn_stop_handler);
}

/**
 * @brief 更新示意显示（收到新帧后调用）
 *
 * 计算逻辑：
 *   1. scale = min(示意区宽/框宽, 示意区高/框高) —— 等比缩放，保证框完整显示
 *   2. 框的屏幕大小 = 原始尺寸 × scale
 *   3. 物块屏幕位置 = 物块偏移 × scale（相对框左上角）
 *   4. 颜色映射：0红 1绿 2蓝
 *
 * @param width      AruCo 框原始宽度（像素，<=0 视为无效）
 * @param height     AruCo 框原始高度（像素，<=0 视为无效）
 * @param pos_x      物块 X 偏移（像素，可为负）
 * @param pos_y      物块 Y 偏移（像素，可为负）
 * @param color_type 物块颜色：0=红 1=绿 2=蓝
 */
void AutoDisp_Update(int width, int height, int pos_x, int pos_y, uint8_t color_type)
{
	/* 控件未创建（还没进过自动模式页）→ 直接返回，不操作 LVGL */
    if (cont_bg == NULL || box_frame == NULL || dot_block == NULL)
    {
        return;
    }
	
    /* ---- 1. 无效数据：隐藏框和色块，只更新提示文字 ---- */
    if (width <= 0 || height <= 0)
    {
        lv_obj_set_hidden(box_frame, true);
        lv_obj_set_hidden(dot_block, true);
        lv_label_set_text(label_info, "未检测到有效目标");
        return;
    }

    /* ---- 2. 计算等比缩放系数 ---- */
    /* 取两个方向缩放中较小的那个，保证框不超出示意区 */
    float scale_w = (float)DISP_AREA_W / width;
    float scale_h = (float)DISP_AREA_H / height;
    float scale = (scale_w < scale_h) ? scale_w : scale_h;

    /* ---- 3. 更新 AruCo 框：显示 + 缩放尺寸 ---- */
    lv_obj_set_hidden(box_frame, false);
    lv_obj_set_pos(box_frame, 0, 0);   // 框左上角固定在示意区左上角
    lv_obj_set_size(box_frame,
                    (lv_coord_t)(width * scale),
                    (lv_coord_t)(height * scale));

    /* ---- 4. 更新物块位置：偏移 × 缩放，相对框左上角 ---- */
    lv_obj_set_hidden(dot_block, false);
    lv_coord_t block_x = (lv_coord_t)(pos_x * scale);
    lv_coord_t block_y = (lv_coord_t)(pos_y * scale);
    lv_obj_set_pos(dot_block, block_x, block_y);

    /* ---- 5. 更新物块颜色：0红 1绿 2蓝，其他值兜底为白色 ---- */
    lv_color_t block_color;
    switch (color_type)
    {
        case COLOR_RED:   block_color = lv_color_make(  0,   0, 255); break;  // 红
        case COLOR_GREEN: block_color = lv_color_make(  0, 255,   0); break;  // 绿
        case COLOR_BLUE:  block_color = lv_color_make(255,   0,   0); break;  // 蓝
        default:          block_color = lv_color_make(255, 255, 255); break;  // 未知 → 白
    }
    lv_obj_set_style_local_bg_color(dot_block, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,
                                    block_color);

    /* ---- 6. 更新信息标签 ---- */
    lv_label_set_text_fmt(label_info,
                          "%d×%d |%.2f |(%d,%d)",
                          width, height, scale, pos_x, pos_y);
}

void AutoDisp_Reset(void)
{
    cont_bg = NULL;
    box_frame = NULL;
    dot_block = NULL;
    label_info = NULL;
}
