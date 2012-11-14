/*
 * onewire.h
 *
 *  Created on: Oct 18, 2012
 *      Author: Csabi
 */

#include "onewire.h"

OW_InitTypeDef OW_settings;

//treba vyskusat:
#define SETPIN() (OW_settings.PORT->BSRRL = 1<< OW_settings.GPIO_PinSource)
#define CLEARPIN() (OW_settings.PORT->BSRRH = 1<< OW_settings.GPIO_PinSource)
#define PINVALUE() (OW_settings.PORT->IDR & OW_settings.GPIO_PinSource)

//TODO toto je len docasny hack:
int pom;
#define SREG pom

//TODO DIRECTION
#define ONEWIRE_PIN pom
#define ONEWIRE_DDR pom
#define cli()

#define _delay_us(x) delay_us(x)

void OW_Config(OW_InitTypeDef OWx){
	assert_param(IS_GPIO_PIN_SOURCE(OWx.GPIO_PinSource));

	OW_settings = OWx;

	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;			//input at configuration???
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;		//not very important, do not change
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;		//output is open drain type
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//use pull up
	GPIO_InitStructure.GPIO_Pin = OW_settings.GPIO_PinSource;	//choosing the pins
	GPIO_Init(OW_settings.PORT, &GPIO_InitStructure);				//applying settings
}

//--------------------------------------------------------------------------
// Reset the 1-wire interface
unsigned char OW_reset (){
	unsigned char presence=0;
	unsigned char sreg_temp=SREG;
	cli();//disable interrupts
	SETPIN(); //ONEWIRE=1;
	ONEWIRE_DDR=1;//out
	//ONEWIRE=0;_delay_us(500);ONEWIRE=1;//pull low for 500us
	CLEARPIN(); delay_us(500); SETPIN();
	ONEWIRE_DDR=0;//in
	SETPIN();//ONEWIRE=1;
	_delay_us(90);
	presence=ONEWIRE_PIN==0?1:0;
	SREG=sreg_temp;
	_delay_us(480-90);
	return presence;
}

//--------------------------------------------------------------------------
// Write 1 bit to 1-wire interface
void OW_write_bit(unsigned char bit){
	unsigned char sreg_temp=SREG;
	SETPIN();//ONEWIRE=1;
	cli();
 	ONEWIRE_DDR=1;//out
 	CLEARPIN();//ONEWIRE=0;
 	_delay_us(2);
 	if(bit!=0) SETPIN();//ONEWIRE=1;
 	_delay_us(60);
 	SETPIN();//ONEWIRE=1;
 	ONEWIRE_DDR=0;//in
 	SETPIN();//ONEWIRE=1;
	SREG=sreg_temp;
 	_delay_us(1);
}


//--------------------------------------------------------------------------
// Read 1 bit from 1-wire interface
unsigned char OW_read_bit (){
	unsigned char sreg_temp=SREG;
	unsigned char bit;
	SETPIN();//ONEWIRE=1;
	cli();
	ONEWIRE_DDR=1;//out
	CLEARPIN();//ONEWIRE=0;
	_delay_us(1);
	ONEWIRE_DDR=0;//in
	SETPIN();//ONEWIRE=1;
	_delay_us(12);
	bit=ONEWIRE_PIN;
	SREG=sreg_temp;
	_delay_us(60);
	SETPIN();//ONEWIRE=1;
	return bit;
}


//--------------------------------------------------------------------------
// Write 8 bits to 1-wire interface
void OW_write_byte(unsigned char byte){
	unsigned char i;
	for(i=0;i<8;i++){
	 	OW_write_bit(byte & 1);//byte & 0x80
		 byte>>=1;//byte<<=1;
	}
}


//--------------------------------------------------------------------------
// Read 8 bits from 1-wire interface
unsigned char OW_read_byte(){
	unsigned char i,byte=0;
	for(i=0;i<8;i++){
		byte|=(OW_read_bit() << (i));
	}
	return byte;
}








