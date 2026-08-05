/*
 * 事件回调文件
 *
 * 本文件由 GUI Guider 生成 + 手动补充。所有 UI 控件的交互逻辑写在这里。
 *
 * 控件命名规则（GUI Guider 自动生成）：
 *   screen_btn_1   = 主屏上的按钮
 *   screen_1_btn_5 = screen_1（手动控制页）上的按钮 5
 *   screen_1_slider_1 = screen_1 上的滑块 1
 *
 * 舵机控制原理：
 *   拖动滑块/点击按钮 → 更新 servo_target[] 数组
 *   → Task_ServoCtrl 任务轮询到变化 → 调用 Servo_SetAngle() 驱动舵机
 *
 * 引脚对应关系（5个舵机）：
 *   servo_target[1] → PA1(TIM2_CH2)  爪子开合
 *   servo_target[2] → PA2(TIM2_CH3)  轴2
 *   servo_target[3] → PA3(TIM2_CH4)  轴3
 *   servo_target[4] → PA6(TIM3_CH1)  轴4
 *   servo_target[5] → PA7(TIM3_CH2)  轴5
 *   stepper_target  → PB5(DIR)/PB6(STEP)  步进电机（底座旋转）
 */

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"
#include "servo.h"      // servo_target[] 舵机目标角度数组
#include "steeper.h"     // Stepper_Run() 步进电机控制
#include "uart.h"        // stepper_target 步进电机目标步数
#include "action.h"      // 动作组管理（保存/播放/删除/选组）
#include "auto_disp.h"   // 自动模式 LCD 示意显示

void events_init(lv_ui *ui)
{
    /* 总初始化入口，目前不需要额外操作 */
}


/* ===================================================================
 * 主屏（screen）控件回调
 * ===================================================================*/

/**
 * @brief 主屏按钮1："手动控制"
 * 点击后跳转到 screen_1（手动控制页）
 */
static void screen_btn_1event_handler(lv_obj_t * obj, lv_event_t event)
{
    switch (event)
    {
    case LV_EVENT_CLICKED:
    {
        /* 如果 screen_1 还没创建，先创建 */
        if (!lv_debug_check_obj_valid(guider_ui.screen_1))
            setup_scr_screen_1(&guider_ui);
        /* 切换到 screen_1 */
        lv_disp_t * d = lv_obj_get_disp(lv_scr_act());
        if (d->prev_scr == NULL && d->scr_to_load == NULL)
            lv_scr_load_anim(guider_ui.screen_1, LV_SCR_LOAD_ANIM_NONE, 1000, 1000, true);
    }
        break;
    default:
        break;
    }
}

/**
 * @brief 主屏按钮2："自动模式"
 * 点击后跳转到 screen_2（自动模式页）
 */
static void screen_btn_2event_handler(lv_obj_t * obj, lv_event_t event)
{
    switch (event)
    {
    case LV_EVENT_CLICKED:
    {
        if (!lv_debug_check_obj_valid(guider_ui.screen_2))
            setup_scr_screen_2(&guider_ui);
        lv_disp_t * d = lv_obj_get_disp(lv_scr_act());
        if (d->prev_scr == NULL && d->scr_to_load == NULL)
            lv_scr_load_anim(guider_ui.screen_2, LV_SCR_LOAD_ANIM_NONE, 1000, 1000, true);

        AutoDisp_Init();   // ← 切屏后再调，此时 lv_scr_act() 才是 screen_2
    }
        break;
    default:
        break;
    }
}

/**
 * @brief 注册主屏所有控件的事件回调
 * @param ui GUI Guider 生成的 UI 结构体指针
 */
void events_init_screen(lv_ui *ui)
{
    lv_obj_set_event_cb(ui->screen_btn_1, screen_btn_1event_handler);
    lv_obj_set_event_cb(ui->screen_btn_2, screen_btn_2event_handler);
}


/* ===================================================================
 * 手动控制页（screen_1）控件回调
 * ===================================================================*/

/* ---- 公共辅助函数 ---- */

/**
 * @brief 更新角度标签的显示文字
 * @param label 标签对象指针
 * @param value 角度值
 *
 * 所有 add/sub/滑块 操作后调此函数刷新显示
 */
static void update_label(lv_obj_t *label, int16_t value)
{
    /*
        函数：lv_label_set_text_fmt()
        作用：设置标签的文本为格式化后的字符串
        参数：
            label：标签对象指针
            fmt：格式化字符串，例如 "%d" 表示整数
            ...：可变参数，用于填充格式化字符串中的占位符
        返回值：无
    */
    lv_label_set_text_fmt(label, "%d", value);
}

