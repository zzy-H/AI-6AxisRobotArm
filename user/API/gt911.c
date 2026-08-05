#include "gt911.h"

/*
 * GT911 电容触摸屏驱动
 *
 * 通信方式：I2C
 *   PB1  → SCL
 *   PF9  → SDA
 *
 * 控制引脚：
 *   PF10 → INT（中断引脚，浮空输入 / 推挽输出）
 *   PF11 → RST（复位引脚，推挽输出）
 *
 * I2C 地址：
 *   7 位地址：0x14（取决于 RST 拉低时 INT 引脚电平）
 *   8 位写地址：0x28    8 位读地址：0x29
 *
 * 寄存器说明：
 *   0x8140-0x8143  → 产品 ID（"911" + 版本号）
 *   0x8040         → 软件复位
 *   0x814E         → 触摸状态（bit7=有触摸，低4位=触摸点数）
 *   0x814F+        → 触摸点坐标数据（每个点占 8 字节）
 */

/* 触摸点坐标寄存器地址（每个点 8 字节） */
static const uint16_t touch_tp[5] = {
    0x814F,  // 触摸点 1
    0x8157,  // 触摸点 2
    0x815F,  // 触摸点 3
    0x8167,  // 触摸点 4
    0x816F,  // 触摸点 5
};


/**
 * @brief 触摸引脚初始化（短时复位）
 *
 * PF10(INT) = 浮空输入（等待 INT 触发）
 * PF11(RST) = 推挽输出，执行一次 1ms 低脉冲复位
 */
void touch_init(void)
{
    // PF10 = INT（浮空输入），PF11 = RST（推挽输出）
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);

    GPIO_InitTypeDef gpio;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;

    // PF10: INT（中断输入）
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio.GPIO_Pin  = GPIO_Pin_10;
    GPIO_Init(GPIOF, &gpio);

    // PF11: RST（复位输出）
    gpio.GPIO_Pin  = GPIO_Pin_11;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOF, &gpio);

    // 短复位脉冲
    GPIO_ResetBits(GPIOF, GPIO_Pin_11);   // RST = 0
    Delay_SoftMs(1);
    GPIO_SetBits(GPIOF, GPIO_Pin_11);     // RST = 1
    Delay_SoftMs(5);
}


/**
 * @brief 触摸芯片完整复位
 *
 * 标准复位时序（GT911 数据手册要求）：
 *   1. RST=0, INT=1 → 等待 20ms
 *   2. RST=1        → 等待至少 100ms（此处用 300ms 保证稳定）
 *   3. INT 恢复为浮空输入
 *
 * 复位后 GT911 的 I2C 地址由 INT 引脚电平决定：
 *   INT=0 → 地址 0x14；INT=1 → 地址 0x5D
 */
void touch_reset(void)
{
    GPIO_InitTypeDef gpio;

    // 临时将 INT(PF10) 改为推挽输出，配合复位时序
    gpio.GPIO_Pin   = GPIO_Pin_10;
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOF, &gpio);

    // 复位时序
    GPIO_ResetBits(GPIOF, GPIO_Pin_11);   // RST = 0
    GPIO_SetBits(GPIOF, GPIO_Pin_10);     // INT = 1
    Delay_SoftMs(20);

    GPIO_SetBits(GPIOF, GPIO_Pin_11);     // RST = 1（结束复位）
    Delay_SoftMs(300);                    // 等待芯片启动完成

    // INT 恢复为浮空输入
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOF, &gpio);
    Delay_SoftMs(100);
}


/**
 * @brief 通过 I2C 写 GT911 寄存器
 *
 * @param reg 寄存器地址（16 位）
 * @param buf 要写入的数据缓冲区
 * @param len 数据长度（字节）
 * @return uint8_t 0=成功，1=失败（I2C 无应答）
 */
uint8_t touch_write(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    uint8_t ret = 0;

    iic_start();
    iic_sendbyte((0x14 << 1) | 0);   // 写地址 0x28
    iic_waitack();
    iic_sendbyte((uint8_t)(reg >> 8) & 0xFF);  // 寄存器高字节
    iic_waitack();
    iic_sendbyte((uint8_t)reg & 0xFF);          // 寄存器低字节
    iic_waitack();

    for (i = 0; i < len; i++)
    {
        iic_sendbyte(buf[i]);
        ret = iic_waitack();
        if (ret != 0)
        {
            break;    // 从机无应答，退出
        }
    }
    iic_end();

    return (ret != 0) ? 1 : 0;
}


/**
 * @brief 通过 I2C 读 GT911 寄存器
 *
 * 使用 I2C 重复起始（Repeated START）实现写寄存器地址后读数据。
 *
 * @param reg 寄存器地址（16 位）
 * @param buf 接收缓冲区
 * @param len 要读取的字节数
 */
void touch_read(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;

    // 阶段一：写入寄存器地址
    iic_start();
    iic_sendbyte((0x14 << 1) | 0);   // 写地址 0x28
    iic_waitack();
    iic_sendbyte((uint8_t)(reg >> 8) & 0xFF);  // 寄存器高字节
    iic_waitack();
    iic_sendbyte((uint8_t)reg & 0xFF);          // 寄存器低字节
    iic_waitack();

    // 阶段二：重复起始，读取数据
    iic_start();
    iic_sendbyte((0x14 << 1) | 1);   // 读地址 0x29
    iic_waitack();

    // 读取 len-1 个字节，每读完一个发 ACK
    for (i = 0; i < len - 1; i++)
    {
        buf[i] = iic_recvbyte(1);     // 发 ACK → 继续读
    }
    // 最后一个字节发 NACK → 停止
    buf[i] = iic_recvbyte(0);         // 发 NACK → 结束

    iic_end();
}


