/*
 * 动作组管理模块
 *
 * ============ 功能 ============
 * 1. 把当前机械臂姿态（5路舵机角度 + 步进电机步数）保存为"动作点"
 * 2. 支持 3 个动作组，每组最多 50 个动作点
 * 3. 保存到 W25Q64 SPI Flash，掉电不丢失
 * 4. 上电后从 Flash 加载，可循环播放动作序列
 *
 * ============ 数据流 ============
 *   界面"保存"按钮 → Action_Add() → 内存数组
 *                                   → Action_SaveToFlash() → W25Q64
 *
 *   上电启动 → Action_LoadFromFlash() → 内存数组
 *                                     → "自动执行"按钮 → Task_ActionPlay 任务播放
 *
 * ============ Flash 布局 ============
 * 每组占用一个扇区（4KB），互不重叠：
 *   [0x000000] 组1：1字节动作数量 + 动作数据
 *   [0x001000] 组2：1字节动作数量 + 动作数据
 *   [0x002000] 组3：1字节动作数量 + 动作数据
 *
 * ============ 动作点结构 ============
 * typedef struct {
 *     uint16_t servo[6];   // servo[1]~servo[5] 对应 5 个舵机角度（0~180°）
 *     int32_t  stepper;    // 步进电机步数（正=正转，负=反转）
 * } Action_t;
 */

#include "action.h"
#include "w25q64.h"     // W25Q64 Flash 读写
#include "string.h"     // memset 等内存操作
#include "stdio.h"      // printf 调试输出
#include "uart.h"

/* ==================== 全局变量定义 ==================== */

/* 3 个动作组的动作数据数组（每组最多 MAX_ACTIONS 个动作点） */
Action_t action_list_group1[MAX_ACTIONS];
Action_t action_list_group2[MAX_ACTIONS];
Action_t action_list_group3[MAX_ACTIONS];

/* 3 个动作组各自已保存的动作点数 */
uint8_t action_count_group1 = 0;
uint8_t action_count_group2 = 0;
uint8_t action_count_group3 = 0;

/* 当前选中的动作组：0=组1，1=组2，2=组3（默认组1） */
uint8_t current_group = 0;

/* 播放状态控制 */
uint8_t action_play_idx = 0;    // 当前播放到组内第几个动作点
uint8_t is_playing = 0;         // 1=正在播放，0=停止
uint8_t loop_enable = 1;        // 1=循环播放（播完从头再来），0=只播一次

/* ==================== 内部辅助函数 ==================== */

/**
 * @brief 根据 current_group 返回当前组的三个关键指针
 *
 * 所有公共函数操作前都要先调用本函数，避免每个函数重复写
 * if/else 判断是哪一组。三个输出参数一次拿齐：
 *
 * @param p_list  输出：当前组的动作数组指针
 * @param p_count 输出：当前组动作数量的指针（可直接读写）
 * @param p_addr  输出：当前组在 Flash 中的起始地址
 */
static void GetGroupPtr(Action_t **p_list, uint8_t **p_count, uint32_t *p_addr)
{
    switch (current_group)
    {
        case 0:     // 组1
            *p_list  = action_list_group1;
            *p_count = &action_count_group1;
            *p_addr  = FLASH_ADDR_GROUP1;
            break;
        case 1:     // 组2
            *p_list  = action_list_group2;
            *p_count = &action_count_group2;
            *p_addr  = FLASH_ADDR_GROUP2;
            break;
        default:    // 组3（current_group >= 2 都归到组3）
            *p_list  = action_list_group3;
            *p_count = &action_count_group3;
            *p_addr  = FLASH_ADDR_GROUP3;
            break;
    }
}

/* ==================== 动作管理 ==================== */

/**
 * @brief 追加一个动作点（保存当前姿态）
 *
 * 把传入的 5 路舵机角度和步进电机步数，作为一个新的动作点
 * 追加到当前组的末尾。
 *
 * @param servo_angles   舵机角度数组（只用下标 1~5，0 不用）
 * @param stepper_steps  步进电机步数（正=正转，负=反转）
 *
 * 注意：此函数只写入内存，掉电丢失。
 *       需要调用 Action_SaveToFlash() 才会真正存到 Flash。
 */
void Action_Add(uint16_t *servo_angles, int32_t stepper_steps)
{
    Action_t *p_list;
    uint8_t  *p_count;
    uint32_t  flash_addr;

    /* 获取当前组的指针 */
    GetGroupPtr(&p_list, &p_count, &flash_addr);

    /* 动作数量已满，拒绝添加 */
    if (*p_count >= MAX_ACTIONS)
    {
        printf("组%d已满(%d)，无法添加\r\n", current_group + 1, MAX_ACTIONS);
        return;
    }

    /* 复制 5 路舵机角度到新动作点 */
    for (uint8_t i = 1; i <= 5; i++)
    {
        p_list[*p_count].servo[i] = servo_angles[i];
    }

    /* 保存步进电机步数 */
    p_list[*p_count].stepper = stepper_steps;

    /* 动作数量 +1 */
    (*p_count)++;
    printf("组%d 已添加动作%d，共%d个\r\n", current_group + 1, *p_count, *p_count);
}

/**
 * @brief 删除指定索引的动作点
 *
 * 删除后，后面的动作点依次向前移动覆盖，保持数组连续。
 * 同时修正播放索引，避免播放越界。
 *
 * @param index 要删除的动作索引（从 0 开始，0=第一个动作）
 */