/**
 * @brief 更新滑块位置（不触发动画）
 * @param slider 滑块对象指针
 * @param value 目标值
 */
static void update_slider(lv_obj_t *slider, int16_t value)
{
    lv_slider_set_value(slider, value, LV_ANIM_OFF);
}


/* ==================== 轴1：爪子（servo_target[1]）==================== */

/*
 * 控件映射：
 *   screen_1_slider_1  → 滑块，拖动控制角度 0~180
 *   screen_1_btn_5     → add 按钮，角度 +1%
 *   screen_1_btn_6     → sub 按钮，角度 -1%
 *   screen_1_label_8   → 显示当前角度值
 *   screen_1_label_2   → 显示汉字"爪子"（纯装饰，无逻辑）
 */

/**
 * @brief 百分比转角度
 * @param pct 百分比（0~100）
 * @return 角度（0~180）
 */
static int16_t pct_to_angle(int16_t pct)
{
    return (int16_t)((int32_t)pct * 180 / 100);
}

/**
 * @brief 角度转百分比
 * @param angle 角度（0~180）
 * @return 百分比（0~100）
 */
static int16_t angle_to_pct(int16_t angle)
{
    return (int16_t)((int32_t)angle * 100 / 180);
}

/**
 * @brief 爪子滑块回调
 *
 * 滑块范围 0~100（百分比），实际角度 = 百分比 × 180 / 100。
 * LV_EVENT_VALUE_CHANGED = 用户拖动滑块时触发
 */
static void slider_1_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if (event == LV_EVENT_VALUE_CHANGED)
    {
        /* 从滑块读取百分比（0~100） */
        int16_t pct = lv_slider_get_value(obj);

        /* 百分比 → 角度 */
        int16_t angle = pct_to_angle(pct);
        servo_target[1] = angle;

        /* 显示实际角度值 */
        update_label(guider_ui.screen_1_label_8, angle);
    }
}

/**
 * @brief 爪子 add 按钮回调
 *
 * 百分比 +1（相当于角度 +1.8°），上限 100%
 */
static void btn_5_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        /* 读取当前百分比 */
        int16_t pct = lv_slider_get_value(guider_ui.screen_1_slider_1);

        if (pct < 100)
        {
            pct++;                                      // 百分比+1
            int16_t angle = pct_to_angle(pct);          // 转角度
            servo_target[1] = angle;                    // 更新目标
            update_slider(guider_ui.screen_1_slider_1, pct);     // 滑块同步
            update_label(guider_ui.screen_1_label_8, angle);     // 标签同步
        }
    }
}

/**
 * @brief 爪子 sub 按钮回调
 *
 * 百分比 -1（相当于角度 -1.8°），下限 0%
 */
static void btn_6_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        int16_t pct = lv_slider_get_value(guider_ui.screen_1_slider_1);

        if (pct > 0)
        {
            pct--;
            int16_t angle = pct_to_angle(pct);
            servo_target[1] = angle;
            update_slider(guider_ui.screen_1_slider_1, pct);
            update_label(guider_ui.screen_1_label_8, angle);
        }
    }
}


/* ==================== 轴2（servo_target[2]）==================== */
/*
 * 控件映射：
 *   screen_1_slider_2  → 滑块
 *   screen_1_btn_7     → add 按钮
 *   screen_1_btn_8     → sub 按钮
 *   screen_1_label_9   → 角度值标签
 */
/* TODO: 添加 slider_2 / btn_7 / btn_8 的回调函数，格式参考爪子 */

/**
 * @brief 轴2 滑块回调
 *
 * 滑块范围 0~100（百分比），实际角度 = 百分比 × 180 / 100。
 * LV_EVENT_VALUE_CHANGED = 用户拖动滑块时触发
 */
void slider_2_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if (event == LV_EVENT_VALUE_CHANGED)//判断是否是拖动事件
    {
        int16_t pct = lv_slider_get_value(obj);//获取当前百分比
        int16_t angle = pct_to_angle(pct);//百分比转角度
        servo_target[2] = angle;//更新目标角度
        update_label(guider_ui.screen_1_label_9, angle);//更新标签
    }
}

/**
 * @brief 轴2 add 按钮回调
 *
 * 百分比 +1（相当于角度 +1.8°），上限 100%
 */
