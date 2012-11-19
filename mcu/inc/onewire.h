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
	uint8_t Pin;
}OW_InitTypeDef;



void OW_Config(OW_InitTypeDef OWx);

unsigned char OW_reset(void);

void OW_write_bit(unsigned char bit);
unsigned char OW_read_bit(void);

void OW_write_byte(unsigned char byte);
unsigned char OW_read_byte();

void TestTiming(void);

//hladanie adries:

typedef union{
	uint64_t ull;
	uint8_t bytes[8];
	struct{
		uint8_t FamilyCode;
		uint8_t Serial[6];
		uint8_t Crc;
	}info;
}OW_address;

void OW_Match_ROM(uint8_t rom[8]);

uint8_t OW_search(uint8_t *newAddr);
void OW_reset_search(void);


#endif /* ONEWIRE_H_ */
