/* This Program is used for accept input from UI
 * The input is a string of 10 integer
 * After that, this program will sparate and parse
 * the input to 10 of integer and send it back to UI
 * The UI is on the ui_23_02_15
 * The different between project_23_02_06 and this one is
 * This program will save the value on variabel so it will be callable anytime and static
*/

/* Includes */
#include "main.h"
#include "serial.h"
#include <stdio.h>
#include <string.h>

/* Private variables */
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;  /* DMA handle Structure definition */

/* USER CODE BEGIN PV */
/*@brief Text strings printed on PC Com port for user information*/

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);

/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#ifdef __GNUC__
  /* With GCC, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/* USER CODE END 0 */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}

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
  HAL_Init(); //

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_DMA_Init();
  MX_USART2_UART_Init();

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
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  huart2.Instance 			= USART2;
  huart2.Init.BaudRate 		= 115200;
  huart2.Init.WordLength 	= UART_WORDLENGTH_8B;
  huart2.Init.StopBits 		= UART_STOPBITS_1;
  huart2.Init.Parity 		= UART_PARITY_NONE;
  huart2.Init.Mode 			= UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl 	= UART_HWCONTROL_NONE;
  huart2.Init.OverSampling 	= UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */
  /* USER CODE END USART2_Init 2 */

  serial_init();

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

/**********************************************************
 * PARSING HEADER, Used in FW CONFIG - READ/WRITE Process
 **********************************************************/
#define CFG_LENGTH 				10
#define CFG_HEADER_NUM 			7
#define CFG_HEADER_CHARS_LEN 	5
#define CFG_HEADER_READ 		5
#define STRLENMAX				100

static char str_cfg_header[CFG_HEADER_NUM][CFG_HEADER_CHARS_LEN] =
{
	"{MSG:",
	"{CF1:",
	"{CF2:",
	"{CF3:",
	"{RD1}",
	"{RD2}",
	"{RD3}"
};

int32_t i32_res1[CFG_LENGTH] = {10,256,5124,37,10,-45,123,46,-78,89};	// Menyimpan hasil konversi string ke integer
int32_t i32_res2[CFG_LENGTH] = {10,-9,88,-77,666,-555,4444,-3333,22222,-11111};
int32_t i32_res3[CFG_LENGTH] = {1,-2,33,-44,555,-666,7777,-8888,99999,0};

/*********************************************************************
 * @name	: tinysh_dec
 * @brief	: string to decimal conversion (up to 15 chars).
 *********************************************************************/
