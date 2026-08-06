# AI 六轴机械臂 — 项目开发笔记

> 项目路径：`E:\STM32\JiXiebi`
> 主控：STM32F103ZET6（Cortex-M3，72MHz，512KB Flash，64KB RAM）
> 开发环境：Keil MDK v5（ARMCC V5.06）+ FreeRTOS + LVGL v7.11 + GUI Guider

---

## 一、项目概述

 AI 六轴机械臂，两种工作模式：

- **手动模式**：7 寸电容触摸屏（SSD1963 + GT911）控制 5 路舵机 + 1 路步进电机的姿态，可保存动作到 SPI Flash，掉电不丢，可循环播放。
- **自动模式**（后续阶段）：OpenMV 摄像头识别物块颜色与位姿 → 串口发送坐标 → 逆运动学计算 → 自动抓取放入对应颜色物料盒。

### 硬件清单

| 硬件 | 型号/规格 | 接口 |
|------|-----------|------|
| 主控 | STM32F103ZET6 | - |
| 屏幕 | 7寸 800×480，SSD1963 驱动 | FSMC 8080 并口 |
| 触摸 | GT911 电容触摸 | 软件 I2C |
| 舵机 ×5 | LD-1501MG / LD-20MG 数字舵机 | TIM2/TIM3 PWM |
| 步进电机 | OC57HB54-401A（57 步进，1.3N·m） | TB67S109A 驱动器 |
| 步进驱动器 | TB67S109A | PUL/DIR 脉冲 |
| 存储 | W25Q64 SPI Flash（8MB） | SPI2 |
| 视觉 | OpenMV（后续） | USART1 |

---

## 二、引脚分配总表

| 功能 | 引脚 | 外设/说明 |
|------|------|-----------|
| 舵机1（爪子） | PA1 | TIM2_CH2 |
| 舵机2 | PA2 | TIM2_CH3 |
| 舵机3 | PA3 | TIM2_CH4 |
| 舵机4 | PA6 | TIM3_CH1 |
| 舵机5 | PA7 | TIM3_CH2 |
| 步进 DIR | PB5 | GPIO 推挽 |
| 步进 STEP | PB6 | GPIO 推挽 |
| 串口调试 | PB10(TX)/PB11(RX) | USART3，115200 |
| LED0/LED1 | PE4/PE5 | GPIO 推挽 |
| LCD 数据线 | PD0~PD15 + PE7~PE15 | FSMC_D0~D15 |
| LCD RS | PG0 | FSMC_A10 |
| LCD CS | PG12 | FSMC_NE4 |
| LCD 背光 | PB0 | GPIO |
| GT911 INT/RST | PF10/PF11 | GPIO |
| GT911 I2C | PF9(SDA)/PB1(SCL) | 软件 I2C |
| W25Q64 CS | PB12 | GPIO（软件片选）|
| W25Q64 SPI | PB13(SCK)/PB14(MISO)/PB15(MOSI) | SPI2 |

---

## 三、系统架构（FreeRTOS 任务划分）

| 任务 | 优先级 | 周期 | 职责 |
|------|--------|------|------|
| Task_UartDeal | 3 | 20ms | 解析串口指令（S=舵机，M=步进） |
| Task_ServoCtrl | 2 | 20ms | 轮询 servo_target[]，变化则调 Servo_SetAngle |
| Task_StepperCtrl | 2 | 20ms | 轮询 stepper_target，执行 Stepper_Run 并累计位置 |
| Task_Lvgl | 2 | 5ms | lv_task_handler() 驱动 LVGL 事件循环 |
| Task_ActionPlay | 1 | 50ms | 播放动作组（舵机设角度 + 步进转差值） |
| LED_Task | 1 | 500ms | LED 翻转（心跳指示） |

**核心设计：全局变量 + 任务轮询解耦**

```
UI 滑块/按钮 → 修改 servo_target[] / stepper_target
     ↓
Task_ServoCtrl / Task_StepperCtrl 轮询到变化 → 驱动硬件
```

任何模块（UI、串口、动作播放、将来的 OpenMV）都能通过修改目标数组控制机械臂，互不干扰。

---

## 四、各模块实现要点（详细版）

---

### 1. 舵机控制（servo.c）

#### 1.1 舵机工作原理

数字舵机内部是一个**位置闭环系统**：

```
PWM 信号（脉宽）→ 控制电路 → 电机转动 → 齿轮组减速 → 输出轴
                    ↑                              ↓
                    └── 电位计位置反馈 ←───────────┘
```

- 控制信号是 **20ms 周期**的 PWM（50Hz）
- 脉宽 **0.5ms~2.5ms** 对应 **0°~180°**（线性）
- 脉宽 1.5ms = 90°（中位）
- 舵机收到脉宽后，内部电位计比较"当前角度"和"目标脉宽"，有偏差就转，直到一致

**关键认知**：舵机是**角度伺服**，你给它脉宽，它就转到对应角度并保持。不需要自己写 PID，舵机内部已经闭环了。

#### 1.2 PWM 参数计算（核心）

STM32F103 主频 72MHz，TIM2/TIM3 挂在 APB1（36MHz），但 TIM2/TIM3 的时钟倍频器是 ×2，所以定时器时钟 = 72MHz。

```
目标：PWM 周期 20ms，分辨率 1μs

预分频 PSC = 71 → 定时器时钟 = 72MHz / (71+1) = 1MHz（每 tick = 1μs）
自动重载 ARR = 19999 → 周期 = 20000 × 1μs = 20ms ✓

占空比（比较值 CCR）：
  0°   → 500μs  → CCR = 500
  90°  → 1500μs → CCR = 1500
  180° → 2500μs → CCR = 2500

角度→脉宽公式：CCR = 500 + 角度 × 2000 / 180
```

```c
// 角度转脉宽宏
#define ANGLE_TO_PULSE(angle)  (500 + (angle) * 2000 / 180)

// 设置舵机角度
void Servo_SetAngle(uint8_t servo_id, uint16_t angle)
{
    if (angle > 180) angle = 180;              // 限幅
    uint16_t pulse = ANGLE_TO_PULSE(angle);
    switch (servo_id)
    {
        case 1: TIM_SetCompare2(TIM2, pulse); break;   // PA1 → 爪子
        case 2: TIM_SetCompare3(TIM2, pulse); break;   // PA2 → 腕部旋转
        case 3: TIM_SetCompare4(TIM2, pulse); break;   // PA3 → 腕部俯仰
        case 4: TIM_SetCompare1(TIM3, pulse); break;   // PA6 → 小臂
        case 5: TIM_SetCompare2(TIM3, pulse); break;   // PA7 → 大臂
    }
}
```

#### 1.3 5 路舵机的引脚/定时器分配

