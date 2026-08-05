#include "uart.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>//atoi要用到
#include "servo.h"
#include "steeper.h"

uint8_t USART3_ReceiveBuff[USART3_RECEIVE_BUFF_SIZE];//接收缓冲区
uint16_t USART3_ReceiveBuffIndex = 0;//接收缓冲区索引
uint8_t USART3_ReceiveBuffFlag = 0;//接收缓冲区标志位
uint16_t servo_target[6] = {0, 90, 90, 90, 90, 90};//舵机目标角度数组，初始值为90度
int32_t stepper_target = 0;//步进电机目标步数，初始值为0
int32_t g_stepper_pos = 0; //步进电机当前累计位置（相对零点），初始为0

//初始化USART3
void USART3_Init(void)
{
    //开时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); //使能USART3时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能GPIOB时钟

    //PB10 TX 复用推挽输出
    GPIO_InitTypeDef GPIO_InitStructure;//定义GPIO结构体变量
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //PB11 RX 输入浮空
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//输入浮空
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //串口3基础配置
    USART_InitTypeDef USART3_InitStructure;//定义USART结构体变量
    USART3_InitStructure.USART_BaudRate = 115200;//波特率
    USART3_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流控制
    USART3_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式
    USART3_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验
    USART3_InitStructure.USART_StopBits = USART_StopBits_1;//1位停止位
    USART3_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据位
    USART_Init(USART3, &USART3_InitStructure);//初始化串口3

    //开启接收中断和空闲中断
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//接收中断使能
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//空闲中断使能

    //NVIC中断配置
    NVIC_InitTypeDef NVIC_InitStructure;//定义NVIC结构体变量
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//中断通道
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//子优先级
    NVIC_Init(&NVIC_InitStructure);//初始化NVIC

    USART_Cmd(USART3, ENABLE);//使能串口3
}

//发送单个字节
void USART3_SendByte(uint8_t byte)
{
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//等待发送缓冲区为空
    //将数据写入发送缓冲区，触发发送
    USART_SendData(USART3, byte);//发送数据
}

//发送指定长度到缓冲区的数据
void USART3_SendBuff(uint8_t *buff, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        USART3_SendByte(*buff++);//发送数据
    }
}

//发送字符串
void USART3_SendString(char *str)
{
    while (*str)
    {
        USART3_SendByte(*str++);//发送数据
    }
}

//printf重定向
int fputc(int ch, FILE *f)
{
    USART3_SendByte((uint8_t)ch);//发送数据
    return ch;
}

//串口3中断服务函数
void USART3_IRQHandler(void)
{
    uint8_t data;//定义接收数据变量
    //接收中断
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//接收中断标志位被置位
    {
        data = USART_ReceiveData(USART3);//读取接收到的数据
        //处理接收到的数据,将接收到的数据存入缓冲区（数组）
        if (USART3_ReceiveBuffIndex < USART3_RECEIVE_BUFF_SIZE-1)//判断缓冲区是否已满
        {
            USART3_ReceiveBuff[USART3_ReceiveBuffIndex++] = data;//将接收到的数据存入缓冲区
        }
        else
        {
            //缓冲区已满，处理溢出情况，例如丢弃数据或重置索引
            USART3_ReceiveBuffIndex = 0;//重置索引
        }
        
    }

    //空闲中断
    if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
    {
        USART_ClearITPendingBit(USART3, USART_IT_IDLE);//清除空闲中断标志位
        data = USART_ReceiveData(USART3);//读取接收到的数据，清除中断标志位

        USART3_ReceiveBuffFlag = 1;//设置接收缓冲区标志位，表示接收到一帧数据
    }
}

//指令解析函数
void USART3_Process(void)
{
    if (USART3_ReceiveBuffFlag == 0)//判断接收缓冲区标志位是否被置位
    {
        return;//未接收到一帧数据，直接返回
    }
    
    USART3_ReceiveBuffFlag = 0;//重置接收缓冲区标志位，表示已处理一帧数据
    USART3_ReceiveBuff[USART3_ReceiveBuffIndex] = '\0';//在接收缓冲区末尾添加字符串结束符

    printf("Received: %s\n", USART3_ReceiveBuff);//打印接收到的数据

    // 解析舵机指令: S<舵机编号>,<角度> 例如: "S2,90"
    if(USART3_ReceiveBuff[0] == 'S')//判断指令是否为舵机指令
    {
        uint8_t servo_id = USART3_ReceiveBuff[1] - '0';//获取舵机编号
        char *p = strstr((char *)USART3_ReceiveBuff, ",");//查找逗号分隔符
        if(p != NULL && servo_id >= 1 && servo_id <= 5)
        {
            // 解析角度值
            //atoi函数将字符串转换为整数
            uint16_t angle = atoi(p + 1);

            // 限制角度范围
            if(angle > 180)
                angle = 180;

            // 更新舵机目标角度
            servo_target[servo_id] = angle; // 设目标值，ServoCtrl任务会去执行
        }
    }

    //M指令(步进电机控制)解析代码
    // M,0,200 → 正转200步；M,1,100 → 反转100步
    if(USART3_ReceiveBuff[0] == 'M')//判断指令是否为步进电机指令
    {
        char *p1 = strstr((char *)USART3_ReceiveBuff, ",");//查找逗号分隔符
        if(p1 != NULL)
        {
            // 解析步数值
           int dir = atoi(p1 + 1); //0为正转，1为反转
           char *p2 = strstr(p1 + 1, ",");//查找逗号分隔符
           if(p2 != NULL)
           {
               int steps = atoi(p2 + 1);
                if(dir == 0)
                {
                    stepper_target = steps;     // 正转
                    printf("正转 %d 步\r\n", stepper_target);
                }
                else
                {
                    stepper_target = -steps;    // 反转
                    printf("反转 %d 步\r\n", stepper_target);
                }
            }
        }
        else
        {
            Stepper_Stop();//停止电机
        }
    }


    //清空接收缓冲区和索引
    memset(USART3_ReceiveBuff, 0, USART3_RECEIVE_BUFF_SIZE);//清空接收缓冲区
    USART3_ReceiveBuffIndex = 0;//重置接收缓冲区索引
}
