#ifndef __LCD_H_
#define __LCD_H_

#include <stm32f10x.h>
#include "delay.h"       // 延时函数
#include "font.h"        // ASCII 字库（asc2_1206 / asc2_1608 / asc2_2412）

/*==============================================================================
 * 一、FSMC 地址映射
 *
 * SSD1963 通过 FSMC 8080 并行接口驱动，映射到 Bank1 NOR/SRAM4：
 *   地址范围：0x6C000000 ~ 0x6FFFFFFF  （片选 NE4 = PG12）
 *
 * FSMC_A10（PG0）接 LCD 的 RS 引脚，用于区分命令/数据：
 *   RS = 0 → 写命令
 *   RS = 1 → 写数据
 *
 * STM32 内部 HADDR[11] 控制 FSMC_A10：
 *   命令地址：HADDR[11]=0 → 0x6C0007FE
 *   数据地址：HADDR[11]=1 → 0x6C000800
 *==============================================================================*/

typedef struct {
    vu16 LCD_REG;   // 命令地址：0x6C0007FE（写 = 发命令）
    vu16 LCD_RAM;   // 数据地址：0x6C000800（写 = 发数据，读 = 读数据）
} LCD_TypeDef;

#define LCD  ((LCD_TypeDef *)0x6C0007FE)


/*==============================================================================
 * 二、LCD 分辨率
 *==============================================================================*/

#define SSD_HOR_RESOLUTION   800     // 水平像素（宽）
#define SSD_VER_RESOLUTION   480     // 垂直像素（高）

/*==============================================================================
 * 三、SSD1963 时序参数
 *
 * 根据 SSD1963 数据手册和屏幕规格计算：
 *   HT  = HOR_RESOLUTION + HBP + HFP
 *   VT  = VER_RESOLUTION + VBP + VFP
 *   HPS = HBP（水平脉冲开始位置）
 *   VPS = VBP（垂直脉冲开始位置）
 *==============================================================================*/

#define SSD_HOR_PULSE_WIDTH    1     // 水平同步脉冲宽度
#define SSD_HOR_BACK_PORCH    46     // 水平后廊（HBP）
#define SSD_HOR_FRONT_PORCH  210    // 水平前廊（HFP）
#define SSD_VER_PULSE_WIDTH    1     // 垂直同步脉冲宽度
#define SSD_VER_BACK_PORCH    23     // 垂直后廊（VBP）
#define SSD_VER_FRONT_PORCH   22     // 垂直前廊（VFP）

#define SSD_HT   (SSD_HOR_RESOLUTION + SSD_HOR_BACK_PORCH + SSD_HOR_FRONT_PORCH)  // 水平总周期
#define SSD_HPS  (SSD_HOR_BACK_PORCH)                                              // 水平脉冲起始
#define SSD_VT   (SSD_VER_RESOLUTION + SSD_VER_BACK_PORCH + SSD_VER_FRONT_PORCH)    // 垂直总周期
#define SSD_VPS  (SSD_VER_BACK_PORCH)                                              // 垂直脉冲起始


/*==============================================================================
 * 四、扫描方向定义
 *
 * 用于控制像素写入 GRAM 时的递增顺序。
 * 8 种方向组合：
 *   L2R = 从左到右   R2L = 从右到左
 *   U2D = 从上到下   D2U = 从下到上
 *==============================================================================*/

#define L2R_U2D  0     // 从左到右，从上到下（默认）
#define L2R_D2U  1     // 从左到右，从下到上
#define R2L_U2D  2     // 从右到左，从上到下
#define R2L_D2U  3     // 从右到左，从下到上
#define U2D_L2R  4     // 从上到下，从左到右
#define U2D_R2L  5     // 从上到下，从右到左
#define D2U_L2R  6     // 从下到上，从左到右
#define D2U_R2L  7     // 从下到上，从右到左


/*==============================================================================
 * 五、颜色定义（RGB565 格式）
 *
 * RGB565：R(5位) | G(6位) | B(5位)
 *   红色 = 0xF800   绿色 = 0x07E0   蓝色 = 0x001F
 *==============================================================================*/

#define WHITE     0xFFFF    // 白色
#define BLACK     0x0000    // 黑色
#define RED       0x001F    // 红色（LCD 为 BGR 模式，红蓝通道交换）
#define GREEN     0x07E0    // 绿色
#define BLUE      0xF800    // 蓝色（LCD 为 BGR 模式，红蓝通道交换）
#define YELLOW    0xFFE0    // 黄色（红+绿）
#define CYAN      0x7FFF    // 青色（绿+蓝）
#define MAGENTA   0xF81F    // 洋红（红+蓝）
#define BRED      0xF81F    // 棕红色
#define GRED      0xFFE0    // 黄绿色
#define GBLUE     0x07FF    // 青蓝色
#define BROWN     0xBC40    // 棕色
#define BRRED     0xFC07    // 棕红色
#define GRAY      0x8430    // 灰色


