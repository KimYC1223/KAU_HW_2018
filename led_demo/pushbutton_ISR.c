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
	
	if(press == 1) // key0�� ������ ��
	{
		*JP1_ptr = 0x1; // D0���� high�� ������ bread board�� led on
	}
	else if(press == 2) // key1�� ������ ��
	{
		*JP1_ptr = 0; // D0���� low�� ������ bread board�� led off
	}
}