| 舵机 | 关节 | 引脚 | 定时器通道 |
|------|------|------|-----------|
| 1 | 爪子 | PA1 | TIM2_CH2 |
| 2 | 腕部旋转 | PA2 | TIM2_CH3 |
| 3 | 腕部俯仰 | PA3 | TIM2_CH4 |
| 4 | 小臂 | PA6 | TIM3_CH1 |
| 5 | 大臂 | PA7 | TIM3_CH2 |

#### 1.4 平滑逼近（防止"砸下去"）

**问题**：直接写 `servo_target` → 舵机全速跳变，机械臂猛转。

**解决**：Task_ServoCtrl 每 20ms 只让舵机逼近目标 3°：

```c
// main.c Task_ServoCtrl 核心逻辑
int16_t diff = (int16_t)servo_target[i] - (int16_t)last_angle[i];  // 差值
int16_t step = (diff > 0) ? 3 : -3;    // 每轮走 3°
if (diff > 0 && diff < 3)  step = diff;   // 剩余不足 3° 直接到位
if (diff < 0 && diff > -3) step = diff;
last_angle[i] += step;
Servo_SetAngle(i, last_angle[i]);
```

效果：90° 行程约 0.6 秒平滑走完，动作柔和。

> **速度计算**：每轮走 3° × 每秒 50 轮（1000ms/20ms）= 150°/秒。90° ÷ 150°/秒 = 0.6 秒。想更快改 `step=5`（250°/秒），更慢改 `step=1`（50°/秒）。

#### 1.4.1 到位检测（关键设计）

**问题**：状态机/动作播放用**固定延时**（如 vTaskDelay(1000)）等舵机。但角度差大时（如 150°）需要 1 秒，固定 1 秒不够 → **上一个动作没走完，下一个就开始了**（动作错乱）。

**解决**：加"到位检测"——判断 5 路舵机的 `servo_target` 和 `last_angle` 是否一致：

```c
// main.c：判断是否还有舵机在移动
uint8_t Servo_IsMoving(void)
{
    for (uint8_t i = 1; i <= 5; i++)
    {
        if (servo_target[i] != last_angle[i])
            return 1;   // 还有没到位的
    }
    return 0;
}

// kinematic.c：等待到位（带超时保护，防止舵机卡住死等）
static void Wait_Servo_Arrive(uint32_t timeout_ms)
{
    uint16_t timeout = 0;
    while (Servo_IsMoving() && timeout < timeout_ms / 20)
    {
        vTaskDelay(pdMS_TO_TICKS(20));
        timeout++;
    }
}
```

**状态机/动作播放里的用法**：

```c
// 原来：vTaskDelay(pdMS_TO_TICKS(1000));   // 固定等 1 秒
// 现在：
Wait_Servo_Arrive(3000);   // 等舵机真正到位，最多 3 秒
```

**效果对比**：

| 场景 | 固定延时 | 到位检测 |
|------|---------|---------|
| 角度差 90° | 等 1 秒（可能没走完）| 等 0.6 秒（走完才切）|
| 角度差 10° | 等 1 秒（白等）| 等 0.07 秒（到位就切）|
| 舵机卡住 | 照样切（动作错乱）| 3 秒超时兜底（不死等）|

> **关键点**：到位检测基于 `last_angle`（平滑逼近的当前值），不是舵机物理位置——因为舵机没有反馈，软件认为"逼近完成"就视为到位。这是开环控制的常见做法。

#### 1.5 舵机发烫说明

通电后即使不动，舵机也在持续"保持角度"（内部闭环），电流流过线圈产生热量。**正常工作温度 50~70°C**。异常发烫（伴随啸叫/抖动）= 堵转或 PWM 信号异常。

---

### 2. 步进电机（steeper.c）

#### 2.1 与舵机的本质区别

| | 舵机 | 步进电机 |
|---|------|---------|
| 控制方式 | PWM 脉宽 = 绝对角度 | 脉冲数 = 相对步数 |
| 反馈 | 内部电位计闭环 | 无反馈（开环）|
| 控制量 | 位置（0~180°）| 转多少步 |
| 驱动 | 直接接 MCU | 需要驱动器（TB67S109A）|

**核心：一个脉冲 = 走一个微步**。步进电机本身不知道"自己在哪"，只响应脉冲。位置靠软件累计。

#### 2.2 硬件连接

```
PB5 → DIR（方向）：高=正转，低=反转
PB6 → STEP（脉冲）：一个上升沿走一步
（经 PC817 光耦隔离 → TB67S109A → 电机）
```

#### 2.3 脉冲产生（软件延时）

```c
// 步间延时 = 1秒 / 步数（决定转速）
// 800步/秒 → 1250μs
g_step_delay = 1000000 / speed;

void StepOnce(void)
{
    STEP_PORT->BSRR = STEP_PIN;   // 脉冲拉高
    Delay_us(2);                  // 保持 2μs（驱动器要求 ≥1μs）
    STEP_PORT->BRR = STEP_PIN;    // 脉冲拉低
    Delay_us(g_step_delay);       // 步间间隔（决定转速）
}

void Stepper_Run(int steps)
{
    if (steps > 0) DIR_PORT->BSRR = DIR_PIN;   // 正转
    else           DIR_PORT->BRR  = DIR_PIN;   // 反转
    Delay_us(5);                                // 方向稳定延时
    for (uint16_t i = 0; i < abs(steps); i++)
        StepOnce();                             // 阻塞式发脉冲
}
```

**注意**：`Stepper_Run` 是**阻塞式**的——发脉冲期间任务卡住（800 步 ≈ 1 秒）。步数多时会影响其他任务，但当前场景（几百步内）可接受。

#### 2.4 细分与角度换算

TB67S109A 拨码设置细分，决定"每步转多少度"：

| 细分 | 每度步数 | 一圈步数 |
|------|---------|---------|
| 整步 | 0.556 | 200 |
| 2 | 1.11 | 400 |
| **4（本机）** | **2.22** | **800** |
| 8 | 4.44 | 1600 |

```
每度步数 = 200 × 细分 / 360
本机 4 细分：200×4/360 = 2.22 步/度
```

#### 2.5 位置累计（关键设计）

步进电机是开环的，**位置必须软件记录**：

```c
// Task_StepperCtrl 执行后：
g_stepper_pos += stepper_target;   // 当前位置累计
```

**为什么存"位置"不存"步数"**：
- 保存动作时存**当前位置**（绝对）
- 播放时算差值：`目标位置 - 当前位置 = 要转的步数`
- 循环播放不会累积误差

#### 2.6 回零点约定

上电时 `g_stepper_pos = 0`，约定**0 = 正前方**（机械臂初始朝向）。回零 = 走到 0：

```c
if (g_stepper_pos != 0)
    stepper_target = -g_stepper_pos;   // 反向走回 0
```

