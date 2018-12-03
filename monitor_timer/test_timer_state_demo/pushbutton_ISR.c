#include "address_map_nios2.h"
#include "stdio.h"

extern volatile int *KEY_ptr;
extern volatile int *SW_ptr;
extern int PORT1_STATE; //��Ʈ 1�� �ʱ����_RED
extern int PORT2_STATE; //��Ʈ 2�� �ʱ����_RED
extern int PORT3_STATE; //��Ʈ 3�� �ʱ����_RED
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
			if (PORT1_STATE == RED) // �����ִµ� Ű�� on���¸� ��Ÿ����
			{
				PORT1_STATE = GREEN; // �������̸� �ʷϻ����� �ٲ� off���� on���� �ٲ��
				printf("hi %d", PORT1_STATE);
			}
			else if (PORT1_STATE == GREEN) // �ʷϻ��̸� ���������� �ٲ� on���� off���� �ٲ��
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
			if (PORT2_STATE == RED) // �����ִµ� Ű�� on���¸� ��Ÿ����
			{
				PORT2_STATE = GREEN; // �������̸� �ʷϻ����� �ٲ� off���� on���� �ٲ��
			}
			else if (PORT2_STATE == GREEN) // �ʷϻ��̸� ���������� �ٲ� on���� off���� �ٲ��
			{
				PORT2_STATE = RED;
			}
		}
	}
	if ((press & 0b100) != 0)
	{
		if (count3 != 0)
		{
			if (PORT3_STATE == RED) // �����ִµ� Ű�� on���¸� ��Ÿ����
			{
				PORT3_STATE = GREEN; // �������̸� �ʷϻ����� �ٲ� off���� on���� �ٲ��
			}
			else if (PORT3_STATE == GREEN) // �ʷϻ��̸� ���������� �ٲ� on���� off���� �ٲ��
			{
				PORT3_STATE = RED;
			}
		}
	}
	if ((press & 0b1000) != 0)
	{
		int sw = *SW_ptr;
		if (sw & 0b01) // sw0�� �ö󰡸� port1�� 10�� ������Ų��.
		{
			count1 += 10;
		}
		if ((sw & 0b10) != 0) // sw1�� �ö󰡸� port2�� 10�� ������Ų��.
		{
			count2 += 10;
		}
		if ((sw & 0b100) != 0) // sw2�� �ö󰡸� prt3�� 10�� ������Ų��.
		{
			count3 += 10;
		}
		if ((sw & 0b1000) != 0)
		{
			if (sw & 0b01) // sw0�� �ö󰡸� port1�� 10�� ������Ų��.
			{
				count1 = 0;
			}
			if (sw & 0b10) // sw0�� �ö󰡸� port1�� 10�� ������Ų��.
			{
				count2 = 0;
			}
			if (sw & 0b100) // sw0�� �ö󰡸� port1�� 10�� ������Ų��.
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
