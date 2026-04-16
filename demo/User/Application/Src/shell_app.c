#include "shell_app.h"
#include <string.h>
#include <stdio.h>
#include "main.h"   // 该文件已包含 CMSIS 核心头文件

/* 软件版本号（可根据版本管理工具自动生成） */
#define FIRMWARE_VERSION "V1.0.0"

/**
 * @brief 执行具体命令（应用层业务逻辑）
 */
void Shell_Execute(const char *cmd)
{
    // 去除前导空格
    while (*cmd == ' ') {
        cmd++;
    }

    // 如果命令长度大于1，且第一个字符不是 'c'、'r'、'h'（有效命令首字母），
    // 且第二个字符是有效命令首字母，则跳过第一个干扰字符。
    if (strlen(cmd) > 1) {
        char first = cmd[0];
        char second = cmd[1];
        if (first != 'c' && first != 'r' && first != 'h') {
            if (second == 'c' || second == 'r' || second == 'h') {
                cmd++;  // 跳过干扰前缀
            }
        }
    }

    if (strcmp(cmd, "checkVer") == 0) {
        printf("Software Version: %s\r\n", FIRMWARE_VERSION);
    }
    else if (strcmp(cmd, "reset") == 0) {
        printf("System will reset now...\r\n");
        for (volatile int i = 0; i < 100000; i++) { __NOP(); }
        NVIC_SystemReset();
    }
    else if (strcmp(cmd, "help") == 0 || strcmp(cmd, "h") == 0) {
        printf("\r\nAvailable commands:\r\n");
        printf("  checkVer  - Display firmware version\r\n");
        printf("  reset     - Software reset the MCU\r\n");
        printf("  help / h  - Show this help message\r\n");
    }
    else if (strlen(cmd) > 0) {
        printf("Unknown command: '%s'\r\n", cmd);
        printf("Type 'help' for list of commands.\r\n");
    }
}