---

### 3. 串口（uart.c）

#### 3.1 配置概要

- USART3，115200，PB10(TX)/PB11(RX)
- 接收用 **RXNE 中断**（每收到一个字节触发）+ **IDLE 空闲中断**（判断一帧结束）
- printf 重定向：`fputc` → `USART3_SendByte`

#### 3.2 中断接收设计（关键）

```c
void USART3_IRQHandler(void)
{
    if (RXNE)   // 收到一个字节
    {
        data = USART_ReceiveData(USART3);
        if (USART3_ReceiveBuffIndex < 254)
            USART3_ReceiveBuff[USART3_ReceiveBuffIndex++] = data;
    }
    if (IDLE)   // 一帧结束（总线空闲）
    {
        USART_ReceiveData(USART3);      // 清 IDLE 标志
        USART3_ReceiveBuffFlag = 1;     // 置位：有完整一帧
    }
}
```

**设计原则**：**中断只收数据 + 置标志位，解析在主循环任务里做**。为什么？
- 中断里做耗时操作会阻塞其他中断
- 解析涉及 strstr/atoi 等函数，不适合放中断
- 任务轮询到标志 → 调 USART3_Process() 解析

**IDLE 中断判帧原理**：串口收完一帧后，总线空闲一段时间 → 触发 IDLE 中断 → 说明"这一帧收完了"。

#### 3.3 指令协议

```
S<编号>,<角度>  → 舵机控制，如 S2,90
M,<方向>,<步数> → 步进控制，如 M,0,200（0正转/1反转）
```

解析用 `strstr` 找逗号 + `atoi` 转数字（atoi 遇到非数字自动停，正好用来分段解析）。

---

### 4. LCD（lcd.c）—— 从零基础详解

> **本部分从零讲起，是项目最核心也最容易卡住的部分。** 按"接口原理 → FSMC → 地址映射 → 初始化 → 读写"的顺序学习。

#### 4.1 LCD 接口方式对比：为什么用 FSMC 并口

| 接口 | 信号线 | 速度 | 适用 |
|------|--------|------|------|
| SPI | 3~4 根 | 慢 | 小屏、Flash |
| **FSMC 8080 并口** | 20+ 根 | **快** | **7寸大屏（本机）** |

**为什么 7 寸屏必须用并口**：
- 800×480 分辨率，一帧 = 384000 像素
- 刷新一帧数据量巨大，SPI 带宽不够
- FSMC 相当于把 LCD **映射到 CPU 内存空间**，写像素 = 写内存，一条指令完成

#### 4.2 FSMC 是什么（零基础理解）

**FSMC（灵活的静态存储控制器）** = STM32 内置的"外部存储器接口"。

**核心思想**：把外部设备（LCD/外部 SRAM/Flash）映射到 CPU 的地址空间。CPU 执行"写某个内存地址"的指令时，FSMC 自动产生对应的时序信号（片选、读写、数据），把数据送到外部设备。

```
CPU 写地址 0x6C000800 = 0x1234
        ↓
FSMC 自动产生：CS 拉低 → 地址信号 → 写使能 WR 拉低 → 数据上线 → WR 拉高
        ↓
LCD 收到数据 0x1234
```

**对程序员来说**：写 LCD 就像写普通变量一样简单：
```c
*(volatile uint16_t *)0x6C000800 = 0xF800;   // 往"内存地址"写，实际是 LCD 收到数据
```

#### 4.3 FSMC 地址映射原理（重点难点）

STM32F103ZE 的 FSMC Bank1 分为 4 个片选区（NE1~NE4）：

| 片选 | 地址范围 | 引脚 |
|------|---------|------|
| NE1 | 0x60000000~0x63FFFFFF | PD7 |
| NE2 | 0x64000000~0x67FFFFFF | PE7 |
| NE3 | 0x68000000~0x6BFFFFFF | PE8 |
| **NE4（本机）** | **0x6C000000~0x6FFFFFFF** | **PG12** |

**本机用 NE4** → 基地址 `0x6C000000`。

**关键：用地址线的某一位来控制 LCD 的 RS 引脚（命令/数据选择）**

SSD1963 的 RS 引脚（D/CX）决定当前传输的是**命令**还是**数据**：
- RS=0 → 命令
- RS=1 → 数据

STM32 把 **FSMC_A10（地址线第10位）** 接到 LCD 的 RS 引脚。FSMC_A10 由 CPU 地址的 **bit11** 控制（16 位模式下地址右移 1 位，A10 = HADDR[11]）。

```
写地址 0x6C0007FE：bit11 = 0 → FSMC_A10 = 0 → RS=0 → 命令
写地址 0x6C000800：bit11 = 1 → FSMC_A10 = 1 → RS=1 → 数据
```

**所以**：
```c
typedef struct {
    vu16 LCD_REG;   // 0x6C0007FE，bit11=0 → 命令
    vu16 LCD_RAM;   // 0x6C000800，bit11=1 → 数据
} LCD_TypeDef;

#define LCD  ((LCD_TypeDef *)0x6C0007FE)

// 写命令：LCD->LCD_REG = 命令值
// 写数据：LCD->LCD_RAM = 数据值
```

> **一句话记忆**：往 `0x6C0007FE` 写 = 发命令；往 `0x6C000800` 写 = 发数据。差异就在地址 bit11 不同，FSMC 自动帮你把 RS 引脚置高/置低。

#### 4.4 引脚分配

| 信号 | STM32 引脚 | 说明 |
|------|-----------|------|
| D0~D15 数据线 | PD0~PD15 + PE7~PE15 | 16 位数据 |
| RS（命令/数据）| PG0 | FSMC_A10 |
| CS（片选）| PG12 | FSMC_NE4 |
| WR（写使能）| PD5 | FSMC_NWE |
| RD（读使能）| PD4 | FSMC_NOE |
| 背光 | PB0 | GPIO 普通输出 |

#### 4.5 FSMC 时序配置（关键参数）

```c
void LCD_FSMCConfig(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);   // 开启 FSMC 时钟

    FSMC_NORSRAMTimingInitTypeDef timing;
    timing.FSMC_AddressSetupTime = 0;   // 地址建立时间（HCLK 周期）
    timing.FSMC_AddressHoldTime  = 0;   // 地址保持时间
    timing.FSMC_DataSetupTime    = 3;   // 数据建立时间（关键，调大更稳）
    timing.FSMC_AccessMode       = FSMC_AccessMode_A;

    FSMC_NORSRAMInitTypeDef fsmc;
    fsmc.FSMC_Bank            = FSMC_Bank1_NORSRAM4;      // NE4
    fsmc.FSMC_MemoryType      = FSMC_MemoryType_SRAM;     // 当 SRAM 用
    fsmc.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b; // 16 位
    fsmc.FSMC_WriteOperation  = FSMC_WriteOperation_Enable;
    fsmc.FSMC_ReadWriteTimingStruct = &timing;
    fsmc.FSMC_WriteTimingStruct     = &timing;

    FSMC_NORSRAMInit(&fsmc);
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);         // 使能
}
```

