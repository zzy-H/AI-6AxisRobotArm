#include "main.h"
#include <stdio.h>
#include "led.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "servo.h"
#include "steeper.h"
#include "lcd.h"
#include "gt911.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "gui_guider.h"
#include "spi.h"
#include "action.h"
#include "w25q64.h"
#include "usart1.h"
#include "auto_disp.h"
#include "kinematic.h"

//LED任务
void LED_Task(void *pvParameters)
{
    (void)pvParameters; // 避免未使用参数的警告

    while (1)
    {
        LED0_Toggle(); // 切换LED0状态
        LED1_Toggle(); // 切换LED1状态
        vTaskDelay(pdMS_TO_TICKS(500));  // ← 直接调 vTaskDelay
    }
}

/*串口数据处理任务*/
void Task_UartDeal(void *pvParameters)
{
    while (1)
    {
        USART3_Process(); // 解析接收缓冲区中的数据
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

/* 舵机控制任务 */
/* last_angle 只在 Task_ServoCtrl 内使用，static 限定作用域，去掉多余 volatile */
static uint16_t last_angle[6] = {0};// 记录上一次设置的角度，避免重复写入
void Task_ServoCtrl(void *pvParameters)
{
    while (1)
    {
        for (uint8_t i = 1; i <= 5; i++) // 遍历所有舵机
        {
            if (servo_target[i] != last_angle[i]) // 如果目标角度与上一次设置的角度不同
            {
                /* 平滑逼近：每次最多走 SMOOTH_STEP 度，防止舵机猛转"砸下去"
                 * 参考六轴工程 Servo_SmoothTick 思路，每 20ms 逼近一步 */
                int16_t diff = (int16_t)servo_target[i] - (int16_t)last_angle[i];
                int16_t step = (diff > 0) ? 3 : -3;   // 每轮走 3°（20ms×3°=150°/s）
                if (diff > 0 && diff < 3)  step = diff;   // 剩余不足一步则走完
                if (diff < 0 && diff > -3) step = diff;

                last_angle[i] = (uint16_t)((int16_t)last_angle[i] + step);
                Servo_SetAngle(i, last_angle[i]); // 设置舵机角度（逼近一步）
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20)); // 延时20毫秒，避免任务占用过多CPU时间
    }
}

/*步进电机控制任务*/
void Task_StepperCtrl(void *pvParameters)
{
    while (1)
    {
        if (stepper_target != 0)
        {
            Stepper_Run(stepper_target);
            g_stepper_pos += stepper_target;   // 更新累计位置
            printf("步进电机: %d 步，当前位置 %d\r\n", stepper_target, g_stepper_pos);
            stepper_target = 0;  // 执行完清0
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

/* LVGL 刷新标志：Task_Test 置位，Task_Lvgl 消费（LVGL 操作只在 LVGL 任务里做） */
volatile uint8_t g_lv_update_req = 0;

/* LVGL 任务：驱动界面刷新 + 更新自动模式示意显示 */
void Task_Lvgl(void *pvParameters)
{
    setup_ui(&guider_ui);    // 创建并显示 GUI Guider 设计的界面

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(5);

    for (;;)
    {
        /* 在 LVGL 任务里更新示意（线程安全） */
        if (g_lv_update_req)
        {
            g_lv_update_req = 0;
            AutoDisp_Update(g_target.width, g_target.height,
                            g_target.pos_x, g_target.pos_y,
                            g_target.color);
        }
        lv_task_handler();
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}

/**
 *  播放动作任务
 *  @param pvParameters 任务参数，未使用
 *  @return void
 */
void Task_ActionPlay(void *pvParameters)
{
    while (1)
    {
        if (is_playing)
        {
            // 取当前组的动作数组和数量
            Action_t *p_list = (current_group == 0) ? action_list_group1 :
                               (current_group == 1) ? action_list_group2 :
                               action_list_group3;
            uint8_t *p_count = (current_group == 0) ? &action_count_group1 :
                               (current_group == 1) ? &action_count_group2 :
                               &action_count_group3;

            // 播放第 action_play_idx 条：5 路舵机依次到位
            for (uint8_t i = 1; i <= 5; i++)
            {
                servo_target[i] = p_list[action_play_idx].servo[i];
            }

            /* 步进电机：目标位置 - 当前位置 = 需要转的步数 */
            stepper_target = p_list[action_play_idx].stepper - g_stepper_pos;

            // 等舵机转到目标位置
            vTaskDelay(pdMS_TO_TICKS(1000));

            action_play_idx++;
            if (action_play_idx >= *p_count)
            {
                if (loop_enable) action_play_idx = 0;   // 循环
                else is_playing = 0;                     // 播完停止
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

/* 全局目标数据：Task_Test 收帧更新，Task_AutoGrab 状态机读取 */
Target_t g_target_data = {0};

/* 最后收到有效帧的时刻（tick），用于判断物块数据是否"新鲜" */
volatile uint32_t g_last_frame_tick = 0;

// 测试任务：接收 OpenCV 数据，更新目标
void Task_Test(void *pvParameters)
{
    while (1)
    {
        if (g_frame_ready)
        {
            if (USART1_ParseFrame() == 0)
			{
				if (g_target.width < 100 || g_target.height < 100)   // 框太小 = 无效
				{
					continue;   //丢弃
				}
			
				// 偏移超出框范围 = 无效（物块必须在框内）
				if (g_target.pos_x > (int16_t)g_target.width  || g_target.pos_x < -(int16_t)g_target.width  ||
					g_target.pos_y > (int16_t)g_target.height || g_target.pos_y < -(int16_t)g_target.height)
				{
					continue;  // 丢弃
				}
				
                printf("w%d h%d c%d x%d y%d\r\n",
                       g_target.width, g_target.height, g_target.color,
                       g_target.pos_x, g_target.pos_y);
			
                /* 像素坐标 → 物理坐标（距离 + 偏转角） */
                Pixel_To_Phys(g_target.width, g_target.height,
                              g_target.pos_x, g_target.pos_y, &g_target_data);
                g_target_data.color = g_target.color;

                /* 记录最后有效帧时刻（供状态机判断物块是否还在） */
                g_last_frame_tick = xTaskGetTickCount();

                printf("距离=%.1fcm 角度=%.1f° 颜色=%d\r\n",
                       g_target_data.dist, g_target_data.alpha, g_target_data.color);
				
				/* 临时调试：看步进目标步数（系数 2.22 = 4细分） */
                printf("alpha=%.1f° → bu=%d\r\n",
                       g_target_data.alpha, (int)(g_target_data.alpha * 2.22f));

				g_lv_update_req = 1;   // ← 只置标志，不调 LVGL！
			}
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

/* 自动抓取任务：定时轮询状态机（不依赖每帧数据，抓取中被遮挡也能推进） */
void Task_AutoGrab(void *pvParameters)
{
    while (1)
    {
        /* 状态机内部有 vTaskDelay 控制节奏，这里每 50ms 唤醒检查 */
        Arm_AutoGrab(&g_target_data);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组
	
	USART3_Init(); // 初始化USART3
    USART1_Init(9600);      //初始化USART1（OpenCV 数据）
    LED_Init(); // 初始化LED
    servo_Init(); // 初始化舵机
    Stepper_Init();//初始化步进电机
    Stepper_SetSpeed(800);//设置步进电机速度800
	
	lv_init();
	
	lv_port_disp_init();       // 负责 LCD 初始化
	lv_port_indev_init();      // 负责触摸初始化

    printf("System Start!\r\n");  // 测试串口输出

    SPI_Config();                    // SPI2 初始化（W25Q64 用）

    
    /*
     * 历史 bug 记录（已修复）：
     * 早期 sFLASH_WritePage 里写了两遍 pBuffer++，导致写入 Flash 时跳过一个字节，
     * 存进去的动作数据全部错乱（读回出现 5682/10310 等乱值）。
     * 修复 w25q64.c 后，之前写入 Flash 的脏数据仍在，必须整扇区擦除一次。
     * 现在已擦除干净，此代码块不再需要。
     */

	// 上电加载 3 个组的动作
	current_group = 0;
	Action_LoadFromFlash();
	current_group = 1;
	Action_LoadFromFlash();
	current_group = 2;
	Action_LoadFromFlash();
	current_group = 0;               // 恢复默认组1
   
    /*
        xTaskCreate函数参数说明：
        1. LED_Task: 任务函数的入口地址
        2. "LED": 任务的名字，便于调试
        3. configMINIMAL_STACK_SIZE: 任务堆栈大小，单位为字节
        4. NULL: 传递给任务函数的参数，这里不需要参数
        5. 1: 任务的优先级，数值越大优先级越高
        6. NULL: 任务的句柄，用于后续控制任务
    */
	// 创建LED任务
    xTaskCreate(LED_Task, "LED", 128, NULL, 1 , NULL);
    // 创建串口数据处理任务
    xTaskCreate(Task_UartDeal, "UARTDeal", 256, NULL, 3 , NULL);
    // 创建舵机控制任务
    xTaskCreate(Task_ServoCtrl, "ServoCtrl", 128, NULL, 2 , NULL);
    //创建步进电机控制任务
    xTaskCreate(Task_StepperCtrl, "StepperCtrl", 128, NULL, 2 , NULL);
    // 创建lvgl任务
    xTaskCreate(Task_Lvgl, "Lvgl", 2048, NULL, 2 , NULL);
    // 创建播放动作任务
    xTaskCreate(Task_ActionPlay, "ActionPlay", 256, NULL, 1, NULL);
    // 创建测试任务（收 OpenCV 数据）
    xTaskCreate(Task_Test, "Test", 256, NULL, 1, NULL);
    // 创建自动抓取任务（定时轮询状态机，不依赖每帧数据）
    xTaskCreate(Task_AutoGrab, "AutoGrab", 256, NULL, 1, NULL);

    // 启动调度器
    vTaskStartScheduler();

    // 如果调度器启动失败，程序会运行到这里
    while (1)
    {
        printf("Scheduler failed to start!\r\n");
        // 可以在这里添加错误处理代码
        Delay_SoftMs(1000); // 延时1秒，避免占用过多CPU时间
    }
}

//堆栈溢出钩子函数
void vApplicationStackOverflowHook(void) 
{
    LED0_On(); // LED0亮起
    while(1); // 无限循环，等待复位
}

//内存分配失败钩子函数
void vApplicationMallocFailedHook(void) 
{
    LED1_On(); // LED1亮起
    while(1); // 无限循环，等待复位
}

//系统滴答钩子函数
void vApplicationTickHook(void) 
{
	lv_tick_inc(1);   // 每 1ms 喂一次 tick
}
