#include "address_map_nios2.h"
#include "nios2_ctrl_reg_macros.h"
#include <stdio.h>

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

void monitor_count_bitmap(int x, int y, int count); /*0~9������ ����Ϳ� ǥ���ϱ� ���� BIT MAP*/
void wait_for_vsync(); /*VSYNC�� ���ߴ� �Լ�*/
void draw_square(int x1, int y1, int x2, int y2, short int color); /*����Ϳ� �簢���� �׸��� �Լ�*/
void plot_pixel(unsigned int x, unsigned int y, short int line_color); /*PIXEL�� ���� ������ �Լ�*/
void clear_screen(); /*ȭ�� Ŭ����*/

volatile int pixel_buffer_start;
volatile int * pixel_ctrl_ptr = (int *)PIXEL_BUF_CTRL_BASE;

short int front_buffer[512 * 256];
short int back_buffer[512 * 256];/*VGA ��Ʈ�� ���� �ȼ� ���� ����*/

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

	*(pixel_ctrl_ptr + 1) = front_buffer;
	wait_for_vsync();
	pixel_buffer_start = *pixel_ctrl_ptr;
	clear_screen();
	*(pixel_ctrl_ptr + 1) = back_buffer; /* front ���ۿ� back������ vsync�� �����ش�. */


	while (1) // wait for an interrupt
	{
		pixel_buffer_start = *(pixel_ctrl_ptr + 1);
		clear_screen(); /*back buffer�� ���������� ��� ȭ�� Ŭ����*/
		int ten = count1 / 10;
		int one = count1 % 10;
		monitor_count_bitmap(PORT1_TEN, NUMBER_y_axis, ten); /*port1�� 10���ڸ� Ÿ�̸�*/
		monitor_count_bitmap(PORT1_ONE, NUMBER_y_axis, one); /*port1�� 1���ڸ� Ÿ�̸�*/
		ten = count2 / 10;
		one = count2 % 10;
		monitor_count_bitmap(PORT2_TEN, NUMBER_y_axis, ten); /*port2�� 10���ڸ� Ÿ�̸�*/
		monitor_count_bitmap(PORT2_ONE, NUMBER_y_axis, one); /*port2�� 1���ڸ� Ÿ�̸�*/
		ten = count3 / 10;
		one = count3 % 10;
		monitor_count_bitmap(PORT3_TEN, NUMBER_y_axis, ten); /*port3�� 10���ڸ� Ÿ�̸�*/
		monitor_count_bitmap(PORT3_ONE, NUMBER_y_axis, one); /*port3�� 1���ڸ� Ÿ�̸�*/
		wait_for_vsync(); /*���� frame�� ��ٸ���.*/
	}
}


void monitor_count_bitmap(int x, int y, int count) /*0~9������ ����Ϳ� ��Ÿ�� bit map */
{
	if (count == 0)
	{
		draw_square(x, y, x + 5, y + 35, 0x0000);
		draw_square(x + 5, y, x + 15, y + 5, 0x0000);
		draw_square(x + 15, y, x + 20, y + 35, 0x0000);
		draw_square(x + 5, y + 30, x + 15, y + 35, 0x0000);
	}

	if (count == 1)
	{
		draw_square(x + 15, y, x + 20, y + 35, 0x0000);
	}

	if (count == 2)
	{
		draw_square(x, y, x + 20, y + 5, 0x0000);
		draw_square(x + 15, y + 5, x + 20, y + 15, 0x0000);
		draw_square(x, y + 15, x + 20, y + 20, 0x0000);
		draw_square(x, y + 20, x + 5, y + 30, 0x0000);
		draw_square(x, y + 30, x + 20, y + 35, 0x0000);
	}

	if (count == 3)
	{
		draw_square(x, y, x + 20, y + 5, 0x0000);
		draw_square(x + 15, y + 5, x + 20, y + 15, 0x0000);
		draw_square(x, y + 15, x + 20, y + 20, 0x0000);
		draw_square(x + 15, y + 20, x + 20, y + 30, 0x0000);
		draw_square(x, y + 30, x + 20, y + 35, 0x0000);
	}

	if (count == 4)
	{
		draw_square(x, y, x + 5, y + 15, 0x0000);
		draw_square(x, y + 15, x + 20, y + 20, 0x0000);
		draw_square(x + 10, y, x + 15, y + 35, 0x0000);
	}

	if (count == 5)
	{
		draw_square(x, y, x + 20, y + 5, 0x0000);
		draw_square(x, y + 5, x + 5, y + 15, 0x0000);
		draw_square(x, y + 15, x + 20, y + 20, 0x0000);
		draw_square(x + 15, y + 20, x + 20, y + 30, 0x0000);
		draw_square(x, y + 30, x + 20, y + 35, 0x0000);
	}

	if (count == 6)
	{
		draw_square(x, y, x + 5, y + 35, 0x0000);
		draw_square(x + 5, y + 10, x + 20, y + 15, 0x0000);
		draw_square(x + 15, y + 15, x + 20, y + 30, 0x0000);
		draw_square(x + 5, y + 30, x + 20, y + 35, 0x0000);
	}

	if (count == 7)
	{
		draw_square(x, y, x + 20, y + 5, 0x0000);
		draw_square(x + 15, y + 5, x + 20, y + 35, 0x0000);
	}

	if (count == 8)
	{
		draw_square(x, y, x + 20, y + 5, 0x0000);
		draw_square(x, y + 15, x + 20, y + 20, 0x0000);
		draw_square(x, y + 30, x + 20, y + 35, 0x0000);
		draw_square(x, y, x + 5, y + 35, 0x0000);
		draw_square(x + 15, y, x + 20, y + 35, 0x0000);
	}

	if (count == 9)
	{
		draw_square(x, y, x + 20, y + 5, 0x0000);
		draw_square(x, y, x + 5, y + 20, 0x0000);
		draw_square(x, y + 15, x + 20, y + 20, 0x0000);
		draw_square(x + 15, y, x + 20, y + 35, 0x0000);
	}
}

void wait_for_vsync() /*vsync ���߱�*/
{
	register int status;
	*pixel_ctrl_ptr = 1;

	status = *(pixel_ctrl_ptr + 3);
	while ((status & 0x01) != 0)
		status = *(pixel_ctrl_ptr + 3);
}

void clear_screen() /*ȭ�� Ŭ����*/
{
	draw_square(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, 0xffff);
}


void draw_square(int x1, int y1, int x2, int y2, short int color) /*on/off ��Ÿ�� �� �� �簢��*/
{
	int x, y;
	for (x = x1; x <= x2; x++)
	{
		for (y = y1; y <= y2; y++)
		{
			plot_pixel(x, y, color);
		}

	}
}

void plot_pixel(unsigned int x, unsigned int y, short int line_color) /*�� �ϳ� ���*/
{
	*(short int*)(pixel_buffer_start + (((y) & 0xFF) << 10) + (((x) & 0x1FF) << 1)) = line_color;
}

