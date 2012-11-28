/*
 * onewire.h
 *
 *  Created on: Oct 18, 2012
 *      Author: Csabi
 */

#include "onewire.h"

OW_InitTypeDef OW_settings;

//treba vyskusat:
#define SETPIN() (OW_settings.PORT->BSRRL = 1<< OW_settings.Pin)
#define CLEARPIN() (OW_settings.PORT->BSRRH = 1<< OW_settings.Pin)
#define PINVALUE() (OW_settings.PORT->IDR & (1<< OW_settings.Pin))


//TODO DIRECTION
#define SETOUT() (OW_settings.PORT->MODER |= 1<< (OW_settings.Pin*2))
#define SETIN() (OW_settings.PORT->MODER &= ~(1<< (OW_settings.Pin*2)))
#define cli() asm volatile("cpsid i");
#define sei() asm volatile("cpsie i");

// s optimalizaciou
#define _delay_us(x) delay_us((long)((x-1.4)*2))
//bez optimalizacie
//#define _delay_us(x) delay_us((long)((x-4.6)/1.376))

void OW_Config(OW_InitTypeDef OWx){
	assert_param(IS_GPIO_PIN_SOURCE(OWx.Pin));

	OW_settings = OWx;

	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//input at configuration???
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;		//not very important, do not change
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//output is open drain type
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//use pull up
	GPIO_InitStructure.GPIO_Pin = 1<<OW_settings.Pin;	//choosing the pins
	GPIO_Init(OW_settings.PORT, &GPIO_InitStructure);				//applying settings
	SETPIN();
}

//--------------------------------------------------------------------------
// Reset the 1-wire interface
unsigned char OW_reset (){
	unsigned char presence=0;
	//unsigned char sreg_temp=SREG;
	cli();//disable interrupts

	SETPIN(); //ONEWIRE=1;
	SETOUT();//out
	//ONEWIRE=0;_delay_us(500);ONEWIRE=1;//pull low for 500us
	CLEARPIN(); _delay_us(500); SETPIN();
	SETIN();//in
	SETPIN();//ONEWIRE=1;
	_delay_us(90);
	presence=PINVALUE()==0?1:0;
	sei();//SREG=sreg_temp;
	_delay_us(480-90);
	return presence;
}

//--------------------------------------------------------------------------
// Write 1 bit to 1-wire interface
void OW_write_bit(unsigned char bit){
	//unsigned char sreg_temp=SREG;
	SETPIN();//ONEWIRE=1;
	cli();
 	SETOUT();//out
 	CLEARPIN();//ONEWIRE=0;
 	_delay_us(2);
 	if(bit!=0) SETPIN();//ONEWIRE=1;
 	_delay_us(60);
 	SETPIN();//ONEWIRE=1;
 	//SETIN();//in
 	SETPIN();//ONEWIRE=1;
	sei();//SREG=sreg_temp;
 	_delay_us(1);
}


