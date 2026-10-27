/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "input_reading.h"
#include "output_display.h"
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
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
static volatile uint8_t current_mode = 1U;
static uint8_t red_duration = 5U;    /* Giá trị đã lưu */
static uint8_t red_edit_value = 5U;  /* Giá trị đang chỉnh */
static uint8_t amber_duration = 2U;    /* Giá trị vàng đã lưu */
static uint8_t amber_edit_value = 2U;  /* Giá trị vàng đang chỉnh */
static uint8_t green_duration = 3U;    /* Giá trị xanh đã lưu */
static uint8_t green_edit_value = 3U;  /* Giá trị xanh đang chỉnh */
static uint16_t traffic_cycle_second = 0U;
static uint32_t traffic_last_ms = 0U;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void show_traffic(void)
{
  uint16_t green = green_duration;
  uint16_t amber = amber_duration;
  uint16_t red = red_duration;
  uint16_t second = traffic_cycle_second;
  uint16_t total = green + amber + red;

  uint16_t pin_a;
  uint16_t pin_b;
  uint8_t time_a;
  uint8_t time_b;

  /* Trang thai va thoi gian con lai cua duong A. */
  if (second < green)
  {
    pin_a = GREEN_A_Pin;
    time_a = (uint8_t)(green - second);
  }
  else if (second < green + amber)
  {
    pin_a = AMBER_A_Pin;
    time_a = (uint8_t)(green + amber - second);
  }
  else
  {
    pin_a = RED_A_Pin;
    time_a = (uint8_t)(total - second);
  }

  /* Trang thai va thoi gian con lai cua duong B. */
  if (second < red)
  {
    pin_b = RED_B_Pin;
    time_b = (uint8_t)(red - second);
  }
  else if (second < red + green)
  {
    pin_b = GREEN_B_Pin;
    time_b = (uint8_t)(red + green - second);
  }
  else
  {
    pin_b = AMBER_B_Pin;
    time_b = (uint8_t)(total - second);
  }

  /* Cac LED mau active-low: tat het, sau do bat dung hai mau. */
  HAL_GPIO_WritePin(GPIOB,
                    RED_A_Pin | AMBER_A_Pin | GREEN_A_Pin |
                    RED_B_Pin | AMBER_B_Pin | GREEN_B_Pin,
                    GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, pin_a | pin_b, GPIO_PIN_RESET);

  output_display_set_values(time_a, time_b);
}

