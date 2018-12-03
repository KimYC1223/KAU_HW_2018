#include "address_map_nios2.h"
#include "nios2_ctrl_reg_macros.h"
#include "monitor_count_bitmap.h"

#define SCREEN_WIDTH 320 // ������� ���� �ȼ��� 320��
#define SCREEN_HEIGHT 240 // ������� ���̸� �ȼ��� 240��
#define PORT1_TEN 65 // ��Ƽ���� port1�� 10���ڸ� ��Ÿ�� �ڸ�, state�� ��Ÿ�� �ڸ�
#define PORT1_ONE 95 // ��Ƽ���� port1�� 1���ڸ� ��Ÿ�� �ڸ�
#define PORT2_TEN 125 // ��Ƽ���� port2�� 10���ڸ� ��Ÿ�� �ڸ�, state�� ��Ÿ�� �ڸ�
#define PORT2_ONE 155 // ��Ƽ���� port2�� 1���ڸ� ��Ÿ�� �ڸ�
#define PORT3_TEN 185 // ��Ƽ���� port3�� 10���ڸ� ��Ÿ�� �ڸ�, state�� ��Ÿ�� �ڸ�
#define PORT3_ONE 215 // ��Ƽ���� port3�� 1���ڸ� ��Ÿ�� �ڸ�
#define NUMBER_y_axis 80 // Ÿ�̸� ���ڸ� ��Ÿ�� �ڸ�
#define state_y_axis 125 // ��Ƽ���� on/off�� ��Ÿ�� ��

void monitor_count_bitmap(int x, int y, int count);
void clear_screen();
void wait_for_vsync();

volatile int *KEY_ptr = (int *)KEY_BASE;
volatile int *TIMER_ptr = (int *)TIMER_BASE;
volatile int *SW_ptr = (int *)SW_BASE;
int count1 = 30; /* port1�� �ð���*/
int count2 = 20; /* port2�� �ð���*/
int count3 = 50; /* port3�� �ð���*/
int PORT1_STATE;
int PORT2_STATE;
int PORT3_STATE;
int RED = 0xF800; // ������_off����
int GREEN = 0x07E0; // �ʷϻ�_on����
int time = 0x5F5E0FF; /*100MHz���� 1�ʿ� 1�� �����ϵ��� �ʱⰪ�� 1�ʷ� ����*/

volatile int pixel_buffer_start;
volatile int *pixel_ctrl_ptr;

void clear_screen();
void wait_for_vsync();

short int front_buffer[512 * 256];
short int back_buffer[512 * 256];


/* Set up timer */
void config_timer()
{
	*(TIMER_ptr + 2) = time;
	*(TIMER_ptr + 3) = time >> 16;
	*(TIMER_ptr + 1) = 0b0111; //start�� continue�� ito�� 1�� ����
}

void config_KEYs(void)
{
	*(KEY_ptr + 2) = 0b1111;
}


/* Turn on interrupts in the Nios II processor */
void enable_nios2_interrupts()
{
	NIOS2_WRITE_IENABLE(0x3);
	NIOS2_WRITE_STATUS(1);
}

void wait_for_vsync() /*vsync ���߱�*/
{
	register int status;
	*pixel_ctrl_ptr = 1;

	status = *(pixel_ctrl_ptr + 3);
	while ((status & 0x01) != 0)
		status = *(pixel_ctrl_ptr + 3);
}

void clear_screen() /* ȭ�� Ŭ���� �κ� */
{
	draw_square(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, 0xffff);
}

int main(void)
{
	config_timer(); // configure interval timer
	config_KEYs(); // configure pushbutton
	enable_nios2_interrupts(); // enable interrupts in the Nios II processor
	pixel_ctrl_ptr = (int *)PIXEL_BUF_CTRL_BASE; // De1-SoC�� �ȼ����� ������ ����
	*(pixel_ctrl_ptr + 1) = front_buffer; //  
	wait_for_vsync(); // tearing effect ����

	pixel_buffer_start = *pixel_ctrl_ptr; // buffer address�� ����Ŵ
	clear_screen(); // ������ ���������� clear
	*(pixel_ctrl_ptr + 1) = back_buffer;

	PORT1_STATE = RED;
	PORT2_STATE = RED;
	PORT3_STATE = RED;

	while (1) // wait for an interrupt
	{
		pixel_buffer_start = *(pixel_ctrl_ptr + 1); // back buffer ���� front buffer�� ���� �������� back buffer�� ��´�.
		clear_screen();
		int ten = count1 / 10; // port1�� 10���ڸ�
		int one = count1 % 10; // port1�� 1���ڸ�
		monitor_count_bitmap(PORT1_TEN, NUMBER_y_axis, ten); // port1�� 10���ڸ��� ����Ϳ� ǥ��
		monitor_count_bitmap(PORT1_ONE, NUMBER_y_axis, one); // port1�� 1���ڸ��� ����Ϳ� ǥ��
		if (count1 == 0) // ī��Ʈ�� ����Ǹ� ��� ���������� ��Ÿ����.
		{
			PORT1_STATE = RED;
		}
		draw_square(PORT1_TEN, state_y_axis, PORT1_ONE + 20, state_y_axis + 35, PORT1_STATE);
		ten = count2 / 10; // port2�� 10���ڸ�
		one = count2 % 10; // port2�� 1���ڸ�
		monitor_count_bitmap(PORT2_TEN, NUMBER_y_axis, ten); // port2�� 10���ڸ��� ����Ϳ� ǥ��
		monitor_count_bitmap(PORT2_ONE, NUMBER_y_axis, one); // port2�� 1���ڸ��� ����Ϳ� ǥ��
		if (count2 == 0) // ī��Ʈ�� ����Ǹ� ��� ���������� ��Ÿ����.
		{
			PORT2_STATE = RED;
		}
		draw_square(PORT2_TEN, state_y_axis, PORT2_ONE + 20, state_y_axis + 35, PORT2_STATE);
		ten = count3 / 10; // port3�� 10���ڸ�
		one = count3 % 10; // port3�� 1���ڸ�
		monitor_count_bitmap(PORT3_TEN, NUMBER_y_axis, ten); // port3�� 10���ڸ��� ����Ϳ� ǥ��
		monitor_count_bitmap(PORT3_ONE, NUMBER_y_axis, one); // port3�� 1���ڸ��� ����Ϳ� ǥ��
		if (count3 == 0) // ī��Ʈ�� ����Ǹ� ��� ���������� ��Ÿ����.
		{
			PORT3_STATE = RED;
		}
		draw_square(PORT3_TEN, state_y_axis, PORT3_ONE + 20, state_y_axis + 35, PORT3_STATE);
		wait_for_vsync();
	}
}
