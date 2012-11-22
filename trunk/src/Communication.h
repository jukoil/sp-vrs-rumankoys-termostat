/*
 * Communication.h
 *
 *  Created on: Nov 7, 2012
 *      Author: student
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "usart.h"
#include "main.h"
#include <math.h>
#include "termostat.h"

void Communicate(unsigned char data);
char* PrintAddress(uint64_t data);

void myprintf( char *format, void *number );

#define myprintf(arg1,arg2) myprintf(arg1,(void *)(& (arg2)))

void myprintf_( char *format );

#endif /* COMMUNICATION_H_ */