void btn_7_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)//判断是否是点击事件
    {
        int16_t pct = lv_slider_get_value(guider_ui.screen_1_slider_2);//获取当前百分比

        if (pct < 100)
        {
            pct++;//百分比+1
            int16_t angle = pct_to_angle(pct);//百分比转角度
            servo_target[2] = angle;//更新目标角度

            update_slider(guider_ui.screen_1_slider_2, pct);//更新滑块
            update_label(guider_ui.screen_1_label_9, angle);//更新标签
        }
    }
}

/**
 * @brief 轴2 sub 按钮回调
 *
 * 百分比 -1（相当于角度 -1.8°），下限 0%
 */
void btn_8_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)//判断是否是点击事件
    {
        int16_t pct = lv_slider_get_value(guider_ui.screen_1_slider_2);//获取当前百分比

        if (pct > 0)
        {
            pct--;//百分比-1
            int16_t angle = pct_to_angle(pct);//百分比转角度
            servo_target[2] = angle;//更新目标角度

            update_slider(guider_ui.screen_1_slider_2, pct);//更新滑块
            update_label(guider_ui.screen_1_label_9, angle);//更新标签
        }
    }
}

/* ==================== 轴3（servo_target[3]）==================== */
/*
 * 控件映射：
 *   screen_1_slider_3  → 滑块
 *   screen_1_btn_9     → add 按钮
 *   screen_1_btn_10    → sub 按钮
 *   screen_1_label_10  → 角度值标签
 */
/* TODO: 添加 slider_3 / btn_9 / btn_10 的回调函数 */

/**
 * @brief 轴3 滑块回调
 *
 * 滑块范围 0~100（百分比），实际角度 = 百分比 × 180 / 100。
 * LV_EVENT_VALUE_CHANGED = 用户拖动滑块时触发
 */
void slider_3_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if (event == LV_EVENT_VALUE_CHANGED)//判断是否是拖动事件
    {
        int16_t pct = lv_slider_get_value(obj);//获取当前百分比
        int16_t angle = pct_to_angle(pct);//百分比转角度
        servo_target[3] = angle;//更新目标角度
        update_label(guider_ui.screen_1_label_10, angle);//更新标签
    }
}

/**
 * @brief 轴3 add 按钮回调
 *
 * 百分比 +1（相当于角度 +1.8°），上限 100%
 */
void btn_9_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)//判断是否是点击事件
    {
        int16_t pct = lv_slider_get_value(guider_ui.screen_1_slider_3);//获取当前百分比

        if (pct < 100)
        {
            pct++;//百分比+1
            int16_t angle = pct_to_angle(pct);//百分比转角度
            servo_target[3] = angle;//更新目标角度

            update_slider(guider_ui.screen_1_slider_3, pct);//更新滑块
            update_label(guider_ui.screen_1_label_10, angle);//更新标签
        }
    }
}

/**
 * @brief 轴3 sub 按钮回调
 *
 * 百分比 -1（相当于角度 -1.8°），下限 0%
 */
void btn_10_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)//判断是否是点击事件
    {
        int16_t pct = lv_slider_get_value(guider_ui.screen_1_slider_3);//获取当前百分比

        if (pct > 0)
        {
            pct--;//百分比-1
            int16_t angle = pct_to_angle(pct);//百分比转角度
            servo_target[3] = angle;//更新目标角度

            update_slider(guider_ui.screen_1_slider_3, pct);//更新滑块
            update_label(guider_ui.screen_1_label_10, angle);//更新标签
        }
    }
}

/* ==================== 轴4（servo_target[4]）==================== */
/*
 * 控件映射：
 *   screen_1_slider_4  → 滑块
 *   screen_1_btn_11    → add 按钮
 *   screen_1_btn_12    → sub 按钮
 *   screen_1_label_11  → 角度值标签
 */
/* TODO: 添加 slider_4 / btn_11 / btn_12 的回调函数 */

/** 
 * @brief 轴4 滑块回调
 *
 * 滑块范围 0~100（百分比），实际角度 = 百分比 × 180 / 100。
 * LV_EVENT_VALUE_CHANGED = 用户拖动滑块时触发
 */
void slider_4_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if (event == LV_EVENT_VALUE_CHANGED)//判断是否是拖动事件
    {
        int16_t pct = lv_slider_get_value(obj);//获取当前百分比
        int16_t angle = pct_to_angle(pct);//百分比转角度
        servo_target[4] = angle;//更新目标角度
        update_label(guider_ui.screen_1_label_11, angle);//更新标签
    }
}

