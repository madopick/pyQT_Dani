/* This Program is used for accept multiple input of integer from UI.
 * The data can be read as byte or ASCII as you want and it can chosen at UI
 * Data will be transmit to Slave by I2C.
 * Slave will save the value on variables and send back to Master Board to be compared when it needed
 * The UI is on the ui_23_03_14
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "serial.h"
#include "uart_prog.h"
#include "i2c_prog.h"

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
I2C_HandleTypeDef I2cHandle;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);


/* Private user code ---------------------------------------------------------*/
#ifdef __GNUC__
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#ifdef RX_EVENT_CB
static uint8_t u8arr_eventBuff[UART_BUF_SZ];
static uint8_t u8arr_uartEvent[UART_BUF_SZ];
#endif

static uint16_t u16_oldPos = 0;
static uint16_t u16_lenCnt = 0;

/* @brief  Retargets the C library printf function to the USART. */
PUTCHAR_PROTOTYPE {
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}




/*MASTER BOARD*/
#define MASTER_BOARD;	/* Activated this part for make as master board*/



char sendStr[STRLENMAX];



/* bit flag */
uint16_t bitFlag=0;

/* I2C handler declaration */
I2C_HandleTypeDef I2cHandle;

/* Buffer used for transmission */
int32_t aTxBuffer[] = {0,10,-10,10,-10,10,-10,10,-10,10,-10};
/* TRIGGER CODE of Slave
 * {0,-,--}  // DEFAULT
 * {1,-,--}	 // Master Transmit data to slave(res1)
 * {2,-,--}	 // Master Transmit data to slave(res2)
 * {3,-,--}	 // Master Transmit data to slave(res3)
 * {4,-,--}	 // Slave Receive trigger and transmit res1 to master
 * {5,-,--}	 // Slave Receive trigger and transmit res2 to master
 * {6,-,--}	 // Slave Receive trigger and transmit res3 to master
 * {7,-,--}	 // Transmite All 1
 * {8,-,--}	 // Transmite All 2
 * {9,-,--}	 // Transmite All 3
 * */

/* Buffer used for reception */
int32_t aRxBuffer[11];



/* Default variabel for 3 array */
#ifndef MASTER_BOARD
int32_t res1[10] = {1,-1,11,-11,4,-4,44,-44,7,-7};
int32_t res2[10] = {2,-2,22,-22,5,-5,55,-55,8,-8};
int32_t res3[10] = {3,-3,33,-33,6,-6,66,-66,9,-9};
#endif

