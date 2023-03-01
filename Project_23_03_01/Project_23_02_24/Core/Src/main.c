/* This Program is used for accept string input from UI.
 * The string will be transmite to Slave by I2C
 * and Slave will send back to Master Board to be compared
 * The UI is on the ui_23_02_23
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "serial.h"

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
I2C_HandleTypeDef I2cHandle;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

/* Private user code ---------------------------------------------------------*/
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

/* I2C handler declaration */
I2C_HandleTypeDef I2cHandle;

#define MASTER_BOARD;	/*Aktifkan bagian ini jika digunakan untuk master board*/

//char aTxBuffer[20] = "Hello My Dream\r\n";	// Result save atau yang akan ditampilkan
//char aRxBuffer[20];

/* Buffer used for transmission */
uint8_t aTxBuffer[] = "Hello My World";

/* Buffer used for reception */
uint8_t aRxBuffer[RXBUFFERSIZE];


#ifdef RX_EVENT_CB
	static uint8_t u8arr_eventBuff[UART_BUF_SZ];
	static uint8_t u8arr_uartEvent[UART_BUF_SZ];
#endif

static uint16_t u16_oldPos = 0;
static uint16_t u16_lenCnt = 0;

/* bit flag */
uint8_t bitFlag = 0;

int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();

  /*##-1- Configure the I2C peripheral ######################################*/
  I2cHandle.Instance             = I2Cx;
  I2cHandle.Init.AddressingMode  = I2C_ADDRESSINGMODE_10BIT;
  I2cHandle.Init.ClockSpeed      = 400000;
  I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  I2cHandle.Init.DutyCycle       = I2C_DUTYCYCLE_16_9;
  I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  I2cHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
  I2cHandle.Init.OwnAddress1     = I2C_ADDRESS;
  I2cHandle.Init.OwnAddress2     = 0xFE;

  if(HAL_I2C_Init(&I2cHandle) != HAL_OK){ Error_Handler(); }

  serial_init();
  #ifdef MASTER_BOARD
  	  printf("Init OK\r\n");
  #endif


  /****************** MAIN **********************/
  while (1) {
	  /*Perintah untuk kirim data ke UART*/
  	  if (bitFlag & BFLAG_UART_RCV) {
  		  uartProcessing (u8arr_uartEvent, u16_lenCnt - 2); // remove \r & \n
  		  memset(u8arr_uartEvent, 0, UART_BUF_SZ);
  		  u16_lenCnt = 0;

  		  /* Reset bit 0 bitflag */
  		  bitFlag 	&= ~BFLAG_UART_RCV;
  	  }

  	  /* Perintah untuk WRITE atau sending I2C */
  	  if (bitFlag & BFLAG_I2C_WR) {
	  	  #ifdef MASTER_BOARD
			  printf("Master I2C Sending... \r\n");

			  do
			  {
				  /*##-2- Proses transmission START #####################################*/
				  if(HAL_I2C_Master_Transmit_IT(&I2cHandle, (uint16_t)I2C_ADDRESS, (uint8_t*)aTxBuffer, TXBUFFERSIZE)!= HAL_OK)
				  	  {Error_Handler();
				  }

				  /*##-3- Menunggu transfer data selesai ###################################*/
				  while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
				  {}
				  printf("Sending Success!!\r\n\n");
			  } while(HAL_I2C_GetError(&I2cHandle) == HAL_I2C_ERROR_AF);

			  /* Reset bit 2 bitflag */
			  bitFlag 	&= ~BFLAG_I2C_WR;
	  	  #endif
  	 }

  	/* Perintah untuk Compare buffer dan READ dari I2C */
  	 if (bitFlag & BFLAG_I2C_RD1)
  	 {
  		 #ifdef MASTER_BOARD

  		 	 /*##-4- I2C periperal siap menerima data ############################*/
  			  do {
  				  printf("Master I2C Receiving... \r\n");

  				  if(HAL_I2C_Master_Receive_IT(&I2cHandle, (uint16_t)I2C_ADDRESS, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK) {Error_Handler();}

  				  printf("I2C RX-Master: %s \r\n\n", (char*)aRxBuffer);

  				  /* Saat Acknowledge failure (Slave tidak mengakui alamatnya) Master memulai ulang komunikasi */
  			  } while (HAL_I2C_GetError(&I2cHandle) == HAL_I2C_ERROR_AF);

  			  /*##-5- Menunggu akhir transfer ###################################*/
  			  while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY){}

  			  /*##-6- Compare the sent and received buffers ##############################*/
  			  if(Buffercmp((uint8_t*)aTxBuffer,(uint8_t*)aRxBuffer, RXBUFFERSIZE)){
  				 printf("Buffer compare Fail!!!\r\n\n");
  			  }
  			  else {
  				 printf("Buffer compare Success!!!\r\n\n");
  			  }

  			 /* Reset bit 1 bitflag */
			 bitFlag 	&= ~BFLAG_I2C_RD1;
		 #endif
	 }
  	 if (bitFlag & BFLAG_I2C_RD0)
  	 {
  	  	 #ifdef MASTER_BOARD

  	  		 printf("TX buffer: %s\r\n", aTxBuffer);
  	  		 printf("RX buffer: %s\r\n\n", aRxBuffer);

  	  		 /* Reset bit 1 bitflag */
  			 bitFlag 	&= ~BFLAG_I2C_RD0;
  		 #endif
  	 }

  	 else			// Atur default jika nilai bitFlag 0000 0000 atau default
  	 {
		#ifndef MASTER_BOARD			//Hanya di def untuk Slave
			//printf("Slave I2C Receiving \r\n\n");

			if(HAL_I2C_Slave_Receive_IT(&I2cHandle, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
			{
			  Error_Handler();
			}

			/*##-3- Wait for the end of the transfer ###################################*/
			while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
			{}

			//printf("I2C RX: %s \r\n", (char*)aRxBuffer);

			//memset(&aTxBuffer[0], 0, 20);
			//memcpy(&aTxBuffer[0], &aRxBuffer[0], strlen(aRxBuffer));
			//printf("I2C TX: %s \r\n", (char*)aTxBuffer);

			/*##-4- Kirim data ke Master #####################################*/
			//printf("Slave I2C Sending \r\n\n");
			if(HAL_I2C_Slave_Transmit_IT(&I2cHandle, (uint8_t*)aRxBuffer, TXBUFFERSIZE)!= HAL_OK) {
				Error_Handler();
			}

			//printf("Sending Success\r\n");

			/*##-5- Menunggu akhir transfer ###################################*/
			while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
			{}

	  	#endif
  	 }
  }
}


/************************************************************************************
  * @brief  Tx Transfer completed callback / Mengirim data.
  * @param  I2cHandle: I2C handle
  * @note	Turn LED2 on: Jika kirim data berhasil
  ***********************************************************************************/
#ifdef MASTER_BOARD
//Untuk MASTER BOARD
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *I2cHandle){
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
}

