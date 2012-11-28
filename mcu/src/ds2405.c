/*
 * ds2405.c
 *
 *  Created on: Nov 19, 2012
 *      Author: Csabi
 */
#include "ds2405.h"

void ds2405_toggle_bit(uint8_t rom[8]){
	if(OW_reset()==0) return;
	OW_Match_ROM(rom);
	OW_write_bit(1);
}

void ds2405_set_bit(uint8_t rom[8], uint8_t bit){
	static uint8_t lastState = 255;
	heating = bit;
	if( lastState == bit ) return;
	if(OW_reset()==0) return;
	OW_Match_ROM(rom);
	lastState = !OW_read_bit();// uz aj pri citani sa prepne vystup... dost blbe, teraz to blika
	//if((stav!=0)==(bit!=0)){
	if( lastState != bit ){
		ds2405_toggle_bit(rom);
	}
	lastState = bit;
}



uint8_t ds2405_read_bit(uint8_t rom[8]){
	if(OW_reset()==0) return 0;
	OW_Match_ROM(rom);
	return OW_read_bit();
}
