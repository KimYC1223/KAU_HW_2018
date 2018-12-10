#include "address_map_nios2.h"
#include "stdio.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

volatile int pixel_buffer_start;
volatile int *pixel_ctrl_ptr;

short int front_buffer[512 * 256];
short int back_buffer[512 * 256];

void monitor_bit_map(int x, int y, int count) /*헤더파일로 사용*/
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