int32_t temp[30];


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
  while (1)
  {
  	  if (bitFlag & BFLAG_UART_RCV) {			/* UART Processing*/
  		  uartProcessing (u8arr_uartEvent, u16_lenCnt - 2); // remove \r & \n
  		  memset(u8arr_uartEvent, 0, UART_BUF_SZ);
  		  u16_lenCnt = 0;

  		  bitFlag 	&= ~BFLAG_UART_RCV;
  	  }

  	  // Trigger default input at form
  	  if (bitFlag & BFLAG_I2C_RBA) {			//Read Byte All (Trigger once at the first time)
	  	  #ifdef MASTER_BOARD
  		  memset (sendStr, 0, STRLENMAX);
		  sendStr[0] = 0x10;
		  sendStr[1] = 0x11;
		  for (uint8_t i=4; i<=6; i++){
			  aTxBuffer[0] = i;

			  I2C_Master_Transmit();

			  I2C_Master_Receive();

			  if (aRxBuffer[0]==4){
				  memcpy(&sendStr[2], &aRxBuffer[1], CFG_LENGTH * sizeof(&aRxBuffer[1]));
			  }
			  else if (aRxBuffer[0]==5){
				  memcpy(&sendStr[42], &aRxBuffer[1], CFG_LENGTH * sizeof(&aRxBuffer[1]));
			  }
			  else if (aRxBuffer[0]==6){
				  memcpy(&sendStr[82], &aRxBuffer[1], CFG_LENGTH * sizeof(&aRxBuffer[1]));
			  }
		  }

		  HAL_UART_Transmit(&huart2, (uint8_t *)sendStr, 122, 0xFFFF);

		  bitFlag &= ~BFLAG_I2C_RBA;
		  #endif
  	  }

  	  if (bitFlag & BFLAG_I2C_RAA) {
		  #ifdef MASTER_BOARD
		  for (uint8_t i=4; i<=6; i++){
			  aTxBuffer[0] = i;

			  I2C_Master_Transmit();


			  I2C_Master_Receive();

			  if (aRxBuffer[0]==4){
				  printf("R1: ");
			  }
			  else if (aRxBuffer[0]==5){
				  printf("R2: ");
			  }
			  else if (aRxBuffer[0]==6){
				  printf("R3: ");
			  }
			  printf("%ld; %ld; %ld; %ld; %ld; %ld; %ld; %ld; %ld; %ld;\r\n",
					 aRxBuffer[1], aRxBuffer[2], aRxBuffer[3],
					 aRxBuffer[4], aRxBuffer[5], aRxBuffer[6],
					 aRxBuffer[7], aRxBuffer[8], aRxBuffer[9], aRxBuffer[10]);
		  }
		  printf("END\r\n");
		  bitFlag &= ~BFLAG_I2C_RAA;
		  #endif
  	  }

  	  /* Command to receive from I2C */
  	  if (bitFlag & BFLAG_I2C_RAS)			//Read ASCII Slave
  	  {
			#ifdef MASTER_BOARD

			I2C_Master_Receive();

			// BFLAG_I2C_WAS		WRITE SLAVE
			if (aRxBuffer[0]==1)				//WR1
			{
				printf("Send RES1: \r\n");
			}
			if (aRxBuffer[0]==2)				//WR2
			{
				printf("Send RES2: \r\n");
			}
			if (aRxBuffer[0]==3)				//WR3
			{
				printf("Send RES3: \r\n");
			}

			if (aRxBuffer[0]<4)				// Compare TxBuffer and Rx Buffer
			{
				/*##-6- Compare the sent and received buffers ##############################*/
				if(Buffercmp(aTxBuffer,aRxBuffer, 11)){
					printf("Buffer compare Fail!!!\r\n\n");
				}
				bitFlag 	&= ~BFLAG_I2C_WAS;
			}


			// BFLAG_I2C_RAS		READ SLAVE
			if (aRxBuffer[0]==4)				//RD1
			{
				printf("Read RES1: ");
			}
			if (aRxBuffer[0]==5)				//RD2
			{
				printf("READ RES2: ");
			}
			if (aRxBuffer[0]==6)				//RD3
			{
				printf("READ RES3: ");
			}

			if ((aRxBuffer[0]>=4) && (aRxBuffer[0]<=6)){				// Print the variable value
				printf("%ld; %ld; %ld; %ld; %ld; %ld; %ld; %ld; %ld; %ld\r\n\n",
						aRxBuffer[1], aRxBuffer[2], aRxBuffer[3],
						aRxBuffer[4], aRxBuffer[5], aRxBuffer[6],
						aRxBuffer[7], aRxBuffer[8], aRxBuffer[9], aRxBuffer[10]);
				bitFlag 	&= ~BFLAG_I2C_WAS;
			}

			// BFLAG_I2C_RAA			Trigger default input at form
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
				bitFlag &= ~BFLAG_I2C_WAA;
			}

			if (aRxBuffer[0]>=7)				// Print the variable value
			{
				printf("%ld; %ld; %ld; %ld; %ld; %ld; %ld; %ld; %ld; %ld\r\n\n",
						aRxBuffer[1], aRxBuffer[2], aRxBuffer[3],
						aRxBuffer[4], aRxBuffer[5], aRxBuffer[6],
						aRxBuffer[7], aRxBuffer[8], aRxBuffer[9], aRxBuffer[10]);
			}

			/* Reset bit 1 bitflag */
			bitFlag 	&= ~BFLAG_I2C_RAS;
			#endif
	 }

	 if (bitFlag & BFLAG_I2C_RDB)
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

			 bitFlag 	&= ~BFLAG_I2C_RDB;
		 #endif
	 }

  	  /* Command to WRITE or sending I2C */
  	  if ((bitFlag & BFLAG_I2C_WAS) || (bitFlag & BFLAG_I2C_WAA)) {
	  	  #ifdef MASTER_BOARD

  		  I2C_Master_Transmit();

		  bitFlag |= BFLAG_I2C_RAS;
		  /* Reset bit 2 bitflag ada di bagian BFLAG_I2C_RD0 */
	  	  #endif
  	 }

  	 if((bitFlag & BFLAG_I2C_WBS) || (bitFlag & BFLAG_I2C_WBA))
  	 {
		 #ifdef MASTER_BOARD

  		 I2C_Master_Transmit();

  		 bitFlag |= BFLAG_I2C_RBS;
 	 	 #endif
  	 }

  	 if (bitFlag & BFLAG_I2C_RBS)
	 {
  		 #ifdef MASTER_BOARD

  		 I2C_Master_Receive();


		 // BFLAG_I2C_WAS		WRITE SLAVE
		 if (aRxBuffer[0]==1)				//WR1
		 {
			 char send[] = {0x14, 0x15,
				  'S', 'E', 'N', 'D', ' ', 'R', 'E', 'S', '1', ':', 0x0d, 0x0a};
			 HAL_UART_Transmit(&huart2, (uint8_t *)send, 14, 0xFFFF);
		 }
		 if (aRxBuffer[0]==2)				//WR2
		 {
			 char send[] = {0x14, 0x15,
				  'S', 'E', 'N', 'D', ' ', 'R', 'E', 'S', '2', ':', 0x0d, 0x0a};
			 HAL_UART_Transmit(&huart2, (uint8_t *)send, 14, 0xFFFF);
		 }
		 if (aRxBuffer[0]==3)				//WR3
		 {
			 char send[] = {0x14, 0x15,
				  'S', 'E', 'N', 'D', ' ', 'R', 'E', 'S', '3', ':', 0x0d, 0x0a};
			 HAL_UART_Transmit(&huart2, (uint8_t *)send, 14, 0xFFFF);
		 }

		 if (aRxBuffer[0]<4)				// Compare TxBuffer and Rx Buffer
		 {
			 //##-6- Compare the sent and received buffers ##############################
			 if(Buffercmp_Byte(aTxBuffer,aRxBuffer, 11)){
				 char send[] = {0x14, 0x15,
						 'B','u','f','f','e','r',' ','C','o','m','p','a','r','e',' ','F','a','i','l','!','!',0x0d, 0x0a};
				 HAL_UART_Transmit(&huart2, (uint8_t *)send, 25, 0xFFFF);
			 }
			 bitFlag 	&= ~BFLAG_I2C_WBS;
		 }


		 // BFLAG_I2C_RAS		READ SLAVE
		 if (aRxBuffer[0]==4)				//RD1
		 {
			 char send[] = {0x14, 0x15,
				  'R', 'E', 'A', 'D', ' ', 'R', 'E', 'S', '1', ':', 0x0d, 0x0a};
			 HAL_UART_Transmit(&huart2, (uint8_t *)send, 14, 0xFFFF);
		 }
		 if (aRxBuffer[0]==5)				//RD2
		 {
			 char send[] = {0x14, 0x15,
				  'R', 'E', 'A', 'D', ' ', 'R', 'E', 'S', '2', ':', 0x0d, 0x0a};
			 HAL_UART_Transmit(&huart2, (uint8_t *)send, 14, 0xFFFF);
		 }
		 if (aRxBuffer[0]==6)				//RD3
		 {
			 char send[] = {0x14, 0x15,
				  'R', 'E', 'A', 'D', ' ', 'R', 'E', 'S', '3', ':', 0x0d, 0x0a};
			 HAL_UART_Transmit(&huart2, (uint8_t *)send, 14, 0xFFFF);
		 }

		 if ((aRxBuffer[0]>=4) && (aRxBuffer[0]<=6)){				// Print the variable value
			 memset (sendStr, 0, STRLENMAX);
			 sendStr[0] = 0x12;
			 sendStr[1] = 0x13;

			 memcpy(&sendStr[2], &aRxBuffer[1], CFG_LENGTH * sizeof(aRxBuffer[0]));

			 sendStr[42] = 0x0d;
			 sendStr[43] = 0x0a;

			 HAL_UART_Transmit(&huart2, (uint8_t *)sendStr, 44, 0xFFFF);

			 bitFlag 	&= ~BFLAG_I2C_WBS;
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
			 bitFlag &= ~BFLAG_I2C_WBA;
		 }

		 /* Reset bit 1 bitflag */
		 bitFlag 	&= ~BFLAG_I2C_RBS;
		 #endif
	 }

  	 else
  	 {
		 #ifndef MASTER_BOARD			// Slave Only
  		 I2C_Slave_Receive();


		 if((aRxBuffer[0]==1)||(aRxBuffer[0]==7))		// WR1
		 {
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

		 I2C_Slave_Transmit();
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
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *I2cHandle){
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
}
#else
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
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2cHandle) {
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
}
#else
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
