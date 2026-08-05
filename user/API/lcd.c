#include "lcd.h"
#include "stdio.h"
/*
 * SSD1963 LCD 驱动
 * 接口：FSMC 8080 并行，16 位数据宽度
 * 分辨率：800 × 480
 *
 * 文件结构：
 *   1. 全局变量
 *   2. GPIO 初始化 (lcd_init)
 *   3. FSMC 时序配置 (LCD_FSMCConfig)
 *   4. 底层读写函数 (LCD_WR_REG / LCD_WR_DATA / LCD_RD_DATA)
 *   5. SSD1963 配置 (LCD_Config)
 *   6. 扫描方向与显示方向 (LCD_Scan_Dir / LCD_Display_Dir)
 *   7. 画点与填充 (LCD_DrawPoint / LCD_Fill / LCD_Clear)
 *   8. 字符显示 (LCD_ShowChar / LCD_ShowString)
 *   9. 背光控制 (LCD_SSD_BackLightSet)
 */

/*==============================================================================
 * 1. 全局变量
 *==============================================================================*/

u16 POINT_COLOR = WHITE;      // 画笔颜色（画点/画字时的前景色）
u16 BACK_COLOR = BLUE;        // 背景色

Dir_t lcd_dir;                // 屏幕方向（枚举：dir_0 / dir_90 / ...）
uint16_t lcd_width;           // 当前显示宽度
uint16_t lcd_height;          // 当前显示高度

_lcd_dev lcddev = {0};        // LCD 参数结构体（宽/高/ID/命令字等）


/*==============================================================================
 * 2. GPIO 初始化
 *
 * 将所有 FSMC 相关 GPIO 配置为复用推挽输出。
 * 引脚分配：
 *   PB0  → LCD 背光控制          （普通推挽输出）
 *   PD0~PD1, PD4~PD5, PD8~PD10  → FSMC 控制/数据线（复用推挽输出）
 *   PD14~PD15                     → FSMC 数据线
 *   PE7~PE15                      → FSMC 数据线
 *   PG0                           → RS（命令/数据选择，接 FSMC_A10）
 *   PG12                          → CS（片选，接 FSMC_NE4）
 *==============================================================================*/
void lcd_init(void)
{
    // 开启 GPIOB / GPIOD / GPIOE / GPIOG 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_GPIOD |
                           RCC_APB2Periph_GPIOE |
                           RCC_APB2Periph_GPIOG, ENABLE);

    GPIO_InitTypeDef lcd_init;
    lcd_init.GPIO_Speed = GPIO_Speed_50MHz;

    // PB0：背光控制（推挽输出）
    lcd_init.GPIO_Mode = GPIO_Mode_Out_PP;
    lcd_init.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOB, &lcd_init);

    // PD0/1/4/5/8/9/10/14/15：FSMC 控制与数据线（复用推挽输出）
    lcd_init.GPIO_Mode = GPIO_Mode_AF_PP;
    lcd_init.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_4  | GPIO_Pin_5 |
                         GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 |
                         GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD, &lcd_init);

    // PE7~PE15：FSMC 数据线（复用推挽输出）
    lcd_init.GPIO_Pin = GPIO_Pin_7  | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 |
                         GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 |
                         GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOE, &lcd_init);

    // PG0(RS)、PG12(CS)：FSMC 控制线（复用推挽输出）
    lcd_init.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_12;
    GPIO_Init(GPIOG, &lcd_init);
}


/*==============================================================================
 * 3. FSMC 时序配置
 *
 * 将 LCD 映射到 FSMC Bank1 NOR/SRAM4（NE4），地址范围 0x6C000000~0x6FFFFFFF。
 * 通过地址 bit11 控制 FSMC_A10（即 LCD 的 RS 引脚），
 *   bit11=0 → 命令，bit11=1 → 数据。
 *
 * 时序参数（根据 SSD1963 数据手册，72MHz HCLK）：
 *   AddressSetupTime = 0   → 地址建立 0 个 HCLK
 *   DataSetupTime    = 3   → 数据建立 3 个 HCLK（≈42ns，SSD1963 要求 ≥ 20ns）
 *   AccessMode       = A   → 模式 A（读写时序相同）
 *==============================================================================*/
