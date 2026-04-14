#ifndef __DEBUG_CONSOLE_H
#define __DEBUG_CONSOLE_H

#include <stdint.h>

/**
 * @brief 初始化调试控制台（开启接收中断，重定向 printf）
 */
void Console_Init(void);

/**
 * @brief 非阻塞读取一行命令
 * @param buf  存放命令的缓冲区
 * @param size 缓冲区大小
 * @return     0：暂无完整命令行；>0：读取到的字符数
 */
int Console_ReadLine(char *buf, uint32_t size);

/**
 * @brief 后台处理函数（目前可留空，保留扩展性）
 */
void Console_Process(void);

#endif


