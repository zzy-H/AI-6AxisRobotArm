#ifndef __W25Q64_H_
#define __W25Q64_H_
#include "spi.h"
#include "stdio.h"
#include "stm32f10x.h"

/*********************************************
 * W25Qxx 系列 SPI Nor Flash 驱动头文件
 * 适用型号：W25Q64 / W25Q128 / W25Q32 等兼容标准指令的Flash芯片
 * 功能：包含SPI指令定义、硬件抽象宏、读写擦除功能接口声明
 *********************************************/

/* ====================== Flash 标准SPI指令码定义 ====================== */
#define sFLASH_CMD_WRITE          0x02  /* 页编程指令（Page Program）：向Flash写入数据，必须先执行写使能；单次写入不可跨页，单页最大256字节 */
#define sFLASH_CMD_WRSR           0x01  /* 写状态寄存器指令：修改Flash状态寄存器，配置写保护、四线模式QE位等硬件功能 */
#define sFLASH_CMD_WREN           0x06  /* 写使能指令（Write Enable）：所有写/擦除/写状态寄存器操作前必须发送，置位内部写使能锁存位WEL */
#define sFLASH_CMD_READ           0x03  /* 普通读数据指令：低速读取内存数据，无需虚拟字节，时钟速率通常≤50MHz */
#define sFLASH_CMD_RDSR           0x05  /* 读状态寄存器指令：读取芯片工作状态，用于判断忙闲、写使能状态、保护位等 */
#define sFLASH_CMD_RDID           0x9F  /* JEDEC ID读取指令：读取芯片厂商ID+存储类型+容量ID，用于芯片识别与型号校验（W25Q64返回0xEF4017） */
#define sFLASH_CMD_SE             0x20  /* 扇区擦除指令（Sector Erase）：擦除指定4KB扇区，擦除后所有数据变为0xFF；必须先执行写使能 */
#define sFLASH_CMD_BE             0xC7  /* 整片擦除指令（Bulk Erase / Chip Erase）：擦除整个芯片全部存储空间，耗时数秒，必须先执行写使能 */

/* ====================== Flash 状态标志与参数宏定义 ====================== */
#define sFLASH_WIP_FLAG           0x01  /* 写忙标志位（Write In Progress）：状态寄存器第0位，置1表示芯片正在执行写/擦除操作，0表示空闲 */
#define sFLASH_DUMMY_BYTE         0xA5  /* SPI虚拟填充字节：SPI为全双工总线，读取数据时主机需发送空字节提供时钟，该值无实际意义，仅作时钟填充 */
#define sFLASH_SPI_PAGESIZE       0x100 /* Flash页大小：单页256字节，页写入操作不可跨该边界，否则数据会在页内回卷覆盖 */

/* ====================== 硬件引脚与SPI接口抽象宏 ====================== */
/* 片选CS引脚操作：PB12引脚，低电平选中芯片启动通信，高电平释放芯片结束通信 */
#define sFLASH_CS_LOW()       GPIO_ResetBits(GPIOB, GPIO_Pin_12)  /* 拉低片选，开启本次SPI通信 */
#define sFLASH_CS_HIGH()      GPIO_SetBits(GPIOB, GPIO_Pin_12)    /* 拉高片选，结束本次SPI通信 */

/* SPI单字节收发函数别名：SPI为全双工机制，发送1字节的同时接收1字节，读写操作统一调用该底层函数 */
#define sFLASH_SendByte       SPI_Send_Rec_Byte

/* ====================== 功能函数声明 ====================== */
/**
 * @brief  使用0x90指令读取芯片ID（制造商ID+设备ID）
 * @note   与0x9F指令格式不同，0x90需发送地址参数，可读取双字节设备ID
 */
void W25Q64_ReadID_0x90(void);

/**
 * @brief  使用0x9F指令读取JEDEC标准ID
 * @note   返回3字节ID：厂商ID + 内存类型 + 容量值，驱动初始化时用于校验芯片是否正常连接
 */
void W25Q64_ReadID_0x9F(void);

/**
 * @brief  发送写使能指令
 * @note   所有写操作、擦除操作、写状态寄存器操作前必须调用本函数
 */
void sFLASH_WriteEnable(void);

/**
 * @brief  阻塞等待芯片写/擦除操作完成
 * @note   循环读取状态寄存器的WIP忙标志，直到芯片进入空闲状态才返回
 */
void sFLASH_WaitForWriteEnd(void);

/**
 * @brief  单页写入函数：向指定地址写入单页数据
 * @param  pBuffer        待写入数据缓冲区指针
 * @param  WriteAddr      写入起始地址（需保证不跨页边界）
 * @param  NumByteToWrite 写入字节数，最大256，不可超出当前页范围
 * @note   调用前需确保目标扇区已擦除，且已执行写使能
 */
void sFLASH_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);

/**
 * @brief  任意长度缓冲区写入函数
 * @param  pBuffer        待写入数据缓冲区指针
 * @param  WriteAddr      写入起始地址（支持任意地址）
 * @param  NumByteToWrite 写入总字节数，支持跨页、跨扇区
 * @note   内部自动处理跨页拆分，调用前需确保目标存储区域已擦除为全0xFF
 */
void sFLASH_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);

/**
 * @brief  任意长度数据读取函数
 * @param  pBuffer       接收数据的缓冲区指针
 * @param  ReadAddr      读取起始地址（支持任意地址）
 * @param  NumByteToRead 读取总字节数，无页/扇区限制
 */
void sFLASH_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);

/**
 * @brief  扇区擦除函数：擦除指定地址所在的4KB扇区
 * @param  SectorAddr  扇区内任意地址，函数自动对齐到扇区起始地址
 * @note   擦除后扇区内所有数据变为0xFF，操作内部会自动执行写使能
 */
void sFLASH_EraseSector(uint32_t SectorAddr);

#endif