#else
//Untuk SLAVE
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *I2cHandle){
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
}
#endif /* MASTER_BOARD */


/*************************************************************************************
  * @brief  Rx Transfer completed callback / Menerima data.
  * @param  I2cHandle: I2C handle
  * @note	Turn LED2 on: Jika menerima data berhasil
  ************************************************************************************/
#ifdef MASTER_BOARD
// MASTER BOARD
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2cHandle) {
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
}
#else
// SLAVE
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *I2cHandle){
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
}
#endif /* MASTER_BOARD */



/* @brief  I2C error callbacks
   @param  I2cHandle: I2C handle */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *I2cHandle){
	printf("\r\nI2C ERROR \r\n\n");
	while(1) {
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		HAL_Delay(20);
	}
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 0  : pBuffer1 identical to pBuffer2
  *         >0 : pBuffer1 differs from pBuffer2
  */
static uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength){
	printf("Compare prosess...\r\n");
	while (BufferLength--){
		if ((*pBuffer1) != *pBuffer2){
			printf("Buffer different!!!\r\n");
			return BufferLength;
		}
		pBuffer1++;
		pBuffer2++;
	}
	printf("Buffer same!!!\r\n");
	return 0;
}

/* @brief System Clock Configuration */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.*/
  RCC_OscInitStruct.OscillatorType 		= RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState 			= RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState 		= RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource 		= RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM 			= 16;
  RCC_OscInitStruct.PLL.PLLN 			= 336;
  RCC_OscInitStruct.PLL.PLLP 			= RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ 			= 2;
  RCC_OscInitStruct.PLL.PLLR 			= 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK){
    Error_Handler();
  }
}