void LCD_FSMCConfig(void)
{
    // 开启 FSMC 时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

    // 时序参数结构体
    FSMC_NORSRAMTimingInitTypeDef p;
    p.FSMC_AddressSetupTime      = 0;    // 地址建立时间（HCLK 周期数）
    p.FSMC_AddressHoldTime       = 0;    // 地址保持时间
    p.FSMC_DataSetupTime         = 3;    // 数据建立时间
    p.FSMC_BusTurnAroundDuration = 0;    // 总线周转时间
    p.FSMC_CLKDivision           = 0;    // 时钟分频（异步模式无效）
    p.FSMC_DataLatency           = 0;    // 数据延迟（同步模式用）
    p.FSMC_AccessMode            = FSMC_AccessMode_A;  // 模式 A

    // FSMC 配置结构体
    FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMInitStructure.FSMC_Bank                = FSMC_Bank1_NORSRAM4;  // NE4
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux      = FSMC_DataAddressMux_Disable;     // 地址/数据不复用
    FSMC_NORSRAMInitStructure.FSMC_MemoryType          = FSMC_MemoryType_SRAM;            // SRAM 类型
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth     = FSMC_MemoryDataWidth_16b;        // 16 位数据宽度
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode     = FSMC_BurstAccessMode_Disable;    // 禁止突发
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait    = FSMC_AsynchronousWait_Disable;   // 禁止异步等待
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity  = FSMC_WaitSignalPolarity_Low;     // 等待信号低有效
    FSMC_NORSRAMInitStructure.FSMC_WrapMode            = FSMC_WrapMode_Disable;           // 禁止绕回
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive    = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation      = FSMC_WriteOperation_Enable;      // 使能写操作
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal          = FSMC_WaitSignal_Disable;         // 禁止等待信号
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode        = FSMC_ExtendedMode_Disable;       // 禁止扩展模式
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst          = FSMC_WriteBurst_Disable;         // 禁止写突发
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;  // 读写共用时序
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = &p;

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);       // 写入 FSMC 寄存器
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);       // 使能 Bank4
}


/*==============================================================================
 * 4. 底层读写函数
 *
 * 往 LCD->LCD_REG 写 = 发送命令（地址 bit11=0 → FSMC_A10=0 → RS=0）
 * 往 LCD->LCD_RAM 写 = 发送数据（地址 bit11=1 → FSMC_A10=1 → RS=1）
 *==============================================================================*/

/**
 * @brief 写命令到 SSD1963
 * @param regval 命令字节
 */
void LCD_WR_REG(u16 regval)
{
    LCD->LCD_REG = regval;
}

/**
 * @brief 写数据到 SSD1963
 * @param data 数据值
 */
void LCD_WR_DATA(u16 data)
{
    LCD->LCD_RAM = data;
}

/**
 * @brief 从 SSD1963 读数据
 * @return 读到的 16 位数据
 */
u16 LCD_RD_DATA(void)
{
    vu16 ram;
    ram = LCD->LCD_RAM;
    return ram;
}

/**
 * @brief 写寄存器（命令+数据合并）
 * @param LCD_Reg      命令
 * @param LCD_RegValue 要写入的数据
 */
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
{
    LCD->LCD_REG = LCD_Reg;
    LCD->LCD_RAM = LCD_RegValue;
}

/**
 * @brief 准备写入 GRAM（显存）
 * 调用后连续写 LCD->LCD_RAM 即可填充像素
 */
void LCD_WriteRAM_Prepare(void)
{
    LCD->LCD_REG = lcddev.wramcmd;   // wramcmd = 0x2C
}


/*==============================================================================
 * 5. SSD1963 初始化配置
 *
 * 按 SSD1963 数据手册要求的初始化序列：
 *   PLL 配置 → 软复位 → 像素时钟 → 屏幕参数 → 数据格式 → 开启显示 → PWM 背光
 *==============================================================================*/

/**
 * @brief 读取 LCD 控制器 ID（用于验证通信是否正常）
 */
void LCD_ReadID(void)
{
    LCD_WR_REG(0xA1);           // 读 ID 命令
    LCD_RD_DATA();              // 第一个 dummy 数据
    lcddev.id  = LCD_RD_DATA(); // 高字节
    lcddev.id  = lcddev.id << 8;
    lcddev.id |= LCD_RD_DATA(); // 低字节
    printf("LCD ID: %X\r\n", lcddev.id);
}

/**
 * @brief SSD1963 寄存器配置序列
 *
 * 包括 PLL 倍频、屏幕时序、数据格式、背光 PWM 等。
 * 如果外部晶振不是 10MHz，需要调整 PLL 参数。
 */