**DataSetupTime 的意义**：数据建立时间 = 数据线上数据稳定需要等几个 HCLK。设 3（约 42ns@72MHz）满足 SSD1963 要求（≥20ns）。**如果花屏，先试把这个值调大（6/9）**——但注意本机花屏另有原因（见踩坑）。

#### 4.6 SSD1963 初始化序列（按手册顺序）

```c
void LCD_Config(void)
{
    // 1. 配置 PLL（倍频）
    //    外部晶振 10MHz → VCO=300MHz → PLL=100MHz
    LCD_WR_REG(0xE2);
    LCD_WR_DATA(0x1D);   // N=29，VCO = 10×(29+1) = 300MHz
    LCD_WR_DATA(0x02);   // M=2，PLL = 300/(2+1) = 100MHz
    LCD_WR_DATA(0x04);
    Delay_us(100);

    // 2. 使能 PLL
    LCD_WR_REG(0xE0);
    LCD_WR_DATA(0x01);   // 开启
    Delay_SoftMs(10);
    LCD_WR_REG(0xE0);
    LCD_WR_DATA(0x03);   // PLL 作为系统时钟
    Delay_SoftMs(12);

    // 3. 软复位
    LCD_WR_REG(0x01);
    Delay_SoftMs(10);

    // 4. 像素时钟
    LCD_WR_REG(0xE6);
    LCD_WR_DATA(0x2F); LCD_WR_DATA(0xFF); LCD_WR_DATA(0xFF);

    // 5. 设置 LCD 模式（800×480，24位）
    LCD_WR_REG(0xB0);
    LCD_WR_DATA(0x20);                        // 24 位
    LCD_WR_DATA(0x00);                        // TFT
    LCD_WR_DATA((800-1)>>8); LCD_WR_DATA(800-1);    // 水平 799
    LCD_WR_DATA((480-1)>>8); LCD_WR_DATA(480-1);    // 垂直 479
    LCD_WR_DATA(0x00);

    // 6. 水平时序（HT/HPS/HPW）
    LCD_WR_REG(0xB4);
    LCD_WR_DATA(...);   // 按屏幕参数表

    // 7. 垂直时序（VT/VPS）
    LCD_WR_REG(0xB6);
    LCD_WR_DATA(...);

    // 8. 16 位数据格式（RGB565）
    LCD_WR_REG(0xF0);
    LCD_WR_DATA(0x03);

    // 9. 开启显示
    LCD_WR_REG(0x29);

    // 10. PWM 背光
    LCD_WR_REG(0xBE);
    LCD_WR_DATA(0x05); LCD_WR_DATA(0xFF); LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x00); LCD_WR_DATA(0x00); LCD_WR_DATA(0x00);
}
```

**初始化顺序为什么重要**：SSD1963 必须"先配 PLL → 再配时序 → 再开显示"，顺序错会白屏或花屏。每个 Delay 都是给芯片内部稳定时间。

#### 4.7 画点/填充/清屏（应用层）

```c
// 设置显示窗口（要写入的矩形区域）
void LCD_Set_Window(u16 sx, u16 sy, u16 ex, u16 ey)
{
    LCD_WR_REG(0x2A);   // 列地址
    LCD_WR_DATA(sx>>8); LCD_WR_DATA(sx&0xFF);
    LCD_WR_DATA(ex>>8); LCD_WR_DATA(ex&0xFF);
    LCD_WR_REG(0x2B);   // 行地址
    LCD_WR_DATA(sy>>8); LCD_WR_DATA(sy&0xFF);
    LCD_WR_DATA(ey>>8); LCD_WR_DATA(ey&0xFF);
    LCD_WR_REG(0x2C);   // 准备写 GRAM
}

// 清屏 = 设置全屏窗口 + 连续写颜色
void LCD_Clear(u16 color)
{
    LCD_Set_Window(0, 0, 799, 479);
    for (u32 i = 0; i < 800*480; i++)
        LCD->LCD_RAM = color;   // 写 384000 次
}
```

**原理**：设置窗口后，SSD1963 内部自动递增地址，连续写 `LCD_RAM` 就自动填满整个窗口（不需要每次指定坐标）。

#### 4.8 花屏问题（本机最重要的坑）

**现象**：LVGL 界面花屏（彩色竖条），但直接画纯色正常。

**排查过程**：
1. 全屏 `LCD_Color_Fill` 一次写 → 花屏
2. 逐像素画 → 正常
3. 逐行写 → 正常

**结论**：FSMC 连续大量写入时，**排线信号不稳定**（排线长 + 电机干扰），数据出错。

**解决**：LVGL 的 `disp_flush` 改成**逐行写入**：
```c
static void disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p)
{
    int32_t y;
    for (y = area->y1; y <= area->y2; y++)
    {
        LCD_Color_Fill(area->x1, y, area->x2, y, (uint16_t*)color_p);
        color_p += lv_area_get_width(area);   // 每行写完指针后移
    }
    lv_disp_flush_ready(drv);
}
```

> 逐行写 = 每行之间有一次函数调用间隙，信号有时间稳定。速度比全屏写慢一点，但稳定。

### 5. GT911 触摸（gt911.c）—— 从零基础详解

#### 5.1 GT911 是什么

GT911 是电容触摸控制芯片，负责**检测手指触摸位置**，通过 I2C 接口把坐标读出来。

**电容触摸原理**：手指接近屏幕时，触摸点的电容变化 → GT911 检测到 → 算出坐标。

#### 5.2 通信接口：软件 I2C（重点）

**为什么不用 STM32 硬件 I2C**：
- STM32F1 的硬件 I2C 有已知的 bug（总线锁死等），坑多
- 软件 I2C 用 GPIO 模拟时序，**完全可控、好调试**

**I2C 协议要点**（面试必问）：
```
起始条件：SCL 高电平时，SDA 由高变低
停止条件：SCL 高电平时，SDA 由低变高
数据传输：SCL 低时变数据，SCL 高时采样（数据在 SCL 高时必须稳定）
应答 ACK：第 9 个时钟，从机拉低 SDA
```

**开漏输出实现双向通信**：
```c
// GPIO 配置成开漏（GPIO_Mode_Out_OD）
// 写 1 = 释放总线（外部上拉拉高）
// 写 0 = 拉低
// 读用 GPIO_ReadInputDataBit 读引脚实际电平
```

