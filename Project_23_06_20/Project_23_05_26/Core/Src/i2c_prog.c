#include "main.h"
#include <string.h>
#include "uart_prog.h"
#include "i2c_prog.h"

/* Private function declarations */
extern uint16_t bitFlag;
extern int16_t aTxBuffer[];
extern int16_t aRxBuffer[];
extern int16_t temp[];
extern UART_HandleTypeDef huart2;
extern I2C_HandleTypeDef I2cHandle;


/* Public functions definitions */
/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 0  : pBuffer1 identical to pBuffer2
  *         >0 : pBuffer1 differs from pBuffer2
  */

uint16_t Buffercmp_Byte(int16_t pBuffer1[], int16_t pBuffer2[], uint8_t length){
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

void I2C_Master_Transmit() {
	do
	{
		if(HAL_I2C_Master_Transmit_IT(&I2cHandle, (uint16_t)I2C_ADDRESS, (uint8_t*)aTxBuffer, TXBUFFERSIZE)!= HAL_OK)
		{Error_Handler();}

		// Wait transfer data successful
		while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
		{}
	} while(HAL_I2C_GetError(&I2cHandle) == HAL_I2C_ERROR_AF);
}

void I2C_Master_Receive() {
	do
	{
		if(HAL_I2C_Master_Receive_IT(&I2cHandle, (uint16_t)I2C_ADDRESS, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
		{Error_Handler();}

		/* Saat Acknowledge failure (Slave tidak mengakui alamatnya) Master memulai ulang komunikasi */
	} while (HAL_I2C_GetError(&I2cHandle) == HAL_I2C_ERROR_AF);

	// Wait transfer data successful
	while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
	{}
}

void I2C_Slave_Transmit(){
	if(HAL_I2C_Slave_Transmit_IT(&I2cHandle, (uint8_t*)aTxBuffer, TXBUFFERSIZE)!= HAL_OK)
	{Error_Handler();}

	// Wait transfer data successful
	while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
	{}
}

void I2C_Slave_Receive(){
	if(HAL_I2C_Slave_Receive_IT(&I2cHandle, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
	{Error_Handler();}

	// Wait for the end of the transfer
	while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
	{}
}

void Set_aTxBuffer(int16_t idx0, int16_t target[], uint16_t idx_target, uint16_t size){
	for (uint8_t i=0; i<10; i++){
		aTxBuffer[i+1] = 0;
	}
	memcpy(&aTxBuffer[1], &target[idx_target], size);
	aTxBuffer[0]=idx0;
}
