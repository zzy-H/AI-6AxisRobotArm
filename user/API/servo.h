#ifndef __SERVO_H_
#define __SERVO_H_

#include "stm32f10x.h"

void servo_Init(void);//初始化舵机
void Servo_SetAngle(uint8_t servo_id,uint16_t angle);//设置舵机角度
void Servo_Home(void);//舵机返回初始位置


#endif
