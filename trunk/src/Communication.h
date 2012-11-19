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
#include "usart.h"
#include "main.h"

void Communicate(unsigned char data);
void PrintAddress(uint64_t data);

#endif /* COMMUNICATION_H_ */
