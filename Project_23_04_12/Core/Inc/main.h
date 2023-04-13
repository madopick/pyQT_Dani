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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void uartProcessing (uint8_t *u8p_buffer, uint16_t u16_size);

#define RX_EVENT_CB

/* BIT FLAG */
#define BFLAG_UART_RCV								(1 << 0)
#define BFLAG_SPIM_RD								(1 << 1)
#define BFLAG_SPIM_WR								(1 << 2)
#define BFLAG_SPIS_RD								(1 << 3)
#define BFLAG_SPIS_WR								(1 << 4)
#define BFLAG_BTN								 	(1 << 5)



/**********************************************************
 * PARSING HEADER, Used in FW CONFIG - READ/WRITE Process
 **********************************************************/
#define CFG_LENGTH 				10
#define CFG_HEADER_NUM 			20
#define CFG_HEADER_LEN 			5
#define STRLENMAX				256

/* Private defines -----------------------------------------------------------*/
#define Blue_Btn_Pin 		GPIO_PIN_13
#define Blue_Btn_GPIO_Port 	GPIOC
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

/* Definition for SPIx Pins */
#define SPI2_SCK_PIN                     GPIO_PIN_10
#define SPI2_SCK_GPIO_PORT               GPIOB
#define SPI2_SCK_AF                      GPIO_AF5_SPI2

#define SPI2_MISO_PIN                    GPIO_PIN_2
#define SPI2_MISO_GPIO_PORT              GPIOC
#define SPI2_MISO_AF                     GPIO_AF5_SPI2

#define SPI2_MOSI_PIN                    GPIO_PIN_1
#define SPI2_MOSI_GPIO_PORT              GPIOC
#define SPI2_MOSI_AF                     GPIO_AF7_SPI2

#define BUFFERSIZE						 100

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