**地址**：GT911 的 7 位地址是 0x14（复位时 INT 脚电平决定）：
```c
0x14 << 1 | 0 = 0x28   // 写地址
0x14 << 1 | 1 = 0x29   // 读地址
```

#### 5.3 引脚与复位时序

| 功能 | 引脚 | 说明 |
|------|------|------|
| SCL | PB1 | 开漏输出 |
| SDA | PF9 | 开漏输出 |
| INT | PF10 | 中断/地址选择 |
| RST | PF11 | 复位 |

**复位时序（必须按顺序）**：
```c
// 完整复位（决定 I2C 地址）
GPIO_ResetBits(GPIOF, GPIO_Pin_11);   // RST = 0
GPIO_SetBits(GPIOF, GPIO_Pin_10);     // INT = 1（此时地址 = 0x14）
Delay_SoftMs(20);
GPIO_SetBits(GPIOF, GPIO_Pin_11);     // RST = 1
Delay_SoftMs(300);                    // 等芯片启动
// INT 恢复为浮空输入
```

#### 5.4 读取触摸流程（核心）

```c
// 1. 读状态寄存器 0x814E
touch_read(0x814E, &tp_info, 1);
// bit7 = 1 → 有触摸
// 低 4 位 = 触摸点数

// 2. 有触摸则读坐标（每点 8 字节，从 0x814F 起）
touch_read(0x814F, tpn_info, 8);
// X = tpn_info[2]<<8 | tpn_info[1]   （大端）
// Y = tpn_info[4]<<8 | tpn_info[3]

// 3. 清除触摸标志（写 0 到 0x814E）—— 必须清！否则一直报有触摸
touch_write(0x814E, &zero, 1);
```

**为什么必须清除标志**：GT911 检测到触摸后一直保持状态位，读完后写 0 清除，才能接收下一次触摸。

#### 5.5 坐标转换（屏幕方向适配）

触摸原始坐标和屏幕显示方向不一定一致，需要旋转映射：

```c
switch (lcd_dir)   // 屏幕当前方向
{
    case dir_0:   // 竖屏
        point.x = w - raw.y;
        point.y = raw.x;
        break;
    case dir_90:  // 横屏
        point.x = raw.x;
        point.y = raw.y;
        break;
    // ...
}
```

#### 5.6 软件 I2C 踩坑

- **发送字节时 SCL 必须正确拉低**（曾经 GPIOF/GPIOB 写错，SCL 没拉低，通信全挂）
- **起始信号后 SCL 要拉低**（否则下一字节时序错）
- 读数据前要**释放 SDA**（写 1 让从机控制）
- 每读一个字节后要发 ACK/NACK 决定是否继续

### 6. SPI + W25Q64（spi.c / w25q64.c）

#### 6.1 SPI 是什么（零基础）

**SPI（串行外设接口）** = 4 根线的同步串行通信：
- SCK：时钟（主机产生）
- MOSI：主机→从机数据
- MISO：从机→主机数据
- CS：片选（低电平选中）

**全双工**：发送和接收同时进行——发一个字节的同时收到一个字节。

#### 6.2 SPI2 配置（本机）

```c
// 引脚：PB13(SCK) PB14(MISO) PB15(MOSI)，PB12(CS 软件控制)
SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;  // 18MHz
SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;      // 空闲高电平
SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;     // 第2边沿采样（模式3）
SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
SPI_InitStruct.SPI_NSS  = SPI_NSS_Soft;       // 软件片选
```

**时钟计算**：APB1 = 36MHz，最小分频 2 → 36/2 = 18MHz（W25Q64 支持 80MHz，够用）。

**为什么模式 3**：W25Q64 支持模式 0 和模式 3，选哪个都行，但要一致。

**单字节收发（全双工核心）**：
```c
uint8_t SPI_Send_Rec_Byte(uint8_t Byte)
{
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) != SET);  // 等发送空
    SPI_I2S_SendData(SPI2, Byte);     // 写 DR → 自动清 TXE
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) != SET); // 等收到
    return SPI_I2S_ReceiveData(SPI2); // 读 DR → 自动清 RXNE
}
```

**读数据为什么发 0xFF**：SPI 全双工，读必须同时发。发哑元字节（0xFF）把从机数据"顶"回来。

#### 6.3 W25Q64 特性（关键概念）

**① Flash 只能 1→0，不能 0→1**：
- 写 0 不需要擦除（1→0 直接写）
- 写 1 必须先擦除（擦除 = 整扇区变 0xFF）
- **所以写之前必须擦除扇区**（4KB 一个扇区）

**② 页大小 256 字节**：
- 一次页写命令最多写 256 字节
- 跨页写入必须拆分（写满当前页 → 写下一页）
- `sFLASH_WriteBuffer` 自动处理跨页

**③ 写流程**：
```
写使能(0x06) → 写命令(0x02)+地址 → 数据 → 等 WIP 位清零（查 0x05）
```

#### 6.4 关键 API

```c
W25Q64_ReadID_0x9F();        // 读 ID（0xEF4017），验证 SPI 通没通
sFLASH_EraseSector(addr);    // 擦除 4KB 扇区（写前必须）
sFLASH_WriteBuffer(buf, addr, len);  // 写数据（自动跨页）
sFLASH_ReadBuffer(buf, addr, len);   // 读数据（无跨页限制）
```

#### 6.5 SPI 踩坑

- **F1 的 GPIO 复用不需要 GPIO_Pin_AF**（那是 F4 的写法）
- **F1 的宏名和 F4 不同**：`SPI_BaudRatePrescaler`（F1）vs `SPI_BaudRatePres`（F4）、`SPI_Direction_2Lines_FullDuplex`（F1）vs `SPI_Direction_DualLine_FullDuplex`（F4）
- **sFLASH_WritePage 指针自增 bug**：`*pBuffer++` 后又写一次 `pBuffer++` → 每写 1 字节跳 1 字节 → 数据全乱（本项目最大坑，读回 5682/10310 乱值）

---

### 7. 动作管理（action.c）

#### 7.1 设计思路

把机械臂的**一组姿态**（5 路舵机角度 + 步进位置）保存为"动作点"，多个动作点组成"动作组"，存到 Flash 掉电不丢。

**为什么用外部 Flash 不用片内 Flash**：
- 片内 Flash 擦除会阻塞 CPU（擦除期间程序卡住）
- 外部 W25Q64 独立、容量大（8MB）、擦写不阻塞主程序

#### 7.2 数据结构

```c
typedef struct {
    uint16_t servo[6];    // servo[1]~servo[5] 舵机角度
    int32_t  stepper;     // 步进电机位置（不是步数！）
} Action_t;               // sizeof = 16 字节

#define MAX_ACTIONS 50    // 每组最多 50 个
// 3 组：action_list_group1/2/3，每组独立计数
```