void LCD_Config(void)
{
    /*---- 1. 配置 PLL 倍频 ----*/
    // 外部晶振 10MHz，目标 PLL=100MHz
    // VCO = OSC × (N+1) = 10 × 30 = 300MHz
    // PLL = VCO / (M+1) = 300 / 3 = 100MHz
    LCD_WR_REG(0xE2);
    LCD_WR_DATA(0x1D);    // N = 29
    LCD_WR_DATA(0x02);    // M = 2
    LCD_WR_DATA(0x04);    // 使能 N/M 参数生效
    Delay_us(100);

    /*---- 2. 使能 PLL ----*/
    LCD_WR_REG(0xE0);
    LCD_WR_DATA(0x01);    // 开启 PLL
    Delay_SoftMs(10);
    LCD_WR_REG(0xE0);
    LCD_WR_DATA(0x03);    // PLL 输出作为系统时钟
    Delay_SoftMs(12);

    /*---- 3. 软复位 ----*/
    LCD_WR_REG(0x01);
    Delay_SoftMs(10);

    /*---- 4. 设置像素时钟频率 ----*/
    LCD_WR_REG(0xE6);
    LCD_WR_DATA(0x2F);    // 像素时钟分频系数
    LCD_WR_DATA(0xFF);
    LCD_WR_DATA(0xFF);

    /*---- 5. 设置 LCD 模式 ----*/
    // 24 位模式，TFT 面板，800×480
    LCD_WR_REG(0xB0);
    LCD_WR_DATA(0x20);                              // 24 位模式
    LCD_WR_DATA(0x00);                              // TFT 模式
    LCD_WR_DATA((SSD_HOR_RESOLUTION - 1) >> 8);     // 水平像素高字节
    LCD_WR_DATA(SSD_HOR_RESOLUTION - 1);            // 水平像素低字节 (799)
    LCD_WR_DATA((SSD_VER_RESOLUTION - 1) >> 8);     // 垂直像素高字节
    LCD_WR_DATA(SSD_VER_RESOLUTION - 1);            // 垂直像素低字节 (479)
    LCD_WR_DATA(0x00);                              // RGB 序列

    /*---- 6. 设置水平时序 ----*/
    LCD_WR_REG(0xB4);
    LCD_WR_DATA((SSD_HT - 1) >> 8);
    LCD_WR_DATA(SSD_HT - 1);
    LCD_WR_DATA(SSD_HPS >> 8);
    LCD_WR_DATA(SSD_HPS);
    LCD_WR_DATA(SSD_HOR_PULSE_WIDTH - 1);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    /*---- 7. 设置垂直时序 ----*/
    LCD_WR_REG(0xB6);
    LCD_WR_DATA((SSD_VT - 1) >> 8);
    LCD_WR_DATA(SSD_VT - 1);
    LCD_WR_DATA(SSD_VPS >> 8);
    LCD_WR_DATA(SSD_VPS);
    LCD_WR_DATA(SSD_VER_FRONT_PORCH - 1);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    /*---- 8. 设置 CPU 接口为 16 位 ----*/
    LCD_WR_REG(0xF0);
    LCD_WR_DATA(0x03);    // 16-bit (565 格式)

    /*---- 9. 开启显示 ----*/
    LCD_WR_REG(0x29);

    /*---- 10. 关闭自动白平衡 ----*/
    LCD_WR_REG(0xD0);
    LCD_WR_DATA(0x00);    // disable

    /*---- 11. 配置 PWM 背光 ----*/
    LCD_WR_REG(0xBE);
    LCD_WR_DATA(0x05);    // PWM 频率
    LCD_WR_DATA(0xFE);    // PWM 占空比
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    /*---- 12. GPIO 引脚配置 ----*/
    LCD_WR_REG(0xB8);
    LCD_WR_DATA(0x03);    // 2 个 IO 口设为输出
    LCD_WR_DATA(0x01);    // 正常 IO 功能
    LCD_WR_REG(0xBA);
    LCD_WR_DATA(0x01);    // GPIO[1:0] 控制屏幕方向

    /*---- 13. 设置背光亮度 ----*/
    LCD_SSD_BackLightSet(100);   // 亮度 0~100
}


/*==============================================================================
 * 6. 扫描方向与显示方向
 *==============================================================================*/

