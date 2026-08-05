#include "spi.h"

/*
 * SPI2 驱动（与 W25Q64 SPI Flash 通信）
 *
 * 引脚分配：
 *   PB12 → CS（片选，通用推挽输出，软件控制）
 *   PB13 → SCK（时钟，复用推挽输出 → SPI2_SCK）
 *   PB14 → MISO（主机输入，浮空输入 → SPI2_MISO）
 *   PB15 → MOSI（主机输出，复用推挽输出 → SPI2_MOSI）
 *
 * 关于 PB12(CS) 的模式：
 *   如果 STM32 作为从机，片选必须由硬件管理，需要配置成输入模式；
 *   如果 STM32 作为主机，片选由软件（GPIO）控制，配置成通用推挽输出即可，
 *   随便一个普通 IO 都能当片选用。本项目使用软件控制片选。
 *
 * 时钟说明：
 *   SPI2 挂载在 APB1 总线（36MHz），分频系数最小为 2，
 *   所以 SPI2 最高速率 = 36MHz / 2 = 18MHz。
 *   W25Q64 支持最大 80MHz 时钟，18MHz 完全满足要求。
 */

/**
 * @brief SPI2 初始化（主机模式）
 *
 * 配置步骤：
 *   1. 开启 GPIOB 时钟
 *   2. 配置 PB12(CS)/PB13(SCK)/PB14(MISO)/PB15(MOSI) 引脚
 *   3. 开启 SPI2 外设时钟
 *   4. 配置 SPI2 为：主机、8位数据、模式3、MSB先发、软件NSS
 *   5. 使能 SPI2，片选拉高（空闲状态）
 */
void SPI_Config(void)
{
    /* ---------- 1. 开启 GPIOB 时钟 ---------- */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /* ---------- 2. 配置 4 个引脚 ---------- */
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // PB12: CS（片选）→ 通用推挽输出，软件控制高低电平
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // PB13(SCK) + PB15(MOSI) → 复用推挽输出
    // 注意：F1 的引脚复用不需要设置 GPIO_Pin_AF（那是 F4 才有的成员），
    //       配置成 GPIO_Mode_AF_PP 后引脚自动连接到 SPI2 的对应功能
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // PB14(MISO) → 浮空输入（SPI 主机接收从机数据）
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* ---------- 3. 开启 SPI2 外设时钟 ---------- */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    /* ---------- 4. 配置 SPI2 工作参数 ---------- */
    SPI_InitTypeDef SPI_InitStruct = {0};

    // 波特率预分频：2 分频 → 36MHz/2 = 18MHz（APB1 最高就是 18MHz，取最大）
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

    // 时钟相位：第 2 个边沿采样（配合 CPOL=High 即模式 3）
    // W25Q64 数据手册：支持模式 0 和模式 3，这里选模式 3
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;

    // 时钟极性：空闲时为高电平（模式 3）
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;

    // 数据宽度：8 位
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;

    // 工作模式：双线双向全工（F1 标准库的宏名，F4 才叫 DualLine）
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;

    // 位顺序：最高位先发（W25Q64 要求 MSB first）
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;

    // 主从模式：主机
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;

    // NSS 管理：软件控制（片选用普通 GPIO 手动拉高拉低）
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;

    /* ---------- 5. 初始化并使能 ---------- */
    SPI_Init(SPI2, &SPI_InitStruct);
    SPI_Cmd(SPI2, ENABLE);

    // 片选拉高 = 未选中任何从机（W25Q64 片选低电平有效）
    GPIO_SetBits(GPIOB, GPIO_Pin_12);
}

/**
 * @brief SPI 单字节收发（全双工）
 *
 * SPI 是全双工协议：发送一个字节的同时，也会接收从机返回的一个字节。
 * 所以读数据时发送任意字节（如 0xFF 哑元），把从机数据"顶"回来。
 *
 * @param Byte 要发送的字节
 * @return uint8_t 接收到的字节
 */
uint8_t SPI_Send_Rec_Byte(uint8_t Byte)
{
    uint8_t Data = 0;

    // 1. 等待发送缓冲区为空（上一次数据已移出）
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) != SET)
    {
    }

    // 2. 写入要发送的数据
    SPI_I2S_SendData(SPI2, Byte);// 写 DR → 自动清 TXE

    // 3. 等待接收缓冲区非空（从机数据已移入）
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) != SET)
    {
    }

    // 4. 读取接收到的数据
    Data = SPI_I2S_ReceiveData(SPI2);// 读 DR → 自动清 RXNE
	
    // 5. 返回
    return Data;
}
