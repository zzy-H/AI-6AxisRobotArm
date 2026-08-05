#include "led.h"
#include "stm32f10x.h"

/* LED初始化 */
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;//定义GPIO结构体变量

	/* 使能GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

	/* 配置LED0 GPIOE.4为输出模式 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; // 输出速度50MHz
	GPIO_Init(GPIOE, &GPIO_InitStruct); // 初始化GPIOE.4
	GPIO_SetBits(GPIOE, GPIO_Pin_4); // 设置GPIOE.4为高电平，关闭LED0

	/* 配置LED1 GPIOE.5为输出模式 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; // 输出速度50MHz	
	GPIO_Init(GPIOE, &GPIO_InitStruct); // 初始化GPIOE.5
	GPIO_SetBits(GPIOE, GPIO_Pin_5); // 设置GPIOE.5为高电平，关闭LED1
}

//LED0点亮
void LED0_On(void)
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_4); // 设置GPIOE.4为低电平，点亮LED0
}

//LED0熄灭
void LED0_Off(void)
{
	GPIO_SetBits(GPIOE, GPIO_Pin_4); // 设置GPIOE.4为高电平，熄灭LED0
}

//LED1点亮
void LED1_On(void)
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_5); // 设置GPIOE.5为低电平，点亮LED1
}

//LED1熄灭
void LED1_Off(void)
{
	GPIO_SetBits(GPIOE, GPIO_Pin_5); // 设置GPIOE.5为高电平，熄灭LED1
}

//LED0闪烁
void LED0_Toggle(void)
{
	GPIOE->ODR ^= GPIO_Pin_4; // 切换GPIOE.4的状态，实现LED0的闪烁
	//GPIO_WriteBit(GPIOE, GPIO_Pin_4, !GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_4)); // 另一种实现方式
}

//LED1闪烁
void LED1_Toggle(void)
{
	GPIO_WriteBit(GPIOE, GPIO_Pin_5, 
              (GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_5) == Bit_RESET) ? Bit_SET : Bit_RESET); // 切换GPIOE.5的状态，实现LED1的闪烁
}
