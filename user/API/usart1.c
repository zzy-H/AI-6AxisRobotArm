#include "usart1.h"

//接收缓冲区
uint8_t g_rx_buf[FRAME_LEN];

//接收字节数
uint8_t g_rx_len = 0;

// usart1.c 顶部需要加：
volatile uint8_t g_frame_ready = 0;   // 帧接收完成标志
Target_Info_t g_target;               // 解析结果

/**
 * @brief 初始化USART1
 *USART1 + PA9/PA10 + 9600 + NVIC
 */
void USART1_Init(uint32_t brr)
{
    //声明结构体变量
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

    //开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    //配置PA9为复用推挽输出（TX）
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

    //配置PA10为浮空输入（RX）
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

    //配置USART1参数
    uint32_t pclk2 = SystemCoreClock / 2; // APB2时钟频率
	USART_InitStructure.USART_BaudRate = brr;//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1位停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //接收和发送模式
	USART_Init(USART1, &USART_InitStructure);//初始化USART1

    //配置中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//接收中断使能
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//空闲中断使能
	NVIC_SetPriority(USART1_IRQn, 1);//设置中断优先级
	//使能USART1中断
	NVIC_EnableIRQ(USART1_IRQn);//使能USART1中断

	USART_Cmd(USART1, ENABLE);//使能USART1
}

//USART1中断服务函数
void USART1_IRQHandler(void)
{
    uint8_t data;

    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        data = USART_ReceiveData(USART1);

        /*
         * 越界保护：缓冲区只有 10 字节（下标 0~9）。
         * 如果 g_rx_len 已满还继续写 g_rx_buf[10]，会越界写坏相邻的
         * g_rx_len / g_frame_ready / g_target 变量（它们在内存中紧挨着），
         * 导致收帧逻辑错乱（表现为：只收到第一帧，后面再无数据）。
         * 修复：写入前判断，缓冲区满后多余字节直接丢弃。
         */
        if (g_rx_len < FRAME_LEN)
        {
            g_rx_buf[g_rx_len++] = data;
        }
        /* else: 缓冲已满，丢弃该字节（等 IDLE 判断帧结束） */
    }
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        USART_ReceiveData(USART1);   // 清 IDLE 标志
        if (g_rx_len == FRAME_LEN) 
        {
            g_frame_ready = 1;   // 满帧才置位
        }
        g_rx_len = 0;               // 一帧结束，重置接收计数
    }
}

//解析帧
uint8_t USART1_ParseFrame(void)
{
    uint8_t sum = 0;
    // 校验：前9字节累加 == 第10字节
    for (uint8_t i = 0; i < 9; i++) 
    {
        sum += g_rx_buf[i];
    }
    if (sum != g_rx_buf[9])
    {
        g_rx_len = 0;                     // 校验失败：清缓冲
        g_frame_ready = 0;
        return 1;
    }

    /**
     * 数据格式：
     * [0] [1]   [2] [3]   [4]   [5] [6]  [7] [8]  [9]
     *  框宽        框高    颜色   Y偏移   X偏移    校验和
     */
    g_target.width  = (g_rx_buf[0] << 8) | g_rx_buf[1];
    g_target.height = (g_rx_buf[2] << 8) | g_rx_buf[3];
    g_target.color  = g_rx_buf[4];
    g_target.pos_y  = (int16_t)((g_rx_buf[5] << 8) | g_rx_buf[6]);  // int16 有符号
    g_target.pos_x  = (int16_t)((g_rx_buf[7] << 8) | g_rx_buf[8]);

    g_rx_len = 0;                         // 成功：清缓冲
    g_frame_ready = 0;

    return 0;
}