/**
 * @brief 设置像素扫描方向（GRAM 写入时像素递增顺序）
 * @param dir 扫描方向宏（L2R_U2D / L2R_D2U / ...）
 *
 * 横屏（dir=1）时内部做了方向映射，确保操作符合物理方向。
 */
void LCD_Scan_Dir(u8 dir)
{
    u16 regval = 0;
    u16 dirreg = 0;
    u16 temp;

    // 横屏时转换方向编号
    if (lcddev.dir == 1)
    {
        switch (dir)
        {
            case 0: dir = 6; break;
            case 1: dir = 7; break;
            case 2: dir = 4; break;
            case 3: dir = 5; break;
            case 4: dir = 1; break;
            case 5: dir = 0; break;
            case 6: dir = 3; break;
            case 7: dir = 2; break;
        }
    }

    // 这里按照 SSD1963 处理（id==0x9341 是 ILI9341 分支，不走这里）
    // SSD1963 用 0x36 命令控制扫描方向
    switch (dir)
    {
        case L2R_U2D: regval |= (0 << 7) | (0 << 6) | (0 << 5); break;
        case L2R_D2U: regval |= (1 << 7) | (0 << 6) | (0 << 5); break;
        case R2L_U2D: regval |= (0 << 7) | (1 << 6) | (0 << 5); break;
        case R2L_D2U: regval |= (1 << 7) | (1 << 6) | (0 << 5); break;
        case U2D_L2R: regval |= (0 << 7) | (0 << 6) | (1 << 5); break;
        case U2D_R2L: regval |= (0 << 7) | (1 << 6) | (1 << 5); break;
        case D2U_L2R: regval |= (1 << 7) | (0 << 6) | (1 << 5); break;
        case D2U_R2L: regval |= (1 << 7) | (1 << 6) | (1 << 5); break;
    }
    dirreg = 0x36;
    regval |= 0x08;           // BGR=1（SSD1963 默认 RGB，需要 BGR 修正）
    LCD_WriteReg(dirreg, regval);

    // 非 SSD1963 才需要交换宽高（SSD1963 不做坐标交换）
    if (lcddev.id != 0x1963)
    {
        if (regval & 0x20)
        {
            if (lcddev.width < lcddev.height)
            {
                temp = lcddev.width;
                lcddev.width  = lcddev.height;
                lcddev.height = temp;
            }
        }
        else
        {
            if (lcddev.width > lcddev.height)
            {
                temp = lcddev.width;
                lcddev.width  = lcddev.height;
                lcddev.height = temp;
            }
        }
    }

    // 更新窗口范围为全屏
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(0); LCD_WR_DATA(0);
    LCD_WR_DATA((lcddev.width  - 1) >> 8); LCD_WR_DATA((lcddev.width  - 1) & 0xFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(0); LCD_WR_DATA(0);
    LCD_WR_DATA((lcddev.height - 1) >> 8); LCD_WR_DATA((lcddev.height - 1) & 0xFF);
}

/**
 * @brief 设置 LCD 显示方向（横屏/竖屏）
 * @param dir 0=竖屏(480×800)，1=横屏(800×480)
 */
void LCD_Display_Dir(u8 dir)
{
    if (dir == 0)     // 竖屏
    {
        lcddev.dir     = 0;
        lcddev.width   = 480;
        lcddev.height  = 800;
        lcddev.wramcmd = 0x2C;
        lcddev.setxcmd = 0x2A;
        lcddev.setycmd = 0x2B;
    }
    else              // 横屏
    {
        lcddev.dir     = 1;
        lcddev.width   = 800;
        lcddev.height  = 480;
        lcddev.wramcmd = 0x2C;
        lcddev.setxcmd = 0x2A;
        lcddev.setycmd = 0x2B;
    }

    LCD_Scan_Dir(L2R_U2D);    // 设置从左到右、从上到下的扫描方向

    lcd_dir    = lcddev.dir ? dir_90 : dir_0;
    lcd_width  = lcddev.width;
    lcd_height = lcddev.height;
}


/*==============================================================================
 * 7. 画点与填充
 *==============================================================================*/

/**
 * @brief 清屏（填充全屏为指定颜色）
 * @param color RGB565 颜色值
 */
void LCD_Clear(u16 color)
{
    u32 index;
    u32 totalpoint = (u32)lcddev.width * lcddev.height;   // 总像素数
    LCD_Set_Window(0, 0, lcddev.width - 1, lcddev.height - 1);
    LCD_WriteRAM_Prepare();                                 // 发送 0x2C 命令
    for (index = 0; index < totalpoint; index++)
    {
        LCD->LCD_RAM = color;
    }
}

/**
 * @brief 设置光标位置（下一个像素写入的坐标）
 * @param x 列坐标
 * @param y 行坐标
 */
void LCD_SetCursor(u16 x, u16 y)
{
    LCD_WR_REG(0x2A);                 // 列地址
    LCD_WR_DATA(x >> 8);
    LCD_WR_DATA(x & 0xFF);
    LCD_WR_DATA(x >> 8);
    LCD_WR_DATA(x & 0xFF);

    LCD_WR_REG(0x2B);                 // 行地址
    LCD_WR_DATA(y >> 8);
    LCD_WR_DATA(y & 0xFF);
    LCD_WR_DATA(y >> 8);
    LCD_WR_DATA(y & 0xFF);
}

/**
 * @brief LCD 完整初始化（外部调用的入口）
 *
 * 调用顺序：GPIO 初始化 → FSMC 配置 → 读 ID → SSD1963 配置 → 设方向 → 开背光 → 清屏
 */
void LCD_Init(void)
{
    lcd_init();             // 1. GPIO 初始化
    LCD_FSMCConfig();       // 2. FSMC 时序配置
    Delay_SoftMs(50);       // 等待硬件稳定
    LCD_ReadID();           // 3. 读 LCD ID（验证通信）
    LCD_Config();           // 4. SSD1963 寄存器配置
    LCD_Display_Dir(1);     // 5. 设置横屏（800×480）
    LCD_LED(1);             // 6. 点亮背光
    LCD_Clear(BLUE);        // 7. 清屏为蓝色
}

/**
 * @brief 画点（使用 POINT_COLOR 当前颜色）
 * @param x 列坐标
 * @param y 行坐标
 */
void LCD_DrawPoint(u16 x, u16 y)
{
    LCD_SetCursor(x, y);
    LCD_WR_REG(0x2C);                 // 写 GRAM 命令
    LCD->LCD_RAM = POINT_COLOR;
}

/**
 * @brief 快速画点（直接指定颜色）
 * @param x     列坐标
 * @param y     行坐标
 * @param color RGB565 颜色
 */
void LCD_Fast_DrawPoint(u16 x, u16 y, u16 color)
{
    LCD_Set_Window(x, y, x, y);       // 设置 1×1 窗口
    LCD->LCD_REG = lcddev.wramcmd;    // 0x2C
    LCD->LCD_RAM = color;
}

/**
 * @brief 快速画点（颜色指针版）
 * @param x     列坐标
 * @param y     行坐标
 * @param color 指向 RGB565 颜色的指针
 */
void LCD_Fast_DrawPoint2(u16 x, u16 y, u16 *color)
{
    LCD_Set_Window(x, y, x, y);
    LCD->LCD_REG = lcddev.wramcmd;
    LCD->LCD_RAM = *color;
}

/**
 * @brief 设置显示窗口（像素写入的矩形范围）
 * @param sx 起始列
 * @param sy 起始行
 * @param ex 结束列
 * @param ey 结束行
 */
void LCD_Set_Window(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
{
    LCD_WR_REG(0x2A);
    LCD_WR_DATA(sx >> 8);
    LCD_WR_DATA(sx & 0xFF);
    LCD_WR_DATA(ex >> 8);
    LCD_WR_DATA(ex & 0xFF);

    LCD_WR_REG(0x2B);
    LCD_WR_DATA(sy >> 8);
    LCD_WR_DATA(sy & 0xFF);
    LCD_WR_DATA(ey >> 8);
    LCD_WR_DATA(ey & 0xFF);
}

/**
 * @brief 在指定矩形区域填充单一颜色
 * @param sx,sy 左上角坐标
 * @param ex,ey 右下角坐标
 * @param color RGB565 颜色
 */
void LCD_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 color)
{
    u32 i;
    u16 width  = ex - sx + 1;
    u16 height = ey - sy + 1;
    u32 pixel_cnt = (u32)width * height;

    if (sx > ex || sy > ey) return;
    if (sx + width > lcddev.width || sy + height > lcddev.height) return;

    LCD_Set_Window(sx, sy, ex, ey);
    LCD_WR_REG(0x2C);

    for (i = 0; i < pixel_cnt; i++)
        LCD->LCD_RAM = color;
}

/**
 * @brief 在指定矩形区域填充颜色数组
 * @param sx,sy 左上角坐标
 * @param ex,ey 右下角坐标
 * @param color 颜色数组指针
 */
void LCD_Color_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 *color)
{
    u32 i;
    u16 width  = ex - sx + 1;
    u16 height = ey - sy + 1;
    u32 pixel_cnt = (u32)width * height;

    if (sx > ex || sy > ey) return;
    if (sx + width > lcddev.width || sy + height > lcddev.height) return;

    LCD_Set_Window(sx, sy, ex, ey);
    LCD_WR_REG(0x2C);

    for (i = 0; i < pixel_cnt; i++)
        LCD->LCD_RAM = *(color + i);
}


