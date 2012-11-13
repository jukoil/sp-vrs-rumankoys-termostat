/*
* Communication.c
*
* Created: 1. 4. 2012 15:36:56
*  Author: Csabi
*/
#include "Communication.h"

//#define NULL ((void *)0x00)




/*
int8_t findChar(char* str, char c){
	uint8_t index = 0;
	while(*str != '\0'){
		if(*str == c){
			return index;
		}
		index++;
		str++;
	}
	return -1;
}

*/
//TODO treba vyskusat
uint8_t StartsWith(volatile char* source, volatile char* search,volatile int len){
	while(*source == *search){
		if(!--len){
			return 1;
		}
		if(*search == '\0'){
			return 1;
		}
		source++;
		search++;
	}
	return 0;
}

void printInvalidParameter(void){
	PutsUART2("Invalid parameter\r\n");
}

#define myprintf( a1, a2 ) //sprintf(outbuffer,a1,a2); PutsUART2(outbuffer);

void Communicate(unsigned char znak)
{
	static char buffer[32];
	//static char outbuffer[64];
	static int i;
	float pomf;
	buffer[i] = (char) znak;
	PutcUART2(znak);//echo
	/*if(buffer[i]=='\n')
	{
		i = 0; // LF ignoruje, prikaz berie len z CR
	}
	else*/ if(buffer[i]=='\r')
	{
		PutcUART2('\n');
		buffer[i] = '\0';
		char* space;
		space = strchr(buffer,'=');

		if(StartsWith(buffer,"man",3))
		{
			static int ManualnyRezim;
			if(space != NULL){//set
				if(*(space+1) == '1'){
					ManualnyRezim = 1;
				}else if(*(space+1) == '0')	{
					ManualnyRezim = 0;
				}else{
					printInvalidParameter();
				}
			}
			PutsUART2("Manual ");
			if(ManualnyRezim){
				PutsUART2("ON\r\n");
			}else{
				PutsUART2("OFF\r\n");
			}
		}else
		if(StartsWith(buffer,"set temp",8)){
			if(space != NULL){//set
				pomf = atof(space+1);
				if(pomf == pomf){
					desired_temp = pomf;
				}else{
					printInvalidParameter();
				}
			}
			myprintf("set temp=%f\r\n", desired_temp);
		}else
		if(StartsWith(buffer,"hyst",4)){
			if(space != NULL){//set
				pomf = atof(space+1);
				if(pomf == pomf){
					hysteresis = pomf;
				}else{
					printInvalidParameter();
				}
			}
			myprintf("hyst=%f\r\n", hysteresis);
		}else
		if(StartsWith(buffer,"def",3)){
			//SetDefaults();
			PutsUART2("Defaults loaded.\r\n");
		}else
		if(StartsWith(buffer,"help",4)){
			PutsUART2("Help text\r\n");
		}else
		if(StartsWith(buffer,"ver",3)){
			PutsUART2("V0.0\r\n");
		}
		else{
			PutsUART2("Unknown command\r\n");
		}
		i = 0;
	}else{//nie je koniec
		if(i<31) i++;//buffer overflow
	}
}
/*
static inline void usart_rs232_getchar(volatile avr32_usart_t *usart, uint8_t *data)
{
	*data = usart_getchar(usart);
}*/

/*
void initUSART(volatile long PBA_Hz){
	// Assign GPIO pins to USART0.
	gpio_enable_module(USART_GPIO_MAP,
		sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));
		
	// Initialize USART in RS232 mode at 16MHz.
	usart_init_rs232(SERVICE_USART, &USART_OPTIONS, PBA_Hz);
	
	stdio_base = (void *)SERVICE_USART;
	ptr_put = (int (*)(void volatile*,int))&usart_putchar;
	ptr_get = (void (*)(void volatile*,int*))&usart_rs232_getchar;
	
	//stdio_serial_init(USART_SERIAL_EXAMPLE, &USART_OPTIONS);
}
*/