/* @brief USART2 Initialization Function*/
static void MX_USART2_UART_Init(void) {
  huart2.Instance 			= USART2;
  huart2.Init.BaudRate 		= 115200;
  huart2.Init.WordLength 	= UART_WORDLENGTH_8B;
  huart2.Init.StopBits 		= UART_STOPBITS_1;
  huart2.Init.Parity 		= UART_PARITY_NONE;
  huart2.Init.Mode 			= UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl 	= UART_HWCONTROL_NONE;
  huart2.Init.OverSampling 	= UART_OVERSAMPLING_16;

  if (HAL_UART_Init(&huart2) != HAL_OK){
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

/* @brief GPIO Initialization Function*/
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin 		= B1_Pin;
  GPIO_InitStruct.Mode 		= GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull 		= GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin 		= LD2_Pin;
  GPIO_InitStruct.Mode 		= GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull 		= GPIO_NOPULL;
  GPIO_InitStruct.Speed 	= GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);
}

/**********************************************************
 * PARSING HEADER, Used in FW CONFIG - READ/WRITE Process
 **********************************************************/
#define CFG_HEADER_NUM 			3
#define CFG_HEADER_CHARS_LEN 	5
#define STRLENMAX				100

static char str_cfg_header[CFG_HEADER_NUM][CFG_HEADER_CHARS_LEN] =
{
	"{Tes}",
	"{WR1:",
	"{RD1}"
};


/********************************************************
 * 	Parsing incoming message						   	*
 ********************************************************/
static void vShell_cmdParse(char *input) {
	for(uint8_t u8_idx = 0; u8_idx < CFG_HEADER_NUM; u8_idx++) {
		if(!memcmp(input,(char*)&str_cfg_header[u8_idx][0], CFG_HEADER_CHARS_LEN)) {
			char *pChar 		= &input[CFG_HEADER_CHARS_LEN];		//pointer untuk menyimpan header (5 char)
			char *pChar2 		= &input[CFG_HEADER_CHARS_LEN];		//pointer untuk menyimpan header (5 char)
			uint8_t u8_start 	= 0;			// ini penanda yang akan bergeser tiap indeks (menandai lokasi dari sparating symboll)
			uint8_t u8_stop 	= 0;			// ini penenda start pointer paling awal atau awal baru setelah sparating symboll
			uint8_t u8_cnt 		= 0;			// menampung ada berapa sih inputan nya

			if (u8_idx == 1) {
				/* WRITE HEADER */
				while (*pChar) {
					if (*pChar == '}') {
						memset(&aTxBuffer[0], 0, 20);
						memcpy(&aTxBuffer[0], &pChar2[u8_stop], u8_start - u8_stop);

						u8_cnt++;
						break;
					}
					pChar++;
					u8_start++;
				}
				bitFlag |= BFLAG_I2C_WR;
				bitFlag |= BFLAG_I2C_RD1;
			}

			else if (u8_idx == 2){
				/* READ HEADER */
				bitFlag |= BFLAG_I2C_RD0;
			}
		}
	}
}


void uartProcessing (uint8_t *u8p_buffer, uint16_t u16_size){
	//Bagian ini akan dipanggil di serial.c, line 57
	//printf("UART jalan\r\n"); // Harusnya bisa langsung jalan kalo di konek lewat UI
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
		if((u8arr_uartEvent[u16_lenCnt - 1] == '\n')&&(u8arr_uartEvent[u16_lenCnt - 2]== '\r'))
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


/************************************************************
  * @brief Button Callback
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  ***********************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if ((GPIO_Pin == B1_Pin) && ((bitFlag & BFLAG_BTN) == 0))
  {
	  bitFlag |= BFLAG_BTN;
  }
}


/*******************************************************************
  * @brief  This function is executed in case of error occurrence.
  ******************************************************************/
void Error_Handler(void) {
  printf("\r\nerror handler!!!\r\n");
  __disable_irq();

  while (1) {
	  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	  HAL_Delay(50);
  }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line){}
#endif /* USE_FULL_ASSERT */
