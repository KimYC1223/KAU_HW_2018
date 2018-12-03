#include "address_map_nios2.h"
#include "stdio.h"

extern volatile int *KEY_ptr;
extern volatile int *SW_ptr;
extern int PORT1_STATE; //포트 1의 초기상태_RED
extern int PORT2_STATE; //포트 2의 초기상태_RED
extern int PORT3_STATE; //포트 3의 초기상태_RED
extern int count1;
extern int count2;
extern int count3;
extern int RED;
extern int GREEN;

void pushbutton_ISR(void)
{
	int press = *(KEY_ptr + 3);
	*(KEY_ptr + 3) = press;
	int reset_flag;
	if ((press & 0b01) != 0)
	{
		if (count1 != 0)
		{
			if (PORT1_STATE == RED) // 꺼져있는데 키면 on상태를 나타내라
			{
				PORT1_STATE = GREEN; // 빨강색이면 초록색으로 바꿔 off에서 on으로 바꿔라
				printf("hi %d", PORT1_STATE);
			}
			else if (PORT1_STATE == GREEN) // 초록색이면 빨강색으로 바꿔 on에서 off으로 바꿔라
			{
				PORT1_STATE = RED;
			}
		}
		if (count1 == 0)
		{
			PORT1_STATE = RED;
		}
	}
	if ((press & 0b10) != 0)
	{
		if (count2 != 0)
		{
			if (PORT2_STATE == RED) // 꺼져있는데 키면 on상태를 나타내라
			{
				PORT2_STATE = GREEN; // 빨강색이면 초록색으로 바꿔 off에서 on으로 바꿔라
			}
			else if (PORT2_STATE == GREEN) // 초록색이면 빨강색으로 바꿔 on에서 off으로 바꿔라
			{
				PORT2_STATE = RED;
			}
		}
	}
	if ((press & 0b100) != 0)
	{
		if (count3 != 0)
		{
			if (PORT3_STATE == RED) // 꺼져있는데 키면 on상태를 나타내라
			{
				PORT3_STATE = GREEN; // 빨강색이면 초록색으로 바꿔 off에서 on으로 바꿔라
			}
			else if (PORT3_STATE == GREEN) // 초록색이면 빨강색으로 바꿔 on에서 off으로 바꿔라
			{
				PORT3_STATE = RED;
			}
		}
	}
	if ((press & 0b1000) != 0)
	{
		int sw = *SW_ptr;
		if (sw & 0b01) // sw0가 올라가면 port1을 10초 증가시킨다.
		{
			count1 += 10;
		}
		if ((sw & 0b10) != 0) // sw1이 올라가면 port2를 10초 증가시킨다.
		{
			count2 += 10;
		}
		if ((sw & 0b100) != 0) // sw2가 올라가면 prt3를 10초 증가시킨다.
		{
			count3 += 10;
		}
		if ((sw & 0b1000) != 0)
		{
			if (sw & 0b01) // sw0가 올라가면 port1을 10초 증가시킨다.
			{
				count1 = 0;
			}
			if (sw & 0b10) // sw0가 올라가면 port1을 10초 증가시킨다.
			{
				count2 = 0;
			}
			if (sw & 0b100) // sw0가 올라가면 port1을 10초 증가시킨다.
			{
				count3 = 0;
			}
		}
		if (count1 > 99)
		{
			count1 = 99;
		}
		if (count2 > 99)
		{
			count2 = 99;
		}
		if (count3 > 99)
		{
			count3 = 99;
		}
	}
}
