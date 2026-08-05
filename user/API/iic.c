#include "iic.h"

/*
 * 软件 I2C 驱动
 *
 * 引脚分配：
 *   PB1  → SCL（时钟线）
 *   PF9  → SDA（数据线）
 *
 * I2C 协议简述：
 *   1. 空闲时：SCL = 1, SDA = 1（总线空闲状态）
 *   2. 起始信号：SCL = 1 时，SDA 由 1 → 0
 *   3. 数据传输：SCL = 0 时改变 SDA，SCL = 1 时从机采样
 *   4. 停止信号：SCL = 1 时，SDA 由 0 → 1
 *   5. 每 8 位数据后跟一个 ACK/NACK 应答位
 *
 * GPIO 模式说明：
 *   使用开漏输出（GPIO_Mode_Out_OD），可实现双向通信：
 *   - 输出 1 时 = 释放总线（靠外部上拉电阻拉高）
 *   - 输出 0 时 = 拉低总线
 *   - 读取时用 GPIO_ReadInputDataBit() 读引脚实际电平
 */


/**
 * @brief I2C 引脚初始化
 *
 * 将 PB1(SCL) 和 PF9(SDA) 配置为开漏输出，
 * 初始化后释放总线（拉高 = 空闲状态）。
 */
void iic_init(void)
{
    // PF9 → SDA, PB1 → SCL
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOF, ENABLE);

    GPIO_InitTypeDef gpio;
    gpio.GPIO_Mode  = GPIO_Mode_Out_OD;   // 开漏输出（双向通信）
    gpio.GPIO_Speed = GPIO_Speed_2MHz;

    // PB1: SCL
    gpio.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOB, &gpio);

    // PF9: SDA
    gpio.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOF, &gpio);

    // 释放总线（拉高 = 空闲状态）
    GPIO_SetBits(GPIOB, GPIO_Pin_1);   // SCL = 1
    GPIO_SetBits(GPIOF, GPIO_Pin_9);   // SDA = 1
}


/**
 * @brief I2C 起始信号（START）
 *
 * 时序：SCL 高电平时，SDA 由高变低。
 * 结束后 SCL 拉低，为后续数据传输做准备。
 *
 *  SCL  ─────────────────────────────
 *  SDA  ──────___───────────────────
 *        ↑ START     ↓ SCL 拉低
 */
void iic_start(void)
{
    // 1. 确保 SCL、SDA 都是高电平（空闲状态）
    GPIO_SetBits(GPIOB, GPIO_Pin_1);   // SCL = 1
    GPIO_SetBits(GPIOF, GPIO_Pin_9);   // SDA = 1
    Delay_us(2);

    // 2. SDA 拉低 → 产生 START 条件（SCL 高时 SDA 下降沿）
    GPIO_ResetBits(GPIOF, GPIO_Pin_9); // SDA = 0
    Delay_us(5);

    // 3. SCL 拉低，释放总线，准备传输数据
    GPIO_ResetBits(GPIOB, GPIO_Pin_1); // SCL = 0
    Delay_us(2);
}


/**
 * @brief I2C 停止信号（STOP）
 *
 * 时序：SCL 高电平时，SDA 由低变高。
 * 结束后 SCL、SDA 均为高电平（总线空闲）。
 *
 *  SCL  ______________────────────
 *  SDA  ___________________───___
 *        ↑ SCL 拉高   ↑ STOP
 */
void iic_end(void)
{
    // 1. 先确保 SDA 为低
    GPIO_ResetBits(GPIOF, GPIO_Pin_9); // SDA = 0
    Delay_us(2);

    // 2. SCL 拉高
    GPIO_SetBits(GPIOB, GPIO_Pin_1);   // SCL = 1
    Delay_us(5);

    // 3. SDA 拉高 → 产生 STOP 条件（SCL 高时 SDA 上升沿）
    GPIO_SetBits(GPIOF, GPIO_Pin_9);   // SDA = 1
    Delay_us(5);
}


/**
 * @brief 等待从机应答（ACK）
 *
 * 发送完 8 位数据后，主设备释放 SDA 总线，
 * 从设备拉低 SDA 表示应答（ACK）。
 *
 * @return uint8_t 0=收到应答(ACK)，1=未收到应答(NACK/超时)
 *
 * 时序：
 *  SDA  ──────────────────___
 *  SCL  ───────────────────────
 *        ↑ 第 8 位  ↑ SCL 高时从机拉低 SDA = ACK
 */
