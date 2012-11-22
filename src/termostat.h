/*
 * termostat.h
 *
 *  Created on: Nov 21, 2012
 *      Author: user
 */

#ifndef TERMOSTAT_H_
#define TERMOSTAT_H_

#include "Communication.h"
#include "onewire.h"
#include "ds18b20.h"
#include "ds2405.h"
#include "usart.h"
#include <string.h>

extern int heating;
extern float actual_temp;
extern float desired_temp;
extern float hysteresis;
extern int selected_thermo;
extern int selected_switch;

extern int var_print_variables;
extern int var_print_display;

void OW_Init();

void rescan(void);

void print_avail_devices(uint8_t desired_code);

void print_all_avail_devices(void);

int avail_devices_count(uint8_t desired_code);

OW_address poradie2address(uint8_t desired_code, int desired_poradie);

void SetDefaults(void);

#endif /* TERMOSTAT_H_ */
