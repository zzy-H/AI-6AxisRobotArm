#include "steeper.h"
#include "delay.h"
#include <stdlib.h>
#include <math.h>

/*引脚定义
    PB5 = DIR  方向引脚
    PB6 = STEP  脉冲引脚

    脉冲引脚
    1.决定转动角度，一个脉冲对应一个固定角度（步距角）
    2.决定转动速度，脉冲频率越高，电机转动越快（单位：步/秒）
*/
#define STEP_PIN GPIO_Pin_6
#define DIR_PIN GPIO_Pin_5
#define STEP_PORT GPIOB//脉冲引脚
#define DIR_PORT GPIOB//方向引脚

uint32_t g_step_delay = 100; // 步进延迟，控制步进速度单位为微秒

//步进电机初始化
void Stepper_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    //开启GPIOB时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    //配置DIR引脚为推挽输出PB5
    GPIO_InitStructure.GPIO_Pin = DIR_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//通用推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DIR_PORT, &GPIO_InitStructure);

    //配置STEP引脚为推挽输出PB6
    GPIO_InitStructure.GPIO_Pin = STEP_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(STEP_PORT, &GPIO_InitStructure);

    //默认输出低电平
    STEP_PORT->BRR = STEP_PIN; // 设置STEP引脚为低电平
    DIR_PORT->BRR = DIR_PIN;   // 设置DIR引脚为低电平
}

//设置电机转速：步/秒
void Stepper_SetSpeed(uint32_t speed)
{
    //限制在安全范围内
    if(speed < 100)
    {
        speed = 100; // 最小速度为100步/秒
    }
    else if(speed > 5000)
    {
        speed = 5000; // 最大速度为5000步/秒
    }

    //计算步进延迟，单位为微秒
    g_step_delay = 1000000 / speed; // 步进延迟 = 1000000微秒 / 步数
}

//输出一个步进脉冲
/*
    BSRR: 位设置寄存器，用于设置指定引脚为高电平
    BRR: 位重置寄存器，用于设置指定引脚为低电平
    1.设置STEP引脚为高电平，使电机转动
    2.设置DIR引脚为高电平，使电机顺时针转动
*/
void StepOnce(void)
{
    STEP_PORT->BSRR = STEP_PIN; // 脉冲拉高
    Delay_us(2); // 延时2微秒，确保STEP引脚为高电平
    STEP_PORT->BRR = STEP_PIN; // 脉冲拉低
    Delay_us(g_step_delay); // 延时步进延迟，决定转速
}

//运行指定步数
void Stepper_Run(int steps)
{
   //步数绝对值
    uint16_t count = abs(steps);//取绝对值，确保步数为正数

    //正数代表正转，负数代表反转
    if(steps == 0)
    {
        return;
    }
    else if(steps > 0)
    {
        DIR_PORT->BSRR = DIR_PIN; // 正转
    }
    else
    {
        DIR_PORT->BRR = DIR_PIN; // 反转
    }

    Delay_us(5);//方向稳定延时，确保方向引脚稳定，避免电机异常转动

    //运行指定步数
    for(uint16_t i = 0; i < count; i++)
    {
        StepOnce();
    }
}

//停止电机
void Stepper_Stop(void)
{
    STEP_PORT->BRR = STEP_PIN; // 脉冲拉低
}















