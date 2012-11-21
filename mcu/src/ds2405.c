/*
 * ds2405.c
 *
 *  Created on: Nov 19, 2012
 *      Author: Csabi
 */
#include "ds2405.h"

void ds2405_set_bit(uint8_t rom[8], uint8_t bit){
	if(OW_reset()==0) return;
	OW_Match_ROM(rom);
	uint8_t stav = OW_read_bit();
	if((stav==0)!=(bit!=0)){
		OW_write_bit(1);
	}
}

uint8_t ds2405_read_bit(uint8_t rom[8]){
	if(OW_reset()==0) return 0;
	OW_Match_ROM(rom);
	return OW_read_bit();
}
