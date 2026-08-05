#ifndef __USART1_H_
#define __USART1_H_

#include "stm32f10x.h"

#define FRAME_LEN  10    // 帧长度

// 解析结果
typedef struct {
    uint16_t width;      // AruCo 框宽
    uint16_t height;     // AruCo 框高
    uint8_t  color;      // 0红 1绿 2蓝
    int16_t  pos_x;      // 物块X偏移（有符号！）
    int16_t  pos_y;      // 物块Y偏移（有符号！）
} Target_Info_t;

extern volatile uint8_t g_frame_ready;   // 1=收到完整帧
extern Target_Info_t g_target;          // 解析结果

void USART1_Init(uint32_t brr);  // 初始化USART1，brr为波特率
uint8_t USART1_ParseFrame(void);   // 返回0=成功 1=失败

#endif
