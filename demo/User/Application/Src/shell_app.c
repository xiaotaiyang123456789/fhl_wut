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
    /* 去除前导空格 */
    while (*cmd == ' ') {
        cmd++;
    }

    if (strcmp(cmd, "checkVer") == 0) {
        printf("Software Version: %s\r\n", FIRMWARE_VERSION);
    }





    else if (strcmp(cmd, "reset") == 0) {
        printf("System will reset now...\r\n");
        for (volatile int i = 0; i < 100000; i++) { __NOP(); }
        __disable_irq();

        // 使用 SysTick 定时器实现延时，确保复位信号有足够时间生效
        SysTick->LOAD = 72000 - 1;  // 假设72MHz主频，延时1ms
        SysTick->VAL = 0;
        SysTick->CTRL = 5;          // 使能定时器，使用内核时钟
        while (!(SysTick->CTRL & (1 << 16))); // 等待1ms
        SysTick->CTRL = 0;

        // 执行系统复位
        SCB->AIRCR = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk);
        __DSB();
        __ISB();

        while(1);
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


