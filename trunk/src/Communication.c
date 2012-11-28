/*
* Communication.c
*
* Created: 1. 4. 2012 15:36:56
*  Author: Csabi
*/
#include "Communication.h"

#define HELP_STRING "temp		Actual temperature(Read only)\r\n" \
					"heating		Heating state(Read only)\r\n" \
					"set temp	Desired temperature\r\n" \
					"hyst		Termostat hysteresis\r\n" \
					"thermometer	Number of selected thermometer from list\r\n" \
					"switch		Number of selected switch from list\r\n" \
					"list devices	Prints all devices at one wire bus\r\n" \
					"list switches	Prints all switches at one wire bus\r\n" \
					"list thermo	Prints all thermometers at one wire bus\r\n" \
					"display		Periodically refreshed termostat display\r\n" \
					"variables	Periodically prints termostat variables \r\n" \
					"defaults	Default settings(Not implemented yet)\r\n" \
					"help		Displays this stuff\r\n" \
					"rescan		Starts searching for 1-wire devices\r\n"\
					"ver		Version info\r\n"


#define VERSION_STRING "Thermostat V1.0\r\n" \
					   "Authors:\r\n"\
					   "Csaba Ruman\r\n"\
					   "Juraj Koys\r\n"


uint8_t StartsWith(volatile char* source, char* search){
	int len = strlen(search);
	//TODO case insensitive
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

char* PrintAddress(uint64_t data){
	static char buffer[17];
	char* ptr = buffer;
	int i = 14;
	while(i>=0){
		uint8_t nibble = (data >> (i*4)) & 0x0f;
		if(nibble<10){
			*ptr = nibble+'0';
		}else{
			*ptr = nibble-10+'A';
		}
		ptr++;
		i--;
	}
	buffer[16] = '\0';
	//myprintf_(buffer);
	return buffer;
}

/*!
* \brief print_integer function : convert the given number to an ASCII decimal representation.
*/
char *print_integer(signed int n)
{
	static char str[64];
	int j,i = 10;
	unsigned char negative = 0;

	for( j=0; j<10; j++)
		str[j]=0;

	if(n<0){
		negative = 1;
		n = -1*n;
	}

	str[i] = '\0';
	do
	{
		str[--i] = '0' + n%10;
		n /= 10;
	}while(n);

	if(negative){
		str[--i] = '-';
	}

	return &str[i];
}


char* print_float( float value, int precision){
	static char buffer[64];
	if( value != value ){
		strcpy(buffer,"NaN\0");
		return buffer;
	}

	char *pom;
	int celaCast = (int)value;
	int desatinnaCast = (int)((value-celaCast) * pow(10, precision));
	if( desatinnaCast < 0 ) desatinnaCast *= -1;
	pom=print_integer(celaCast);
	strcpy(buffer,pom);
	buffer[strlen(buffer)+1]=0;
	buffer[strlen(buffer)]='.';
	if( value < 0 && celaCast == 0 )
	    strcpy(buffer,"-0.");
	pom=print_integer(desatinnaCast);
	strcpy(buffer+strlen(buffer),pom);
	buffer[strlen(buffer)]=0;
	return buffer;
}

void printInvalidParameter(void){
	myprintf_("Invalid parameter\r\n");
}

#undef myprintf

void myprintf( char *format, void *number ){
	static char buffer[256];
	buffer[0]=0;
	char *percento = strchr( format, '%');
	if( percento ){
		char znak=(char)toupper((int)percento[1]);
		strncpy( buffer, format, percento-format );
		buffer[percento-format]=0;
		if( znak == 'F' )
			strcpy( buffer+strlen(buffer), print_float(*((float*) number), 2));
		else if( znak == 'I' || znak == 'D' || znak == 'U' )
			strcpy( buffer+strlen(buffer), print_integer(*((int*) number)));
		else if( znak == 'X' )
			strcpy( buffer+strlen(buffer), PrintAddress(*((uint64_t*) number)));
		else if( znak == 'S' )
			strcpy( buffer+strlen(buffer), ((char*) number));
		else{
			PutsUART2(format);
		    return;
        }

		strcpy( buffer+strlen(buffer), percento+2 );
		PutsUART2(buffer);
	}else{
		PutsUART2(format);
	}
}


void myprintf_( char *format ){
	int null=0;
	myprintf( format, (void *)&null );
}
#define myprintf(arg1,arg2) myprintf(arg1,(void *)(& (arg2)))


void Communicate(unsigned char znak)
{
	static char buffer[32];
	static int i;
	float pomf;
	int pomi;

	if( var_print_display ){
		var_print_display = 0;
		myprintf_("\r\nDisplay canceled\r\n");
	}
	if( var_print_variables ){
		var_print_variables = 0;
		myprintf_("\r\nVariables printing canceled\r\n");
	}

	buffer[i] = (char) znak;
	PutcUART2(znak);//echo
	if(buffer[i]=='\b')
	{
		if( i>0 ){
			buffer[i-1]=0;
			i--;
			myprintf_(" \b");	// kvoli putty
			return;
		}
	}
	else if(buffer[i]=='\r')
	{
		PutcUART2('\n');
		buffer[i] = '\0';
		char* space;
		space = strchr(buffer,'=');

		if(StartsWith(buffer,"set temp")){
			if(space != NULL){//set
				pomf = atof(space+1);
				if(pomf == pomf){
					desired_temp = pomf;
				}else{
					printInvalidParameter();
				}
			}
			myprintf("set temp=%f\r\n",desired_temp);
		}else
		if(StartsWith(buffer,"hyst")){
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
		if(StartsWith(buffer,"temp")){
			if(space != NULL){//set
				myprintf_("Temp is read only\r\n");
			}
			myprintf("temp=%f\r\n", actual_temp);
		}else
		if(StartsWith(buffer,"heating")){
			if(space != NULL){//set
				myprintf_("heating is read only\r\n");
			}
			if( heating )
				myprintf_("heating=on\r\n");
			else
				myprintf_("heating=off\r\n");
		}else
		if(StartsWith(buffer,"thermometer")){
			if(space != NULL){//set
				pomi = atoi(space+1);
				if(pomi<0 || pomi>=avail_devices_count(DS18B20_FAMILY_CODE) ){
					printInvalidParameter();
					print_avail_devices(DS18B20_FAMILY_CODE);
				}else{
					OW_address selected_switch_address = poradie2address( DS2405_FAMILY_CODE, selected_switch );
					ds2405_set_bit( selected_switch_address.bytes, 0 );
					selected_thermo = pomi;
				}
			}
			myprintf("thermometer=%i\r\n", selected_thermo);
		}else
		if(StartsWith(buffer,"switch")){
			if(space != NULL){//set
				pomi = atoi(space+1);
				if(pomi<0 || pomi>=avail_devices_count(DS2405_FAMILY_CODE) ){
					printInvalidParameter();
					print_avail_devices(DS2405_FAMILY_CODE);
				}else{
					selected_switch = pomi;
				}
			}
			myprintf("switch=%i\r\n", selected_switch);
		}else
		if(StartsWith(buffer,"rescan")){
			rescan();

			print_avail_devices(DS18B20_FAMILY_CODE);
			print_avail_devices(DS2405_FAMILY_CODE);
		}else
		if(StartsWith(buffer,"display")){
			var_print_display = 1;
		}else
		if(StartsWith(buffer,"variables")){
			var_print_variables = 1;
		}else
		if(StartsWith(buffer,"list devices")){
			print_all_avail_devices();
		}else
		if(StartsWith(buffer,"list switches")){
			print_avail_devices(DS2405_FAMILY_CODE);
		}else
		if(StartsWith(buffer,"list thermo")){
			print_avail_devices(DS18B20_FAMILY_CODE);
		}else
		if(StartsWith(buffer,"defaults")){
			SetDefaults();
			myprintf_("Defaults loaded.\r\n");
		}else
		if(StartsWith(buffer,"help")){
			myprintf_(HELP_STRING);
		}else
		if(StartsWith(buffer,"ver")){
			myprintf_(VERSION_STRING);
		}
		else{
			myprintf("Unknown command \"%s\"\r\n",buffer);
		}
		i = 0;
	}else{//nie je koniec
		i++;
		if(i>=31){
			i=0;
			myprintf_("Command buffer overflow\r\n");
		}
	}
}
