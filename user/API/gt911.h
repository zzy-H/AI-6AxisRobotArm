#ifndef __GT911_H_
#define __GT911_H_
#include "stm32f10x.h"

#include "iic.h"
#include "delay.h"
#include "string.h"
#include "stddef.h"
#include "uart.h"
#include "lcd.h"
#include "stdio.h"

// 触摸点结构体
typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t size;    
}TouchPoint_t;

//uint8_t GT911_ScanMulti(TouchPoint_t point[]);
void touch_init(void);//初始化触摸屏
void touch_reset(void);//软件复位触摸屏
uint8_t touch_write(uint16_t reg, uint8_t *buf, uint8_t len);//写入触摸屏寄存器
void touch_read(uint16_t reg, uint8_t *buf, uint8_t len);//读取触摸屏寄存器
void touch_sw_reset(void);//软件复位触摸屏
void touch_pid(char *pid);//设置触摸屏ID
uint8_t touch_config(void);//配置触摸屏
uint8_t touch_scan(TouchPoint_t *point, uint8_t cnt);//扫描触摸点

#endif
