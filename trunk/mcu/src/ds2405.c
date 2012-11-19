/*
 * ds2405.c
 *
 *  Created on: Nov 19, 2012
 *      Author: Csabi
 */
#include "ds2405.h"

//neviem ci som dobre pochopil ako to funguje
uint8_t ds2405_toggle_bit(){
	if(OW_reset()==0) return 0;
	OW_write_byte(OW_SKIP_ROM_COMMAND); // skip ROM command
	return OW_read_bit();
}
