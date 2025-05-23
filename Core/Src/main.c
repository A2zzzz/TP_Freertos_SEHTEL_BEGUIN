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
#include "cmsis_os.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define STACK_SIZE 256
#define TASK1_PRIORITY 1
#define TASK2_PRIORITY 2
#define TASK1_DELAY 1
#define TASK2_DELAY 2


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
TaskHandle_t LedTaskHandle;
TaskHandle_t TaskGiveHandle;
TaskHandle_t TaskTakeHandle;
xQueueHandle QueueTask;
SemaphoreHandle_t xMutex;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
void StartLedTask(void *argument);
void TaskGive(void *argument);
void TaskTake(void *argument);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch) {
HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
return ch;
}

void StartLedTask(void *argument) {
  uint8_t ledState = 0;
  vTaskSuspend( NULL); // Suspend itself);
  printf("Entering LedTask\n");
  for (;;) {
    ledState = !ledState;
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, ledState ? GPIO_PIN_SET : GPIO_PIN_RESET);
    printf("LED : %s\r\n", ledState ? "ON" : "OFF");
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void TaskGive(void *argument) {
  uint32_t delay = 100;
  vTaskSuspend( NULL); // Suspend TaskGive
  for (;;) {
    printf("TaskGive: Sending delay value %lu to queue\r\n", delay);
    if (xQueueSend(QueueTask, &delay, pdMS_TO_TICKS(100)) != pdPASS) {
      printf("TaskGive: Failed to send delay value to queue\r\n");
    }
    delay += 100; // Increment delay
    vTaskDelay(pdMS_TO_TICKS(delay));
  }
}

void TaskTake(void *argument) {
  uint32_t receivedValue = 0;
  vTaskSuspend( NULL); // Suspend TaskTake 
  for (;;) {
    printf("TaskTake: Waiting to receive value from queue\r\n");
    if (xQueueReceive(QueueTask, &receivedValue, pdMS_TO_TICKS(1000)) == pdPASS) {
      printf("TaskTake: Received delay value %lu from queue\r\n", receivedValue);
    } else {
      printf("TaskTake: Failed to receive value from queue within 1000 ms. Triggering system reset.\r\n");
      vTaskDelay(pdMS_TO_TICKS(10));
      NVIC_SystemReset(); // Trigger reset

    }
  }
}


void task_bug(void * pvParameters) {
    int delay = (int) pvParameters;
    for (;;) {
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) { 
            printf("Je suis %s et je m'endors pour %d ticks\r\n", pcTaskGetName(NULL), delay);
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(delay);
    }
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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  QueueTask = xQueueCreate(10, sizeof(uint32_t)); // Queue
  if (QueueTask == NULL) {
    printf("Failed to create queue\r\n");
    Error_Handler();
  }

  xMutex = xSemaphoreCreateMutex(); // Mutex
  if (xMutex == NULL) {
    printf("Failed to create Mutex\r\n");
    Error_Handler();
  }

  xTaskCreate(TaskGive, "TaskGive", 128, NULL, tskIDLE_PRIORITY + 1, &TaskGiveHandle);
  xTaskCreate(TaskTake, "TaskTake", 128, NULL, tskIDLE_PRIORITY + 1, &TaskTakeHandle);
  xTaskCreate(StartLedTask, "LedTask", 128, NULL, tskIDLE_PRIORITY + 1, &LedTaskHandle);

  BaseType_t ret;
  ret = xTaskCreate(task_bug, "Tache 1", STACK_SIZE,(void *) TASK1_DELAY, TASK1_PRIORITY, NULL);
  configASSERT(pdPASS == ret);
  ret = xTaskCreate(task_bug, "Tache 2", STACK_SIZE,(void *) TASK2_DELAY, TASK2_PRIORITY, NULL);
  configASSERT(pdPASS == ret);



  printf("_\\||TP_FreeRTOS_SEHTEL_BEGUIN||//_\n");

  /* USER CODE END 2 */

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();


  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if (HAL_GPIO_ReadPin(BTN_USER_GPIO_Port, BTN_USER_Pin) == GPIO_PIN_SET)
	  		{
	  			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
	  		}
	  		else
	  		{
	  			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
	  		}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
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
