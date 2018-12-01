#include "address_map_nios2.h"

extern volatile int *TIMER_ptr;
extern int count1;
extern int count2;
extern int count3;


void timer_ISR(void)
{
	*TIMER_ptr = 0;
	if (count1 != 0) // port1의 타이머가 0이 아니면 계속 감소시키고 0이면 계속 0출력
	{
		count1 = count1 - 1;
	}
	if (count2 != 0) // port2의 타이머가 0이 아니면 계속 감소시키고 0이면 계속 0출력
	{
		count2 = count2 - 1;
	}
	if (count3 != 0) // port3의 타이머가 0이 아니면 계속 감소시키고 0이면 계속 0출력
	{
		count3 = count3 - 1;
	}
}