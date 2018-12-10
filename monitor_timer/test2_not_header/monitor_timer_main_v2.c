#include "address_map_nios2.h"
#include "nios2_ctrl_reg_macros.h"
#include <stdio.h>

#define SCREEN_WIDTH 320 // 모니터의 폭을 픽셀로 320행
#define SCREEN_HEIGHT 240 // 모니터의 높이를 픽셀로 240열
#define PORT1_TEN 55 // 멀티탭의 port1의 10의자리 나타낼 자리, state를 나타낼 자리
#define PORT1_ONE 85 // 멀티탭의 port1의 1의자리 나타낼 자리
#define PORT2_TEN 115 // 멀티탭의 port2의 10의자리 나타낼 자리, state를 나타낼 자리
#define PORT2_ONE 145 // 멀티탭의 port2의 1의자리 나타낼 자리
#define PORT3_TEN 175 // 멀티탭의 port3의 10의자리 나타낼 자리, state를 나타낼 자리
#define PORT3_ONE 205 // 멀티탭의 port3의 1의자리 나타낼 자리
#define NUMBER_y_axis 80 // 멀티탭의 port1의 10의자리 나타낼 자리
#define state_y_axis 100 // 멀티탭의 port1의 10의자리 나타낼 자리

void monitor_count_bitmap(int x, int y, int count); /*0~9까지의 모니터에 표현하기 위한 BIT MAP*/
void wait_for_vsync(); /*VSYNC를 맞추는 함수*/
void draw_square(int x1, int y1, int x2, int y2, short int color); /*모니터에 사각형을 그리는 함수*/
void plot_pixel(unsigned int x, unsigned int y, short int line_color); /*PIXEL에 색을 입히는 함수*/
void clear_screen(); /*화면 클리어*/

volatile int pixel_buffer_start;
volatile int * pixel_ctrl_ptr = (int *)PIXEL_BUF_CTRL_BASE;

short int front_buffer[512 * 256];
short int back_buffer[512 * 256];/*VGA 포트를 위한 픽셀 관련 선언*/

volatile int *TIMER_ptr = (int *)TIMER_BASE;
int count1 = 30; /* port1의 시간값*/
int count2 = 20; /* port1의 시간값*/
int count3 = 50; /* port1의 시간값*/
int time = 0x5F5E0FF; /*100MHz으로 1초에 1씩 감소하도록 초기값을 1초로 설정*/

/* Set up timer */
void config_timer()
{
	*(TIMER_ptr + 2) = time;
	*(TIMER_ptr + 3) = time >> 16;
	*(TIMER_ptr + 1) = 0b0111; //start와 continue와 ito를 1로 설정
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
	*(pixel_ctrl_ptr + 1) = back_buffer; /* front 버퍼와 back버퍼의 vsync를 맞춰준다. */


	while (1) // wait for an interrupt
	{
		pixel_buffer_start = *(pixel_ctrl_ptr + 1);
		clear_screen(); /*back buffer를 시작점으로 잡고 화면 클리어*/
		int ten = count1 / 10;
		int one = count1 % 10;
		monitor_count_bitmap(PORT1_TEN, NUMBER_y_axis, ten); /*port1의 10의자리 타이머*/
		monitor_count_bitmap(PORT1_ONE, NUMBER_y_axis, one); /*port1의 1의자리 타이머*/
		ten = count2 / 10;
		one = count2 % 10;
		monitor_count_bitmap(PORT2_TEN, NUMBER_y_axis, ten); /*port2의 10의자리 타이머*/
		monitor_count_bitmap(PORT2_ONE, NUMBER_y_axis, one); /*port2의 1의자리 타이머*/
		ten = count3 / 10;
		one = count3 % 10;
		monitor_count_bitmap(PORT3_TEN, NUMBER_y_axis, ten); /*port3의 10의자리 타이머*/
		monitor_count_bitmap(PORT3_ONE, NUMBER_y_axis, one); /*port3의 1의자리 타이머*/
		wait_for_vsync(); /*다음 frame을 기다린다.*/
	}
}


void monitor_count_bitmap(int x, int y, int count) /*0~9까지의 모니터에 나타낼 bit map */
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

void wait_for_vsync() /*vsync 맞추기*/
{
	register int status;
	*pixel_ctrl_ptr = 1;

	status = *(pixel_ctrl_ptr + 3);
	while ((status & 0x01) != 0)
		status = *(pixel_ctrl_ptr + 3);
}

void clear_screen() /*화면 클리어*/
{
	draw_square(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, 0xffff);
}


void draw_square(int x1, int y1, int x2, int y2, short int color) /*on/off 나타낼 때 쓸 사각형*/
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

void plot_pixel(unsigned int x, unsigned int y, short int line_color) /*점 하나 찍기*/
{
	*(short int*)(pixel_buffer_start + (((y) & 0xFF) << 10) + (((x) & 0x1FF) << 1)) = line_color;
}