/*==============================================================================
 * 六、LCD 参数结构体
 *
 * 用于记录当前 LCD 的工作状态：
 *   width/height  当前显示分辨率（横屏/竖屏切换时变化）
 *   id            LCD 控制器型号（SSD1963 / ILI9341 等）
 *   dir           显示方向（0=竖屏，1=横屏）
 *   wramcmd/setxcmd/setycmd  寄存器命令字
 *==============================================================================*/

typedef struct {
    u16  width;        // LCD 当前宽度
    u16  height;       // LCD 当前高度
    u16  id;           // LCD 控制器 ID
    u8   dir;          // 方向：0=竖屏，1=横屏
    u16  wramcmd;      // 写 GRAM 命令（= 0x2C）
    u16  setxcmd;      // 设置 X 坐标命令（= 0x2A）
    u16  setycmd;      // 设置 Y 坐标命令（= 0x2B）
} _lcd_dev;


/*==============================================================================
 * 七、屏幕方向枚举
 *==============================================================================*/

typedef enum {
    dir_0   = 0x00,    // 不旋转
    dir_90  = 0x01,    // 顺时针 90°
    dir_180 = 0x02,    // 顺时针 180°
    dir_270 = 0x03,    // 顺时针 270°
} Dir_t;


/*==============================================================================
 * 八、背光控制宏
 *
 * PB0 控制 LCD 背光开关：
 *   LCD_LED(1) = 点亮，LCD_LED(0) = 熄灭
 * 亮度调节用 LCD_SSD_BackLightSet(pwm) 函数。
 *==============================================================================*/

#define LCD_LED(x)  ((x) ? (GPIO_SetBits(GPIOB, GPIO_Pin_0)) : (GPIO_ResetBits(GPIOB, GPIO_Pin_0)))


/*==============================================================================
 * 九、外部全局变量
 *==============================================================================*/

extern _lcd_dev lcddev;     // LCD 参数结构体
extern Dir_t    lcd_dir;    // 当前屏幕方向
extern uint16_t lcd_width;  // 当前显示宽度
extern uint16_t lcd_height; // 当前显示高度
extern u16      POINT_COLOR; // 画笔颜色
extern u16      BACK_COLOR;  // 背景色
extern char     hz1616[];    // 中文字库数组


/*==============================================================================
 * 十、函数声明
 *==============================================================================*/

/* ---- 初始化与配置 ---- */
void lcd_init(void);                    // GPIO 初始化
void LCD_FSMCConfig(void);              // FSMC 时序配置
void LCD_Init(void);                    // LCD 完整初始化（入口）
void LCD_Config(void);                  // SSD1963 寄存器配置
void LCD_ReadID(void);                  // 读取 LCD ID

/* ---- 底层读写 ---- */
void LCD_WR_REG(u16 regval);            // 写命令
void LCD_WR_DATA(u16 data);             // 写数据
u16   LCD_RD_DATA(void);               // 读数据
void LCD_WriteReg(u16 reg, u16 val);    // 写寄存器（命令+数据）
void LCD_WriteRAM_Prepare(void);        // 准备写 GRAM

/* ---- 扫描方向 ---- */
void LCD_Scan_Dir(u8 dir);              // 设置像素扫描方向
void LCD_Display_Dir(u8 dir);           // 设置横屏/竖屏

/* ---- 画点与填充 ---- */
void LCD_SetCursor(u16 x, u16 y);       // 设置光标
void LCD_Set_Window(u16 sx, u16 sy, u16 ex, u16 ey);  // 设置显示窗口
void LCD_DrawPoint(u16 x, u16 y);       // 画点（使用当前画笔颜色）
void LCD_Fast_DrawPoint(u16 x, u16 y, u16 color);    // 快速画点
void LCD_Fast_DrawPoint2(u16 x, u16 y, u16 *color);  // 快速画点（指针版）
void LCD_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 color);     // 填充单一颜色
void LCD_Color_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 *color); // 填充颜色数组
void LCD_Clear(u16 color);              // 清屏

/* ---- 字符显示（依赖 font.h 字库）---- */
void LCD_ShowChar(u16 x, u16 y, u8 num, u8 size, u8 mode);   // 显示单个字符
void LCD_ShowString(u16 x, u16 y, u8 size, u8 mode, u8 *str); // 显示字符串
void LCD_ShowHz(u16 x, u16 y, u16 backColor, u16 fontColor, u8 buff[]);  // 显示汉字
void LCD_ShowChineseStr(u16 x, u16 y, u16 backColor, u16 fontColor, char str[]); // 中文字符串

/* ---- 图像显示 ---- */
void LCD_ShowPhoto(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *p);

/* ---- 背光控制 ---- */
void LCD_SSD_BackLightSet(u8 pwm);      // 设置背光亮度 0~100

#endif /* __LCD_H_ */