/**
 * @brief 轴4 add 按钮回调
 *
 * 百分比 +1（相当于角度 +1.8°），上限 100%
 */
void btn_11_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)//判断是否是点击事件
    {
        int16_t pct = lv_slider_get_value(guider_ui.screen_1_slider_4);//获取当前百分比

        if (pct < 100)
        {
            pct++;//百分比+1
            int16_t angle = pct_to_angle(pct);//百分比转角度
            servo_target[4] = angle;//更新目标角度

            update_slider(guider_ui.screen_1_slider_4, pct);//更新滑块
            update_label(guider_ui.screen_1_label_11, angle);//更新标签
        }
    }
}

/**
 * @brief 轴4 sub 按钮回调
 *
 * 百分比 -1（相当于角度 -1.8°），下限 0%
 */
void btn_12_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)//判断是否是点击事件
    {
        int16_t pct = lv_slider_get_value(guider_ui.screen_1_slider_4);//获取当前百分比

        if (pct > 0)
        {
            pct--;//百分比-1
            int16_t angle = pct_to_angle(pct);//百分比转角度
            servo_target[4] = angle;//更新目标角度

            update_slider(guider_ui.screen_1_slider_4, pct);//更新滑块
            update_label(guider_ui.screen_1_label_11, angle);//更新标签
        }
    }
}

/* ==================== 轴5（servo_target[5]）==================== */
/*
 * 控件映射：
 *   screen_1_slider_5  → 滑块
 *   screen_1_btn_13    → add 按钮
 *   screen_1_btn_14    → sub 按钮
 *   screen_1_label_12  → 角度值标签
 */
/* TODO: 添加 slider_5 / btn_13 / btn_14 的回调函数 */

/**
 * @brief 轴5 滑块回调
 *
 * 滑块范围 0~100（百分比），实际角度 = 百分比 × 180 / 100。
 * LV_EVENT_VALUE_CHANGED = 用户拖动滑块时触发
 */
void slider_5_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if (event == LV_EVENT_VALUE_CHANGED)//判断是否是拖动事件
    {
        int16_t pct = lv_slider_get_value(obj);//获取当前百分比
        int16_t angle = pct_to_angle(pct);//百分比转角度
        servo_target[5] = angle;//更新目标角度
        update_label(guider_ui.screen_1_label_12, angle);//更新标签
    }
}

/**
 * @brief 轴5 add 按钮回调
 *
 * 百分比 +1（相当于角度 +1.8°），上限 100%
 */
void btn_13_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)//判断是否是点击事件
    {
        int16_t pct = lv_slider_get_value(guider_ui.screen_1_slider_5);//获取当前百分比
        if (pct < 100)
        {
            pct++;//百分比+1
            int16_t angle = pct_to_angle(pct);//百分比转角度
            servo_target[5] = angle;//更新目标角度
            update_slider(guider_ui.screen_1_slider_5, pct);//更新滑块
            update_label(guider_ui.screen_1_label_12, angle);//更新标签
        }
    }
}

/**
 * @brief 轴5 sub 按钮回调
 *
 * 百分比 -1（相当于角度 -1.8°），下限 0%
 * LV_EVENT_CLICKED = 用户点击按钮时触发
 */
void btn_14_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)//判断是否是点击事件
    {
        int16_t pct = lv_slider_get_value(guider_ui.screen_1_slider_5);//获取当前百分比

        if (pct > 0)
        {
            pct--;//百分比-1
            int16_t angle = pct_to_angle(pct);//百分比转角度
            servo_target[5] = angle;//更新目标角度
            update_slider(guider_ui.screen_1_slider_5, pct);//更新滑块
            update_label(guider_ui.screen_1_label_12, angle);//更新标签
        }
    }
}

/* ==================== 轴6：步进电机（stepper_target）==================== */
/*
 * 控件映射：
 *   screen_1_slider_6  → 滑块，设置步数（0~2000）
 *   screen_1_btn_15    → 正转按钮（按 slider_6 的步数正转）
 *   screen_1_btn_16    → 反转按钮（按 slider_6 的步数反转）
 *   screen_1_label_13  → 显示当前步数值
 */

/**
 * @brief 步进电机步数滑块回调
 * 拖动滑块设置步进电机的步数，不立即执行，等按 btn_15/btn_16 才走
 */
