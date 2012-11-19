#include "mcu.h"

void delay_us(signed long us)
{
//	int i, tus;
//	tus = 9 * us;
//	for(i = 0; i < tus; i++) asm("nop");

	for(; us>0; us--) asm("nop");
}


void SwBreak()
{
    asm("nop");
}
