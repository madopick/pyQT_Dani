/* Includes ------------------------------------------------------------------*/
#include "main.h"

extern DMA_HandleTypeDef hdma_usart2_rx;

void HAL_MspInit(void)
{
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();

	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);
}

/**
* @brief SPI MSP Initialization
* This function configures the hardware resources used in this example
* @param hspi: SPI handle pointer
* @retval None
*/
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(hspi->Instance==SPI2)
	{
		/* Peripheral clock enable */
		__HAL_RCC_SPI2_CLK_ENABLE();

		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();

		GPIO_InitStruct.Pin 		= SPI2_MOSI_PIN;
		GPIO_InitStruct.Mode 		= GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull 		= GPIO_PULLDOWN;
		GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate 	= SPI2_MOSI_AF;
		HAL_GPIO_Init(SPI2_MOSI_GPIO_PORT, &GPIO_InitStruct);

		GPIO_InitStruct.Pin 		= SPI2_MISO_PIN; //GPIO_PIN_2;
		GPIO_InitStruct.Mode 		= GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull 		= GPIO_PULLDOWN;
		GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate 	= SPI2_MISO_AF;
		HAL_GPIO_Init(SPI2_MISO_GPIO_PORT, &GPIO_InitStruct);

		GPIO_InitStruct.Pin 		= SPI2_SCK_PIN;
		GPIO_InitStruct.Mode 		= GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull 		= GPIO_PULLDOWN;
		GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate 	= SPI2_SCK_AF;
		HAL_GPIO_Init(SPI2_SCK_GPIO_PORT, &GPIO_InitStruct);

		/* SPI2 interrupt Init */
		HAL_NVIC_SetPriority(SPI2_IRQn, 2, 0);
		HAL_NVIC_EnableIRQ(SPI2_IRQn);
	}
}

/**
* @brief SPI MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hspi: SPI handle pointer
* @retval None
*/
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
	if(hspi->Instance==SPI2)
	{
		__HAL_RCC_SPI2_CLK_DISABLE();

		/**SPI2 GPIO Configuration
    	PC1     ------> SPI2_MOSI
    	PC2     ------> SPI2_MISO
    	PB10     ------> SPI2_SCK
		 */
		HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1|GPIO_PIN_2);

		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10);

		/* SPI2 interrupt DeInit */
		HAL_NVIC_DisableIRQ(SPI2_IRQn);
	}

}

/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(huart->Instance==USART2)
	{
		/* Peripheral clock enable */
		__HAL_RCC_USART2_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**USART2 GPIO Configuration
    	PA2     ------> USART2_TX
    	PA3     ------> USART2_RX
		 */
		GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	    /* USART2 DMA Init */
	    /* USART2_RX Init */
	    hdma_usart2_rx.Instance 				= DMA1_Stream5;
	    hdma_usart2_rx.Init.Channel 			= DMA_CHANNEL_4;
	    hdma_usart2_rx.Init.Direction 			= DMA_PERIPH_TO_MEMORY;
	    hdma_usart2_rx.Init.PeriphInc 			= DMA_PINC_DISABLE;
	    hdma_usart2_rx.Init.MemInc 				= DMA_MINC_ENABLE;
	    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	    hdma_usart2_rx.Init.MemDataAlignment 	= DMA_MDATAALIGN_BYTE;
	    hdma_usart2_rx.Init.Mode 				= DMA_CIRCULAR;
	    hdma_usart2_rx.Init.Priority 			= DMA_PRIORITY_LOW;
	    hdma_usart2_rx.Init.FIFOMode 			= DMA_FIFOMODE_DISABLE;
	    if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
	    {
	      Error_Handler();
	    }

	    __HAL_LINKDMA(huart,hdmarx,hdma_usart2_rx);

	    /* USART2 interrupt Init */
	    HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
	    HAL_NVIC_EnableIRQ(USART2_IRQn);
	}

}

/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
	if(huart->Instance==USART2)
	{
		__HAL_RCC_USART2_CLK_DISABLE();

		/**USART2 GPIO Configuration
    	PA2     ------> USART2_TX
    	PA3     ------> USART2_RX
		*/
		HAL_GPIO_DeInit(GPIOA, USART_TX_Pin|USART_RX_Pin);

		/* USART2 DMA DeInit */
		HAL_DMA_DeInit(huart->hdmarx);

		/* USART2 interrupt DeInit */
		HAL_NVIC_DisableIRQ(USART2_IRQn);
	}

}
