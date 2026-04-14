/*
 * SPI_LCD.h
 *
 *  Created on: Apr 14, 2026
 *      Author: user
 */

#ifndef APPLICATION_INC_SPI_LCD_H_
#define APPLICATION_INC_SPI_LCD_H_

#include <stdint.h>

/* LCD 行地址映射表（4行 × 8列） */
extern uint8_t LCD_addr[4][8];

/* 函数声明 */
void Lcd_WriteCmd(uint8_t Cmd);                             // LCD写指令
void Lcd_WriteData(uint8_t Dat);                            // LCD写数据
void Lcd_Init(void);                                        // LCD初始化
void LCD_Display_Words(uint8_t x, uint8_t y, uint8_t *str); // LCD显示文字
void LCD_Clear(void);                                       // LCD清屏
void LCD_Display_Picture(uint8_t *img);                     // LCD显示图片
void LCD_Display_8Number(uint8_t numb);                     // LCD显示8位数字
void LCD_Display_32Number(uint32_t numb);                   // LCD显示32位数字
void lcd12864_display(uint8_t row, uint8_t col, uint8_t* buffer, uint8_t length);

#endif /* APPLICATION_INC_SPI_LCD_H_ */

