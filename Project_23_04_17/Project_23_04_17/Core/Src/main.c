/* This Program is used for accept multiple input of string from UI and send as Byte.
 * The string will be parsed in to integer and it will be transmit to Slave by SPI.
 * Slave will save the value on variables and send back to Master Board to be compared when it needed
 * The UI is on the ui_23_04_17
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "uart_prog.h"

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_usart2_rx;
UART_HandleTypeDef huart2;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI2_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
uint16_t Buffercmp_Byte(int32_t[], int32_t[], uint8_t);
void Set_aTxBuffer(int16_t, int32_t[], uint16_t, uint16_t);

#ifdef __GNUC__
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* @brief  Retargets the C library printf function to the USART. */
PUTCHAR_PROTOTYPE {
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}

#ifdef RX_EVENT_CB
static uint8_t u8arr_eventBuff[UART_BUF_SZ];
static uint8_t u8arr_uartEvent[UART_BUF_SZ];
#endif

static uint16_t u16_oldPos = 0;
static uint16_t u16_lenCnt = 0;

#define MASTER_BOARD

/* Buffer used for transmission */
int32_t aTxBuffer[11] = {1, -10, 10, -10, 10, -10, 10, -10, 10, -10, 10};
/* Buffer used for reception */
int32_t aRxBuffer[11];

char sendStr[STRLENMAX];

/* Default variabel for 3 array */
#ifndef MASTER_BOARD
int32_t res1[10] = {1,-1,11,-11,4,-4,44,-44,7,-7};
int32_t res2[10] = {2,-2,22,-22,5,-5,55,-55,8,-8};
int32_t res3[10] = {3,-3,33,-33,6,-6,66,-66,9,-9};
#endif

int32_t temp[30];

uint16_t bitFlag;
volatile uint8_t spi2F;

