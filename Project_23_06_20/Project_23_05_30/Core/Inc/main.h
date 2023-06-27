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
#include <stdio.h>
#include <string.h>


/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/


/* Exported constants --------------------------------------------------------*/


/* Exported macro ------------------------------------------------------------*/


/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(char * file, int line);


/* Private defines -----------------------------------------------------------*/
#define RX_EVENT_CB
#define UART_BUF_SZ                                 (512)

/* BIT FLAG */
//#define BFLAG_UART_RCV                              (1 << 0)
//#define BFLAG_I2CM_RD                               (1 << 1)
//#define BFLAG_I2CM_WR                               (1 << 2)
//#define BFLAG_I2CS_RD                               (1 << 3)
//#define BFLAG_I2CS_WR                               (1 << 4)
//#define BFLAG_RD1                                   (1 << 5)
//#define BFLAG_RD2                                   (1 << 6)
//#define BFLAG_RD3                                   (1 << 7)
//#define BFLAG_WR1                                   (1 << 8)
//#define BFLAG_WR2                                   (1 << 9)
//#define BFLAG_WR3                                   (1 << 10)
//#define BFLAG_RBA                                   (1 << 11)
//#define BFLAG_WBA                                   (1 << 12)
#define BFLAG_USB                                   (1 << 13)
//#define BFLAG_SPI_SLAVE_WR                          (1 << 14)
//#define BFLAG_SPI_SLAVE_RD                          (1 << 15)

#define BFLAG_UART_RCV		(1 << 0)		// To Interact with UART
#define BFLAG_I2C_WBA		(1 << 1)		// To Write all form data to Slave
#define BFLAG_I2C_WBS		(1 << 2)		// To Write data to Slave as byte
#define BFLAG_I2C_RBA		(1 << 3)		// To Trigger default input at form as byte
#define BFLAG_I2C_RBS		(1 << 4)		// To Read data from Slave as byte

#define BFLAG_TABLE1		(1 << 5)		// Run Table
#define BFLAG_TABLE2		(1 << 6)		// Run Table
#define BFLAG_TABLE3		(1 << 7)		// Run Table
#define BFLAG_TABLE4		(1 << 8)		// Run Table
#define BFLAG_TABLE5		(1 << 9)		// Run Table
#define BFLAG_TABLE6		(1 << 10)		// Run Table
#define BFLAG_TABLE7		(1 << 11)		// Run Table
#define BFLAG_TABLE8		(1 << 12)		// Run Table
#define BFLAG_TABLE9		(1 << 13)		// Run Table
#define BFLAG_TABLE10		(1 << 14)		// Run Table

#define BFLAG_RND			(1 << 15)		// Random Generate for Config



#define SIZE_BYTE					64
#define SIZE_BYTE_HEX				0x40

///Header Request
typedef enum
{
    MSG_HEADER = 0,
    CF1_HEADER,
    CF2_HEADER,
    CFA_HEADER,
    USB_OUT_HEADER,
    RD1_HEADER,
    RD2_HEADER,
    RD3_HEADER,
    RD4_HEADER,
    RDALL_HEADER,
} eREQ_HEADER_t;




#define USER_Btn_Pin GPIO_PIN_13
#define USER_Btn_GPIO_Port GPIOC
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOH
#define RMII_MDC_Pin GPIO_PIN_1
#define RMII_MDC_GPIO_Port GPIOC
#define RMII_REF_CLK_Pin GPIO_PIN_1
#define RMII_REF_CLK_GPIO_Port GPIOA
#define RMII_MDIO_Pin GPIO_PIN_2
#define RMII_MDIO_GPIO_Port GPIOA
#define RMII_CRS_DV_Pin GPIO_PIN_7
#define RMII_CRS_DV_GPIO_Port GPIOA
#define RMII_RXD0_Pin GPIO_PIN_4
#define RMII_RXD0_GPIO_Port GPIOC
#define RMII_RXD1_Pin GPIO_PIN_5
#define RMII_RXD1_GPIO_Port GPIOC
#define LD1_Pin GPIO_PIN_0
#define LD1_GPIO_Port GPIOB
#define RMII_TXD1_Pin GPIO_PIN_13
#define RMII_TXD1_GPIO_Port GPIOB
#define LD3_Pin GPIO_PIN_14
#define LD3_GPIO_Port GPIOB
#define STLK_RX_Pin GPIO_PIN_8
#define STLK_RX_GPIO_Port GPIOD
#define STLK_TX_Pin GPIO_PIN_9
#define STLK_TX_GPIO_Port GPIOD
#define USB_PowerSwitchOn_Pin GPIO_PIN_6
#define USB_PowerSwitchOn_GPIO_Port GPIOG
#define USB_OverCurrent_Pin GPIO_PIN_7
#define USB_OverCurrent_GPIO_Port GPIOG
#define USB_SOF_Pin GPIO_PIN_8
#define USB_SOF_GPIO_Port GPIOA
#define USB_VBUS_Pin GPIO_PIN_9
#define USB_VBUS_GPIO_Port GPIOA
#define USB_ID_Pin GPIO_PIN_10
#define USB_ID_GPIO_Port GPIOA
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define RMII_TX_EN_Pin GPIO_PIN_11
#define RMII_TX_EN_GPIO_Port GPIOG
#define RMII_TXD0_Pin GPIO_PIN_13
#define RMII_TXD0_GPIO_Port GPIOG
#define LD2_Pin GPIO_PIN_7
#define LD2_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
