
#include "ds18b20.h"

void ds18b20_command_ROM(uint8_t rom[8], uint8_t command){
	if(OW_reset()==0) return;
    OW_Match_ROM(rom);
	OW_write_byte(command);
}

void ds18b20_command(unsigned char command) {
    if(OW_reset()==0) return;
	OW_write_byte(OW_SKIP_ROM_COMMAND); // skip ROM command
    OW_write_byte(command);
}

unsigned int ds18b20_read_temp(){
	unsigned int scratchpad=0;
	//ds18b20_command(DS18B20_CONVERT_T);
	//_delay_ms(750);
	ds18b20_command(DS18B20_READ_SCRATCHPAD);
	scratchpad=OW_read_byte();
	scratchpad|=((uint16_t)OW_read_byte())<<8;
	OW_reset();
	return scratchpad;
}

/*signed int convert_temp(unsigned int raw){
	unsigned char negative=0;
	signed int result;

	if(raw&0x8000) {raw=0-raw;negative=1;}
	result=(raw/16)*10;
	if(raw & 8) result+=5;//1xxx 0,5
	if(raw&4 && raw&2 && raw&1) result+=4;//x111 0,4375
	else{
		if(raw & 4){ result+=2;//x1xx 0,25
	    	if(raw & 2)result+=1;//x110 0,375
	    	if(raw & 1)result+=1;//x101 0,3125
		}else{
			if(raw & 2) result+=1;//x01x 0,125 OR 0,1875
		}
	}
	if(negative)result*=-1;
	return result;

	return ((double)((signed int)raw))/16.0;
}*/

float convert_temp(unsigned int raw){
	return ((float)((signed int)raw))/16.0;
}




