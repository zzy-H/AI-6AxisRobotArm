#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"

//毫秒延时函数
void Delay_SoftMs(uint32_t ms)
{
    volatile uint32_t i, j;
    for(i = 0; i < ms; i++)
	{
		for(j = 0; j < 18000; j++);
	}
        
}

//微秒延时函数
void Delay_us(uint32_t us)
{
    volatile uint32_t i;
    for (; us > 0; us--)
    {
        i = 18;   // 72MHz 下约 1us
        while (i--);
    }
}
