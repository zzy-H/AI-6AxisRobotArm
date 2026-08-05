#ifndef __STEEPER_H_
#define __STEEPER_H_

#include "stm32f10x.h"

void Stepper_Init(void);//步进电机初始化
void Stepper_SetSpeed(uint32_t speed);//设置电机转速：步/秒
void Stepper_Run(int steps);//运行指定步数
void Stepper_Stop(void);//停止电机


#endif