static void slider_6_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if (event == LV_EVENT_VALUE_CHANGED)//判断是否是拖动滑块事件
    {
        int16_t val = lv_slider_get_value(obj);
        update_label(guider_ui.screen_1_label_13, val);
    }
}

/**
 * @brief 步进电机正转按钮回调
 * 按 slider_6 设定的步数正转
 */
static void btn_15_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        int16_t steps = lv_slider_get_value(guider_ui.screen_1_slider_6);
        if (steps > 0)
        {
            stepper_target = steps;     // 正转
        }
    }
}

/**
 * @brief 步进电机反转按钮回调
 * 按 slider_6 设定的步数反转
 */
static void btn_16_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        int16_t steps = lv_slider_get_value(guider_ui.screen_1_slider_6);
        if (steps > 0)
        {
            stepper_target = -steps;    // 反转（负值 = 反转）
        }
    }
}


/* ==================== 功能按钮回调 ==================== */

/**
 * @brief "保存"按钮回调（预留）
 * 将当前所有舵机角度保存到 Flash
 */
static void screen_1_btn_1event_handler(lv_obj_t * obj, lv_event_t event)
{
    switch (event)
    {
    case LV_EVENT_CLICKED:
    {
        /* TODO: 保存当前角度到 W25Q64 Flash */
        Action_Add(servo_target, g_stepper_pos);   // 存 5 路舵机角度 + 步进电机当前位置
        Action_SaveToFlash();          // 写入 Flash
    }
        break;
    default:
        break;
    }
}

/**
 * @brief "返回"按钮回调
 * 从手动控制页回到主菜单
 */
static void screen_1_btn_2event_handler(lv_obj_t * obj, lv_event_t event)
{
    switch (event)
    {
    case LV_EVENT_CLICKED:
    {
        if (!lv_debug_check_obj_valid(guider_ui.screen))
            setup_scr_screen(&guider_ui);
        lv_disp_t * d = lv_obj_get_disp(lv_scr_act());
        if (d->prev_scr == NULL && d->scr_to_load == NULL)
            lv_scr_load_anim(guider_ui.screen, LV_SCR_LOAD_ANIM_NONE, 1000, 1000, true);
    }
        break;
    default:
        break;
    }
}

/**
 * @brief "自动执行"按钮回调
 *
 * 第一次按：开始循环播放当前动作组。
 * 再按一次：停止播放，恢复手动控制。
 */
static void screen_1_btn_3event_handler(lv_obj_t * obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        if (is_playing)
        {
            Action_StopPlay();      // 正在播放 → 停止
            printf("end bofang\r\n");
        }
        else
        {
            Action_StartPlay();     // 未播放 → 开始
            printf("start bofang\r\n");
        }
    }
}

/**
 * @brief "删除"按钮回调
 *
 * 删除当前组最后一个动作，并同步到 Flash。
 */
static void screen_1_btn_4event_handler(lv_obj_t * obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        uint8_t count = (current_group == 0) ? action_count_group1 :
                        (current_group == 1) ? action_count_group2 :
                        action_count_group3;

        if (count > 0)
        {
            Action_Delete(count - 1);   // 删除最后一个
            Action_SaveToFlash();       // 同步到 Flash
        }
        else
        {
            printf("end组%d没有动作可删除\r\n", current_group + 1);
        }
    }
}

/**
 * @brief 下拉列表回调（选择动作组）
 *
 * 选项顺序对应 GUI Guider 里设置的 "list1\nlist2\n..."，
 * 选中索引 0→组1，1→组2，2→组3（后面的选项暂不启用）。
 */
static void screen_1_ddlist_1event_handler(lv_obj_t * obj, lv_event_t event)
{
    if (event == LV_EVENT_VALUE_CHANGED)
    {
        uint16_t sel = lv_dropdown_get_selected(obj);
        printf("选中下拉项: %d\r\n", sel);

        // 先停止播放，避免切换组时 Task_ActionPlay 读到新组数据
        Action_StopPlay();

        // 切换当前组（只支持 3 组，超出归到组3）
        current_group = (sel < 3) ? sel : 2;
        printf("切换到组%d\r\n", current_group + 1);

        // 加载该组已保存的动作
        Action_LoadFromFlash();
    }
}

/**
 * @brief 注册 screen_1 所有控件的事件回调
 *
 * 新加的控件回调函数，都需要在这里通过 lv_obj_set_event_cb 注册才能生效。
 * @param ui GUI Guider 生成的 UI 结构体指针
 */