/*==============================================================================
 * 8. 字符显示（依赖 font.h 中的字库数组）
 *==============================================================================*/

/**
 * @brief 在指定位置显示一个 ASCII 字符
 * @param x,y  左上角坐标
 * @param num  字符 ASCII 码
 * @param size 字体大小（12/16/24）
 * @param mode 0=有背景，1=透明
 */
void LCD_ShowChar(u16 x, u16 y, u8 num, u8 size, u8 mode)
{
    u8 temp, t1, t;
    u16 y0 = y;
    u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);

    num = num - ' ';    // 字库从空格开始

    for (t = 0; t < csize; t++)
    {
        if (size == 12)      temp = asc2_1206[num][t];
        else if (size == 16) temp = asc2_1608[num][t];
        else if (size == 24) temp = asc2_2412[num][t];
        else return;

        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)
                LCD_Fast_DrawPoint(x, y, POINT_COLOR);
            else if (mode == 0)
                LCD_Fast_DrawPoint(x, y, BACK_COLOR);

            temp <<= 1;
            y++;
            if (y >= lcddev.height) return;
            if ((y - y0) == size)
            {
                y = y0;
                x++;
                if (x >= lcddev.width) return;
                break;
            }
        }
    }
}

/**
 * @brief 显示字符串
 * @param x,y  左上角坐标
 * @param size 字体大小
 * @param mode 0=有背景，1=透明
 * @param str  字符串指针
 */
