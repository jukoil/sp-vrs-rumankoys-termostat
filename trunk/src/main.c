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
#include "main.h"


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

    		if( selected_thermo_address.ull != 0 ){
    			actual_temp = convert_temp(ds18b20_read_temp_ROM( selected_thermo_address.bytes ));
    		}else
    			actual_temp = -273.15f;

    		if( selected_switch_address.ull != 0 ){
				if( actual_temp > desired_temp+hysteresis/2.0 ){
					ds2405_set_bit( selected_switch_address.bytes, 0 );
				}if( actual_temp < desired_temp-hysteresis/2.0 ){
					ds2405_set_bit( selected_switch_address.bytes, 1 );
				}
    		}


    		if( var_print_variables )
    			print_variables();

    		if( var_print_display )
    			print_display();

//    		TestTiming();

    }

	return 0;
}



