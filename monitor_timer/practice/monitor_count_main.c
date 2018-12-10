#include "address_map_nios2.h"
#include "nios2_ctrl_reg_macros.h"
#include "monitor_count_bitmap.h"

#define SCREEN_WIDTH 320 // ������� ���� �ȼ��� 320��
#define SCREEN_HEIGHT 240 // ������� ���̸� �ȼ��� 240��
#define PORT1_TEN 55 // ��Ƽ���� port1�� 10���ڸ� ��Ÿ�� �ڸ�, state�� ��Ÿ�� �ڸ�
#define PORT1_ONE 85 // ��Ƽ���� port1�� 1���ڸ� ��Ÿ�� �ڸ�
#define PORT2_TEN 115 // ��Ƽ���� port2�� 10���ڸ� ��Ÿ�� �ڸ�, state�� ��Ÿ�� �ڸ�
#define PORT2_ONE 145 // ��Ƽ���� port2�� 1���ڸ� ��Ÿ�� �ڸ�
#define PORT3_TEN 175 // ��Ƽ���� port3�� 10���ڸ� ��Ÿ�� �ڸ�, state�� ��Ÿ�� �ڸ�
#define PORT3_ONE 205 // ��Ƽ���� port3�� 1���ڸ� ��Ÿ�� �ڸ�
#define NUMBER_y_axis 80 // ��Ƽ���� port1�� 10���ڸ� ��Ÿ�� �ڸ�
#define state_y_axis 100 // ��Ƽ���� port1�� 10���ڸ� ��Ÿ�� �ڸ�
void monitor_count_bitmap(int x, int y, int count);

volatile int *TIMER_ptr = (int *)TIMER_BASE;
int count1 = 30; /* port1�� �ð���*/
int count2 = 20; /* port1�� �ð���*/
int count3 = 50; /* port1�� �ð���*/
int time = 0x5F5E0FF; /*100MHz���� 1�ʿ� 1�� �����ϵ��� �ʱⰪ�� 1�ʷ� ����*/

/* Set up timer */
void config_timer()
{
	*(TIMER_ptr + 2) = time;
	*(TIMER_ptr + 3) = time >> 16;
	*(TIMER_ptr + 1) = 0b0111; //start�� continue�� ito�� 1�� ����
}
/* Turn on interrupts in the Nios II processor */
void enable_nios2_interrupts()
{
	NIOS2_WRITE_IENABLE(0x1);
	NIOS2_WRITE_STATUS(1);
}

int main(void)
{
	config_timer(); // configure interval timer
	enable_nios2_interrupts(); // enable interrupts in the Nios II processor
	while (1) // wait for an interrupt
	{
		int ten = count1 / 10;
		int one = count1 % 10;
		monitor_count_bitmap(PORT1_TEN, NUMBER_y_axis, ten);
		monitor_count_bitmap(PORT1_ONE, NUMBER_y_axis, one);
		ten = count2 / 10;
		one = count2 % 10;
		monitor_count_bitmap(PORT2_TEN, NUMBER_y_axis, ten);
		monitor_count_bitmap(PORT2_ONE, NUMBER_y_axis, one);
		ten = count3 / 10;
		one = count3 % 10;
		monitor_count_bitmap(PORT3_TEN, NUMBER_y_axis, ten);
		monitor_count_bitmap(PORT3_ONE, NUMBER_y_axis, one);
	}
}