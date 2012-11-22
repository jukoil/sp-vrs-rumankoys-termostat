/*
 * ds2405.h
 *
 *  Created on: Nov 19, 2012
 *      Author: Csabi
 */

#ifndef DS2405_H_
#define DS2405_H_

#include "onewire.h"
#include "termostat.h"


#define DS2405_FAMILY_CODE	0x05

void ds2405_set_bit(uint8_t rom[8], uint8_t bit);

uint8_t ds2405_read_bit(uint8_t rom[8]);

#endif /* DS2405_H_ */
