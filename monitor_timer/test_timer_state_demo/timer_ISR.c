#include "address_map_nios2.h"

extern volatile int *TIMER_ptr;
extern int count1;
extern int count2;
extern int count3;
extern int PORT1_STATE; //포트 1의 초기상태_RED
extern int PORT2_STATE; //포트 2의 초기상태_RED
extern int PORT3_STATE; //포트 3의 초기상태_RED
extern int RED;
extern int GREEN;

void timer_ISR(void)
{
	*TIMER_ptr = 0;
	if (count1 != 0) // port1의 타이머가 0이 아니면 계속 감소시키고 0이면 계속 0출력
	{
		if (PORT1_STATE == GREEN) // ON이면 -1씩 감소시켜라
		{
			count1 = count1 - 1;
		}
	}
	if (count2 != 0) // port2의 타이머가 0이 아니면 계속 감소시키고 0이면 계속 0출력
	{
		if (PORT2_STATE == GREEN) // ON이면 -1씩 감소시켜라
		{
			count2 = count2 - 1;
		}
	}
	if (count3 != 0) // port3의 타이머가 0이 아니면 계속 감소시키고 0이면 계속 0출력
	{
		if (PORT3_STATE == GREEN) // ON이면 -1씩 감소시켜라
		{
			count3 = count3 - 1;
		}
	}
}