void LCD_ShowString(u16 x, u16 y, u8 size, u8 mode, u8 *str)
{
    while (*str != '\0')
    {
        // 超出右边界则停止
        if (x + size / 2 > lcddev.width) break;
        LCD_ShowChar(x, y, *str, size, mode);
        x += size / 2;    // 字符宽度 ≈ 字号/2
        str++;
    }
}

// 以下中文字符显示函数依赖于 font.h 中的 hz1616 字库，如需使用取消注释
// void LCD_ShowHz(...) { ... }
// void LCD_ShowChineseStr(...) { ... }

//显示图片
//x,y:起点坐标
//width,height:区域大小
//*p:图片起始地址
void LCD_ShowPhoto(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *p)
{
	uint16_t i=0, j=0;
	uint16_t pcolor = 0;
	LCD_Set_Window(x, y, width, height);
	LCD->LCD_REG=lcddev.wramcmd;  //0x2C的命令
	for(j=0; j<height; j++)
	{
		for(i=0; i<width; i++)
		{
			pcolor = (*p<<8)|(*(p+1));
			LCD->LCD_RAM=pcolor;  //写显示颜色
			p+=2;
		}
	}
	LCD_Set_Window(x, y, lcddev.width, lcddev.height);
}

//显示汉字
void LCD_ShowHz(uint16_t x, uint16_t y, uint16_t backColor, uint16_t fontColor, uint8_t buff[])
{
        uint16_t cnt = 0;
        for(uint8_t i=0;i<32;i++) {
                for(uint8_t j=0;j<8;j++) {
                        if((buff[i] & (1<<(7-j))) != 0) {
                                LCD_Fast_DrawPoint(cnt%16+x, cnt/16+y, fontColor);        
                        }
                        else {
                                LCD_Fast_DrawPoint(cnt%16+x, cnt/16+y, backColor);
                        }
                        cnt++;        
                }
        }
}

/*==============================================================================
 * 9. 背光控制
 *==============================================================================*/

/**
 * @brief 设置 SSD1963 内部 PWM 背光亮度
 * @param pwm 亮度值（0~100）
 */
void LCD_SSD_BackLightSet(u8 pwm)
{
    LCD_WR_REG(0xBE);
    LCD_WR_DATA(0x05);              // PWM 频率
    LCD_WR_DATA(pwm * 2.55);        // 占空比（0~255 映射到 0~100）
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0xFF);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
}
