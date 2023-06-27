/*
 * i2c_prog.h
 *
 *  Created on: Mar 23, 2023
 *      Author: Yohanes Vianney Dani
 */

#ifndef INC_I2C_PROG_H_
#define INC_I2C_PROG_H_

#include "main.h"

/*******************************************************************************
	Extern Function Declaration
  *****************************************************************************/
uint16_t Buffercmp_Byte(int16_t pBuffer1[], int16_t pBuffer2[], uint8_t length);
void I2C_Master_Transmit();
void I2C_Master_Receive();
void I2C_Slave_Transmit();
void I2C_Slave_Receive();
void Set_aTxBuffer(int16_t, int16_t[], uint16_t, uint16_t);

#endif /* INC_I2C_PROG_H_ */
