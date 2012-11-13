/*
 * onewire.h
 *
 *  Created on: Oct 18, 2012
 *      Author: Csabi
 */

#ifndef ONEWIRE_H_
#define ONEWIRE_H_

#include "mcu.h"
#define OW_SEARCH_ROM_COMMAND	0xF0
#define OW_READ_ROM_COMMAND		0x33
#define OW_MATCH_ROM_COMMAND	0x55
#define OW_SKIP_ROM_COMMAND		0xCC
#define OW_ALARM_SEARCH_COMMAND	0xEC

typedef struct
{
	GPIO_TypeDef* PORT;
	uint16_t GPIO_PinSource;
}OW_InitTypeDef;

void OW_Config(OW_InitTypeDef OWx);

unsigned char OW_reset(void);

void OW_write_bit(unsigned char bit);
unsigned char OW_read_bit(void);

void OW_write_byte(unsigned char byte);
unsigned char OW_read_byte();

#endif /* ONEWIRE_H_ */
