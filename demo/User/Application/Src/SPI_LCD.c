/*
 * SPI_LCD.c
 * 修改适配软件 SPI
 */

#include "SPI_LCD.h"
#include "soft_spi.h"
#include "main.h"

//------------------------------------------------------------------
uint8_t LCD_addr[4][8] = {
    {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87},   // 第一行
    {0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97},   // 第二行
    {0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F},   // 第三行
    {0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F}    // 第四行
};

// 发送缓冲区
static uint8_t spi_buf[1];

/**
 * @brief 通过软件 SPI 发送一个字节（供 LCD 内部使用）
 */
static void LCD_SPI_WriteByte(uint8_t byte)
{
    Soft_SPI_WriteByte(byte);
}

/**
 * @brief LCD 写指令
 */
void Lcd_WriteCmd(uint8_t Cmd)
{
    Soft_SPI_CS_Control(1);                     // 拉低 CS，选中 LCD
    HAL_Delay(1);                               // 代替忙检测

    spi_buf[0] = 0xF8;                          // 命令同步标志
    LCD_SPI_WriteByte(spi_buf[0]);

    spi_buf[0] = 0xF0 & Cmd;                    // 高四位
    LCD_SPI_WriteByte(spi_buf[0]);

    spi_buf[0] = Cmd << 4;                      // 低四位
    LCD_SPI_WriteByte(spi_buf[0]);

    Soft_SPI_CS_Control(0);                     // 拉高 CS，结束传输
}

/**
 * @brief LCD 写数据
 */
void Lcd_WriteData(uint8_t Dat)
{
    Soft_SPI_CS_Control(1);                     // 拉低 CS
    HAL_Delay(1);

    spi_buf[0] = 0xFA;                          // 数据同步标志
    LCD_SPI_WriteByte(spi_buf[0]);

    spi_buf[0] = 0xF0 & Dat;                    // 高四位
    LCD_SPI_WriteByte(spi_buf[0]);

    spi_buf[0] = Dat << 4;                      // 低四位
    LCD_SPI_WriteByte(spi_buf[0]);

    Soft_SPI_CS_Control(0);                     // 拉高 CS
}

/**
 * @brief LCD 初始化
 */
void Lcd_Init(void)
{
    Soft_SPI_Init();
    HAL_Delay(100);                     // 等待电源充分稳定（>40ms）

    // === ST7920 串行模式必须连续两次发送 0x30 ===
    Lcd_WriteCmd(0x30);                 // 第一次功能设定（8位，基本指令）
    HAL_Delay(10);                      // 延长延时，确保指令执行完毕
    Lcd_WriteCmd(0x30);                 // 第二次功能设定（关键！）
    HAL_Delay(10);

    Lcd_WriteCmd(0x0C);                 // 开显示，关光标
    HAL_Delay(10);
    Lcd_WriteCmd(0x01);                 // 清屏
    HAL_Delay(50);                      // 清屏需要较长时间（>10ms）
    Lcd_WriteCmd(0x06);                 // 进入设定点，光标右移，画面不动
    HAL_Delay(10);
}

/**
 * @brief 显示字符或汉字
 */
void LCD_Display_Words(uint8_t x, uint8_t y, uint8_t *str)
{
    Lcd_WriteCmd(LCD_addr[x][y]);   // 设置起始地址
    while (*str > 0) {
        Lcd_WriteData(*str++);
    }
}

/**
 * @brief 清屏
 */
void LCD_Clear(void)
{
    Lcd_WriteCmd(0x01);
    HAL_Delay(2);
}

/**
 * @brief 显示图片（函数内部逻辑未修改，保持原样）
 */
void LCD_Display_Picture(uint8_t *img)
{
    uint8_t x, y, i;
    Lcd_WriteCmd(0x34);             // 切换到扩充指令
    Lcd_WriteCmd(0x34);             // 关闭图形显示
    for (i = 0; i < 1; i++) {
        for (y = 0; y < 32; y++) {
            for (x = 0; x < 8; x++) {
                Lcd_WriteCmd(0x80 + y);      // 行地址
                Lcd_WriteCmd(0x80 + x + i);  // 列地址
                Lcd_WriteData(*img++);
                Lcd_WriteData(*img++);
            }
        }
    }
    Lcd_WriteCmd(0x36);             // 打开图形显示
    Lcd_WriteCmd(0x30);             // 切换回基本指令
}

/**
 * @brief 显示 uint8 数字
 */
void LCD_Display_8Number(uint8_t numb)
{
    uint8_t ch[3];
    ch[0] = numb / 100 + '0';
    ch[1] = (numb / 10) % 10 + '0';
    ch[2] = numb % 10 + '0';

    if (ch[0] != '0') {
        Lcd_WriteData(ch[0]);
        Lcd_WriteData(ch[1]);
        Lcd_WriteData(ch[2]);
    } else if (ch[1] != '0') {
        Lcd_WriteData(ch[1]);
        Lcd_WriteData(ch[2]);
    } else {
        Lcd_WriteData(ch[2]);
    }
}

/**
 * @brief 显示 uint32 数字
 */
void LCD_Display_32Number(uint32_t numb)
{
    uint8_t *p8 = (uint8_t *)&numb;
    uint8_t num[4];
    num[0] = *(p8 + 0);
    num[1] = *(p8 + 1);
    num[2] = *(p8 + 2);
    num[3] = *(p8 + 3);

    LCD_Display_8Number(num[3]);
    LCD_Display_8Number(num[2]);
    LCD_Display_8Number(num[1]);
    LCD_Display_8Number(num[0]);
}

/**
 * @brief 在指定行列显示字符串（支持英文、数字、空格、下划线）
 * @param row   行号 0~3
 * @param col   列号 0~15（每个中文字符位置占2列，英文字符占1列）
 * @param buffer 字符串指针
 * @param length 要显示的字符个数
 */
void lcd12864_display(uint8_t row, uint8_t col, uint8_t* buffer, uint8_t length)
{
    if (row >= 4 || col >= 16 || length == 0) {
        return;
    }

    // 计算起始地址块（每行8个地址，每个地址可显示两个英文字符）
    uint8_t addr_index = col / 2;   // 0~7
    uint8_t offset = col % 2;       // 0 或 1

    // 设置起始地址
    Lcd_WriteCmd(LCD_addr[row][addr_index]);

    // 若起始列为奇数，先写一个空格占位，使后续字符对齐到正确的半字位置
    if (offset == 1) {
        Lcd_WriteData(' ');
    }

    // 连续写入字符（ST7920 地址指针自动递增）
    for (uint8_t i = 0; i < length; i++) {
        Lcd_WriteData(buffer[i]);
    }
}


