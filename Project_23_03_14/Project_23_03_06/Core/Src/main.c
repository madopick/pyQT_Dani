/* This Program is used for accept multiple input of string from UI and send as ASCII of string.
 * The string will be parsed in to integer and it will be transmite to Slave by I2C.
 * Slave will save the value on variables and send back to Master Board to be compared when it needed
 * The UI is on the ui_23_03_06
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
static uint16_t Buffercmp(uint32_t pBuffer1[], uint32_t pBuffer2[], uint8_t length);

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

/* Default variabel for 3 array */
#ifndef MASTER_BOARD
	int32_t res1[10] = {1,-1,11,-11,4,-4,44,-44,7,-7};
	int32_t res2[10] = {2,-2,22,-22,5,-5,55,-55,8,-8};
	int32_t res3[10] = {3,-3,33,-33,6,-6,66,-66,9,-9};
#endif

/* Buffer used for transmission */
int32_t aTxBuffer[11] = {0,10,-10,10,-10,10,-10,10,-10,10,-10};
/* TRIGGER CODE of Slave
 * {0,-,--}  // DEFAULT
 * {-1,-,--} // Ask for the first time only
 * {1,-,--}	 // Master Transmite data to slave(res1)
 * {2,-,--}	 // Master Transmite data to slave(res2)
 * {3,-,--}	 // Master Transmite data to slave(res3)
 * {4,-,--}	 // Slave Receive trigger and transmit res1 to master
 * {5,-,--}	 // Slave Receive trigger and transmit res2 to master
 * {6,-,--}	 // Slave Receive trigger and transmit res3 to master
 * {7,-,--}	 // Transmite All 1
 * {8,-,--}	 // Transmite All 2
 * {9,-,--}	 // Transmite All 3
 * */

/* Used for save 3 input immediately*/
int32_t temp[30];

/* Buffer used for reception */
int32_t aRxBuffer[11];

