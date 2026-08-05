# AI 六轴机械臂 — 项目开发笔记

> 项目路径：`E:\STM32\JiXiebi`
> 主控：STM32F103ZET6（Cortex-M3，72MHz，512KB Flash，64KB RAM）
> 开发环境：Keil MDK v5（ARMCC V5.06）+ FreeRTOS + LVGL v7.11 + GUI Guider

---

## 一、项目概述

信盈达视觉 AI 六轴机械臂，两种工作模式：

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

## 四、各模块实现要点

### 1. 舵机控制（servo.c）

- TIM2/TIM3 输出 50Hz PWM（周期 20ms），预分频 71 → 1MHz，ARR=19999
- 脉宽 = 500 + 角度 × 2000/180（0°=500μs，90°=1500μs，180°=2500μs）
- `Servo_SetAngle(id, angle)` 通过 TIM_SetCompareX 更新脉宽

### 2. 步进电机（steeper.c）

- PB5=DIR 方向，PB6=STEP 脉冲，软件延时产生脉冲
- `Stepper_SetSpeed(800)`：步间延时 = 1000000/800 = 1250μs
- `Stepper_Run(steps)`：正数正转，负数反转，阻塞式发脉冲
- 位置累计：`g_stepper_pos += stepper_target`（保存/播放用位置而非步数）

### 3. 串口（uart.c）

- USART3，115200，接收中断 RXNE + IDLE（空闲中断判断一帧结束）
- 中断只收数据 + 置标志位，主循环任务解析（中断里不做解析，避免阻塞）
- printf 重定向到 fputc → USART3

指令格式：

```
S<编号>,<角度>   → 舵机控制，如 S2,90
M,<方向>,<步数>  → 步进控制，如 M,0,200（0正转/1反转）
```

### 4. LCD（lcd.c）

- SSD1963 通过 FSMC Bank1 NOR/SRAM4（NE4）驱动，16 位数据宽度
- 地址映射：命令 0x6C0007FE，数据 0x6C000800（地址 bit11 控制 RS）
- 初始化序列：GPIO → FSMC 时序 → PLL 配置 → 屏幕时序参数 → 开显示 → 背光
- **坑：全屏一次写入会花屏（排线信号不稳定），改为逐行写入解决**

### 5. GT911 触摸（gt911.c）

- 软件 I2C（PB1=SCL，PF9=SDA），开漏输出实现双向
- 7 位地址 0x14 → 写 0x28 / 读 0x29
- 复位时序：RST 拉低 → INT 拉高 → 20ms → RST 拉高 → 300ms
- 读取流程：0x814E 状态寄存器（bit7=有触摸，低4位=点数）→ 坐标寄存器 0x814F 起
- 坐标转换：根据 LCD 方向（lcd_dir）做旋转映射

### 6. SPI + W25Q64（spi.c / w25q64.c）

- SPI2 主机，18MHz（APB1 36MHz / 2 分频），模式 3（CPOL=High, CPHA=2Edge）
- 写前必须擦除扇区（Flash 只能 1→0）；页大小 256 字节，跨页写入需拆分
- 关键 API：ReadID / EraseSector / WriteBuffer / ReadBuffer

### 7. 动作管理（action.c）

- 3 个动作组 × 每组最多 50 个动作点
- `Action_t { uint16_t servo[6]; int32_t stepper; }`（16 字节）
- Flash 布局：每组一个 4KB 扇区，第 1 字节存动作数量
- 保存 = 擦除扇区 → 写数量 → 写数据；加载 = 读数量 → 校验 → 读数据
- **步进电机存"位置"而非"步数"**：播放时 `目标位置 - 当前位置 = 转动的步数`

### 8. LVGL + GUI Guider

- LVGL v7.11，800×480，RGB565，单缓冲（10 行）
- `lv_port_disp.c`：disp_flush 逐行调用 LCD_Color_Fill
- `lv_port_indev.c`：touchpad_read 调 touch_scan 获取坐标
- tick 来源：`vApplicationTickHook` 里 `lv_tick_inc(1)`
- 界面由 GUI Guider 生成（setup_scr_screen_x.c），事件回调在 events_init.c 手写

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