unsigned long tinysh_dec(char *s) {
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
 * 	Example: {MSG:1;23;21009}						*
 ********************************************************/
static void vShell_cmdParse(char *input) {
	for(uint8_t u8_idx = 0; u8_idx < CFG_HEADER_NUM; u8_idx++) {
		if(!memcmp(input,(char*)&str_cfg_header[u8_idx][0], CFG_HEADER_CHARS_LEN)) {
			char *pChar 		= &input[CFG_HEADER_CHARS_LEN];		//pointer untuk menyimpan header (5 char)
			char *pChar2 		= &input[CFG_HEADER_CHARS_LEN];		//pointer untuk menyimpan header (5 char)
			uint8_t u8_start 	= 0;			// ini penanda yang akan bergeser tiap indeks (menandai lokasi dari sparating symboll)
			uint8_t u8_stop 	= 0;			// ini penenda start pointer paling awal atau awal baru setelah sparating symboll
			uint8_t u8_cnt 		= 0;			// menampung ada berapa sih inputan nya

			char str_res[20];				// Menyimpan satu token (satu value utuh sebelum sparating symboll)

			if (u8_idx == 1) {
				/* WRITE HEADER */
				while (*pChar) {
					if(*pChar == ';') {				// iterasi pertama pasti skip ini karena ini fungsinya kalo ketemu sparating symboll
						memset(&str_res[0], 0, 10);		// default/reset untuk mendeklarasikan array str_res akan bernilai 0 semua sebanyak 10 indeks mulai dari str_res[0]
						memcpy(&str_res[0], &pChar2[u8_stop], u8_start - u8_stop);	// mengcopy data array dari pChar2[u8_stop] sejumlah (u8_start - u8_stop0 char menuju str_res
						if(strlen(str_res)>0){
							i32_res1[u8_cnt] = tinysh_dec(&str_res[0]);
						}

						u8_stop = u8_start + 1;
						u8_cnt++;
					}
					else if (*pChar == '}') {		// konsepnya mirip yang atas, cuma dia akan break
						memset(&str_res[0], 0, 10);
						memcpy(&str_res[0], &pChar2[u8_stop], u8_start - u8_stop);
						if(strlen(str_res)>0){
							i32_res1[u8_cnt] = tinysh_dec(&str_res[0]);
						}

						u8_cnt++;
						break;
					}
					pChar++;
					u8_start++;
				}
				break;
			}

			else if (u8_idx == 2) {
				/* WRITE HEADER */
				while (*pChar) {
					if(*pChar == ';') {				// iterasi pertama pasti skip ini karena ini fungsinya kalo ketemu sparating symboll
						memset(&str_res[0], 0, 10);		// default/reset untuk mendeklarasikan array str_res akan bernilai 0 semua sebanyak 10 indeks mulai dari str_res[0]
						memcpy(&str_res[0], &pChar2[u8_stop], u8_start - u8_stop);	// mengcopy data array dari pChar2[u8_stop] sejumlah (u8_start - u8_stop0 char menuju str_res
						if(strlen(str_res)>0){
							i32_res2[u8_cnt] = tinysh_dec(&str_res[0]);
						}

						u8_stop = u8_start + 1;
						u8_cnt++;
					}
					else if (*pChar == '}') {		// konsepnya mirip yang atas, cuma dia akan break
						memset(&str_res[0], 0, 10);
						memcpy(&str_res[0], &pChar2[u8_stop], u8_start - u8_stop);
						if(strlen(str_res)>0){
							i32_res2[u8_cnt] = tinysh_dec(&str_res[0]);
						}

						u8_cnt++;
						break;
					}
					pChar++;
					u8_start++;
				}
				break;
			}

			else if (u8_idx == 3) {
				/* WRITE HEADER */
				while (*pChar) {
					if(*pChar == ';') {				// iterasi pertama pasti skip ini karena ini fungsinya kalo ketemu sparating symboll
						memset(&str_res[0], 0, 10);		// default/reset untuk mendeklarasikan array str_res akan bernilai 0 semua sebanyak 10 indeks mulai dari str_res[0]
						memcpy(&str_res[0], &pChar2[u8_stop], u8_start - u8_stop);	// mengcopy data array dari pChar2[u8_stop] sejumlah (u8_start - u8_stop0 char menuju str_res
						if(strlen(str_res)>0){
							i32_res3[u8_cnt] = tinysh_dec(&str_res[0]);
						}

						u8_stop = u8_start + 1;
						u8_cnt++;
					}
					else if (*pChar == '}') {		// konsepnya mirip yang atas, cuma dia akan break
						memset(&str_res[0], 0, 10);
						memcpy(&str_res[0], &pChar2[u8_stop], u8_start - u8_stop);
						if(strlen(str_res)>0){
							i32_res3[u8_cnt] = tinysh_dec(&str_res[0]);
						}

						u8_cnt++;
						break;
					}
					pChar++;
					u8_start++;
				}
				break;
			}

			else if (u8_idx == 4){
				/* READ HEADER */
				read_header(i32_res1);
			}

			else if (u8_idx == 5){
				/* READ HEADER */
				read_header(i32_res2);
			}

			else if (u8_idx == 6){
				/* READ HEADER */
				read_header(i32_res3);
			}
		}
	}
}


void read_header(int32_t arr[]){		/* READ HEADER */
	char sendStr[STRLENMAX];
	memset (sendStr, 0, STRLENMAX);
	snprintf(sendStr, STRLENMAX,
			"READ: %ld; %ld; %ld; %ld; %ld; %ld; %ld; %ld; %ld; %ld",
			arr[0] , arr[1], arr[2], arr[3],
			arr[4] , arr[5], arr[6],
			arr[7] , arr[8], arr[9]);
	HAL_UART_Transmit(&huart2, (uint8_t *)sendStr, strlen(sendStr), 0xFFFF);
	//HAL_UART_Transmit (&huart2, yang dikirim, panjangnya yang dikirim, timeout)
}


void uartProcessing (uint8_t *u8p_buffer, uint16_t u16_size)
{		//Bagian ini akan dipanggil di serial.c, line 59
	vShell_cmdParse((char*)u8p_buffer);
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
