/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo.h"
#include <stdio.h>
#include <string.h>

/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void i2cProcessing (uint8_t *u8p_buffer, uint16_t u16_size);
void uartProcessing (uint8_t *u8p_buffer, uint16_t u16_size);

#define BFLAG_UART_RCV		(1 << 0)		// To Interact with UART
#define BFLAG_I2C_RBA		(1 << 1)		// To Trigger default input at form as byte
#define BFLAG_I2C_WBS		(1 << 2)		// To Trigger default input at form as byte
#define BFLAG_I2C_RBS		(1 << 3)		// To Read data from Slave as byte
#define BFLAG_I2C_WBA		(1 << 4)		// To Write all form data to Slave

#define RX_EVENT_CB

/* I2C */
#define I2C_ADDRESS        0x30F

/**********************************************************
 * PARSING HEADER, Used in FW CONFIG - READ/WRITE Process
 **********************************************************/
#define CFG_LENGTH 				10
#define CFG_HEADER_NUM 			20
#define CFG_HEADER_LEN 			5
#define STRLENMAX				256


/* Private defines -----------------------------------------------------------*/
#define B1_Pin 				GPIO_PIN_13
#define B1_GPIO_Port 		GPIOC
#define USART_TX_Pin 		GPIO_PIN_2
#define USART_TX_GPIO_Port 	GPIOA
#define USART_RX_Pin 		GPIO_PIN_3
#define USART_RX_GPIO_Port 	GPIOA
#define LD2_Pin 			GPIO_PIN_5
#define LD2_GPIO_Port 		GPIOA
#define TMS_Pin 			GPIO_PIN_13
#define TMS_GPIO_Port 		GPIOA
#define TCK_Pin 			GPIO_PIN_14
#define TCK_GPIO_Port 		GPIOA
#define SWO_Pin 			GPIO_PIN_3
#define SWO_GPIO_Port 		GPIOB


/* Definition for I2Cx clock resources */
#define I2Cx                             I2C1
#define I2Cx_CLK_ENABLE()                __HAL_RCC_I2C1_CLK_ENABLE()
#define I2Cx_SDA_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()

#define I2Cx_FORCE_RESET()               __HAL_RCC_I2C1_FORCE_RESET()
#define I2Cx_RELEASE_RESET()             __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define I2Cx_SCL_PIN                    GPIO_PIN_6
#define I2Cx_SCL_GPIO_PORT              GPIOB
#define I2Cx_SCL_AF                     GPIO_AF4_I2C1
#define I2Cx_SDA_PIN                    GPIO_PIN_9
#define I2Cx_SDA_GPIO_PORT              GPIOB
#define I2Cx_SDA_AF                     GPIO_AF4_I2C1

/* Definition for I2Cx's NVIC */
#define I2Cx_EV_IRQn                    I2C1_EV_IRQn
#define I2Cx_EV_IRQHandler              I2C1_EV_IRQHandler
#define I2Cx_ER_IRQn                    I2C1_ER_IRQn
#define I2Cx_ER_IRQHandler              I2C1_ER_IRQHandler

/* Size of Transmission buffer */
#define TXBUFFERSIZE                      100 //(COUNTOF(aTxBuffer) - 1)
/* Size of Reception buffer */
#define RXBUFFERSIZE                      TXBUFFERSIZE

/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* Exported functions ------------------------------------------------------- */


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
