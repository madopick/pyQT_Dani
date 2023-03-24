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
	"{RAA}",
	"{WA1:",		"{WA2:",		"{WA3:",
	"{RD0}",		//5
	"{RA1}",		"{RA2}",		"{RA3}",
	"{WAA:",
	"{RBA}",		//10
	"{WB1:",		"{WB2:",		"{WB3:",
	"{RD0}",		//14
	"{RB1}",		"{RB2}",		"{RB3}",
	"{WBA:",		//18
};

/* Public functions definitions */
/********************************************************
 * 	Parsing incoming message						   	*
 ********************************************************/
void vShell_cmdParse(char *input) {
	for(uint8_t u8_idx = 0; u8_idx < CFG_HEADER_NUM; u8_idx++) {
		if(!memcmp(input,(char*)&str_cfg_header[u8_idx][0], CFG_HEADER_LEN)) {
			char *pChar 		= &input[CFG_HEADER_LEN];		//pointer untuk menyimpan header (5 char)
			char *pChar2 		= &input[CFG_HEADER_LEN];		//pointer untuk menyimpan header (5 char)
			uint8_t u8_start 	= 0;			// ini penanda yang akan bergeser tiap indeks (menandai lokasi dari sparating symboll)
			uint8_t u8_stop 	= 0;			// ini penenda start pointer paling awal atau awal baru setelah sparating symboll
			uint8_t u8_cnt 		= 0;			// menampung ada berapa sih inputan nya

			char str_res[20];

			if (u8_idx==1) {						//{RAA}
				bitFlag |= BFLAG_I2C_RAA;
			}

			else if (u8_idx==10) {					//{RBA}
				bitFlag |= BFLAG_I2C_RBA;
			}

			else if (u8_idx==11) {					//{WB1:
				bitFlag |= BFLAG_I2C_WBS;

				uint32_t temp = 0;
				uint8_t start = 5;

			    for (uint8_t j = 0; j<CFG_LENGTH; j++)
			    {
			    	if(input[start+3]<128)
			    	{
			    		temp = input[start]+(input[start+1]*256)+(input[start+2]*65536)+(input[start+3]*16777216);
			    	}
			    	else
			    	{
			    		temp = (input[start]+(input[start+1]*256)+(input[start+2]*65536)+(input[start+3]*16777216))-4294967296;
			    	}
			    	aTxBuffer[j+1] = temp;
			    	start+=4;
			    }
			    aTxBuffer[0] = 1;
			}

			else if (u8_idx==12) {					//{WB2:
				bitFlag |= BFLAG_I2C_WBS;

				uint32_t temp = 0;
				uint8_t start = 5;

				for (uint8_t j = 0; j<CFG_LENGTH; j++)
				{
					if(input[start+3]<128)
					{
						temp = input[start]+(input[start+1]*256)+(input[start+2]*65536)+(input[start+3]*16777216);
					}
					else
					{
						temp = (input[start]+(input[start+1]*256)+(input[start+2]*65536)+(input[start+3]*16777216))-4294967296;
					}
					aTxBuffer[j+1] = temp;
					start+=4;
				}
				aTxBuffer[0] = 2;
			}

			else if (u8_idx==13) {					//{WB3:
				bitFlag |= BFLAG_I2C_WBS;

				uint32_t temp = 0;
				uint8_t start = 5;

				for (uint8_t j = 0; j<CFG_LENGTH; j++)
				{
					if(input[start+3]<128)
					{
						temp = input[start]+(input[start+1]*256)+(input[start+2]*65536)+(input[start+3]*16777216);
					}
					else
					{
						temp = (input[start]+(input[start+1]*256)+(input[start+2]*65536)+(input[start+3]*16777216))-4294967296;
					}
					aTxBuffer[j+1] = temp;
					start+=4;
				}
				aTxBuffer[0] = 3;
			}

			else if (u8_idx==15) {					//{RB1}
				bitFlag |= BFLAG_I2C_WBS;
				aTxBuffer[0] = 4;
			}

			else if (u8_idx==16) {					//{RB2}
				bitFlag |= BFLAG_I2C_WBS;
				aTxBuffer[0] = 5;
			}

			else if (u8_idx==17) {					//{RB3}
				bitFlag |= BFLAG_I2C_WBS;
				aTxBuffer[0] = 6;
			}

			else if (u8_idx==18) {					//{WBA:
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

			else if (u8_idx == 5){		//{RD0} Read Buffer TX and RX from Master
				bitFlag |= BFLAG_I2C_RDB;
			}

			else if (u8_idx == 6){ 		//{RD1} Read from res1
				aTxBuffer[0]=4;
				bitFlag |= BFLAG_I2C_WAS;
			}
			else if (u8_idx == 7){ 		//{RD2} Read from res2
				aTxBuffer[0]=5;
				bitFlag |= BFLAG_I2C_WAS;
			}
			else if (u8_idx == 8){		//{RD3} Read from res3
				aTxBuffer[0]=6;
				bitFlag |= BFLAG_I2C_WAS;
			}

			else if (u8_idx == 9){		//{WAA: Read all
				for (uint8_t i=0; i<30; i++){
					temp[i] = 0;
				}

				while (*pChar) {
					if(*pChar == ';' || *pChar == '}') {
						memset(&str_res[0], 0, 20);		// default/reset untuk mendeklarasikan array str_res akan bernilai 0 semua sebanyak 10 indeks mulai dari str_res[0]
						memcpy(&str_res[0], &pChar2[u8_stop], u8_start - u8_stop);	// mengcopy data array dari pChar2[u8_stop] sejumlah (u8_start - u8_stop0 char menuju str_res

						if(strlen(str_res)>0){
							temp[u8_cnt] = tinysh_dec(&str_res[0]);
						}

						if(*pChar == ';') { u8_stop = u8_start + 1; }
						else if(*pChar == '}') {
							Set_aTxBuffer(7, temp, 0, CFG_LENGTH*sizeof(temp[0]));
							bitFlag |= BFLAG_I2C_WAA;
						}
						u8_cnt++;
					}
					pChar++;
					u8_start++;
				}
				break;
			}

			else if ((u8_idx > 0) && (u8_idx < 5)) {
				/* Reset TX Buffer from index 1 to 10*/
				for (uint8_t i=0; i<10; i++){
					aTxBuffer[i+1] = 0;
				}

				/* WRITE HEADER */
				while (*pChar) {
					if(*pChar == ';' || *pChar == '}') {
						memset(&str_res[0], 0, 20);		// default/reset untuk mendeklarasikan array str_res akan bernilai 0 semua sebanyak 10 indeks mulai dari str_res[0]
						memcpy(&str_res[0], &pChar2[u8_stop], u8_start - u8_stop);	// mengcopy data array dari pChar2[u8_stop] sejumlah (u8_start - u8_stop0 char menuju str_res

						if(strlen(str_res)>0){
							aTxBuffer[u8_cnt+1] = tinysh_dec(&str_res[0]);
						}

						if(*pChar == ';') { u8_stop = u8_start + 1; }
						else if(*pChar == '}') {
							if (u8_idx == 2){ // {WR1: Write at form 1
								/*Set first index as a trigger for slave*/
								aTxBuffer[0]=1;
							}
							else if (u8_idx == 3){ // {WR2: Write at form 2
								aTxBuffer[0]=2;
							}
							else if (u8_idx == 4){ // {WR3: Write at form 3
								aTxBuffer[0]=3;
							}
							bitFlag |= BFLAG_I2C_WAS;
							bitFlag |= BFLAG_I2C_RDB;
						}
						u8_cnt++;
					}
					pChar++;
					u8_start++;
				}
				break;
			}
		}
	}
}

/* Private function definitions */

/*********************************************************************
 * @name	: tinysh_dec
 * @brief	: string to decimal conversion (up to 15 chars).
 *********************************************************************/
long tinysh_dec(char *s) {
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
