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
#include "usb_device.h"

UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_i2c1_rx;
DMA_HandleTypeDef hdma_i2c1_tx;
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi1_tx;
DMA_HandleTypeDef hdma_usart3_rx;
I2C_HandleTypeDef hi2c1;
SPI_HandleTypeDef hspi1;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/* Buffer for Master  */
uint8_t u8_usbBuffer[SIZE_BYTE];

/* bit flag */
uint16_t bitFlag;

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();

  while (1)
  {
      if ((bitFlag & BFLAG_TABLE1) || (bitFlag & BFLAG_TABLE2) ||
    	  (bitFlag & BFLAG_TABLE3) || (bitFlag & BFLAG_TABLE4) ||
		  (bitFlag & BFLAG_TABLE5) || (bitFlag & BFLAG_TABLE6) ||
		  (bitFlag & BFLAG_TABLE7) || (bitFlag & BFLAG_TABLE8) ||
		  (bitFlag & BFLAG_TABLE9) || (bitFlag & BFLAG_TABLE10))
      {
    	  if (bitFlag & BFLAG_TABLE1){
    		  u8_usbBuffer[0] = 3;
			  u8_usbBuffer[1] = 1;
			  bitFlag   &= ~BFLAG_TABLE1;
    	  }

    	  if (bitFlag & BFLAG_TABLE2){
			  u8_usbBuffer[0] = 3;
			  u8_usbBuffer[1] = 2;
			  bitFlag   &= ~BFLAG_TABLE2;
		  }

    	  if (bitFlag & BFLAG_TABLE3){
			  u8_usbBuffer[0] = 3;
			  u8_usbBuffer[1] = 3;
			  bitFlag   &= ~BFLAG_TABLE3;
		  }

		  if (bitFlag & BFLAG_TABLE4){
			  u8_usbBuffer[0] = 3;
			  u8_usbBuffer[1] = 4;
			  bitFlag   &= ~BFLAG_TABLE4;
		  }

		  if (bitFlag & BFLAG_TABLE5){
			  u8_usbBuffer[0] = 3;
			  u8_usbBuffer[1] = 5;
			  bitFlag   &= ~BFLAG_TABLE5;
		  }

		  if (bitFlag & BFLAG_TABLE6){
			  u8_usbBuffer[0] = 3;
			  u8_usbBuffer[1] = 6;
			  bitFlag   &= ~BFLAG_TABLE6;
		  }

		  if (bitFlag & BFLAG_TABLE7){
			  u8_usbBuffer[0] = 3;
			  u8_usbBuffer[1] = 7;
			  bitFlag   &= ~BFLAG_TABLE7;
		  }

		  if (bitFlag & BFLAG_TABLE8){
			  u8_usbBuffer[0] = 3;
			  u8_usbBuffer[1] = 8;
			  bitFlag   &= ~BFLAG_TABLE8;
		  }

		  if (bitFlag & BFLAG_TABLE9){
			  u8_usbBuffer[0] = 3;
			  u8_usbBuffer[1] = 9;
			  bitFlag   &= ~BFLAG_TABLE9;
		  }

		  if (bitFlag & BFLAG_TABLE10){
			  u8_usbBuffer[0] = 3;
			  u8_usbBuffer[1] = 10;
			  bitFlag   &= ~BFLAG_TABLE10;
		  }

		  for (uint8_t idx = 2; idx < SIZE_BYTE; idx++)
		  {
			  u8_usbBuffer[idx] = rand()%256;
		  }

		  sendbytesViausb(u8_usbBuffer);
      }

      else if (bitFlag & BFLAG_USB)                            //Process for Change USB Out buffer.
      {
          printf("USB Out buffer changed\r\n");
          bitFlag   &= ~BFLAG_USB;
      }
      else if (bitFlag & BFLAG_RND)
      {
    	  // Generate Random
		  for (uint8_t idx = 0; idx < SIZE_BYTE; idx++)
		  {
			  u8_usbBuffer[idx] = rand()%256;
		  }

		  u8_usbBuffer[0]=4;
		  u8_usbBuffer[1]=2;

		  sendbytesViausb(u8_usbBuffer);
		  // Generate Random - END*/
      }
  }

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState        = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState    = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM        = 4;
  RCC_OscInitStruct.PLL.PLLN        = 168;
  RCC_OscInitStruct.PLL.PLLP        = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ        = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler(__FILE__, __LINE__);
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider   = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider  = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider  = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler(__FILE__, __LINE__);
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

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD2_Pin|LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin   = USER_Btn_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin   = LD1_Pin|LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin   = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin   = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
/************************************************************
* @brief  This function is executed in case of error occurrence.
* @retval None
************************************************************/
void Error_Handler(char * file, int line)
{
__disable_irq();
printf("\r\nERROR: %s, line: %d \r\n",file,line);

// Configurable Fault Status Register, Consists of MMSR, BFSR and UFSR
volatile unsigned long  _CFSR = (*((volatile unsigned long *)(0xE000ED28)));
printf("CFSR: %lu \r\n",_CFSR);


// Hard Fault Status Register
volatile unsigned long _HFSR = (*((volatile unsigned long *)(0xE000ED2C)));
printf("HFSR: %lu \r\n",_HFSR);

// Debug Fault Status Register
volatile unsigned long _DFSR = (*((volatile unsigned long *)(0xE000ED30)));
printf("DFSR: %lu \r\n",_DFSR);

// Auxiliary Fault Status Register
volatile unsigned long _AFSR = (*((volatile unsigned long *)(0xE000ED3C)));
printf("AFSR: %lu \r\n",_AFSR);

// Check BFARVALID/MMARVALID to see if they are valid values
// MemManage Fault Address Register
volatile unsigned long _MMAR = (*((volatile unsigned long *)(0xE000ED34)));
printf("MMAR: %lu \r\n",_MMAR);

// Bus Fault Address Register
volatile unsigned long _BFAR = (*((volatile unsigned long *)(0xE000ED38)));
printf("BFAR: %lu \r\n",_BFAR);

//__asm("BKPT #0\n") ; // Break into the debugger

while (1)
{
      HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
      HAL_Delay(100);
}
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