//--------------------------------------------------------------------------
// Read 1 bit from 1-wire interface
unsigned char OW_read_bit (){
	//unsigned char sreg_temp=SREG;
	unsigned char bit;
	SETPIN();//ONEWIRE=1;
	cli();
	SETOUT();//out
	CLEARPIN();//ONEWIRE=0;
	_delay_us(1);
	SETIN();//in
	SETPIN();//ONEWIRE=1;
	_delay_us(5);//Optim: t0+9.125us, no optim: t0+18.25us, ma byt: <15us
	bit=PINVALUE()!=0?1:0;
	sei();//SREG=sreg_temp;
	_delay_us(60);
	SETPIN();//ONEWIRE=1;
	SETOUT();//out
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

void TestTiming(){
	SETPIN();
	SETOUT();
	CLEARPIN();
	//optim, no optim [us]
	_delay_us(1);//1.908, 4.614
	SETPIN();
	_delay_us(2);//1.982, 4.614
	CLEARPIN();
	_delay_us(60);//60.183, 59.716
	SETPIN();
	_delay_us(12);//12.11, 11.53
	CLEARPIN();
	_delay_us(90);//90.275, 89.79
	SETPIN();
	_delay_us(500);//500.9, 500.44
	CLEARPIN();
	_delay_us(390);//389, 390.417
	SETPIN();
}


//--------------------------------------------------------
//Adresy:
//--------------------------------------------------------

void OW_Match_ROM(uint8_t rom[8]){
    int i;
    OW_write_byte(OW_MATCH_ROM_COMMAND);
    for( i = 0; i < 8; i++) OW_write_byte(rom[i]);
}

OW_address ROM_NO;
uint8_t LastDiscrepancy;
uint8_t LastFamilyDiscrepancy;
uint8_t LastDeviceFlag;

uint8_t OW_search(uint8_t *newAddr)
{
   uint8_t id_bit_number;
   uint8_t last_zero, rom_byte_number, search_result;
   uint8_t id_bit, cmp_id_bit;

   unsigned char rom_byte_mask, search_direction;

   // initialize for search
   id_bit_number = 1;
   last_zero = 0;
   rom_byte_number = 0;
   rom_byte_mask = 1;
   search_result = 0;

   // if the last call was not the last one
   if (!LastDeviceFlag)
   {
      // 1-Wire reset
      if (!OW_reset())
      {
         // reset the search
         LastDiscrepancy = 0;
         LastDeviceFlag = 0;
         LastFamilyDiscrepancy = 0;
         return 0;
      }

      // issue the search command
      OW_write_byte(OW_SEARCH_ROM_COMMAND);

      // loop to do the search
      do
      {
         // read a bit and its complement
         id_bit = OW_read_bit();
         cmp_id_bit = OW_read_bit();

         // check for no devices on 1-wire
         if ((id_bit == 1) && (cmp_id_bit == 1))
            break;
         else
         {
            // all devices coupled have 0 or 1
            if (id_bit != cmp_id_bit)
               search_direction = id_bit;  // bit write value for search
            else
            {
               // if this discrepancy if before the Last Discrepancy
               // on a previous next then pick the same as last time
               if (id_bit_number < LastDiscrepancy)
                  search_direction = ((ROM_NO.bytes[rom_byte_number] & rom_byte_mask) > 0);
               else
                  // if equal to last pick 1, if not then pick 0
                  search_direction = (id_bit_number == LastDiscrepancy);

               // if 0 was picked then record its position in LastZero
               if (search_direction == 0)
               {
                  last_zero = id_bit_number;

                  // check for Last discrepancy in family
                  if (last_zero < 9)
                     LastFamilyDiscrepancy = last_zero;
               }
            }

            // set or clear the bit in the ROM byte rom_byte_number
            // with mask rom_byte_mask
            if (search_direction == 1)
              ROM_NO.bytes[rom_byte_number] |= rom_byte_mask;
            else
              ROM_NO.bytes[rom_byte_number] &= ~rom_byte_mask;

            // serial number search direction write bit
            OW_write_bit(search_direction);

            // increment the byte counter id_bit_number
            // and shift the mask rom_byte_mask
            id_bit_number++;
            rom_byte_mask <<= 1;

            // if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
            if (rom_byte_mask == 0)
            {
                rom_byte_number++;
                rom_byte_mask = 1;
            }
         }
      }
      while(rom_byte_number < 8);  // loop until through all ROM bytes 0-7

      // if the search was successful then
      if (!(id_bit_number < 65))
      {
         // search successful so set LastDiscrepancy,LastDeviceFlag,search_result
         LastDiscrepancy = last_zero;

         // check for last device
         if (LastDiscrepancy == 0)
            LastDeviceFlag = 1;

         search_result = 1;
      }
   }

   // if no device found then reset counters so next 'search' will be like a first
   if (!search_result || !ROM_NO.bytes[0])
   {
      LastDiscrepancy = 0;
      LastDeviceFlag = 0;
      LastFamilyDiscrepancy = 0;
      search_result = 0;
   }
   int i;
   for (i = 0; i < 8; i++) newAddr[i] = ROM_NO.bytes[i];
   return search_result;
  }


void OW_reset_search()
  {
  // reset the search state
  LastDiscrepancy = 0;
  LastDeviceFlag = 0;
  LastFamilyDiscrepancy = 0;
  int i;
  for(i = 7; ; i--)
    {
    ROM_NO.bytes[i] = 0;
    if ( i == 0) break;
    }
  }



