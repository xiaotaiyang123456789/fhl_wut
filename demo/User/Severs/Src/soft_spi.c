#include "soft_spi.h"
#include "main.h"
#include "gpio.h"   // 包含引脚宏定义（若需要）

/* 引脚定义（必须与 gpio.c 中配置的一致） */
#define SCK_PORT    GPIOA
#define SCK_PIN     GPIO_PIN_5
#define MOSI_PORT   GPIOA
#define MOSI_PIN    GPIO_PIN_6
#define CS_PORT     GPIOA
#define CS_PIN      GPIO_PIN_7

/* 延时计数（可根据实际主频调整） */
#define SPI_DELAY_CNT   500

/**
 * @brief 软件 SPI 初始化
 * @note  GPIO 初始化已在 MX_GPIO_Init() 中完成，此处仅设置初始电平
 */
void Soft_SPI_Init(void)
{
    /* 设置初始电平：CS 低（未选中），SCK 低，MOSI 低 */
    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);   // 改为 RESET
    HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOSI_PORT, MOSI_PIN, GPIO_PIN_RESET);
}

/**
 * @brief 控制片选信号
 */
void Soft_SPI_CS_Control(uint8_t enable)
{
    if (enable) {
        HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);   // 拉高选中
    } else {
        HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET); // 拉低释放
    }
}

/**
 * @brief 发送一个字节（高位在前，模式 CPOL=0, CPHA=0）
 */
void Soft_SPI_WriteByte(uint8_t data)
{
    for (int i = 7; i >= 0; i--) {
        /* 准备数据位 */
        if (data & (1 << i)) {
            HAL_GPIO_WritePin(MOSI_PORT, MOSI_PIN, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(MOSI_PORT, MOSI_PIN, GPIO_PIN_RESET);
        }

        /* 产生时钟上升沿 */
        HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_SET);
        for (volatile int d = 0; d < SPI_DELAY_CNT; d++);   // 延时

        /* 产生时钟下降沿 */
        HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_RESET);
        for (volatile int d = 0; d < SPI_DELAY_CNT; d++);   // 延时
    }
}

/**
 * @brief 发送多个字节
 */
void Soft_SPI_WriteBytes(const uint8_t *data, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) {
        Soft_SPI_WriteByte(data[i]);
    }
}


