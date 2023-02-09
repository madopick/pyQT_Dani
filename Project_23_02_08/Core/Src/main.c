/* Includes */
#include "main.h"
#include "serial.h"
#include <stdio.h>
#include <string.h>

/* Private variables */
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;  /* DMA handle Structure definition */
uint8_t array[10]={0,0,0,0,0,0,0,0,0,0};

/* USER CODE BEGIN PV */
/*@brief Text strings printed on PC Com port for user information*/
uint8_t aRXBufferUser[RX_BUFFER_SIZE];

/*@brief Data buffers used to manage received data in interrupt routine*/
uint8_t aRXBufferA[RX_BUFFER_SIZE];

__IO uint32_t     uwNbReceivedChars;
uint8_t *pBufferReadyForUser;
uint8_t *pBufferResult;
//uint8_t *pBufferReadyForReception;
/* USER CODE END PV */

void SystemClock_Config(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
void Error_Handler(void);
/* USER CODE BEGIN PFP */
void StartReception(void);
void UserDataTreatment(UART_HandleTypeDef *huart, uint8_t* pData, uint16_t Size);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init(); //

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_DMA_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */
  /* Initiate Continuous reception */
  StartReception();
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

  /** Configure the main internal regulator output voltage*/
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.*/
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  RCC_OscInitStruct.PLL.PLLR = 6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
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

  /* USER CODE BEGIN USART2_Init 0 */
  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */
  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */
  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

}

/* USER CODE BEGIN 4 */

/**
  * @brief  This function prints user info on PC com port and initiates RX transfer
  * @retval None
  */
void StartReception(void)
{
  /* Initializes Buffer swap mechanism (used in User callback) :
     - 2 physical buffers aRXBufferA and aRXBufferB (RX_BUFFER_SIZE length)
  */
  //pBufferReadyForReception = aRXBufferA;
  pBufferReadyForUser      = aRXBufferA;
  uwNbReceivedChars        = 0;

  if (HAL_OK != HAL_UARTEx_ReceiveToIdle_DMA(&huart2, aRXBufferUser, RX_BUFFER_SIZE))
  {
    Error_Handler();
  }
}

/**
  * @brief  This function handles buffer containing received data on PC com port
  * @note   In this example, received data are sent back on UART Tx (loopback)
  *         Any other processing such as copying received data in a larger buffer to make it
  *         available for application, could be implemented here.
  * @note   This routine is executed in Interrupt context.
  * @param  huart UART handle.
  * @param  pData Pointer on received data buffer to be processed
  * @retval Size  Nb of received characters available in buffer
  */
void UserDataTreatment(UART_HandleTypeDef *huart, uint8_t* pData, uint16_t Size)
{
  /*
   * This function might be called in any of the following interrupt contexts :
   *  - DMA TC and HT events
   *  - UART IDLE line event
   *
   * pData and Size defines the buffer where received data have been copied, in order to be processed.
   * During this processing of already received data, reception is still ongoing.
   *
   */
  uint8_t* pBuff = pData;
  uint8_t  i;

  /* Implementation of loopback is on purpose implemented in direct register access,
     in order to be able to echo received characters as fast as they are received.
     Wait for TC flag to be raised at end of transmit is then removed, only TXE is checked */
  for (i = 0; i < Size; i++)
  {
    while (!(__HAL_UART_GET_FLAG(huart, UART_FLAG_TXE))) {}
    huart->Instance->DR = *pBuff;
    pBuff++;
  }

}

/**
  * @brief  User implementation of the Reception Event Callback
  *         (Rx event notification called after use of advanced reception service).
  * @param  huart UART handle
  * @param  Size  Number of data available in application reception buffer (indicates a position in
  *               reception buffer until which, data are available)
  * @retval None
  */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  static uint8_t old_pos = 0;
  uint8_t *ptemp;
  uint8_t i;

  /* Check if number of received data in recpetion buffer has changed */
  if (Size != old_pos)
  {
    /* Check if position of index in reception buffer has simply be increased
       of if end of buffer has been reached */
    if (Size > old_pos)
    {
      /* Current position is higher than previous one */
    //uwNbReceivedChars pertama 0 kemudian diubah di bawah ini
      uwNbReceivedChars = Size - old_pos;
      /* Copy received data in "User" buffer for evacuation */
      for (i = 0; i < uwNbReceivedChars; i++)
      {
        pBufferReadyForUser[i] = aRXBufferUser[old_pos + i];
      }
    }
    else
    {
      /* Current position is lower than previous one : end of buffer has been reached */
      /* First copy data from current position till end of buffer */
      uwNbReceivedChars = RX_BUFFER_SIZE - old_pos;
      /* Copy received data in "User" buffer for evacuation */
      for (i = 0; i < uwNbReceivedChars; i++)
      {
        pBufferReadyForUser[i] = aRXBufferUser[old_pos + i];
      }
      /* Check and continue with beginning of buffer */
      if (Size > 0)
      {
        for (i = 0; i < Size; i++)
        {
          pBufferReadyForUser[uwNbReceivedChars + i] = aRXBufferUser[i];
        }
        uwNbReceivedChars += Size;
      }
    }
    /* Process received data that has been extracted from Rx User buffer */

    toInt(array, pBufferReadyForUser);

    tostring(pBufferReadyForUser[0],array[0]);
    tostring(pBufferReadyForUser[1],array[1]);
    tostring(pBufferReadyForUser[2],array[2]);
    tostring(pBufferReadyForUser[3],array[3]);
    tostring(pBufferReadyForUser[4],array[4]);
    tostring(pBufferReadyForUser[5],array[5]);
    tostring(pBufferReadyForUser[6],array[6]);
    tostring(pBufferReadyForUser[7],array[7]);
    tostring(pBufferReadyForUser[8],array[8]);
    tostring(pBufferReadyForUser[9],array[9]);

    UserDataTreatment(huart, pBufferReadyForUser, uwNbReceivedChars);


  }
  /* Update old_pos as new reference of position in User Rx buffer that
     indicates position to which data have been processed */
  old_pos = Size;
}

void toInt(int array[], char str[]){
    char *token;
    int jumlah=0, count=0;

    token = strtok(str, ";");

    while( token != NULL ) {
        array[count]=atoi(token);
        token = strtok(NULL, ";");
        count+=1;
    }
}

void tostring(char str[], int num) {
    int i, rem, len = 0, n;

    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
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
  while (1)
  {
    /* Toggle LED2 for error */
  }
  /* USER CODE END Error_Handler_Debug */
}

extern uint8_t u8arr_uart[UART_BUF_SZ];
void uartProcessing (uint8_t *u8p_buffer, uint16_t u16_size)
{
	printf("UART RX(%d): %s\r\n", u16_size, (char*)u8p_buffer);
	//u8arr_uart[0];
	tostring(u8arr_uart[0],array[0]);
	tostring(u8arr_uart[1],array[1]);
	tostring(u8arr_uart[2],array[2]);
	tostring(u8arr_uart[3],array[3]);
	tostring(u8arr_uart[4],array[4]);
	tostring(u8arr_uart[5],array[5]);
	tostring(u8arr_uart[6],array[6]);
	tostring(u8arr_uart[7],array[7]);
	tostring(u8arr_uart[8],array[8]);
	tostring(u8arr_uart[9],array[9]);

	//UserDataTreatment(huart, u8arr_uart, uwNbReceivedChars);
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
