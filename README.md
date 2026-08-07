# AI-6AxisRobotArm — 六轴机械臂控制项目

> **STM32F103 + FreeRTOS + LVGL 的六轴机械臂**：触摸屏图形化控制 + 运动学算法 + 多自由度伺服驱动，基于标准外设库开发。

## 功能特点

- 🤖 六轴机械臂运动控制，包含**正/逆运动学**计算模块（kinematic）
- 🖥️ **LVGL 图形界面**（GUI Guider 设计）：触摸操作、状态显示、动作控制
- ⏱️ **FreeRTOS 实时系统**：多任务管理（界面、运动、通信、传感器独立运行）
- 🎛️ 舵机 + 步进电机混合驱动（servo / steeper 驱动模块）
- 📶 串口通讯（USART1），支持上位机联调
- 🗄️ W25Q64 SPI Flash 存储（字库/参数）
- 📄 项目开发笔记见仓库根目录 `AI六轴机械臂_项目开发笔记.md`

## 硬件组成

| 模块 | 说明 |
|------|------|
| 主控 | STM32F103（标准外设库，HD 型号） |
| 显示 | TFT 液晶屏（LVGL + 触摸 GT911） |
| 运动 | 6 轴舵机 / 步进电机驱动 |
| 存储 | W25Q64 SPI Flash |
| 其他 | LED、IIC 扩展、PWM 输出等 |

## 项目结构

```
user/        main.c + API 驱动层（servo/steeper/kinematic/lcd/gt911/w25q64 等）
lvgl/        LVGL 图形库 + GUI Guider 生成界面
FreeRTOS/    FreeRTOS 内核源码
STM32F10x_StdPeriph_Driver/   标准外设库
project/     Keil 工程（MDK-ARM）
startup/     启动文件
```

## 快速开始

1. 用 Keil 打开 `project/Demo.uvprojx`
2. 编译烧录到 STM32F103 主控板
3. 上电后屏幕显示控制界面，触摸操作或串口下发指令控制机械臂动作

---
*学习项目，欢迎 Star / Fork 交流。*
