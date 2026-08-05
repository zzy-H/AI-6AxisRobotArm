#include "servo.h"

#define SERVO_ANGLE_MAX 180 //舵机最大角度值
#define SERVO_ANGLE_MIN 0 //舵机最小角度值
#define ANGLE_TO_PULSE(angle) (500 + (angle) * 2000 / 180) //角度转脉宽公式，单位为微秒

/*
    PA1控制爪子开合舵机-》TIM2_CH2
    PA2控制爪子左右舵机-》TIM2_CH3
    PA3控制爪子上下舵机-》TIM2_CH4
    PA6控制手臂中段舵机-》TIM3_CH1
    PA7控制手臂底端舵机-》TIM3_CH2
*/
//舵机初始化
void servo_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;//定义GPIO结构体变量
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  // 时基基础配置：定时器分频、计数周期
    TIM_OCInitTypeDef TIM_OCInitStructure;    // 输出比较配置：PWM模式、占空比、输出极性

    //开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//开启GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//开启TIM2时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//开启TIM3时钟

    //GPIOA配置为复用推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
    GPIO_InitStructure.GPIO_Pin = 
    GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
    GPIO_Pin_6 | GPIO_Pin_7;//配置为PA1、PA2、PA3、PA6、PA7
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA

    //初始化TIM2时基基础配置
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);//初始化TIM2时基基础配置避免默认值错误
    TIM_TimeBaseStructure.TIM_Period = 20000 - 1; //计数周期为20000-1
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1; //分频系数为72-1
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //时钟分频为1
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);//初始化TIM2时基基础配置
    TIM_ARRPreloadConfig(TIM2, ENABLE);//使能TIM2自动重装载预加载

    /* 配置TIM2_CH2、TIM2_CH3、TIM2_CH4为PWM模式1 */
    TIM_OCStructInit(&TIM_OCInitStructure);//初始化TIM2输出比较配置避免默认值错误
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //PWM模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //使能输出比较
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性为高电平有效

    TIM_OC2Init(TIM2, &TIM_OCInitStructure);//初始化TIM2_CH2输出比较配置
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);//初始化TIM2_CH3输出比较配置
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);//初始化TIM2_CH4输出比较配置

    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);//使能TIM2_CH2自动重装载预加载
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);//使能TIM2_CH3自动重装载预加载
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);//使能TIM2_CH4自动重装载预加载

    /* 配置TIM3_CH1、TIM3_CH2为PWM模式1 */
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);//初始化TIM3时基基础配置
    TIM_ARRPreloadConfig(TIM3, ENABLE);//使能TIM3自动重装载预加载

    TIM_OC1Init(TIM3, &TIM_OCInitStructure);//初始化TIM3_CH1输出比较配置
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);//初始化TIM3_CH2输出比较配置

    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);//使能TIM3_CH1自动重装载预加载
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);//使能TIM3_CH2自动重装载预加载

    TIM_Cmd(TIM2, ENABLE);//使能TIM2
    TIM_Cmd(TIM3, ENABLE);//使能TIM3
}

/*
    @brief 舵机角度设置
    @param servo_id 舵机ID
    @param angle 角度值
    @return 无
   */
void Servo_SetAngle(uint8_t servo_id,uint16_t angle)
{
    uint16_t pulse_width = 0;//定义脉宽值变量
    //将角度值转换为脉宽值
    if(angle > SERVO_ANGLE_MAX)
    {
        angle = SERVO_ANGLE_MAX;//限制角度值在0-180之间
    }
    
    pulse_width = ANGLE_TO_PULSE(angle);

    switch(servo_id)
    {
        case 1: //PA1控制爪子开合舵机-》TIM2_CH2
            TIM_SetCompare2(TIM2, pulse_width); //设置TIM2_CH2的比较寄存器值
            break;
        case 2: //PA2控制爪子左右舵机-》TIM2_CH3
            TIM_SetCompare3(TIM2, pulse_width); //设置TIM2_CH3的比较寄存器值
            break;
        case 3: //PA3控制爪子上下舵机-》TIM2_CH4
            TIM_SetCompare4(TIM2, pulse_width); //设置TIM2_CH4的比较寄存器值
            break;
        case 4: //PA6控制手臂中段舵机-》TIM3_CH1
            TIM_SetCompare1(TIM3, pulse_width); //设置TIM3_CH1的比较寄存器值
            break;
        case 5: //PA7控制手臂底端舵机-》TIM3_CH2
            TIM_SetCompare2(TIM3, pulse_width); //设置TIM3_CH2的比较寄存器值
            break;
        default:
            break;
    }
}

/*
    @brief 舵机返回初始位置
    @return 无
*/
void Servo_Home(void)
{
   uint8_t i = 0;
   for(i = 1;i <= 5;i++)//遍历所有舵机
   {
       Servo_SetAngle(i, 90);//设置舵机角度为90度
   }
}
