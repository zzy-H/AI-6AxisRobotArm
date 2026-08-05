#ifndef __UART_H_
#define __UART_H_

#include "stm32f10x.h"

#define USART3_RECEIVE_BUFF_SIZE 254//接收缓冲区大小
//导出给其他文件要用的
void USART3_Init(void);//初始化USART3
void USART3_SendByte(uint8_t byte);//发送单个字节
void USART3_SendBuff(uint8_t *buff, uint16_t len);//发送指定长度到缓冲区的数据
void USART3_SendString(char *str);//发送字符串
void USART3_Process(void);//解析接收缓冲区中的数据

// 导出给 ServoCtrl 任务用的目标角度
extern uint16_t servo_target[6];
extern uint8_t USART3_ReceiveBuffFlag;//接收缓冲区标志位
extern uint8_t USART3_ReceiveBuff[USART3_RECEIVE_BUFF_SIZE];//接收缓冲区
extern uint16_t USART3_ReceiveBuffIndex;//接收缓冲区索引
extern int32_t stepper_target;//步进电机目标步数，初始值为0
extern int32_t g_stepper_pos; //步进电机当前累计位置（相对零点），初始为0

#endif