**为什么存"位置"不存"步数"**：循环播放时如果存步数，每次播放都是"相对当前再走 N 步"，位置会累积漂移。存绝对位置 + 播放算差值（目标-当前）→ 永不漂移。

#### 7.3 Flash 布局

```
扇区1 (0x000000, 4KB)：组1 → [1字节数量] + [动作数据...]
扇区2 (0x001000, 4KB)：组2 → [1字节数量] + [动作数据...]
扇区3 (0x002000, 4KB)：组3 → [1字节数量] + [动作数据...]
```

每组独立扇区，互不干扰。首字节存数量，上电校验数量合法性（0~50）。

#### 7.4 保存/加载流程

```c
// 保存：擦除 → 写数量 → 写数据
sFLASH_EraseSector(flash_addr);                 // 1. 擦除整个扇区
sFLASH_WriteBuffer(p_count, flash_addr, 1);     // 2. 写数量（1字节）
sFLASH_WriteBuffer((uint8_t*)p_list, flash_addr+1, count * sizeof(Action_t));  // 3. 写数据

// 加载：读数量 → 校验 → 读数据
sFLASH_ReadBuffer(&count, flash_addr, 1);
if (count == 0 || count > MAX_ACTIONS) return;  // 数量非法 → 不加载
sFLASH_ReadBuffer((uint8_t*)p_list, flash_addr+1, count * sizeof(Action_t));
```

**为什么要校验数量**：Flash 擦除后是 0xFF，如果从未保存过，数量=255 > 50 → 判定无效，避免加载垃圾数据。

#### 7.5 播放（Task_ActionPlay）

```c
if (is_playing)
{
    // 5 路舵机设目标角度
    for (i = 1; i <= 5; i++)
        servo_target[i] = p_list[action_play_idx].servo[i];

    // 步进：目标位置 - 当前位置 = 转的步数
    stepper_target = p_list[action_play_idx].stepper - g_stepper_pos;

    vTaskDelay(1000);   // 等舵机到位
    action_play_idx++;
    if (action_play_idx >= count)
    {
        if (loop_enable) action_play_idx = 0;   // 循环
        else is_playing = 0;                     // 播完停止
    }
}
```

#### 7.6 动作组选择（UI 下拉列表）

- 界面下拉列表选组（list1~3 → 组1~3）
- 切换组时：`Action_StopPlay()` → `current_group = 选择` → `Action_LoadFromFlash()`

### 9. LVGL 移植 —— 从零基础详解（重点中的重点）

> LVGL 是嵌入式 GUI 库，本机用 v7.11。**学习路径**：理解 LVGL 运行机制 → 显示移植 → 输入移植 → 任务集成 → GUI Guider 配合。

#### 8.1 LVGL 是怎么跑起来的（整体认知）

LVGL 本身**不是操作系统**，是一个"画图 + 事件处理"库。它需要：
1. **一个定时心跳**（tick）：告诉 LVGL 时间流逝（动画、长按检测用）
2. **周期性调用 `lv_task_handler()`**：处理内部任务（刷新、事件分发）
3. **两个移植回调**：
   - `disp_flush`：LVGL 把画好的像素交给你 → 你写到 LCD
   - `touchpad_read`：LVGL 问你"手指在哪" → 你从 GT911 读坐标

```
主循环（每 5ms）
  ├── lv_task_handler()
  │     ├── 调 touchpad_read → 获取触摸坐标
  │     ├── 判断坐标落在哪个控件 → 触发事件回调
  │     ├── 检查哪些区域需要重绘 → 渲染到内部缓冲
  │     └── 调 disp_flush → 把缓冲内容刷到 LCD
  └── vTaskDelay(5ms)
```

**核心理解：LVGL 是"被驱动"的**——你不调 `lv_task_handler()`，它什么都不做。

#### 8.2 显示移植（lv_port_disp.c）—— 三步

**第一步：分配显示缓冲**

LVGL 画图需要一个"画布"（内部缓冲），画完后整个交给 disp_flush 刷到屏幕：

```c
// 缓冲 = 800 × 10 行像素（RGB565 每像素 2 字节）
static lv_color_t draw_buf_1[LV_HOR_RES_MAX * 10];   // 16KB
lv_disp_buf_init(&draw_buf_dsc_1, draw_buf_1, NULL, LV_HOR_RES_MAX * 10);
```

**为什么不能全屏缓冲（800×480×2=768KB）**：STM32 只有 64KB RAM，放不下。用"部分缓冲"（10 行），LVGL 分块渲染。

**第二步：注册显示驱动**

```c
lv_disp_drv_t disp_drv;
lv_disp_drv_init(&disp_drv);
disp_drv.hor_res = 800;         // 屏幕宽
disp_drv.ver_res = 480;         // 屏幕高
disp_drv.flush_cb = disp_flush; // 刷新回调
disp_drv.buffer = &draw_buf_dsc_1;
lv_disp_drv_register(&disp_drv);   // 注册
```

**第三步：实现 disp_flush（核心回调）**

LVGL 渲染完一块区域后调用它，参数：
- `area`：要刷新的屏幕区域（矩形）
- `color_p`：该区域的像素数据指针

```c
static void disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p)
{
    int32_t y;
    for (y = area->y1; y <= area->y2; y++)
    {
        LCD_Color_Fill(area->x1, y, area->x2, y, (uint16_t*)color_p);
        color_p += lv_area_get_width(area);   // 指针移到下一行
    }
    lv_disp_flush_ready(drv);   // ★ 必须调用！告诉 LVGL 刷完了
}
```

**`lv_disp_flush_ready` 为什么必须调**：LVGL 靠它知道"缓冲区可以复用了"。不调用 → LVGL 一直等 → 界面卡死。

**为什么逐行写不一次全刷**：本机全区域批量写会花屏（排线信号问题），逐行写稳定（见 4.8）。

#### 8.3 输入移植（lv_port_indev.c）—— 触摸

**注册输入设备**：
```c
lv_indev_drv_t indev_drv;
lv_indev_drv_init(&indev_drv);
indev_drv.type = LV_INDEV_TYPE_POINTER;   // 类型：触摸屏
indev_drv.read_cb = touchpad_read;        // 读取回调
lv_indev_drv_register(&indev_drv);
```

**实现 touchpad_read**：
```c
static bool touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    TouchPoint_t tp;
    if (touch_scan(&tp, 1))   // 从 GT911 读坐标
    {
        data->point.x = tp.x;
        data->point.y = tp.y;
        data->state = LV_INDEV_STATE_PR;   // 按下
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;  // 松开
    }
    return false;
}
```

