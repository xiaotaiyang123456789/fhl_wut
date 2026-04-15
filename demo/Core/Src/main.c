/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "debug_console.h"   // 服务层头文件
#include "shell_app.h"       // 应用层头文件
#include "string.h"      // 用于字符串比较
#include "soft_spi.h"
#include "SPI_LCD.h"       // LCD 驱动
#include "key_app.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#define FW_VERSION  "V1.0.0"   // 软件版本号
#define ADC_CH_NUM 3
uint16_t adc_values[ADC_CH_NUM];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
 * @brief 执行系统软件复位（绕过所有封装，直接写寄存器）
 */
void SystemSoftReset(void)
{
    /* 1. 等待串口发送完成（如果正在使用 printf） */
    for (volatile int i = 0; i < 100000; i++) { __NOP(); }

    /* 2. 禁用全局中断，防止复位过程被打断 */
    __disable_irq();

    /* 3. 使用 SysTick 延时 1ms，确保复位信号稳定 */
    SysTick->LOAD = 72000 - 1;          // 假设系统时钟为 72MHz
    SysTick->VAL  = 0;
    SysTick->CTRL = 5;                  // 使用内核时钟，使能定时器
    while (!(SysTick->CTRL & (1 << 16))); // 等待计数到 0
    SysTick->CTRL = 0;
    printf("welcome to 111\r\n");
    /* 4. 直接写 SCB 寄存器触发系统复位 */
    SCB->AIRCR = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk);
    __DSB();   // 数据同步屏障
    __ISB();   // 指令同步屏障
    printf("welcome to 222\r\n");
    /* 5. 若复位失败，则死循环防止程序继续执行 */
    while(1);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM3_Init();
  MX_UART4_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim3);

  Console_Init();   // 初始化调试控制台（开启接收中断、打印欢迎信息）


  /* ---------- LCD12864 显示测试 ---------- */
  printf("Initializing LCD12864...\r\n");
  Lcd_Init();
  HAL_Delay(10);
  LCD_Clear();

  // 使用新接口显示
  lcd12864_display(0, 0, (uint8_t*)"Hello LCD!", 10);
  lcd12864_display(1, 0, (uint8_t*)"Val:123", 7);
  lcd12864_display(2, 0, (uint8_t*)"Ver: V1.0", 9);
  lcd12864_display(3, 0, (uint8_t*)"Num:20250414", 12);

  // 示例要求的调用
  lcd12864_display(2, 0, (uint8_t*)"Cviauto_6", 9);

  printf("LCD test done.\r\n");
  printf("> ");

  // ========== 新增：启动 ADC-DMA 并初始化按键 ==========
  printf("Starting ADC DMA for key scan...\r\n");
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_values, ADC_CH_NUM);
  Key_Init();  // 初始化按键状态和消抖结构
  HAL_Delay(100);
  LCD_Clear();

  // 初始显示：第一行 Check，第二三行显示字母=0
  lcd12864_display(0, 0, (uint8_t*)"Check", 5);
  lcd12864_display(1, 0, (uint8_t*)"A=0 B=0 C=0 D=0", 15);
  lcd12864_display(2, 0, (uint8_t*)"E=0 F=0 G=0 H=0", 15);
  // 读取初始 parking 并显示
  char buf[16];
  uint8_t init_parking = (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == GPIO_PIN_RESET) ? 0 : 1;
  snprintf(buf, sizeof(buf), "parking=%d", init_parking);
  lcd12864_display(3, 0, (uint8_t*)buf, strlen(buf));
  // ================================================

  printf("> ");



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//	  printf("welcome to cviauto\r\n");

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	    char cmd[128];
	    if (Console_ReadLine(cmd, sizeof(cmd))) {
	        printf("Got: %s\r\n", cmd);
	        Shell_Execute(cmd);
	        printf("> ");
	    }
	    // 按键扫描与 LCD 动态刷新（新增）
	    Key_ScanAndDisplay();

	    // ========== 调试代码：每秒打印 PB6 寄存器状态 ==========
	    static uint32_t last_diag = 0;
	    if (HAL_GetTick() - last_diag > 1000) {
	        last_diag = HAL_GetTick();
	        uint32_t idr = GPIOB->IDR;                     // 读取整个 IDR 寄存器
	        uint8_t idr_bit6 = (idr & GPIO_PIN_6) ? 1 : 0; // 提取第6位
	        uint8_t hal_bit6 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);
	        printf("IDR=0x%08lX, bit6=%d, HAL=%d\r\n", idr, idr_bit6, hal_bit6);
	    }
	    // ===================================================

	    HAL_Delay(10); // 10ms 扫描周期，同时保证 Shell 响应速度
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the Systick interrupt time
  */
  __HAL_RCC_PLLI2S_ENABLE();
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
