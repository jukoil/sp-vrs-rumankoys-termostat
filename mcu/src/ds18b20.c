
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

unsigned int ds18b20_read_temp_ROM(uint8_t rom[8]){
	unsigned int scratchpad=0;
	//ds18b20_command(DS18B20_CONVERT_T);
	//_delay_ms(750);
	ds18b20_command_ROM(rom, DS18B20_READ_SCRATCHPAD);
	scratchpad=OW_read_byte();
	scratchpad|=((uint16_t)OW_read_byte())<<8;
	OW_reset();
	return scratchpad;
}


float convert_temp(unsigned int raw){
	return ((float)((signed int)raw))/16.0;
}




