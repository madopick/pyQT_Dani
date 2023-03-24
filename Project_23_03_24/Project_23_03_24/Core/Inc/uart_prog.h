/*
 * uart_prog.h
 *
 *  Created on: Mar 23, 2023
 *      Author: Yohanes Vianney Dani
 */

#ifndef SRC_UART_PROG_H_
#define SRC_UART_PROG_H_

#include "main.h"
#include <stdint.h>
#include <serial.h>

/*******************************************************************************
	Extern Function Declaration
  *****************************************************************************/
void vShell_cmdParse(char *input);

#endif /* SRC_UART_PROG_H_ */
