/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
#include "serial.h"
#include "stdin.h"

/* External variables --------------------------------------------------------*/
extern SPI_HandleTypeDef 	hspi2;
extern DMA_HandleTypeDef 	hdma_usart2_rx;
extern UART_HandleTypeDef 	huart2;

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
	while (1)
	{
	}
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
	while (1)
	{
	}
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
	while (1)
	{
	}
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
	while (1)
	{
	}
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
	while (1)
	{
    }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles SPI2 global interrupt.
  */
void SPI2_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&hspi2);
}

/**
  * @brief This function handles DMA1 stream5 global interrupt.
  */
void DMA1_Stream5_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart2_rx);
  serial_handlerDMA();
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart2);
	serial_handlerUART();
}


/******************************************************************************
  * @brief  This function handles external lines 15 to 10 interrupt request.
  * @param  None
  * @retval None
  *****************************************************************************/
void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(USER_BUTTON_PIN);
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
