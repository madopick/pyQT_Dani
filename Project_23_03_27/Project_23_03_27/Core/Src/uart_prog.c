#include "main.h"
#include "serial.h"
#include <string.h>
#include "stdin.h"
#include "uart_prog.h"
#include "i2c_prog.h"

/* Private function declarations */
long tinysh_dec(char *s);

/* Public variable declarations */
extern uint16_t bitFlag;
extern int32_t aTxBuffer[];
extern int32_t aRxBuffer[];
extern int32_t temp[];
extern UART_HandleTypeDef huart2;

static char str_cfg_header[CFG_HEADER_NUM][CFG_HEADER_LEN] =
{
	"{Tes}",
	"{RBA}",
	"{WB1:",	"{WB2:",	"{WB3:",
	"{RB1}",	"{RB2}",	"{RB3}",
	"{WBA:",
};

/* Public functions definitions */
/********************************************************
 * 	Parsing incoming message						   	*
 ********************************************************/
void vShell_cmdParse(char *input) {
	for(uint8_t u8_idx = 0; u8_idx < CFG_HEADER_NUM; u8_idx++) {
		if(!memcmp(input,(char*)&str_cfg_header[u8_idx][0], CFG_HEADER_LEN)) {
			if (u8_idx==1) {					//{RBA}
				bitFlag |= BFLAG_I2C_RBA;
			}

			else if (u8_idx>=2 && u8_idx<=2) {	//{WB1: {WB2: {WB3:
				bitFlag |= BFLAG_I2C_WBS;

				uint32_t num = 0;
				uint8_t start = 5;

			    for (uint8_t j = 0; j<CFG_LENGTH; j++)
			    {
			    	if(input[start+3]<128)
			    	{
			    		num = input[start]+(input[start+1]*256)+(input[start+2]*65536)+(input[start+3]*16777216);
			    	}
			    	else
			    	{
			    		num = (input[start]+(input[start+1]*256)+(input[start+2]*65536)+(input[start+3]*16777216))-4294967296;
			    	}
			    	aTxBuffer[j+1] = num;
			    	start+=4;
			    }
			    if(u8_idx==2){aTxBuffer[0] = 1;}
			    else if(u8_idx==3){aTxBuffer[0] = 2;}
			    else if(u8_idx==4){aTxBuffer[0] = 3;}
			}

			else if (u8_idx==5) {					//{RB1}
				bitFlag |= BFLAG_I2C_WBS;
				aTxBuffer[0] = 4;
			}

			else if (u8_idx==6) {					//{RB2}
				bitFlag |= BFLAG_I2C_WBS;
				aTxBuffer[0] = 5;
			}

			else if (u8_idx==7) {					//{RB3}
				bitFlag |= BFLAG_I2C_WBS;
				aTxBuffer[0] = 6;
			}

			else if (u8_idx==8) {					//{WBA:
				bitFlag |= BFLAG_I2C_WBA;

				uint32_t num = 0;
				uint8_t start = 5;

				for (uint8_t j = 0; j<CFG_LENGTH*3; j++)
				{
					if(input[start+3]<128)
					{
						num = input[start]+(input[start+1]*256)+(input[start+2]*65536)+(input[start+3]*16777216);
					}
					else
					{
						num = (input[start]+(input[start+1]*256)+(input[start+2]*65536)+(input[start+3]*16777216))-4294967296;
					}
					temp[j] = num;
					start+=4;
				}
				Set_aTxBuffer(7, temp, 0, CFG_LENGTH*sizeof(temp[0]));
			}
		}
	}
}

/* Private function definitions */