void events_init_screen_1(lv_ui *ui)
{
    /* --- 已有功能按钮 --- */
    lv_obj_set_event_cb(ui->screen_1_btn_1, screen_1_btn_1event_handler);
    lv_obj_set_event_cb(ui->screen_1_btn_2, screen_1_btn_2event_handler);
    lv_obj_set_event_cb(ui->screen_1_btn_3, screen_1_btn_3event_handler);
    lv_obj_set_event_cb(ui->screen_1_btn_4, screen_1_btn_4event_handler);
    lv_obj_set_event_cb(ui->screen_1_ddlist_1, screen_1_ddlist_1event_handler);

    /* --- 爪子（轴1）控件注册 --- */
    lv_slider_set_range(ui->screen_1_slider_1, 0, 100);         // 0~100 百分比
    lv_obj_set_event_cb(ui->screen_1_slider_1, slider_1_event_handler);
    lv_obj_set_event_cb(ui->screen_1_btn_5,    btn_5_event_handler);
    lv_obj_set_event_cb(ui->screen_1_btn_6,    btn_6_event_handler);

    /* --- 轴2 控件注册（TODO: 写完回调后取消注释） --- */
    lv_obj_set_event_cb(ui->screen_1_slider_2, slider_2_event_handler);
    lv_obj_set_event_cb(ui->screen_1_btn_7,    btn_7_event_handler);
    lv_obj_set_event_cb(ui->screen_1_btn_8,    btn_8_event_handler);

    /* --- 轴3 控件注册（TODO） --- */
    lv_obj_set_event_cb(ui->screen_1_slider_3, slider_3_event_handler);
    lv_obj_set_event_cb(ui->screen_1_btn_9,    btn_9_event_handler);
    lv_obj_set_event_cb(ui->screen_1_btn_10,   btn_10_event_handler);

    /* --- 轴4 控件注册（TODO） --- */
    lv_obj_set_event_cb(ui->screen_1_slider_4, slider_4_event_handler);
    lv_obj_set_event_cb(ui->screen_1_btn_11,   btn_11_event_handler);
    lv_obj_set_event_cb(ui->screen_1_btn_12,   btn_12_event_handler);

    /* --- 轴5 控件注册（TODO） --- */
    lv_obj_set_event_cb(ui->screen_1_slider_5, slider_5_event_handler);
    lv_obj_set_event_cb(ui->screen_1_btn_13,   btn_13_event_handler);
    lv_obj_set_event_cb(ui->screen_1_btn_14,   btn_14_event_handler);

    /* --- 步进电机（轴6）控件注册（TODO） --- */
    lv_obj_set_event_cb(ui->screen_1_slider_6, slider_6_event_handler);
    lv_obj_set_event_cb(ui->screen_1_btn_15,   btn_15_event_handler);
    lv_obj_set_event_cb(ui->screen_1_btn_16,   btn_16_event_handler);
}


/* ===================================================================
 * 自动模式页（screen_2）控件回调
 * ===================================================================*/

/**
 * @brief screen_2 "返回"按钮
 * 回到主菜单
 */
static void screen_2_btn_1event_handler(lv_obj_t * obj, lv_event_t event)
{
    switch (event)
    {
    case LV_EVENT_CLICKED:
    {
        /*
         * 离开自动模式页：先把控件指针置 NULL！
         * 因为 lv_scr_load_anim(..., true) 的 autodel 会删除 screen_2，
         * 挂在它上面的控件指针会变野指针。
         * 必须在切屏前清空，下次进入 AutoDisp_Init 才能重新创建。
         */
        AutoDisp_Reset();

        if (!lv_debug_check_obj_valid(guider_ui.screen))
            setup_scr_screen(&guider_ui);
        lv_disp_t * d = lv_obj_get_disp(lv_scr_act());
        if (d->prev_scr == NULL && d->scr_to_load == NULL)
            lv_scr_load_anim(guider_ui.screen, LV_SCR_LOAD_ANIM_NONE, 1000, 1000, true);
    }
        break;
    default:
        break;
    }
}

void events_init_screen_2(lv_ui *ui)
{
    /* 只注册事件，不在进入时调 AutoDisp_Reset（那是"离开"时才做的事） */
    lv_obj_set_event_cb(ui->screen_2_btn_1, screen_2_btn_1event_handler);
}
