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
**  Distribution: The file is distributed “as is,” without any warranty
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
#include "ds2405.h"


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
int selected_thermo = 0;
int selected_switch = 0;

OW_address OW_device[10];
int  OW_devices_count = 0;

void OW_Init(){
	OW_InitTypeDef OW_InitStructure;
	OW_InitStructure.PORT = GPIOA;
	OW_InitStructure.Pin = 5;
	OW_Config(OW_InitStructure);
}

void rescan(void){
	OW_devices_count=0;
	OW_reset_search();
	while(OW_search(OW_device[OW_devices_count].bytes)){
		PutsUART2("Found address ");
		PutsUART2(PrintAddress(OW_device[OW_devices_count].ull));
		PutsUART2(" Type: ");
		switch(OW_device[OW_devices_count].info.FamilyCode){
			case DS18B20_FAMILY_CODE:
				PutsUART2("DS18B20 thermometer\r\n");
				break;
			case DS2405_FAMILY_CODE:
				PutsUART2("DS2405 switch\r\n");

				static uint8_t poslednybit = 0;
				ds2405_set_bit(OW_device[OW_devices_count].bytes, poslednybit);
				poslednybit = ! poslednybit;

				break;
			default:
				PutsUART2("unknown device\r\n");
				break;
		}
		OW_devices_count++;
	}
}

void print_avail_devices(uint8_t desired_code){
        int i=0;
        int poradie=1;
        char temp[128];

        switch( desired_code ){
        case DS18B20_FAMILY_CODE:
                strcpy( temp, "List of Thermometers:\r\n");
                break;
        case DS2405_FAMILY_CODE:
            	strcpy( temp, "List of Switches:\r\n");
                break;
        default:
        		strcpy( temp, "List of Unknown devices:\r\n");
                break;
        }
        PutsUART2(temp);

        for( i=0; i<OW_devices_count ; i++ ){
                if( OW_device[i].info.FamilyCode == desired_code ){
                        temp[0] = '0'+poradie++;
                        temp[1] = '.';
                        temp[2] = ' ';
                        strcpy( temp+3, PrintAddress( OW_device[i].ull ) );
                        strcpy( temp+strlen(temp), "\r\n\0" );
                        PutsUART2(temp);
                }
        }
}


int avail_devices_count(uint8_t desired_code){
        int i=0;
        int pocet=0;

        for( i=0; i<OW_devices_count ; i++ ){
                if( OW_device[i].info.FamilyCode == desired_code ){
                        pocet++;
                }
        }
        return pocet;
}


OW_address poradie2address(uint8_t desired_code, int desired_poradie){
        int i=0;
        int poradie=1;
        static OW_address null_address;
        null_address.ull=0;

        for( i=0; i<OW_devices_count ; i++ ){
                if( OW_device[i].info.FamilyCode == desired_code ){
                	if( desired_poradie == poradie )
                		return OW_device[i];
                	poradie++;
                }
        }
        return null_address;
}


int main(void)
{

	OW_address selected_thermo_address;
	OW_address selected_switch_address;

	initUSART2();	//configures all necessary to use USART1

	RegisterCallbackUART2(&Communicate);	//register function to be called when interrupt occurs
	PutsUART2("Termostat starting...\r\n");			//write something to usart to see some effect

	OW_Init();

	rescan();

	print_avail_devices(DS18B20_FAMILY_CODE);
	print_avail_devices(DS2405_FAMILY_CODE);


    while(1)
    {
			selected_thermo_address = poradie2address( DS18B20_FAMILY_CODE, selected_thermo );
			selected_switch_address = poradie2address( DS2405_FAMILY_CODE, selected_switch );

    		ds18b20_convert_t();
    		delay_us(750000*2);
    		if( selected_thermo_address.ull != 0 )
    			actual_temp = convert_temp(ds18b20_read_temp_ROM( selected_thermo_address.bytes ));


    		if( actual_temp > desired_temp+hysteresis/2.0 ){
    		    ds2405_set_bit( selected_switch_address.bytes, 0 );
			}if( actual_temp < desired_temp-hysteresis/2.0 ){
				ds2405_set_bit( selected_switch_address.bytes, 1 );
			}

//    		TestTiming();

    }

	return 0;
}



