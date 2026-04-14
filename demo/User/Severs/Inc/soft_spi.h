#ifndef __SOFT_SPI_H
#define __SOFT_SPI_H

#include <stdint.h>

/**
 * @brief 软件 SPI 初始化（仅设置初始电平，不重复初始化 GPIO）
 */
void Soft_SPI_Init(void);

/**
 * @brief 控制片选信号
 * @param enable 0：拉低 CS 使能设备；1：拉高 CS 禁用设备
 */
void Soft_SPI_CS_Control(uint8_t enable);

/**
 * @brief 通过软件 SPI 发送一个字节（高位在前，CPOL=0, CPHA=0）
 * @param data 要发送的 8 位数据
 */
void Soft_SPI_WriteByte(uint8_t data);

/**
 * @brief 通过软件 SPI 发送多个字节
 * @param data 数据缓冲区指针
 * @param len  数据长度
 */
void Soft_SPI_WriteBytes(const uint8_t *data, uint16_t len);

#endif


