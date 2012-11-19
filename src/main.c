/**
*****************************************************************************
**
**  File        : main.c
**
**  Abstract    : main function.
**
**  Functions   : main
**
**  Environment : Atollic TrueSTUDIO/STM32
**                STMicroelectronics STM32Lxx Standard Peripherals Library
**
**  Distribution: The file is distributed �as is,� without any warranty
**                of any kind.
**
**  (c)Copyright Atollic AB.
**  You may use this file as-is or modify it according to the needs of your
**  project. Distribution of this file (unmodified or modified) is not
**  permitted. Atollic AB permit registered Atollic TrueSTUDIO(R) users the
**  rights to distribute the assembled, compiled & linked contents of this
**  file as part of an application binary file, provided that it is built
**  using the Atollic TrueSTUDIO(R) toolchain.
**
**
*****************************************************************************
*/
/* Includes */
#include <stddef.h>
#include <stdio.h>
#include "mcu.h"
#include "usart.h"
#include "Communication.h"
#include "onewire.h"
#include "ds18b20.h"



/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/

int tick = 0;

char heating = 0;
float actual_temp = 20.0f;
float desired_temp = 20.0f;
float hysteresis = 2.0f;

void OW_Init(){
	OW_InitTypeDef OW_InitStructure;
	OW_InitStructure.PORT = GPIOA;
	OW_InitStructure.Pin = 5;
	OW_Config(OW_InitStructure);
}

int main(void)
{
	initUSART2();	//configures all necessary to use USART1

	RegisterCallbackUART2(&Communicate);	//register function to be called when interrupt occurs
	PutsUART2("Termostat starting...\r\n");			//write something to usart to see some effect

	OW_Init();

    while(1)
    {
    		ds18b20_convert_t();
    		delay_us(750000*2);
    		actual_temp = convert_temp(ds18b20_read_temp());

//    		TestTiming();
    		OW_address add;
    		while(OW_search(add.bytes)){
    			PutsUART2("Found address ");
    			PrintAddress(add.ull);
    			PutsUART2("\r\n Type: ");
    			switch(add.info.FamilyCode){
					case DS18B20_FAMILY_CODE:
						PutsUART2("DS18B20 thermometer\r\n");
						break;
					default:
						PutsUART2("unknown\r\n");
						break;
    			}
    		}
    		OW_reset_search();
    }

	return 0;
}



