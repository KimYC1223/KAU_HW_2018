#include "address_map_nios2.h"
#include "stdio.h"

extern volatile int *JP1_ptr;
extern volatile int *JP2_ptr;
extern int value;

void gpio_ISR()
{
	int press = *(JP1_ptr + 3);
	*(JP1_ptr + 3) = 0xffffffff;
	
	if((press & 0b01) == 1)
	{
		value = value^1;
		printf("set : %d\n",*JP1_ptr);
	}
}