/**
 * @brief 软件复位 GT911
 *
 * 向 0x8040 寄存器写 0x02 启动软复位，然后写 0x00 清除。
 */
void touch_sw_reset(void)
{
    uint8_t dat = 0x02;
    touch_write(0x8040, &dat, 1);
    Delay_SoftMs(10);

    dat = 0x00;
    touch_write(0x8040, &dat, 1);
}


/**
 * @brief 读取 GT911 产品 ID
 *
 * 从 0x8140 读取 4 字节：
 *   buf[0] = '9'  (0x39)
 *   buf[1] = '1'  (0x31)
 *   buf[2] = '1'  (0x31)
 *   buf[3] = 版本号（如 0x01）
 *
 * @param pid 输出 PID 字符串，只需判断前 3 字节是否为 "911"
 */
void touch_pid(char *pid)
{
    uint8_t buf[4] = {0};
    touch_read(0x8140, buf, 4);

    printf("PID 原始字节: %02X %02X %02X %02X\r\n",
           buf[0], buf[1], buf[2], buf[3]);

    pid[0] = buf[0];
    pid[1] = buf[1];
    pid[2] = buf[2];
    pid[3] = '\0';      // 截断版本号，只保留 "911"
    pid[4] = '\0';

    printf("触摸 IC PID: %s\r\n", pid);
}


/**
 * @brief 触摸屏完整初始化
 *
 * 调用顺序：
 *   I2C 引脚初始化 → 触摸引脚初始化 → 完整复位 → 读取 PID 验证
 *
 * @return uint8_t 0=成功，1=失败（未检测到 GT911）
 */
uint8_t touch_config(void)
{
    char pid[5];

    iic_init();          // 1. 初始化 I2C 通信引脚
    touch_init();        // 2. 初始化触摸控制引脚
    touch_reset();       // 3. 完整复位 GT911
    touch_pid(pid);      // 4. 读取 PID 验证通信

    // 检查 PID 是否为 "911"（只比较前 3 字节）
    if (strncmp(pid, "911", 3) != 0)
    {
        printf("触摸 IC 识别失败，PID: %s\r\n", pid);
        return 1;
    }

    printf("触摸 IC 识别成功: %s\r\n", pid);
    Delay_SoftMs(50);
    return 0;
}


/**
 * @brief 扫描触摸点
 *
 * 读取触摸状态寄存器，如果有触摸则解析各触摸点坐标，
 * 并根据当前屏幕方向做坐标转换。
 *
 * @param point 触摸点数组（输出）
 * @param cnt   期望读取的最大触摸点数（1~5）
 * @return uint8_t 实际触摸点数，0=无触摸
 */
uint8_t touch_scan(TouchPoint_t *point, uint8_t cnt)
{
    uint8_t tp_info, tp_cnt, i;
    uint8_t tpn_info[8];
    TouchPoint_t raw;
    Dir_t dir = lcd_dir;      // 当前屏幕旋转方向
    uint16_t w = lcd_width;
    uint16_t h = lcd_height;

    if (cnt == 0 || cnt > 5 || point == NULL)
        return 0;

    // 读取触摸状态寄存器（0x814E）
    touch_read(0x814E, &tp_info, 1);

    // bit7=1 表示有触摸事件
    if ((tp_info & 0x80) == 0x80)
    {
        tp_cnt = tp_info & 0x0F;    // 低 4 位 = 触摸点数
        if (tp_cnt > 5) tp_cnt = 5;
        if (tp_cnt > cnt) tp_cnt = cnt;

        for (i = 0; i < tp_cnt; i++)
        {
            // 读取第 i 个触摸点的 8 字节数据
            touch_read(touch_tp[i], tpn_info, 8);

            // 解析坐标（大端格式：高字节在前）
            // GT911 数据格式：ID | X_L | X_H | Y_L | Y_H | Size_L | Size_H
            raw.x    = ((uint16_t)tpn_info[2] << 8) | tpn_info[1];
            raw.y    = ((uint16_t)tpn_info[4] << 8) | tpn_info[3];
            raw.size = ((uint16_t)tpn_info[6] << 8) | tpn_info[5];

            // 根据屏幕方向进行坐标变换
            switch (dir)
            {
                case dir_0:
                    point[i].x = w - raw.y;
                    point[i].y = raw.x;
                    break;
                case dir_90:
                    point[i].x = raw.x;
                    point[i].y = raw.y;
                    break;
                case dir_180:
                    point[i].x = raw.y;
                    point[i].y = h - raw.x;
                    break;
                case dir_270:
                    point[i].x = w - raw.x;
                    point[i].y = h - raw.y;
                    break;
                default:
                    point[i].x = raw.x;
                    point[i].y = raw.y;
                    break;
            }
            point[i].size = raw.size;
        }

        // 清除触摸标志（写 0 到 0x814E），准备接收下一次触摸
        tp_info = 0;
        if (touch_write(0x814E, &tp_info, 1) != 0)
        {
            return 0;   // I2C 写入失败，本次触摸作废
        }

        return tp_cnt;
    }

    return 0;   // 无触摸
}