/* @brief  Main program */
int main(void)
{
	HAL_Init();

	SystemClock_Config();

	MX_GPIO_Init();
	MX_SPI2_Init();
	MX_DMA_Init();
	MX_USART2_UART_Init();

	#ifdef MASTER_BOARD
	printf("Init OKE\r\n");
	#endif

	while (1)
	{
		if (bitFlag & BFLAG_UART_RCV)		// UART Processing
		{
			//printf("UART...\r\n");
			uartProcessing (u8arr_uartEvent, u16_lenCnt - 2); // remove \r & \n
			memset(u8arr_uartEvent, 0, UART_BUF_SZ);
			u16_lenCnt = 0;
			bitFlag 	&= ~BFLAG_UART_RCV;
		}
		#ifndef MASTER_BOARD
		// CORE OF SPI SLAVE RECEIVE
		if(HAL_SPI_Receive_IT(&hspi2, (uint8_t *)aRxBuffer, BUFFERSIZE) != HAL_OK)
		{
			Error_Handler();
		}

		while (spi2F == 0){}
		while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY){}
		spi2F = 0;
		// CORE OF SPI SLAVE -- RECEIVE -- END

		if ((aRxBuffer[0] == 1) || (aRxBuffer[0]==7)) {
			memcpy(res1, &aRxBuffer[1], CFG_LENGTH*sizeof(aRxBuffer[0]));
			memcpy(&aTxBuffer[1], res1, CFG_LENGTH*sizeof(res1[0]));
			if(aRxBuffer[0]==1){aTxBuffer[0] = 1;}
			if(aRxBuffer[0]==7){aTxBuffer[0] = 7;}
		}
		else if((aRxBuffer[0]==2)||(aRxBuffer[0]==8))	// WR2
		{
			memcpy(res2, &aRxBuffer[1], CFG_LENGTH*sizeof(aRxBuffer[0]));
			memcpy(&aTxBuffer[1], res2, CFG_LENGTH*sizeof(res2[0]));
			if(aRxBuffer[0]==2){aTxBuffer[0] = 2;}
			if(aRxBuffer[0]==8){aTxBuffer[0] = 8;}
		}
		else if((aRxBuffer[0]==3)||(aRxBuffer[0]==9))	// WR3
		{
			memcpy(res3, &aRxBuffer[1], CFG_LENGTH*sizeof(aRxBuffer[0]));
			memcpy(&aTxBuffer[1], res3, CFG_LENGTH*sizeof(res3[0]));
			if(aRxBuffer[0]==3){aTxBuffer[0] = 3;}
			if(aRxBuffer[0]==9){aTxBuffer[0] = 9;}
		}
		else if(aRxBuffer[0]==4)			// RD1
		{
			Set_aTxBuffer(4, res1, 0, CFG_LENGTH*sizeof(res1[0]));
		}
		else if(aRxBuffer[0]==5)			// RD2
		{
			Set_aTxBuffer(5, res2, 0, CFG_LENGTH*sizeof(res2[0]));
		}
		else if(aRxBuffer[0]==6)			// RD3
		{
			Set_aTxBuffer(6, res3, 0, CFG_LENGTH*sizeof(res3[0]));
		}


		// CORE OF SPI SLAVE TRANSMIT
		if(HAL_SPI_Transmit_IT(&hspi2, (uint8_t *)aTxBuffer, BUFFERSIZE) != HAL_OK)
		{
			Error_Handler();
		}

		while (spi2F == 0){}
		while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY){}
		// CORE OF SPI SLAVE TRANSMIT -- END

		memset(aRxBuffer, 0, sizeof(aRxBuffer));
	  	#endif

	  	#ifdef MASTER_BOARD
		if (bitFlag & BFLAG_BTN)
		{
			/*
			printf("Button Pressed\r\n");
			while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY){}

			spi2F = 0;

			printf("MASTER TX: %d %d %d %d\r\n\n", aTxBuffer[0], aTxBuffer[1], aTxBuffer[2], aTxBuffer[3]);
			if(HAL_SPI_Transmit_IT(&hspi2, (uint8_t*)aTxBuffer, 10) != HAL_OK)
			{
				Error_Handler();
			}

			while (spi2F == 0){}
			while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY){}
			printf("Send Success\r\n");
			spi2F = 0;

			HAL_Delay(100);	//jangan diilangin soalnya bisa bikin error
			if(HAL_SPI_Receive_IT(&hspi2, (uint8_t*)aRxBuffer, 10) != HAL_OK)
			{
				Error_Handler();
			}

			while (spi2F == 0){}
			while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY){}

			printf("MASTER RX: %d %d %d %d\r\n\n", aRxBuffer[0], aRxBuffer[1], aRxBuffer[2], aRxBuffer[3]);

			memset(aRxBuffer, 0, sizeof(aRxBuffer));
			*/

			bitFlag &= ~BFLAG_BTN;
		}

		if (bitFlag & BFLAG_SPIM_RBA) {			//Read Byte All (Trigger once at the first time)
			memset (sendStr, 0, STRLENMAX);
			sendStr[0] = 0x10;
			sendStr[1] = 0x11;
			HAL_UART_Transmit(&huart2, (uint8_t *)sendStr, 2, 0xFFFF);
			for (uint8_t i=4; i<=6; i++){
				aTxBuffer[0] = i;

				// CORE OF SPI MASTER
				while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY){}

				spi2F = 0;

				if(HAL_SPI_Transmit_IT(&hspi2, (uint8_t*)aTxBuffer, BUFFERSIZE) != HAL_OK)
				{
					Error_Handler();
				}

				while (spi2F == 0){}
				while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY){}
				spi2F = 0;

				HAL_Delay(100);	//jangan diilangin soalnya bisa bikin error*/
				if(HAL_SPI_Receive_IT(&hspi2, (uint8_t*)aRxBuffer, BUFFERSIZE) != HAL_OK)
				{
					Error_Handler();
				}

				//while (spi2F == 0){}
				while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY){}
				// CORE SPI MASTER -- END


				if (aRxBuffer[0]==4){
					//memcpy(&sendStr[2], &aRxBuffer[1], CFG_LENGTH * sizeof(&aRxBuffer[1]));
					memcpy(&sendStr[0], &aRxBuffer[1], CFG_LENGTH * sizeof(&aRxBuffer[1]));
				}
				else if (aRxBuffer[0]==5){
					//memcpy(&sendStr[42], &aRxBuffer[1], CFG_LENGTH * sizeof(&aRxBuffer[1]));
					memcpy(&sendStr[0], &aRxBuffer[1], CFG_LENGTH * sizeof(&aRxBuffer[1]));
				}
				else if (aRxBuffer[0]==6){
					//memcpy(&sendStr[82], &aRxBuffer[1], CFG_LENGTH * sizeof(&aRxBuffer[1]));
					memcpy(&sendStr[0], &aRxBuffer[1], CFG_LENGTH * sizeof(&aRxBuffer[1]));
				}
				HAL_UART_Transmit(&huart2, (uint8_t *)sendStr, 40, 0xFFFF);
			}
			//HAL_UART_Transmit(&huart2, (uint8_t *)sendStr, 120, 0xFFFF);

			bitFlag &= ~BFLAG_SPIM_RBA;
		}

		if ((bitFlag & BFLAG_SPIM_WR) || (bitFlag & BFLAG_SPIM_WBA))
		{
			// CORE OF SPI MASTER
			while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY){}

			spi2F = 0;

			if(HAL_SPI_Transmit_IT(&hspi2, (uint8_t*)aTxBuffer, BUFFERSIZE) != HAL_OK)
			{
				Error_Handler();
			}

			while (spi2F == 0){}
			while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY){}
			spi2F = 0;

			HAL_Delay(100);	//jangan diilangin soalnya bisa bikin error*/
			if(HAL_SPI_Receive_IT(&hspi2, (uint8_t*)aRxBuffer, BUFFERSIZE) != HAL_OK)
			{
				Error_Handler();
			}

			//while (spi2F == 0){}
			while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY){}
			// CORE SPI MASTER -- END

			if(aRxBuffer[0]==1)		// {WB1:
			{
				char send[] = {0x14, 0x15,
					  'S', 'E', 'N', 'D', ' ', 'R', 'E', 'S', '1', ':', 0x0d, 0x0a};
				HAL_UART_Transmit(&huart2, (uint8_t *)send, 14, 0xFFFF);
			}
			else if(aRxBuffer[0]==2)		// {WB2:
			{
				char send[] = {0x14, 0x15,
					  'S', 'E', 'N', 'D', ' ', 'R', 'E', 'S', '2', ':', 0x0d, 0x0a};
				HAL_UART_Transmit(&huart2, (uint8_t *)send, 14, 0xFFFF);
			}
			else if(aRxBuffer[0]==3)		// {WB3:
			{
				char send[] = {0x14, 0x15,
					  'S', 'E', 'N', 'D', ' ', 'R', 'E', 'S', '3', ':', 0x0d, 0x0a};
				HAL_UART_Transmit(&huart2, (uint8_t *)send, 14, 0xFFFF);
			}
			else if(aRxBuffer[0]==4)		// {RB1}
			{
				char send[] = {0x14, 0x15,
					  'R', 'E', 'A', 'D', ' ', 'R', 'E', 'S', '1', ':', 0x0d, 0x0a};
				HAL_UART_Transmit(&huart2, (uint8_t *)send, 14, 0xFFFF);
			}
			else if(aRxBuffer[0]==5)		// {RB2}
			{
				char send[] = {0x14, 0x15,
					  'R', 'E', 'A', 'D', ' ', 'R', 'E', 'S', '2', ':', 0x0d, 0x0a};
				HAL_UART_Transmit(&huart2, (uint8_t *)send, 14, 0xFFFF);
			}
			else if(aRxBuffer[0]==6)		// {RB3}
			{
				char send[] = {0x14, 0x15,
					  'R', 'E', 'A', 'D', ' ', 'R', 'E', 'S', '3', ':', 0x0d, 0x0a};
				HAL_UART_Transmit(&huart2, (uint8_t *)send, 14, 0xFFFF);
			}


			if (aRxBuffer[0]<4)				// Compare TxBuffer and Rx Buffer
			{
				//##-6- Compare the sent and received buffers ##############################
				if(Buffercmp_Byte(aTxBuffer,aRxBuffer, 11))
				{
					char send[] = {0x14, 0x15,
							'B','u','f','f','e','r',' ','C','o','m','p','a','r','e',' ','F','a','i','l','!','!',0x0d, 0x0a};
					HAL_UART_Transmit(&huart2, (uint8_t *)send, 25, 0xFFFF);
				}

				bitFlag &= ~BFLAG_SPIM_WR;
			}

			if (aRxBuffer[0] >= 4 && aRxBuffer[0] <= 6)
			{
				//printf("Master RX: %d %d %d %d\r\n\n", aRxBuffer[0], aRxBuffer[1], aRxBuffer[2], aRxBuffer[3]);
				memset (sendStr, 0, STRLENMAX);
				sendStr[0] = 0x12;
				sendStr[1] = 0x13;

				memcpy(&sendStr[2], &aRxBuffer[1], CFG_LENGTH * sizeof(aRxBuffer[0]));

				sendStr[42] = 0x0d;
				sendStr[43] = 0x0a;

				HAL_UART_Transmit(&huart2, (uint8_t *)sendStr, 44, 0xFFFF);

				bitFlag &= ~BFLAG_SPIM_WR;
			}

			// BFLAG_I2C_RBA			Trigger default input at form
			if (aRxBuffer[0]==7)				//WRA
			{
				Set_aTxBuffer(8, temp, 10, CFG_LENGTH*sizeof(temp[0]));
			}
			if (aRxBuffer[0]==8)
			{
				Set_aTxBuffer(9, temp, 20, CFG_LENGTH*sizeof(temp[0]));
			}
			if (aRxBuffer[0]==9)
			{
				sendStr[0] = 0x10;
				sendStr[1] = 0x12;

				HAL_UART_Transmit(&huart2, (uint8_t *)sendStr, 2, 0xFFFF);
				if(bitFlag & BFLAG_SPIM_WBA){
					bitFlag &= ~BFLAG_SPIM_WBA;
				}
			}
		}

		else
		{
			HAL_Delay(100);
			HAL_GPIO_TogglePin(GPIOB, LED2);
		}
	 	#endif
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
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	RCC_OscInitStruct.OscillatorType 		= RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState 				= RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue 	= RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState 			= RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource 		= RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM 				= 16;
	RCC_OscInitStruct.PLL.PLLN 				= 336;
	RCC_OscInitStruct.PLL.PLLP 				= RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ 				= 2;
	RCC_OscInitStruct.PLL.PLLR 				= 2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType 		= RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource 		= RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider 	= RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider 	= RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider 	= RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{
	hspi2.Instance 					= SPI2;
	hspi2.Init.BaudRatePrescaler 	= SPI_BAUDRATEPRESCALER_128;
	hspi2.Init.Direction 			= SPI_DIRECTION_2LINES;
	hspi2.Init.CLKPhase 			= SPI_PHASE_1EDGE;
	hspi2.Init.CLKPolarity 			= SPI_POLARITY_LOW;
	hspi2.Init.DataSize 			= SPI_DATASIZE_16BIT;
	hspi2.Init.FirstBit 			= SPI_FIRSTBIT_MSB;
	hspi2.Init.TIMode 				= SPI_TIMODE_DISABLE;
	hspi2.Init.CRCCalculation 		= SPI_CRCCALCULATION_DISABLE;
	hspi2.Init.CRCPolynomial 		= 10;
	hspi2.Init.NSS 					= SPI_NSS_SOFT;
  	#ifdef MASTER_BOARD
	hspi2.Init.Mode 				= SPI_MODE_MASTER;
  	#else
	hspi2.Init.Mode 				= SPI_MODE_SLAVE;
  	#endif

	if (HAL_SPI_Init(&hspi2) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{
	huart2.Instance 		 = USART2;
	huart2.Init.BaudRate 	 = 115200;
	huart2.Init.WordLength 	 = UART_WORDLENGTH_8B;
	huart2.Init.StopBits 	 = UART_STOPBITS_1;
	huart2.Init.Parity 		 = UART_PARITY_NONE;
	huart2.Init.Mode 		 = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl 	 = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}

	#ifdef RX_EVENT_CB
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2, u8arr_eventBuff, UART_BUF_SZ);
  	#else
	serial_init();
  	#endif
}