int32_t aStartBuffer[11];

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

  	  if (bitFlag & BFLAG_I2C_RSL) {
		#ifdef MASTER_BOARD
  		  /* Master give trigger to Slave*/
  		  for (uint8_t i=4; i<=6; i++){
  			  aTxBuffer[0] = i;
  			  do
  			  {
  				  /*##-2- Proses transmission START #####################################*/
  				  if(HAL_I2C_Master_Transmit_IT(&I2cHandle, (uint16_t)I2C_ADDRESS, (uint8_t*)aTxBuffer, TXBUFFERSIZE)!= HAL_OK)
  				  {Error_Handler();}

  				  /*##-3- Menunggu transfer data selesai ###################################*/
  				  while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
  				  {}
  				  //printf("Access Slave Success!!\r\n\n");
  			  } while(HAL_I2C_GetError(&I2cHandle) == HAL_I2C_ERROR_AF);

  			  /* Master recieve data from Slave*/
  			  /*##-4- I2C periperal siap menerima data ############################*/
  			  do
  			  {
  				  if(HAL_I2C_Master_Receive_IT(&I2cHandle, (uint16_t)I2C_ADDRESS, (uint8_t *)aStartBuffer, RXBUFFERSIZE) != HAL_OK) {Error_Handler();}

  				  /* Saat Acknowledge failure (Slave tidak mengakui alamatnya) Master memulai ulang komunikasi */
  			  } while (HAL_I2C_GetError(&I2cHandle) == HAL_I2C_ERROR_AF);

  			  /*##-5- Menunggu akhir transfer ###################################*/
  			  while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
  			  {}
  			  if (aStartBuffer[0]==4){
  				  printf("R1: ");
  			  }
  			  else if (aStartBuffer[0]==5){
  				  printf("R2: ");
  			  }
  			  else if (aStartBuffer[0]==6){
  				  printf("R3: ");
  			  }
  			  printf("%ld; %ld; %ld; %ld; %ld; %ld; %ld; %ld; %ld; %ld;\r\n",
  	  				 aStartBuffer[1], aStartBuffer[2], aStartBuffer[3],
					 aStartBuffer[4], aStartBuffer[5], aStartBuffer[6],
					 aStartBuffer[7], aStartBuffer[8], aStartBuffer[9], aStartBuffer[10]);

  		  }
  		  printf("END\r\n");

  		  bitFlag &= ~BFLAG_I2C_RSL;
		#endif
  	  }

  	  /* Perintah untuk WRITE atau sending I2C */
  	  if ((bitFlag & BFLAG_I2C_WR1) || (bitFlag & BFLAG_I2C_WR2) || (bitFlag & BFLAG_I2C_WR3)) {
	  	  #ifdef MASTER_BOARD
			  //printf("Master I2C Sending code %ld\r\n", aTxBuffer[0]);

			  do
			  {
				  /*##-2- Proses transmission START #####################################*/
				  if(HAL_I2C_Master_Transmit_IT(&I2cHandle, (uint16_t)I2C_ADDRESS, (uint8_t*)aTxBuffer, TXBUFFERSIZE)!= HAL_OK)
				  {Error_Handler();}

				  /*##-3- Menunggu transfer data selesai ###################################*/
				  while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
				  {}
				  printf("Sending Success!!\r\n");
			  } while(HAL_I2C_GetError(&I2cHandle) == HAL_I2C_ERROR_AF);

			  bitFlag |= BFLAG_I2C_RD1;
			  /* Reset bit 2 bitflag ada di bagian BFLAG_I2C_RD0 */
	  	  #endif
  	 }

  	/* Perintah untuk Compare buffer dan READ dari I2C */
  	 if (bitFlag & BFLAG_I2C_RD1)
  	 {
  		 #ifdef MASTER_BOARD
  		 	 /*##-4- I2C periperal siap menerima data ############################*/
  			 do {
  				 //printf("Master I2C Receiving... \r\n");

  				 if(HAL_I2C_Master_Receive_IT(&I2cHandle, (uint16_t)I2C_ADDRESS, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK) {Error_Handler();}

  				 /* Saat Acknowledge failure (Slave tidak mengakui alamatnya) Master memulai ulang komunikasi */
  			 } while (HAL_I2C_GetError(&I2cHandle) == HAL_I2C_ERROR_AF);

  			 /*##-5- Menunggu akhir transfer ###################################*/
  			 while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY){}
  			 //printf("Master I2C Receive Success\r\n\n");

  			 /* Jika di write di form 1 maka nilai buffer disimpan di var res1 dan atur BFLAG_I2C_WR1*/
  			 if ((bitFlag & BFLAG_I2C_WR1) && aRxBuffer[0]==1)
  			 {
  				 printf("Send RES1: \r\n");

  				 /* Reset bitflag */
  				 bitFlag 	&= ~BFLAG_I2C_WR1;
  			 }
  			 if ((bitFlag & BFLAG_I2C_WR2) && aRxBuffer[0]==2)
  			 {
  				 printf("Send RES2: \r\n");

  				 /* Reset bitflag */
  				 bitFlag 	&= ~BFLAG_I2C_WR2;
  			 }
  			 if ((bitFlag & BFLAG_I2C_WR3) && aRxBuffer[0]==3)
  			 {
  				 printf("Send RES3: \r\n");

  				 /* Reset bitflag */
  				 bitFlag 	&= ~BFLAG_I2C_WR3;
  			 }
  			 if ((bitFlag & BFLAG_I2C_WR1) && aRxBuffer[0]==4)
  			 {
  				 printf("Read RES1: ");

  				 /* Reset bitflag */
  				 bitFlag 	&= ~BFLAG_I2C_WR1;
  			 }
  			 else if ((bitFlag & BFLAG_I2C_WR2) && aRxBuffer[0]==5)
			 {
  				 printf("READ RES2: ");

  				/* Reset bitflag */
				 bitFlag 	&= ~BFLAG_I2C_WR2;
			 }
  			 else if ((bitFlag & BFLAG_I2C_WR3) && aRxBuffer[0]==6)
			 {
  				 printf("READ RES3: ");

  				/* Reset bitflag */
				 bitFlag 	&= ~BFLAG_I2C_WR3;
			 }
  			 else if ((bitFlag & BFLAG_I2C_WR1) && aRxBuffer[0]==7)
			 {
  				for (uint8_t i=0; i<10; i++){
					aTxBuffer[i+1] = 0;
				}
				copyValue(temp, aTxBuffer, 10, 10);
				aTxBuffer[0]=8;
				bitFlag |= BFLAG_I2C_WR2;
				bitFlag &= ~BFLAG_I2C_WR1;
			 }
  			 else if ((bitFlag & BFLAG_I2C_WR2) && aRxBuffer[0]==8)
			 {
  				for (uint8_t i=0; i<10; i++){
					aTxBuffer[i+1] = 0;
				}
				copyValue(temp, aTxBuffer, 20, 10);
				aTxBuffer[0]=9;
				bitFlag |= BFLAG_I2C_WR3;
				bitFlag &= ~BFLAG_I2C_WR2;
			 }
			 else if ((bitFlag & BFLAG_I2C_WR3) && aRxBuffer[0]==9)
			 {
				bitFlag &= ~BFLAG_I2C_WR3;
			 }

  			 if (aRxBuffer[0]<4)
  			 {
  				 /*##-6- Compare the sent and received buffers ##############################*/
				 if(Buffercmp(aTxBuffer,aRxBuffer, 11)){
					 printf("Buffer compare Fail!!!\r\n\n");
				 }
  			 }
  			 else if (aRxBuffer[0]>=4){
  				 printf("%ld; %ld; %ld; %ld; %ld; %ld; %ld; %ld; %ld; %ld\r\n\n",
  						 aRxBuffer[1], aRxBuffer[2], aRxBuffer[3],
						 aRxBuffer[4], aRxBuffer[5], aRxBuffer[6],
						 aRxBuffer[7], aRxBuffer[8], aRxBuffer[9], aRxBuffer[10]);
  			 }


  			 /* Reset bit 1 bitflag */
			 bitFlag 	&= ~BFLAG_I2C_RD1;
		 #endif
	 }
  	 if (bitFlag & BFLAG_I2C_RD0)
  	 {
  	  	 #ifdef MASTER_BOARD

  	  		 printf("Code: %ld\nTX integer: %ld; %ld; %ld; %ld; %ld; %ld; %ld; %ld; %ld; %ld\r\n",
  	  				 aTxBuffer[0], aTxBuffer[1], aTxBuffer[2], aTxBuffer[3],
					 aTxBuffer[4], aTxBuffer[5], aTxBuffer[6],
					 aTxBuffer[7], aTxBuffer[8], aTxBuffer[9], aTxBuffer[10]);
  	  		 printf("Code: %ld\nRX integer: %ld; %ld; %ld; %ld; %ld; %ld; %ld; %ld; %ld; %ld\r\n\n",
  	  				 aRxBuffer[0], aRxBuffer[1], aRxBuffer[2], aRxBuffer[3],
					 aRxBuffer[4], aRxBuffer[5], aRxBuffer[6],
					 aRxBuffer[7], aRxBuffer[8], aRxBuffer[9], aRxBuffer[10]);

  	  		 /* Reset bit 1 bitflag */
  			 bitFlag 	&= ~BFLAG_I2C_RD0;
  		 #endif
  	 }

  	 else			// Atur default jika nilai bitFlag 0000 0000 atau default
  	 {
		#ifndef MASTER_BOARD			//Hanya di def untuk Slave
  		 	 if(HAL_I2C_Slave_Receive_IT(&I2cHandle, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
  		 	 {Error_Handler();}

  		 	 /*##-3- Wait for the end of the transfer ###################################*/
  		 	 while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
  		 	 {}

  		 	 if(aRxBuffer[0]==-1)
  		 	 {
  		 		 MoveResToBuffer(res1, aTxBuffer, 10);

  		 	 }
  		 	 else if((aRxBuffer[0]==1)||(aRxBuffer[0]==7))
  		 	 {
  		 		 MoveBufferToRes(aRxBuffer, res1, 10);
				 MoveResToBuffer(res1, aTxBuffer, 10);
				 if(aRxBuffer[0]==1){aTxBuffer[0] = 1;}
				 if(aRxBuffer[0]==7){aTxBuffer[0] = 7;}
  		 	 }
  		 	 else if((aRxBuffer[0]==2)||(aRxBuffer[0]==8))
			 {
				 MoveBufferToRes(aRxBuffer, res2, 10);
				 MoveResToBuffer(res2, aTxBuffer, 10);
				 if(aRxBuffer[0]==2){aTxBuffer[0] = 2;}
				 if(aRxBuffer[0]==8){aTxBuffer[0] = 8;}
			 }
  		 	 else if((aRxBuffer[0]==3)||(aRxBuffer[0]==9))
			 {
				 MoveBufferToRes(aRxBuffer, res3, 10);
				 MoveResToBuffer(res3, aTxBuffer, 10);
				 if(aRxBuffer[0]==3){aTxBuffer[0] = 3;}
				 if(aRxBuffer[0]==9){aTxBuffer[0] = 9;}
			 }
  		 	 else if(aRxBuffer[0]==4)
			 {
  		 		 MoveResToBuffer(res1, aTxBuffer, 10);
  		 		 aTxBuffer[0] = 4;
			 }
  		 	 else if(aRxBuffer[0]==5)
			 {
  		 		 MoveResToBuffer(res2, aTxBuffer, 10);
  		 		 aTxBuffer[0] = 5;
			 }
  		 	 else if(aRxBuffer[0]==6)
			 {
  		 		 MoveResToBuffer(res3, aTxBuffer, 10);
  		 		 aTxBuffer[0] = 6;
			 }

  		 	 /*##-4- Kirim data ke Master #####################################*/
  		 	 if(HAL_I2C_Slave_Transmit_IT(&I2cHandle, (uint8_t*)aTxBuffer, TXBUFFERSIZE)!= HAL_OK)
  		 	 {Error_Handler();}

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
static uint16_t Buffercmp(uint32_t pBuffer1[], uint32_t pBuffer2[], uint8_t length){
	printf("Compare prosess...\r\n");
	for (uint8_t i = 0; i <= length; i++){
		if (pBuffer1[i]!=pBuffer1[i]){
			printf("Buffer different!!!\r\n");
			return i;
		}
	}
	printf("Buffer same!!!\r\n");
	return 0;
}

void MoveBufferToRes(uint32_t pBuffer[], uint32_t pRes[], uint8_t length){
	for (uint8_t i = 0; i <= length; i++){
		pRes[i] = pBuffer[i+1];
	}
}

void MoveResToBuffer(uint32_t pRes[], uint32_t pBuffer[], uint8_t length){
	for (uint8_t i = 0; i <= length; i++){
		pBuffer[i+1] = pRes[i];
	}
}

void copyValue(uint32_t source[], uint32_t target[], uint8_t start, uint8_t length){
	for (uint8_t i = 0; i < length; i++){
		target[i+1] = source[i+start];
	}
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
#define CFG_HEADER_NUM 			11
#define CFG_HEADER_CHARS_LEN 	5
#define STRLENMAX				256

static char str_cfg_header[CFG_HEADER_NUM][CFG_HEADER_CHARS_LEN] =
{
	"{Tes}",
	"{RSL}",
	"{WR1:",		"{WR2:",		"{WR3:",
	"{RD0}",
	"{RD1}",		"{RD2}",		"{RD3}",
	"{WRA:"
};

/*********************************************************************
 * @name	: tinysh_dec
 * @brief	: string to decimal conversion (up to 15 chars).
 *********************************************************************/
long tinysh_dec(char *s) {
  unsigned long res=0;
  uint8_t index = 0;
  int8_t min	= 1;			//Penanda masukan - atau +

  while(*s) {
	  res*=10;					//berfungsi menjadikan result kelipatan pangkat 10 (menjadikan puluhan)

	  if((*s == '-')&&(index == 0))			// jika inputan negatif
		  min = -1;
	  else if((*s == '0')&&(index == 0))	// jika inputan 0
		  res = 0;
	  else if(*s>='0' && *s<='9')			// jika inputan normal
		  res+=*s-'0';			//bagian ini berfungsi mengurangi ascii inputna dengan ascii 0 sehingga didapat nilai integer sesungguhnya
	  else
		  break;

	  s++;
	  index++;

	  if(index > 15)
	  {
		 break;
	  }
  }

  return (res * min);
}

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

			char str_res[20];

			if (u8_idx==1) {
				bitFlag |= BFLAG_I2C_RSL;
			}

			else if (u8_idx == 5){		//{RD0}
				/* READ HEADER */
				bitFlag |= BFLAG_I2C_RD0;
			}

			/* READ HEADER */
			else if (u8_idx == 6){ 		//{RD1} Read from res1
				aTxBuffer[0]=4;
				bitFlag |= BFLAG_I2C_WR1;
			}
			else if (u8_idx == 7){ 		//{RD2} Read from res2
				aTxBuffer[0]=5;
				bitFlag |= BFLAG_I2C_WR2;
			}
			else if (u8_idx == 8){		//{RD3} Read from res3
				aTxBuffer[0]=6;
				bitFlag |= BFLAG_I2C_WR3;
			}

			else if (u8_idx == 9){		//{WRA: Read all
				for (uint8_t i=0; i<30; i++){
					temp[i] = 0;
				}

				while (*pChar) {
					if(*pChar == ';' || *pChar == '}') {
						memset(&str_res[0], 0, 20);		// default/reset untuk mendeklarasikan array str_res akan bernilai 0 semua sebanyak 10 indeks mulai dari str_res[0]
						memcpy(&str_res[0], &pChar2[u8_stop], u8_start - u8_stop);	// mengcopy data array dari pChar2[u8_stop] sejumlah (u8_start - u8_stop0 char menuju str_res

						if(strlen(str_res)>0){
							temp[u8_cnt] = tinysh_dec(&str_res[0]);
						}

						if(*pChar == ';') { u8_stop = u8_start + 1; }
						else if(*pChar == '}') {
							for (uint8_t i=0; i<10; i++){
								aTxBuffer[i+1] = 0;
							}

							copyValue(temp, aTxBuffer, 0, 10);
							aTxBuffer[0]=7;
							bitFlag |= BFLAG_I2C_WR1;
						}
						u8_cnt++;
					}
					pChar++;
					u8_start++;
				}
				break;
			}

			else if ((u8_idx > 0) && (u8_idx < 5)) {
				/* Reset TX Buffer from index 1 to 10*/
				for (uint8_t i=0; i<10; i++){
					aTxBuffer[i+1] = 0;
				}

				/* WRITE HEADER */
				while (*pChar) {
					if(*pChar == ';' || *pChar == '}') {
						memset(&str_res[0], 0, 20);		// default/reset untuk mendeklarasikan array str_res akan bernilai 0 semua sebanyak 10 indeks mulai dari str_res[0]
						memcpy(&str_res[0], &pChar2[u8_stop], u8_start - u8_stop);	// mengcopy data array dari pChar2[u8_stop] sejumlah (u8_start - u8_stop0 char menuju str_res

						if(strlen(str_res)>0){
							aTxBuffer[u8_cnt+1] = tinysh_dec(&str_res[0]);
						}

						if(*pChar == ';') { u8_stop = u8_start + 1; }
						else if(*pChar == '}') {
							if (u8_idx == 2){ //Write at form 1
								/*Set first index as a trigger for slave*/
								aTxBuffer[0]=1;
								bitFlag |= BFLAG_I2C_WR1;
								bitFlag |= BFLAG_I2C_RD0;
							}
							else if (u8_idx == 3){ //Write at form 2
								aTxBuffer[0]=2;
								bitFlag |= BFLAG_I2C_WR2;
								bitFlag |= BFLAG_I2C_RD0;
							}
							else if (u8_idx == 4){ //Write at form 3
								aTxBuffer[0]=3;
								bitFlag |= BFLAG_I2C_WR3;
								bitFlag |= BFLAG_I2C_RD0;
							}
						}
						u8_cnt++;
					}
					pChar++;
					u8_start++;
				}
				break;
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
/*void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if ((GPIO_Pin == B1_Pin) && ((bitFlag & BFLAG_BTN) == 0))
  {
	  bitFlag |= BFLAG_BTN;
  }
}*/


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