static void update_traffic(void)
{
  if (current_mode != 1U) return;

  uint32_t now = HAL_GetTick();
  uint16_t cycle_length =
      (uint16_t)green_duration + amber_duration + red_duration;

  while ((uint32_t)(now - traffic_last_ms) >= 1000U)
  {
    traffic_last_ms += 1000U;
    traffic_cycle_second++;

    if (traffic_cycle_second >= cycle_length)
    {
      traffic_cycle_second = 0U;
    }

    show_traffic();
  }
}
static void process_mode_button(void)
{
  static uint8_t waiting_for_release = 0U;

  if (waiting_for_release == 0U)
  {
    if (button_is_pressed(BUTTON_MODE_INDEX))
    {
      current_mode = (current_mode >= 4U) ? 1U : current_mode + 1U;

      uint8_t value = 0U;
      if (current_mode == 2U)
      {
        red_edit_value = red_duration;
        value = red_edit_value;
      }
      if (current_mode == 3U)
      {
        amber_edit_value = amber_duration;
        value = amber_edit_value;
      }
      if (current_mode == 4U)
      {
        green_edit_value = green_duration;
        value = green_edit_value;
      }

      output_display_set_values(current_mode, value);

      HAL_GPIO_WritePin(GPIOB,
                        RED_A_Pin | RED_B_Pin |
                        AMBER_A_Pin | AMBER_B_Pin |
                        GREEN_A_Pin | GREEN_B_Pin,
                        GPIO_PIN_SET);
      if (current_mode == 1U)
      {
        traffic_cycle_second = 0U;
        traffic_last_ms = HAL_GetTick();
        show_traffic();
      }

      waiting_for_release = 1U;
    }
  }
  else if (!button_is_pressed(BUTTON_MODE_INDEX))
  {
    waiting_for_release = 0U;
  }
}
static void process_red_buttons(void)
{
  static uint8_t inc_was_pressed = 0U;
  static uint8_t set_was_pressed = 0U;

  uint8_t inc_pressed = button_is_pressed(BUTTON_INC_INDEX);
  uint8_t set_pressed = button_is_pressed(BUTTON_SET_INDEX);

  if (current_mode == 2U)
  {
    if (inc_pressed && !inc_was_pressed)
    {
      red_edit_value = (red_edit_value >= 99U)
                       ? 1U : red_edit_value + 1U;
      output_display_set_values(2U, red_edit_value);
    }

    if (set_pressed && !set_was_pressed)
    {
      red_duration = red_edit_value;
    }
  }

  inc_was_pressed = inc_pressed;
  set_was_pressed = set_pressed;
}
static void process_amber_buttons(void)
{
  static uint8_t inc_was_pressed = 0U;
  static uint8_t set_was_pressed = 0U;

  uint8_t inc_pressed = button_is_pressed(BUTTON_INC_INDEX);
  uint8_t set_pressed = button_is_pressed(BUTTON_SET_INDEX);

  if (current_mode == 3U)
  {
    if (inc_pressed && !inc_was_pressed)
    {
      amber_edit_value = (amber_edit_value >= 99U)
                         ? 1U : amber_edit_value + 1U;
      output_display_set_values(3U, amber_edit_value);
    }

    if (set_pressed && !set_was_pressed)
    {
      amber_duration = amber_edit_value;
    }
  }

  inc_was_pressed = inc_pressed;
  set_was_pressed = set_pressed;
}
static void process_green_buttons(void)
{
  static uint8_t inc_was_pressed = 0U;
  static uint8_t set_was_pressed = 0U;

  uint8_t inc_pressed = button_is_pressed(BUTTON_INC_INDEX);
  uint8_t set_pressed = button_is_pressed(BUTTON_SET_INDEX);

  if (current_mode == 4U)
  {
    if (inc_pressed && !inc_was_pressed)
    {
      green_edit_value = (green_edit_value >= 99U)
                         ? 1U : green_edit_value + 1U;
      output_display_set_values(4U, green_edit_value);
    }

    if (set_pressed && !set_was_pressed)
    {
      green_duration = green_edit_value;
    }
  }

  inc_was_pressed = inc_pressed;
  set_was_pressed = set_pressed;
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
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  traffic_last_ms = HAL_GetTick();
  show_traffic();
  if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    process_mode_button();
    process_red_buttons();
    process_amber_buttons();
    process_green_buttons();
    update_traffic();
    output_display_scan();
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, EN0_Pin|EN1_Pin|EN2_Pin|EN3_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SEG0_Pin|SEG1_Pin|SEG2_Pin|GREEN_A_Pin
                          |RED_B_Pin|AMBER_B_Pin|GREEN_B_Pin|SEG3_Pin
                          |SEG4_Pin|SEG5_Pin|SEG6_Pin|RED_A_Pin
                          |AMBER_A_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : BUTTON_MODE_Pin BUTTON_INC_Pin BUTTON_SET_Pin */
  GPIO_InitStruct.Pin = BUTTON_MODE_Pin|BUTTON_INC_Pin|BUTTON_SET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : EN0_Pin EN1_Pin EN2_Pin EN3_Pin */
  GPIO_InitStruct.Pin = EN0_Pin|EN1_Pin|EN2_Pin|EN3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SEG0_Pin SEG1_Pin SEG2_Pin GREEN_A_Pin
                           RED_B_Pin AMBER_B_Pin GREEN_B_Pin SEG3_Pin
                           SEG4_Pin SEG5_Pin SEG6_Pin RED_A_Pin
                           AMBER_A_Pin */
  GPIO_InitStruct.Pin = SEG0_Pin|SEG1_Pin|SEG2_Pin|GREEN_A_Pin
                          |RED_B_Pin|AMBER_B_Pin|GREEN_B_Pin|SEG3_Pin
                          |SEG4_Pin|SEG5_Pin|SEG6_Pin|RED_A_Pin
                          |AMBER_A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2)
  {
    static uint32_t elapsed_ms = 0U;

    button_reading_10ms();

    /* PSC = 7999, clock = 8 MHz: moi count tuong ung 1 ms. */
    elapsed_ms += htim->Init.Period + 1U;

    if (elapsed_ms >= 250U)
    {
      elapsed_ms -= 250U;

      switch (current_mode)
      {
        case 2U:
          HAL_GPIO_TogglePin(GPIOB, RED_A_Pin | RED_B_Pin);
          break;

        case 3U:
          HAL_GPIO_TogglePin(GPIOB, AMBER_A_Pin | AMBER_B_Pin);
          break;

        case 4U:
          HAL_GPIO_TogglePin(GPIOB, GREEN_A_Pin | GREEN_B_Pin);
          break;

        default:
          break;
      }
    }
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
