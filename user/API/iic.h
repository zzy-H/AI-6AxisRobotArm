#ifndef __IIC_H_
#define __IIC_H_

#include "stm32f10x.h"
#include "delay.h"

void iic_init(void); //I2C初始化
void iic_start(void);//发送起始信号
void iic_end(void);//发送结束信号
uint8_t iic_waitack(void);//等待应答
void iic_ack(void);//发送应答
void iic_nack(void);//发送非应答
void iic_sendbyte(uint8_t byte);//发送字节
uint8_t iic_recvbyte(uint8_t ack);//接收字节

#endif
