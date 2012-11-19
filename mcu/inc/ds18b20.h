/*
 * ds18b20.h
 *
 *  Created on: Nov 13, 2012
 *      Author: Csabi
 */

#ifndef DS18B20_H_
#define DS18B20_H_

#include "onewire.h"

#define DS18B20_CONVERT_T			0x44
#define DS18B20_FAMILY_CODE			0x28
#define DS18B20_READ_SCRATCHPAD		0xBE
#define DS18B20_WRITE_SCRATCHPAD	0x4E
#define DS18B20_COPY_SCRATCHPAD		0x48
#define DS18B20_RECALL_E2			0xB8
#define DS18B20_READ_POWER_SUPPLY	0xB4

void ds18b20_command(unsigned char command);
unsigned int ds18b20_read_temp(void);
//signed int convert_temp(unsigned int raw);
float convert_temp(unsigned int raw);


#define ds18b20_convert_t() ds18b20_command(DS18B20_CONVERT_T);

#endif /* DS18B20_H_ */
