#include "address_map_nios2.h"

extern volatile int *TIMER_ptr;
extern int count1;
extern int count2;
extern int count3;
extern int PORT1_STATE; //��Ʈ 1�� �ʱ����_RED
extern int PORT2_STATE; //��Ʈ 2�� �ʱ����_RED
extern int PORT3_STATE; //��Ʈ 3�� �ʱ����_RED
extern int RED;
extern int GREEN;

void timer_ISR(void)
{
	*TIMER_ptr = 0;
	if (count1 != 0) // port1�� Ÿ�̸Ӱ� 0�� �ƴϸ� ��� ���ҽ�Ű�� 0�̸� ��� 0���
	{
		if (PORT1_STATE == GREEN) // ON�̸� -1�� ���ҽ��Ѷ�
		{
			count1 = count1 - 1;
		}
	}
	if (count2 != 0) // port2�� Ÿ�̸Ӱ� 0�� �ƴϸ� ��� ���ҽ�Ű�� 0�̸� ��� 0���
	{
		if (PORT2_STATE == GREEN) // ON�̸� -1�� ���ҽ��Ѷ�
		{
			count2 = count2 - 1;
		}
	}
	if (count3 != 0) // port3�� Ÿ�̸Ӱ� 0�� �ƴϸ� ��� ���ҽ�Ű�� 0�̸� ��� 0���
	{
		if (PORT3_STATE == GREEN) // ON�̸� -1�� ���ҽ��Ѷ�
		{
			count3 = count3 - 1;
		}
	}
}