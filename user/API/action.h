#ifndef __ACTION_H
#define __ACTION_H

#include "stm32f10x.h"

#define MAX_ACTIONS      50      // 每组最多 50 个动作点

// 3 个动作组在 Flash 中的存储地址（每组 4KB，一个扇区）
#define FLASH_ADDR_GROUP1  0x000000
#define FLASH_ADDR_GROUP2  0x001000
#define FLASH_ADDR_GROUP3  0x002000

// 一个动作 = 5 路舵机角度 + 步进电机步数
typedef struct {
    uint16_t servo[6];    // servo[1]~servo[5] 对应舵机1~5（下标0不用）
    int32_t  stepper;     // 步进电机步数
} Action_t;

// 3 个动作组的数据
extern Action_t action_list_group1[MAX_ACTIONS];
extern Action_t action_list_group2[MAX_ACTIONS];
extern Action_t action_list_group3[MAX_ACTIONS];
extern uint8_t  action_count_group1;
extern uint8_t  action_count_group2;
extern uint8_t  action_count_group3;

extern uint8_t  current_group;     // 当前选中的组（0/1/2）
extern uint8_t  action_play_idx;   // 播放到第几个
extern uint8_t  is_playing;        // 是否正在播放
extern uint8_t  loop_enable;       // 是否循环播放动作点

// 函数接口
void Action_Add(uint16_t *servo_angles, int32_t stepper_steps);  // 追加一个动作点
void Action_Delete(uint8_t index);                                // 删除指定动作点
void Action_SaveToFlash(void);                                    // 保存当前组到 Flash
void Action_LoadFromFlash(void);                                  // 从 Flash 加载
void Action_StartPlay(void);                                      // 开始循环播放
void Action_StopPlay(void);                                       // 停止播放

#endif