static void MX_DMA_Init(void) {
  __HAL_RCC_DMA1_CLK_ENABLE();

  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : Blue_Btn_Pin */
	GPIO_InitStruct.Pin 	= Blue_Btn_Pin;
	GPIO_InitStruct.Mode 	= GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull 	= GPIO_NOPULL;
	HAL_GPIO_Init(Blue_Btn_GPIO_Port, &GPIO_InitStruct);

	/* Enable and set EXTI lines 15 to 10 Interrupt */
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	/*Configure GPIO pin : LD2_Pin */
	GPIO_InitStruct.Pin 	= LD2_Pin;
	GPIO_InitStruct.Mode 	= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull 	= GPIO_NOPULL;
	GPIO_InitStruct.Speed	= GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (((GPIO_Pin == Blue_Btn_Pin) && ((bitFlag & BFLAG_BTN) == 0)))
	{
		bitFlag |= BFLAG_BTN;
	}
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
}


void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	spi2F = 1;
}


void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	spi2F = 1;
}

/* @brief  I2C error callbacks
   @param  I2cHandle: I2C handle */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi2){
	printf("\r\nSPI ERROR \r\n\n");
	while(1) {
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		HAL_Delay(100);
	}
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 0  : pBuffer1 identical to pBuffer2
  *         >0 : pBuffer1 differs from pBuffer2
  */

uint16_t Buffercmp_Byte(int32_t pBuffer1[], int32_t pBuffer2[], uint8_t length){
	char send[] = {0x14, 0x15,
			'C','o','m','p','a','r','e',' ','p','r','o','c','e','s','s','.','.',0x0d,0x0a};
	HAL_UART_Transmit(&huart2, (uint8_t *)send, 21, 0xFFFF);
	for (uint8_t i = 0; i < length; i++){
		if (pBuffer1[i]!=pBuffer2[i]){
			char comp[] = {0x14, 0x15,
					'B','u','f','f','e','r',' ','D','i','f','f','e','r','e','n','t','!','!',0x0d,0x0a};
			HAL_UART_Transmit(&huart2, (uint8_t *)comp, 22, 0xFFFF);
			return i;
		}
	}

	char comp[] = {0x14, 0x15,
		'B','u','f','f','e','r',' ','s','a','m','e','!','!',0x0d,0x0a};
	HAL_UART_Transmit(&huart2, (uint8_t *)comp, 17, 0xFFFF);
	return 0;
}

void Set_aTxBuffer(int16_t idx0, int32_t target[], uint16_t idx_target, uint16_t size){
	for (uint8_t i=0; i<10; i++){
		aTxBuffer[i+1] = 0;
	}
	memcpy(&aTxBuffer[1], &target[idx_target], size);
	aTxBuffer[0]=idx0;
}


void uartProcessing (uint8_t *u8p_buffer, uint16_t u16_size){
	vShell_cmdParse((char*)u8p_buffer);
}

/*****************************************************************
 * @name 	vUAFE_uart_handle
 * @brief	handle afe uart data copy
 ****************************************************************/
static void vUAFE_uart_handle(uint16_t Size)
{
	uint16_t u16_numData;

	/* Check if number of received data in reception buffer has changed */
	if (Size != u16_oldPos)
	{
		if (Size > u16_oldPos)
		{
			/* Current position is higher than previous one */
			u16_numData = Size - u16_oldPos;
			memcpy(&u8arr_uartEvent[u16_lenCnt],&u8arr_eventBuff[u16_oldPos],u16_numData);
			u16_lenCnt += u16_numData;
		}
		else
		{
			/* End of buffer has been reached */
			u16_numData = UART_BUF_SZ - u16_oldPos;

			memcpy (&u8arr_uartEvent[u16_lenCnt], 			// copy data in that remaining space
					&u8arr_eventBuff[u16_oldPos],
					u16_numData);

			u16_lenCnt += u16_numData;

			memcpy (&u8arr_uartEvent[u16_lenCnt], 			// copy the remaining data
					&u8arr_eventBuff[0],
					Size);

			u16_lenCnt += Size;
		}

		/* Check for ready to process */
		if(((u8arr_uartEvent[u16_lenCnt - 1] == '\n')&&(u8arr_uartEvent[u16_lenCnt - 2]== '\r')) ||
				((u8arr_uartEvent[u16_lenCnt - 1] == '\r')&&(u8arr_uartEvent[u16_lenCnt - 2]== '\n')))
		{
			bitFlag |= BFLAG_UART_RCV;
		}

	}

	u16_oldPos = Size;
}



/*****************************************************************
 * @name HAL_UARTEx_RxEventCallback
 * @brief
 ****************************************************************/
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	#ifdef RX_EVENT_CB
	if (huart->Instance == USART2)
	{
		vUAFE_uart_handle(Size);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart2, u8arr_eventBuff, UART_BUF_SZ);
	}
	#endif
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
	printf("error handler\r\n");
	__disable_irq();
	while (1)
	{
		HAL_GPIO_TogglePin(GPIOB, LED2);
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
