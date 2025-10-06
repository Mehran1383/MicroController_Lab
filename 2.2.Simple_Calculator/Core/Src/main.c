/*
  ******************************************************************************
  * @file           : main.c
	*	@author					: Mehran Hashemi
  * @brief          : Program to simulate a simple calculator.
  ******************************************************************************
*/
	
#include <stdio.h>
#include <ctype.h>

#include "main.h"
#include "keypad.h"
#include "STM_MY_LCD16X2.h"

#define DELAY 500
#define LONGER_DELAY 5 * 500
#define BUF_SIZE 8

const char data_key[] = {
	'7', '8', '9', '/',
	'4', '5', '6', '*',
	'1', '2', '3', '+',
	'C', '0', '=', '-'
};

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

int get_num(void);
char get_opr(void);
void error_routine(int error_type);
void show_on_LCD(int key);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
	
	/* 
		Initialize LCD in 4 bit mode 
		Pin lables have been defined in STM32CubeMX as follows:
			- GPIOA_PIN_0 -> RS_Pin
			- GPIOA_PIN_1 -> RW_Pin
			- GPIOA_PIN_2 -> E_Pin
			- GPIOA_PIN_4 -> D4_Pin
			- GPIOA_PIN_5 -> D5_Pin
			- GPIOA_PIN_6 -> D6_Pin
			- GPIOA_PIN_7 -> D7_Pin
	*/
	LCD1602_Begin4BIT(GPIOA, RS_Pin, E_Pin, GPIOA, D4_Pin, 
										D5_Pin, D6_Pin, D7_Pin);
										
	// Only in write mode
	HAL_GPIO_WritePin(GPIOA, RW_Pin, 0);
	
	int num1, num2, result;
	char operation;

  while (1) {
		// Get the first number
		if ((num1 = get_num()) < 0) {
			error_routine(num1);
			continue;
		}
			
		// Get the operation
		if ((operation = get_opr()) < 0) {
			error_routine(operation);
			continue;
		}

		// Get the second number
		if ((num2 = get_num()) < 0) {
			error_routine(num2);
			continue;
		}
		
		// Get the `=` to calculate the expression
		if (get_opr() != '=') {
			error_routine(INVALID_INPUT);
			continue;
		}
		
		// Calculation
		if (operation == '-') {
			result = num1 - num2;
			LCD1602_PrintInt(result);
			HAL_Delay(DELAY);
		} 
		else if (operation == '+') {
			result = num1 + num2;
			LCD1602_PrintInt(result);
			HAL_Delay(DELAY);
		} 
		else if (operation == '*') {
			result = num1 * num2;
			LCD1602_PrintInt(result);
			HAL_Delay(DELAY);
		} 
		else if (operation == '/') {
			if (num2 == 0) {
				error_routine(ERROR);
				continue;
			}
			float result = (float) num1 / num2;
			LCD1602_PrintFloat(result, 2);
			HAL_Delay(DELAY);
		} 
		else {
			error_routine(ERROR);
			continue;
		}
		
		HAL_Delay(LONGER_DELAY);
		LCD1602_clear();
	}
}

/**
  * @brief Get a digit from keypad, show it on LCD and return it.
  * @retval Pressed key
  */
int get_num(void) 
{
	int key, num;
	
	keypad_write(0xF0);
	while (keypad_read() == 0xF0)
		;
	
	key = keypad();
	if (key != NOT_DETECTED) {
		show_on_LCD(key);
		num = data_key[key] - '0';	
		if (isdigit(data_key[key] - '0'))
			return num;
		else
			return INVALID_INPUT;
	}
	return NOT_DETECTED;
}

/**
  * @brief Get an operation from keypad, show it on LCD and return it.
  * @retval Pressed key
  */
char get_opr(void)
{
	int key;
	char operation;
	
	keypad_write(0xF0);
	while (keypad_read() == 0xF0)
		;
	
	key = keypad();
	if (key != NOT_DETECTED) {
		show_on_LCD(key);
		operation = data_key[key];
		if (isdigit(operation) == 0)
			return operation;
		else
			return INVALID_INPUT;
	}
	return NOT_DETECTED;
}

/**
  * @brief Show the key pressed, on LCD.
	* @param The key, which is the index of data_key[]
  * @retval None
  */
void show_on_LCD(int key)
{
		char str[BUF_SIZE];
		sprintf(str, "%c", data_key[key]);
		LCD1602_print(str);
		HAL_Delay(DELAY);
}

/**
  * @brief Routine for showing the error
  * @retval None
  */
void error_routine(int error_type)
{
	LCD1602_2ndLine();

	switch (error_type) {
		case NOT_DETECTED:
			LCD1602_print("ERROR IN KEYPAD!");
			break;
		case INVALID_INPUT:
			LCD1602_print("INVALID INPUT!");
			break;
		case ERROR:
			LCD1602_print("INVALID EXPR!");
	}
	
	HAL_Delay(LONGER_DELAY);
	LCD1602_clear();
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RS_Pin|RW_Pin|E_Pin|D4_Pin
                          |D5_Pin|D6_Pin|D7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, R1_Pin|R2_Pin|R3_Pin|R4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PE2 PE3 PE0 PE1 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PE4 PE5 PE6 PE7 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : RS_Pin RW_Pin E_Pin D4_Pin
                           D5_Pin D6_Pin D7_Pin */
  GPIO_InitStruct.Pin = RS_Pin|RW_Pin|E_Pin|D4_Pin
                          |D5_Pin|D6_Pin|D7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
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
#ifdef USE_FULL_ASSERT
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
