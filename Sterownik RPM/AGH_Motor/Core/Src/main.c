/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "dac.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef enum BLDC_Directions_Tag
{
	BLDC_Direction_Left = 0,
	BLDC_Direction_Right,
	BLDC_Direction_No
} BLDC_Directions_T;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define UART_MAX_BUFFER_LENGTH	(50U)

#define UART_COMMAND_BLDC_DIRECTION_LEFT	("BLDC_DIRECTION_LEFT")
#define UART_COMMAND_BLDC_DIRECTION_RIGHT	("BLDC_DIRECTION_RIGHT")
#define UART_COMMAND_BLDC_DAC_VAL			("BLDC_DAC_VAL_")

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint8_t toggle_counter = 0U;

uint32_t IC_Val1 = 0U;
uint32_t IC_Val2 = 0U;
uint32_t Difference = 0U;
int Is_First_Captured = 0;

/* Measure Frequency */
float frequency = 0;

volatile uint32_t Rising_Edges_Counter = 0U;

uint8_t Uart_Rx_Char = 0U;
uint8_t Uart_Rx_Buffer[UART_MAX_BUFFER_LENGTH];
uint8_t Uart_Rx_Buffer_Counter = 0U;
uint8_t Uart_Tx_Buffer[UART_MAX_BUFFER_LENGTH];
uint8_t Uart_Tx_Buffer_Counter = 0U;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

static void Set_BLDC_Direction(BLDC_Directions_T bldc_direction);
static void Set_DAC_Value(int voltage);
static void Uart_Parse_Command(void);
static bool Uart_StartsWith(const uint8_t* str1, const uint8_t* str2, size_t size);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static void Set_BLDC_Direction(BLDC_Directions_T bldc_direction)
{
	if (bldc_direction < BLDC_Direction_No)
	{
		if (bldc_direction == BLDC_Direction_Left)
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
		}
	}
}

static void Set_DAC_Value(int voltage)
{
	HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, voltage);
}

static void Uart_Parse_Command(void)
{
	if (true == Uart_StartsWith(Uart_Rx_Buffer, (uint8_t*)UART_COMMAND_BLDC_DIRECTION_LEFT, 19U))
	{
		Set_BLDC_Direction(BLDC_Direction_Left);

		memcpy(Uart_Tx_Buffer, (uint8_t*)"DIRECTION_CHANGED\n", 18U);
		HAL_UART_Transmit_IT(&huart2, Uart_Tx_Buffer, 18U);
	}
	else if (true == Uart_StartsWith(Uart_Rx_Buffer, (uint8_t*)UART_COMMAND_BLDC_DIRECTION_RIGHT, 20U))
	{
		Set_BLDC_Direction(BLDC_Direction_Right);
		memcpy(Uart_Tx_Buffer, (uint8_t*)"DIRECTION_CHANGED\n", 18U);
		HAL_UART_Transmit_IT(&huart2, Uart_Tx_Buffer, 18U);
	}
	else if (true == Uart_StartsWith(Uart_Rx_Buffer, (uint8_t*)UART_COMMAND_BLDC_DAC_VAL, 13U))
	{
		int voltage = 0U;
		voltage = atoi((char*)&Uart_Rx_Buffer[13U]);
		Set_DAC_Value(voltage);

		memcpy(Uart_Tx_Buffer, (uint8_t*)"DAC_VALUE_CHANGED\n", 18U);
		HAL_UART_Transmit_IT(&huart2, Uart_Tx_Buffer, 18U);
	}
	else
	{
		memcpy(Uart_Tx_Buffer, (uint8_t*)"UNRECOGNISED_COMMAND\n", 21U);
		HAL_UART_Transmit_IT(&huart2, Uart_Tx_Buffer, 21U);
	}

	memset(Uart_Rx_Buffer, 0, sizeof(Uart_Rx_Buffer));
	Uart_Rx_Buffer_Counter = 0U;
}

static bool Uart_StartsWith(const uint8_t* str1, const uint8_t* str2, size_t size)
{
	return strncmp((const char*) str1, (const char*) str2, size) == 0;
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
  MX_TIM1_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  MX_USART3_UART_Init();
  MX_USART2_UART_Init();
  MX_DAC1_Init();
  /* USER CODE BEGIN 2 */

  //TIM1->CCR1 = 50;
  //HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_IC_Start_IT(&htim16, TIM_CHANNEL_1);
  HAL_TIM_Base_Start_IT(&htim17);

  HAL_DAC_Start(&hdac1, DAC_CHANNEL_2);
  HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, 2048);

  HAL_UART_Receive_IT(&huart2, &Uart_Rx_Char, 1U);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_USART3
                              |RCC_PERIPHCLK_TIM1|RCC_PERIPHCLK_TIM16
                              |RCC_PERIPHCLK_TIM17;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  PeriphClkInit.Tim16ClockSelection = RCC_TIM16CLK_HCLK;
  PeriphClkInit.Tim17ClockSelection = RCC_TIM17CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM16)
	{
		Rising_Edges_Counter++;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	if (htim->Instance == TIM17)
	{
		uint8_t local_counter = 0U;
		itoa(Rising_Edges_Counter, (char*)Uart_Tx_Buffer, 10);

		if (Rising_Edges_Counter != 0)
		{
			while(Rising_Edges_Counter > 0)
			{
				local_counter++;
				Rising_Edges_Counter /= 10;
			}
		}
		else
		{
			Uart_Tx_Buffer[local_counter] = '0';
			local_counter++;
		}
		Uart_Tx_Buffer[local_counter] = '\n';
		local_counter++;
		HAL_UART_Transmit_IT(&huart2, Uart_Tx_Buffer, local_counter);
		Rising_Edges_Counter = 0;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART2)
	{
		if (Uart_Rx_Buffer_Counter < UART_MAX_BUFFER_LENGTH)
		{
			Uart_Rx_Buffer[Uart_Rx_Buffer_Counter] = Uart_Rx_Char;
			Uart_Rx_Buffer_Counter++;

			if ((Uart_Rx_Char == '\n') || (Uart_Rx_Char == '\r'))
			{
				Uart_Parse_Command();
			}
		}
		else
		{
			Uart_Rx_Buffer_Counter = 0;
		}

		HAL_UART_Receive_IT(&huart2, &Uart_Rx_Char, 1U);
	}
}

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
