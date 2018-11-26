#include "address_map_nios2.h"
#include "nios2_ctrl_reg_macros.h"
#include "stdio.h"

volatile int *KEY_ptr = (int *)KEY_BASE;
volatile int *JP1_ptr = (int *)JP1_BASE;
volatile int *JP2_ptr = (int *)JP2_BASE;
volatile int *LEDR_ptr = (int *)LEDR_BASE;


void config_KEYs(void)
{
	*(KEY_ptr + 2) = 0b011;
}

void enable_nios2_interrupt(void)
{
	NIOS2_WRITE_IENABLE(0x2); /*JP1이랑 JP2는 11이랑 12인데 각각 HEX로 800이랑 1000이다. 바이너리로 합하면1802*/
	NIOS2_WRITE_STATUS(1);
}

int main(void)
{
	/* init interface directions */
	*(JP2_ptr + 1) = 0xffffffff
	*(JP1_ptr + 1) = 0; /*set every JP1 bit direction to input */

	/* Set up the pushbutton KEYs port in the FPGA */

	while (1)
	{
		config_KEYs(); /* configure pushbutton KEYs to generate interrupts */
		enable_nios2_interrupt(); /* enable interrupts in the Nios II processor */
		while (1); /* wait for an interrupt */
	}
}