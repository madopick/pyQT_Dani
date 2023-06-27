/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_custom_hid_if.c
  * @version        : v1.0_Cube
  * @brief          : USB Device Custom HID interface file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_custom_hid_if.h"
#include "main.h"

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint8_t usbBuffer[SIZE_BYTE_HEX];


/** Usb custom HID report descriptor. */
__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
{
    /* USER CODE BEGIN 0 */
    0x06, 0x00, 0xff,              //   Usage Page(Undefined )
    0x09, 0x01,                    //   USAGE (Undefined)
    0xa1, 0x01,                    //   COLLECTION (Application)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x40,                    //   REPORT_COUNT (64)
    0x09, 0x01,                    //   USAGE (Undefined)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x40,                    //   REPORT_COUNT (64)
    0x09, 0x01,                    //   USAGE (Undefined)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x09, 0x01,                    //   USAGE (Undefined)
    0xb1, 0x02,                    //   FEATURE (Data,Var,Abs)

    /* USER CODE END 0 */
    0xC0    /*     END_COLLECTION                */
};


extern USBD_HandleTypeDef hUsbDeviceFS;
extern uint16_t bitFlag;
extern uint8_t u8_usbBuffer[SIZE_BYTE];



/** @defgroup USBD_CUSTOM_HID_Private_FunctionPrototypes USBD_CUSTOM_HID_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CUSTOM_HID_Init_FS(void);
static int8_t CUSTOM_HID_DeInit_FS(void);
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t* state);

/**
  * @}
  */

USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS =
{
  CUSTOM_HID_ReportDesc_FS,
  CUSTOM_HID_Init_FS,
  CUSTOM_HID_DeInit_FS,
  CUSTOM_HID_OutEvent_FS
};


static char usbData[SIZE_BYTE] = {0};				// Size of buffer
static uint8_t usbSendReq = 0;

void sendbytesViausb(uint8_t* data)
{
	memcpy(usbData, data, SIZE_BYTE);			// Copy 64 Datas

	usbSendReq = 1;
}


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_Init_FS(void)
{
  return (USBD_OK);
}

/**
  * @brief  DeInitializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_DeInit_FS(void)
{
  return (USBD_OK);
}

/**
  * @brief  Manage the CUSTOM HID class events
  * @param  event_idx: Event index
  * @param  state: Event state
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t* state)
{
	printf("USB In: %s", state);
	for (uint16_t idx = 0; idx < 2; idx++){
		printf("%d - ", state[idx]);
	}
	printf("\r\n\n");

	if ((state[0]==0)&&(state[1]==0))
	{
		bitFlag |= BFLAG_I2C_WBA;
	}
	if (((state[0]==0)&&(state[1]==1)) ||		// {WB1}
		((state[0]==0)&&(state[1]==2)) ||		// {WB2}
		((state[0]==0)&&(state[1]==3)))			// {WB3}
	{
		bitFlag |= BFLAG_I2C_WBS;
	}

	if ((state[0]==1)&&(state[1]==0))
	{
		bitFlag |= BFLAG_I2C_RBA;
	}
	if (((state[0]==1)&&(state[1]==1)) ||		// {RB1}
		((state[0]==1)&&(state[1]==2)) ||		// {RB2}
		((state[0]==1)&&(state[1]==3)))			// {RB3}
	{
		bitFlag |= BFLAG_I2C_RBS;
	}

	if ((state[0]==2)&&(state[1]==0))
	{
		u8_usbBuffer[0] = 2;
		u8_usbBuffer[1] = 0;

		for (uint8_t idx = 2; idx < SIZE_BYTE; idx++)
		{
			u8_usbBuffer[idx] = rand()%16;
		}

		sendbytesViausb(u8_usbBuffer);
	}
	if ((state[0]==2)&&(state[1]==1))
	{
		u8_usbBuffer[0] = 2;
		u8_usbBuffer[1] = 0;

		for (uint8_t idx = 2; idx < SIZE_BYTE; idx++)
		{
			u8_usbBuffer[idx] += 1;
			if (u8_usbBuffer[idx] > 16) {
				u8_usbBuffer[idx] -= 16;
			}
		}

		sendbytesViausb(u8_usbBuffer);
	}

	// Flag setting for run table

	if ((state[0]==3)&&(state[1]==1))
	{
		bitFlag |= BFLAG_TABLE1;
	}
	if ((state[0]==3)&&(state[1]==2))
	{
		bitFlag |= BFLAG_TABLE2;
	}
	if ((state[0]==3)&&(state[1]==3))
	{
		bitFlag |= BFLAG_TABLE3;
	}
	if ((state[0]==3)&&(state[1]==4))
	{
		bitFlag |= BFLAG_TABLE4;
	}
	if ((state[0]==3)&&(state[1]==5))
	{
		bitFlag |= BFLAG_TABLE5;
	}
	if ((state[0]==3)&&(state[1]==6))
	{
		bitFlag |= BFLAG_TABLE6;
	}
	if ((state[0]==3)&&(state[1]==7))
	{
		bitFlag |= BFLAG_TABLE7;
	}
	if ((state[0]==3)&&(state[1]==8))
	{
		bitFlag |= BFLAG_TABLE8;
	}
	if ((state[0]==3)&&(state[1]==9))
	{
		bitFlag |= BFLAG_TABLE9;
	}
	if ((state[0]==3)&&(state[1]==10))
	{
		bitFlag |= BFLAG_TABLE10;
	}

	if ((state[0]==3)&&(state[1]==10))
	{
		bitFlag |= BFLAG_TABLE10;
	}

	if ((state[0]==4)&&(state[1]==0)){
		bitFlag |= BFLAG_RND;
	}
	if ((state[0]==4)&&(state[1]==1)){
		for (uint8_t idx = 0; idx < SIZE_BYTE; idx++)
		{
			u8_usbBuffer[idx] = rand()%256;
		}
		u8_usbBuffer[0] = 4;
		u8_usbBuffer[1] = 1;

		sendbytesViausb(u8_usbBuffer);
	}
	if ((state[0]==4)&&(state[1]==2)){
		bitFlag |= BFLAG_RND;
	}
	if ((state[0]==4)&&(state[1]==3)){
		bitFlag &= ~BFLAG_RND;
	}


	memset(usbBuffer,0,SIZE_BYTE_HEX);       //Set send buffer out length to 64 bytes.

	memcpy(&usbBuffer[0],usbData,SIZE_BYTE);
	usbSendReq = 0;

	USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,(uint8_t*)usbBuffer,SIZE_BYTE_HEX);	//Bagian ini yang mengirim kan data ke USB

	return (USBD_OK);
}
