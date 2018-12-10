#include "address_map_nios2.h"

extern volatile int *TIMER_ptr;
extern int count1;
extern int count2;
extern int count3;


void timer_ISR(void)
{
	*TIMER_ptr = 0;
	if (count1 != 0) // port1�� Ÿ�̸Ӱ� 0�� �ƴϸ� ��� ���ҽ�Ű�� 0�̸� ��� 0���
	{
		count1 = count1 - 1;
	}
	if (count2 != 0) // port2�� Ÿ�̸Ӱ� 0�� �ƴϸ� ��� ���ҽ�Ű�� 0�̸� ��� 0���
	{
		count2 = count2 - 1;
	}
	if (count3 != 0) // port3�� Ÿ�̸Ӱ� 0�� �ƴϸ� ��� ���ҽ�Ű�� 0�̸� ��� 0���
	{
		count3 = count3 - 1;
	}
}