**LVGL 如何用这个函数**：每次 `lv_task_handler()` 内部会调 `touchpad_read` 获取当前触摸状态 → 判断坐标落在哪个控件上 → 触发对应事件（CLICKED/PRESSED 等）→ 调用你注册的回调。

**事件回调注册**：
```c
lv_obj_set_event_cb(ui->screen_1_btn_5, btn_5_event_handler);
// 之后按钮被点击时，LVGL 自动调 btn_5_event_handler
```

#### 8.4 任务架构与线程安全（重要）

**LVGL 不是线程安全的**——所有 LVGL 操作（创建控件、改属性、刷界面）必须在**同一个任务**里做。

**本机架构**：
```c
// Task_Lvgl（唯一操作 LVGL 的任务）
void Task_Lvgl(void *pvParameters)
{
    setup_ui(&guider_ui);   // 创建界面

    for (;;)
    {
        if (g_lv_update_req)   // 别的任务请求更新界面
        {
            g_lv_update_req = 0;
            AutoDisp_Update(...);   // 在 LVGL 任务里操作控件（安全）
        }
        lv_task_handler();
        vTaskDelay(5ms);
    }
}

// 其他任务（如 Task_Test）只能置标志：
g_lv_update_req = 1;   // 不能直接调 LVGL API！
```

**为什么不能在别的任务直接调 LVGL API**：会和其他任务正在执行的 LVGL 操作冲突，破坏内部链表/状态 → 卡死或花屏（本机踩过）。

#### 8.5 tick 时钟源

LVGL 需要知道时间（动画、事件去抖）。本机用 FreeRTOS 的 tick 钩子：

```c
void vApplicationTickHook(void)
{
    lv_tick_inc(1);   // 每 1ms 喂一次
}
```

#### 8.6 GUI Guider 配合

- GUI Guider 是 NXP 的图形化界面设计工具，拖控件生成代码
- 生成物：`setup_scr_screen_x.c`（建界面）、`gui_guider.h`（控件结构体）、`events_init.c`（事件框架）
- 工作流：GUI Guider 设计界面 → 导出代码 → 手写事件回调 → LVGL 任务加载

**事件回调手写示例**：
```c
static void btn_5_event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        int16_t pct = lv_slider_get_value(guider_ui.screen_1_slider_1);
        // 注意：按钮回调里读滑块要用 guider_ui 全局引用，不能用 obj（obj 是按钮）
        ...
    }
}
```

#### 8.7 LVGL 版本差异坑（v7 vs v8）

本机是 **v7.11**，很多网上教程是 v8，API 不同：

| 功能 | v7 | v8 |
|------|----|----|
| 隐藏控件 | `lv_obj_set_hidden(obj, true)` | `lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN)` |
| 设置样式 | `lv_obj_set_style_local_bg_color(obj, PART, STATE, color)` | `lv_obj_set_style_bg_color(obj, color, 0)` |
| 对齐 | `lv_obj_align(obj, NULL, LV_ALIGN_CENTER, 0, 0)` | `lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0)` |
| 对齐枚举 | `LV_ALIGN_IN_TOP_LEFT` | `LV_ALIGN_TOP_LEFT` |
| 堆剩余 | `lv_mem_monitor(&mon)` → `mon.free_size` | `lv_mem_get_free()` |

**判断方法**：看编译报错，或查 `lv_conf.h` 顶部的版本号。

#### 8.8 LVGL 内存管理

- LVGL 自己的堆：`LV_MEM_SIZE`（本机 20KB），控件/动画都从这里分配
- **控件反复创建会内存泄漏**：不要每次收帧都 `lv_obj_create`，只创建一次、之后改属性
- 监听堆剩余：`lv_mem_monitor(&mon); mon.free_size`（调试用）

---

## 五、事件回调设计（events_init.c）

| UI 控件 | 事件 | 逻辑 |
|---------|------|------|
| slider_1~5 | VALUE_CHANGED | 百分比→角度（×180/100）→ servo_target[n] → 更新标签 |
| btn_5~14（add/sub） | CLICKED | 百分比 ±1 → 角度 → servo_target[n] → 滑块/标签同步 |
| slider_6 | VALUE_CHANGED | 显示步数值到 label_13 |
| btn_15/btn_16 | CLICKED | stepper_target = ±slider_6 值 |
| btn_1 保存 | CLICKED | Action_Add(servo_target, g_stepper_pos) + SaveToFlash |
| btn_3 自动执行 | CLICKED | 开关：is_playing ? Stop : Start |
| btn_4 删除 | CLICKED | 删除最后一个动作 + SaveToFlash |
| ddlist_1 | VALUE_CHANGED | 切换 current_group + LoadFromFlash |
| btn_2 返回 | CLICKED | 切回主菜单 |

**关键经验：**
- 滑块用 `VALUE_CHANGED`（拖动中实时触发），按钮用 `CLICKED`
- 按钮回调里读滑块值必须用 `guider_ui.screen_1_slider_X`，不能用事件参数 `obj`（obj 是按钮本身）

---

## 六、调试踩坑记录

| # | 问题 | 原因 | 解决 |
|---|------|------|------|
| 1 | 链接报 Undefined symbol GPIO_Init | 标准外设库 .c 没加进 Keil 工程 | 添加 stm32f10x_gpio.c 等 |
| 2 | 进 HardFault | Delay_ms 用了 vTaskDelay，调度器未启动时调用 | 拆成 Delay_SoftMs（软件循环）与 vTaskDelay（任务内） |
| 3 | 任务全卡死 | 任务内 Delay_ms 改成软件空转，高优先级任务独占 CPU | 任务内一律 vTaskDelay |
| 4 | printf 需连按多次 Run 才进 main | Keil 启动流程 + 半主机模式 | 勾选 Use MicroLIB |
| 5 | LVGL 花屏（彩色竖条） | 全区域一次写像素，排线信号不稳定 | disp_flush 改逐行写入 |
| 6 | 界面左右镜像 | LCD_Scan_Dir 方向不对 | R2L_D2U → L2R_U2D |
| 7 | 红蓝互换 | SSD1963 配置了 BGR 模式 | 颜色宏 RED/BLUE 值对调 |
| 8 | 动作保存读回乱值（5682 等） | sFLASH_WritePage 里 pBuffer++ 写了两次，跳字节 | 修复 + 整扇区擦除清脏数据 |
| 9 | 自动执行拉满 180° | Flash 里是早期 bug 写入的脏数据 | 擦除后重新保存 |
| 10 | 自动执行步进电机不动 | 保存时步进步数硬编码 0 | 保存位置、播放转差值 |
| 11 | 舵机发烫 | 舵机通电保持角度需要持续电流（正常现象） | 正常温度 50~70°C；异常伴随啸叫 |

---

## 七、RAM 资源规划（64KB）

