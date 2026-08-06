#ifndef __MAIN_H_
#define __MAIN_H_

#include "stm32f10x.h"

/* 舵机到位检测：返回 1=还有舵机在移动，0=全部到位
 * 供状态机/动作播放等待舵机到位后再切下一个动作 */
uint8_t Servo_IsMoving(void);

#endif