void Action_Delete(uint8_t index)
{
    Action_t *p_list;
    uint8_t  *p_count;
    uint32_t  flash_addr;

    GetGroupPtr(&p_list, &p_count, &flash_addr);

    /* 组内没有动作，无需删除 */
    if (*p_count == 0)
    {
        printf("没有动作可删除\r\n");
        return;
    }

    /* 索引越界（超过现有动作数） */
    if (index >= *p_count)
    {
        printf("无效索引%d\r\n", index);
        return;
    }

    /* 从 index 开始，后面的动作依次前移一位，覆盖被删的动作 */
    for (uint8_t i = index; i < *p_count - 1; i++)
    {
        p_list[i] = p_list[i + 1];
    }

    /* 动作数量 -1 */
    (*p_count)--;

    /* 如果在播放中删除了前面的动作，修正播放索引 */
    if (is_playing && action_play_idx > index)
    {
        action_play_idx--;
    }
    /* 播放索引超出剩余数量，回到开头 */
    if (action_play_idx >= *p_count)
    {
        action_play_idx = 0;
    }

    printf("组%d 已删除动作%d，剩余%d个\r\n", current_group + 1, index + 1, *p_count);
}

/* ==================== Flash 存储 ==================== */

/**
 * @brief 保存当前组所有动作到 W25Q64 Flash
 *
 * 存储格式（组起始地址处）：
 *   第 1 字节：动作数量
 *   后续字节：MAX_ACTIONS 个 Action_t 结构体（实际只写 count 个）
 *
 * 注意：写入前必须整扇区擦除（Flash 只能把 1 写成 0，不能把 0 写成 1）。
 *       所以每次保存都会先擦除整个 4KB 扇区。
 */
void Action_SaveToFlash(void)
{
    Action_t *p_list;
    uint8_t  *p_count;
    uint32_t  flash_addr;

    GetGroupPtr(&p_list, &p_count, &flash_addr);

    /* 1. 擦除整个扇区（4KB），清掉旧数据 */
    sFLASH_EraseSector(flash_addr);

    /* 2. 先写动作数量（1 字节），放在扇区开头 */
    sFLASH_WriteBuffer(p_count, flash_addr, 1);

    /* 3. 有动作才写数据区 */
    if (*p_count > 0)
    {
        sFLASH_WriteBuffer((uint8_t *)p_list, flash_addr + 1, *p_count * sizeof(Action_t));
        printf("组%d 已保存%d个动作到Flash\r\n", current_group + 1, *p_count);
    }
    else
    {
        printf("组%d 已清空保存\r\n", current_group + 1);
    }
}

/**
 * @brief 从 Flash 加载当前组动作到内存
 *
 * 上电时调用。读取前先校验数量字段：
 *   - 数量为 0 或超出 MAX_ACTIONS → 数据无效，不加载
 *   - 否则读入全部动作数据
 */
void Action_LoadFromFlash(void)
{
    Action_t *p_list;
    uint8_t  *p_count;
    uint32_t  flash_addr;

    GetGroupPtr(&p_list, &p_count, &flash_addr);

    uint8_t count;

    /* 1. 读动作数量（1 字节） */
    sFLASH_ReadBuffer(&count, flash_addr, 1);

    /* 2. 数量无效（0 或超上限），说明 Flash 里没有有效数据 */
    if (count == 0 || count > MAX_ACTIONS)
    {
        printf("组%d Flash中无有效动作数据\r\n", current_group + 1);
        return;
    }

    /* 3. 数量有效，读入全部动作数据 */
    *p_count = count;
    sFLASH_ReadBuffer((uint8_t *)p_list, flash_addr + 1, count * sizeof(Action_t));
    printf("组%d 已从Flash加载%d个动作\r\n", current_group + 1, *p_count);
}

/* ==================== 播放控制 ==================== */

/**
 * @brief 开始播放当前组动作（从头开始）
 *
 * 只设置播放状态，真正的执行由 main.c 里的 Task_ActionPlay 任务完成：
 *   is_playing = 1 后，任务逐条读取动作写入 servo_target/stepper_target。
 */
void Action_StartPlay(void)
{
    uint8_t *p_count;

    /* 根据当前组取数量指针 */
    p_count = (current_group == 0) ? &action_count_group1 :
              (current_group == 1) ? &action_count_group2 :
              &action_count_group3;

    /* 组内没有动作，无法播放 */
    if (*p_count == 0)
    {
        printf("组%d没有动作可播放\r\n", current_group + 1);
        return;
    }

    /* 从头开始播放 */
    action_play_idx = 0;
    is_playing = 1;
    printf("组%d 开始播放%d个动作\r\n", current_group + 1, *p_count);
}

/**
 * @brief 停止播放
 *
 * 播放任务检测到 is_playing = 0 后停止执行后续动作。
 */
void Action_StopPlay(void)
{
    is_playing = 0;
    printf("停止播放\r\n");
}

/**
 * @brief 切换循环播放模式（开/关）
 *
 * loop_enable = 1：播完最后一个动作后回到第一个继续
 * loop_enable = 0：播完最后一个动作后自动停止
 */
void Action_ToggleLoop(void)
{
    loop_enable = !loop_enable;
    printf("循环播放%s\r\n", loop_enable ? "open" : "close");
}