uint8_t iic_waitack(void)
{
    uint8_t waittime = 0;    // 超时计数器
    uint8_t rack = 0;        // 返回值：0=ACK, 1=NACK

    // 1. 释放 SDA 总线（开漏输出写 1 = 高阻，由上拉拉高）
    GPIO_SetBits(GPIOF, GPIO_Pin_9);   // SDA = 1（释放）
    Delay_us(2);

    // 2. SCL 拉高，让从机在 SDA 上输出应答
    GPIO_SetBits(GPIOB, GPIO_Pin_1);   // SCL = 1
    Delay_us(2);

    // 3. 读取 SDA 电平，等待从机拉低
    //    SDA=0 → 从机应答(ACK)
    //    SDA=1 → 从机无应答(NACK)
    while (GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_9))
    {
        waittime++;
        if (waittime > 250)            // 超时，认为 NACK
        {
            iic_end();                  // 发送 STOP
            rack = 1;                   // 标记 NACK
            break;
        }
    }

    // 4. SCL 拉低，结束应答位
    GPIO_ResetBits(GPIOB, GPIO_Pin_1); // SCL = 0
    Delay_us(5);

    return rack;    // 0=收到 ACK, 1=未收到
}


/**
 * @brief 发送应答信号（ACK）
 *
 * 主设备接收完一个字节后，拉低 SDA 告诉从机继续发送。
 * （仅在主设备还想继续读数据时调用）
 *
 */
void iic_ack(void)
{
    GPIO_ResetBits(GPIOF, GPIO_Pin_9); // SDA = 0
    Delay_us(2);
    GPIO_SetBits(GPIOB, GPIO_Pin_1);   // SCL = 1（从机检测到 ACK）
    Delay_us(5);
    GPIO_ResetBits(GPIOB, GPIO_Pin_1); // SCL = 0
    Delay_us(2);
    GPIO_SetBits(GPIOF, GPIO_Pin_9);   // SDA = 1（释放）
    Delay_us(2);
}


/**
 * @brief 发送非应答信号（NACK）
 *
 * 主设备接收完最后一个字节后，不拉低 SDA，
 * 告诉从机不再继续发送。
 */
void iic_nack(void)
{
    GPIO_SetBits(GPIOF, GPIO_Pin_9);   // SDA = 1（不拉低 = NACK）
    Delay_us(2);
    GPIO_SetBits(GPIOB, GPIO_Pin_1);   // SCL = 1（从机检测到 NACK）
    Delay_us(2);
    GPIO_ResetBits(GPIOB, GPIO_Pin_1); // SCL = 0
    Delay_us(2);
}


/**
 * @brief 发送一个字节（高位先发）
 *
 * I2C 规定：数据在 SCL 低电平时改变，SCL 高电平时被采样。
 * 每发送完 8 位后，需调用 iic_waitack() 等待从机应答。
 *
 * @param byte 要发送的 8 位数据
 *
 */
void iic_sendbyte(uint8_t byte)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_1); // SCL = 0（允许改变 SDA）
        Delay_us(2);

        // 取当前最高位输出到 SDA
        if (byte & 0x80)
            GPIO_SetBits(GPIOF, GPIO_Pin_9);   // 发送 1
        else
            GPIO_ResetBits(GPIOF, GPIO_Pin_9);  // 发送 0

        Delay_us(2);
        GPIO_SetBits(GPIOB, GPIO_Pin_1);   // SCL = 1（从机采样）
        Delay_us(5);

        byte <<= 1;                         // 左移，处理下一位
        GPIO_ResetBits(GPIOB, GPIO_Pin_1); // SCL = 0
    }

    // 发送完保持 SCL 低电平，等待外部调用 iic_waitack()
    GPIO_ResetBits(GPIOB, GPIO_Pin_1); // SCL = 0
}


/**
 * @brief 接收一个字节
 *
 * SCL 高电平时从 SDA 读取一位，SCL 低电平时移位。
 * 读取完成后根据 ack 参数发送 ACK 或 NACK。
 *
 * @param ack 0=接收完成后发 NACK（最后一字节）
 *            非 0=接收完成后发 ACK（还要继续读）
 * @return uint8_t 接收到的 8 位数据
 */
uint8_t iic_recvbyte(uint8_t ack)
{
    uint8_t byte = 0;

    // 先释放 SDA 总线，让从机控制
    GPIO_SetBits(GPIOF, GPIO_Pin_9);

    for (uint8_t i = 0; i < 8; i++)
    {
        byte <<= 1;                         // 左移，为新位腾出空间
        GPIO_SetBits(GPIOB, GPIO_Pin_1);   // SCL = 1（从机输出数据）
        Delay_us(2);

        // 读取 SDA 电平
        if (GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_9))
        {
            byte++;                         // SDA=1 → 该位为 1
        }

        GPIO_ResetBits(GPIOB, GPIO_Pin_1); // SCL = 0
        Delay_us(2);
    }

    // 发应答/非应答
    if (ack == 0)
        iic_nack();     // 最后一字节，通知从机停止发送
    else
        iic_ack();      // 还要继续读，通知从机继续发

    return byte;
}