| 占用 | 大小 |
|------|------|
| LVGL 显示缓冲（800×10×2） | 16KB |
| LVGL 堆（LV_MEM_SIZE） | 30KB |
| FreeRTOS 堆 | 17KB |
| 任务栈（6 任务 + 内核） | ~10KB |
| action 数组（3×50×16） | 2.4KB |
| 其他全局 | ~1KB |
| **合计** | **~76KB（超限）** |

> 实际编译通过说明链接器做了裁剪；后续加功能需关注 RAM，必要时减小 LV_MEM_SIZE 或显示缓冲。

---

## 八、自动模式（OpenCV 视觉抓取）

> 方案变更：用 **Orange Pi 5B + OpenCV（Python）** 替代原计划的 OpenMV，识别能力更强。

### 8.1 硬件链路

```
Orange Pi 5B (Python + OpenCV)
  → USB 摄像头 → AruCo 定位框 + HSV 颜色识别
  → 10字节帧 @ 9600 → USART1 (PA9/PA10)
  → STM32F103ZET6
```

### 8.2 通信协议（10 字节帧）

| 字节 | 内容 | 说明 |
|------|------|------|
| [0][1] | width | AruCo 框宽（大端）|
| [2][3] | height | AruCo 框高（大端）|
| [4] | color | 0=红 1=绿 2=蓝 |
| [5][6] | pos_y | 物块 Y 偏移（大端，有符号）|
| [7][8] | pos_x | 物块 X 偏移（大端，有符号）|
| [9] | checksum | 前 9 字节累加取低 8 位 |

> 注意：OpenCV 端 create_buffer 第4参(水平偏移)→buf[5:7]→解析为 pos_y；第5参(垂直偏移)→buf[7:9]→解析为 pos_x。**Pixel_To_Phys 里 pos_x 用于距离、pos_y 用于横向偏移**（坐标对应关系容易搞反，已踩坑修正）。

### 8.3 软件架构（任务分工）

| 任务 | 优先级 | 周期 | 职责 |
|------|--------|------|------|
| Task_Test | 1 | 50ms | 收帧 → 校验 → 过滤无效帧 → 像素转物理坐标 → 更新 g_target_data |
| Task_AutoGrab | 1 | 50ms | 独立轮询状态机（不依赖每帧数据，被遮挡也能推进）|
| Task_ServoCtrl | 2 | 20ms | 平滑逼近舵机（每轮 3°，防猛转）|
| Task_StepperCtrl | 2 | 20ms | 执行步进目标 + 位置累计 |

### 8.4 坐标转换

```
x(距离) = pos_x/框高 × 16 + 11        (BG_HIGH=16, BASE_LEN=11)
y(横向) = (框宽/2 - pos_y)/框宽 × 24   (BG_WIDTH=24)
dist = √(x²+y²)    alpha = atan2(y,x)
```

### 8.5 逆运动学（分段修正）

- 纯数学 Arm_Inverse 在真实机械臂上不落地（重力/零位/安装偏差）
- **Arm_Kinematic_2_Angle**：按距离区间分段查表（六轴工程实测参数），每段用不同的 y 偏移和 phi 角
- 舵机映射：大臂=servo_target[5]，小臂=[4]，腕部=[3]，爪子=[1]
- 步进对准：`stepper_target = -alpha × 2.22`（4 细分标定，负号=方向修正）

### 8.6 抓取状态机

```
ST_INIT → ST_ALIGN → ST_GRAB → ST_LIFT → ST_RELEASE → ST_HOME → 循环
```

| 状态 | 动作 | 延时 |
|------|------|------|
| ST_INIT | 舵机回初始(90/90/90/100/85) + 步进回零 | 1000ms |
| ST_ALIGN | 三道检查 → 分段逆运动学 → 步进对准 | 1000ms |
| ST_GRAB | 爪子闭合 172° | 500ms |
| ST_LIFT | 大臂抬 120° + 步进转放物区 | 800ms |
| ST_RELEASE | 爪子张开 85° | 500ms |
| ST_HOME | 回初始姿态 + 步进回零 | 500ms |

### 8.7 保护机制

| 机制 | 实现 |
|------|------|
| 颜色门控 | g_target_color==255 → 状态机不执行（上电不自动抓）|
| 颜色选择 | RED/GREEN/BLUE 按钮（toggle，再点取消）|
| STOP | g_auto_stop + g_target_color=255 + g_auto_halt 锁存 → 完全停止 |
| 重新开始 | 再点颜色按钮 → halt 解除 → 从头执行 |
| 数据过滤 | 框<100 / 偏移超范围 → 丢弃 |
| ST_ALIGN 三道检查 | 颜色匹配 + 2秒新鲜度 + 距离5~35cm |
| 舵机平滑 | 每 20ms 走 3°，防"砸下去"|

### 8.8 自动模式踩坑记录

| # | 问题 | 原因 | 解决 |
|---|------|------|------|
| 1 | OpenCV SSH 下崩溃 | cv2.imshow 需要 GUI | `export QT_QPA_PLATFORM=offscreen` |
| 2 | 抓取方向反（转后方）| Pixel_To_Phys 里 pos_x/pos_y 用反 | 交换 x/y 公式 |
| 3 | 机械臂"砸下去"| 舵机直接跳变 + 纯数学逆运动学 | 平滑逼近 + 分段修正表 |
| 4 | 抓取中停止 | 状态机依赖每帧数据，被遮挡无帧 | 独立任务轮询状态机 |
| 5 | 开机自动抓 | 颜色门控缺失 | g_target_color=255 默认不执行 |
| 6 | 步进方向反 | 机械臂方向与例程相反 | 目标取反 |
| 7 | 内存不足白屏 | 新增任务/变量超 64KB | 显示缓冲 10→5 行（后恢复）|
| 8 | 灯光太暗识别不到 | 环境问题 | 加补光灯 + 调低 HSV 阈值 |

### 8.9 待标定参数

| 参数 | 当前值 | 说明 |
|------|--------|------|
| 放物区位置 | 400-100×color | 例程假设值，需实测 |
| 爪子角度 | 85张/172闭/170夹 | 需按爪子实测 |
| 初始姿态 | 90/90/90/100/85 | 需确认自然姿态 |
| 步进系数 | 2.22 步/度 | 4 细分，已标定 |

---

## 九、后续计划

- [x] 自动模式：OpenCV 颜色/位姿识别 → USART1 通信（10 字节协议 + 校验和）
- [x] 逆运动学：坐标 → 各关节角度（分段修正）
- [x] 自动抓取状态机 + 颜色门控 + STOP
- [x] 放物区位置实测标定
- [x] 爪子角度实测标定
- [x] 灯光环境优化（补光灯）
- [x] 爪子电流检测（过载保护）
- [x] 串口协议扩展（查询当前角度、动作组管理指令）
