/*
 * termostat.c
 *
 *  Created on: Nov 21, 2012
 *      Author: juko
 */

#include "termostat.h"

int heating = 0;
float actual_temp = 20.0f;
float desired_temp = 20.0f;
float hysteresis = 2.0f;
int selected_thermo = 0;
int selected_switch = 0;

int var_print_variables = 0;
int var_print_display = 0;

OW_address OW_device[10];
int  OW_devices_count = 0;

void SetDefaults(void){
	heating = 0;
	desired_temp = 20.0f;
	hysteresis = 2.0f;
	selected_thermo = 0;
	selected_switch = 0;
}

void print_variables(void){
	myprintf(";heating=%i;",heating);
	myprintf("actual_temp=%f;",actual_temp);
	myprintf("desired_temp=%f;",desired_temp);
	myprintf("hysteresis=%f;\r\n",hysteresis);
}

#define CLS     "\033[2J"
#define clear_screen() myprintf_( CLS )

void print_display(void){

	clear_screen();

	myprintf("Actual  temperature [Degrees Celsius] %f\r\n",actual_temp);
	myprintf("Desired temperature [Degrees Celsius] %f\r\n",desired_temp);
	myprintf("Hysteresis          [Degrees Celsius] %f\r\n\r\n",hysteresis);

	if( heating )
		myprintf_("heating ON\r\n");
	else
		myprintf_("heating OFF\r\n");

}
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
		myprintf("Found address %x Type: ",OW_device[OW_devices_count].ull);
		switch(OW_device[OW_devices_count].info.FamilyCode){
			case DS18B20_FAMILY_CODE:
				myprintf_("DS18B20 thermometer\r\n");
				break;
			case DS2405_FAMILY_CODE:
				myprintf_("DS2405 switch\r\n");

				static uint8_t poslednybit = 0;
				ds2405_set_bit(OW_device[OW_devices_count].bytes, poslednybit);
				poslednybit = ! poslednybit;

				break;
			default:
				myprintf_("unknown device\r\n");
				break;
		}
		OW_devices_count++;
	}
}

void print_avail_devices(uint8_t desired_code){
        int i=0;
        int poradie=0;
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
        myprintf_(temp);

        for( i=0; i<OW_devices_count ; i++ ){
			if( OW_device[i].info.FamilyCode == desired_code ){
				myprintf(" %i.",poradie);
				poradie++;
				myprintf(" %x \r\n",OW_device[i].ull);
			}
        }
        if( poradie == 0 ){
        	myprintf_("No devices\r\n");
        }
}

void print_all_avail_devices(void){
        int i=0;
        int poradie=1;

        for( i=0; i<OW_devices_count ; i++ ){

			myprintf(" %i.",poradie);
			poradie++;
			myprintf(" %x \r\n",OW_device[i].ull);

            switch( OW_device[i].info.FamilyCode ){
				case DS18B20_FAMILY_CODE:
						myprintf_("thermometer\r\n");
						break;
				case DS2405_FAMILY_CODE:
						myprintf_("switch\r\n");
						break;
				default:
						myprintf_("Unknown device\r\n");
						break;
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
        int poradie=0;
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
