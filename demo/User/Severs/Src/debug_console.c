#include "debug_console.h"
#include "main.h"              // 包含 HAL 库总头文件
#include "usart.h"             // 提供 huart4 外部声明
#include <stdio.h>
#include <string.h>

/* ---------- 环形缓冲区定义 ---------- */
#define RX_BUF_SIZE 128

static uint8_t rx_buf[RX_BUF_SIZE];
static volatile uint16_t rx_head = 0;   // 中断写入位置
static uint16_t rx_tail = 0;            // 主循环读取位置
static volatile uint8_t line_ready = 0; // 收到完整一行标志



extern UART_HandleTypeDef huart4;

/* ---------- printf 重定向（Keil / GCC 通用） ---------- */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart4, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

/* ---------- 必须添加此函数，供 GCC 的 printf 调用 ---------- */
int _write(int file, char *ptr, int len)
{
    HAL_UART_Transmit(&huart4, (uint8_t *)ptr, len, HAL_MAX_DELAY);
    return len;
}

/* ---------- 初始化 ---------- */
void Console_Init(void)
{
    // 1. 中止可能残留的接收中断，防止干扰
    HAL_UART_AbortReceive_IT(&huart4);

    // 2. 清空 UART 接收数据寄存器（F1 系列使用 DR 寄存器）
    __HAL_UART_FLUSH_DRREGISTER(&huart4);

    // 3. 清空软件环形缓冲区
    rx_head = 0;
    rx_tail = 0;
    line_ready = 0;
    memset(rx_buf, 0, sizeof(rx_buf));

    // 4. 开启 UART4 接收中断，每次接收 1 个字符
    HAL_UART_Receive_IT(&huart4, &rx_buf[rx_head], 1);
}

/* ---------- 非阻塞读取一行 ---------- */
int Console_ReadLine(char *buf, uint32_t size)
{
    if (!line_ready) {
        return 0;
    }

    uint32_t i = 0;
    while (rx_tail != rx_head && i < size - 1) {
        buf[i++] = rx_buf[rx_tail];
        rx_tail = (rx_tail + 1) % RX_BUF_SIZE;
    }
    buf[i] = '\0';
    line_ready = 0;
    return (int)i;
}

/* ---------- 后台处理（预留） ---------- */
void Console_Process(void)
{
    /* 可在此处添加与硬件无关的周期性任务 */
}

/* ---------- 串口接收中断回调（HAL 库自动调用） ---------- */
/* ---------- 串口接收中断回调（HAL 库自动调用） ---------- */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance != UART4) {
        return;
    }

    uint8_t ch = rx_buf[rx_head];

    // 过滤无效字符：如果不是回车/换行，且不是可打印 ASCII（空格到 ~），则丢弃
    if (ch != '\r' && ch != '\n' && (ch < 0x20 || ch > 0x7E)) {
        // 无效字符，直接重新启动接收中断，不存入缓冲区
        HAL_UART_Receive_IT(&huart4, &rx_buf[rx_head], 1);
        return;
    }

    /* 检测换行符：\r 或 \n 均视为命令结束 */
    if (ch == '\r' || ch == '\n') {
        if (rx_head != rx_tail) {
            line_ready = 1;
        }
        /* 换行符不存入缓冲区，也不移动写指针 */
    } else {
        /* 正常字符：存入缓冲区并移动写指针 */
        uint16_t next_head = (rx_head + 1) % RX_BUF_SIZE;
        if (next_head != rx_tail) {
            rx_buf[rx_head] = ch;
            rx_head = next_head;
        }
        /* 缓冲区满则丢弃新字符 */
    }

    /* 重新使能下一次接收中断 */
    HAL_UART_Receive_IT(&huart4, &rx_buf[rx_head], 1);
}


