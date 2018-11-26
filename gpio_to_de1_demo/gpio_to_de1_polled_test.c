#include "address_map_nios2.h"
#include "nios2_ctrl_reg_macros.h"
#include "stdio.h"

volatile int *JP1_ptr = (int *)JP1_BASE;
volatile int *JP2_ptr = (int *)JP2_BASE;
volatile int *LEDR_ptr = (int *)LEDR_BASE;



int main(void)
{
	/* init interface directions */
	*(JP2_ptr + 1) = 0xffffffff;
	*(JP1_ptr + 1) = 0xfffffffe; /*set every JP1 bit direction to input */



	while(1)
	{
		*LEDR_ptr = *JP1_ptr;
	}

}