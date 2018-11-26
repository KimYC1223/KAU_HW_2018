#include "address_map_nios2.h"
#include "stdio.h"

extern volatile int *KEY_ptr;
extern volatile int *JP1_ptr;
extern volatile int *JP2_ptr;

void pushbutton_ISR()
{
	int press = *(KEY_ptr + 3);
	*(KEY_ptr + 3) = press;
	printf("hi");
	
	if ((press & 0b01) != 0)
	{
		*JP1_ptr = 0b1;
	}
	else if ((press & 0b10) != 0)
	{
		*JP1_ptr = 0;
	}
}
