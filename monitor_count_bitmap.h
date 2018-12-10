#include "address_map_nios2.h"
#include "stdio.h"
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

//==============================================================================
//	픽셀 버퍼 설정
//==============================================================================
volatile int pixel_buffer_start;
volatile int *pixel_ctrl_ptr;

void draw_square(int x1, int y1, int x2, int y2, short int color);
void plot_pixel(unsigned int x, unsigned int y, short int color);
void monitor_count_bitmap(int x, int y, int count);

short int front_buffer[512 * 256];
short int back_buffer[512 * 256];

//==============================================================================
//	숫자 표현하는 함수
//==============================================================================
void monitor_count_bitmap(int x, int y, int count) 
{
	if (count == 0) {
		draw_square(x, y, x + 5, y + 35, 0x0000);
		draw_square(x + 5, y, x + 15, y + 5, 0x0000);
		draw_square(x + 15, y, x + 20, y + 35, 0x0000);
		draw_square(x + 5, y + 30, x + 15, y + 35, 0x0000);
	} else if (count == 1) {
		draw_square(x + 15, y, x + 20, y + 35, 0x0000);
	} else if (count == 2) {
		draw_square(x, y, x + 20, y + 5, 0x0000);
		draw_square(x + 15, y + 5, x + 20, y + 15, 0x0000);
		draw_square(x, y + 15, x + 20, y + 20, 0x0000);
		draw_square(x, y + 20, x + 5, y + 30, 0x0000);
		draw_square(x, y + 30, x + 20, y + 35, 0x0000);
	} else if (count == 3) {
		draw_square(x, y, x + 20, y + 5, 0x0000);
		draw_square(x + 15, y + 5, x + 20, y + 15, 0x0000);
		draw_square(x, y + 15, x + 20, y + 20, 0x0000);
		draw_square(x + 15, y + 20, x + 20, y + 30, 0x0000);
		draw_square(x, y + 30, x + 20, y + 35, 0x0000);
	} else if (count == 4) {
		draw_square(x, y, x + 5, y + 15, 0x0000);
		draw_square(x, y + 15, x + 20, y + 20, 0x0000);
		draw_square(x + 10, y, x + 15, y + 35, 0x0000);
	} else if (count == 5) {
		draw_square(x, y, x + 20, y + 5, 0x0000);
		draw_square(x, y + 5, x + 5, y + 15, 0x0000);
		draw_square(x, y + 15, x + 20, y + 20, 0x0000);
		draw_square(x + 15, y + 20, x + 20, y + 30, 0x0000);
		draw_square(x, y + 30, x + 20, y + 35, 0x0000);
	} else if (count == 6) {
		draw_square(x, y, x + 5, y + 35, 0x0000);
		draw_square(x + 5, y + 10, x + 20, y + 15, 0x0000);
		draw_square(x + 15, y + 15, x + 20, y + 30, 0x0000);
		draw_square(x + 5, y + 30, x + 20, y + 35, 0x0000);
	} else if (count == 7) {
		draw_square(x, y, x + 20, y + 5, 0x0000);
		draw_square(x + 15, y + 5, x + 20, y + 35, 0x0000);
	} else if (count == 8) {
		draw_square(x, y, x + 20, y + 5, 0x0000);
		draw_square(x, y + 15, x + 20, y + 20, 0x0000);
		draw_square(x, y + 30, x + 20, y + 35, 0x0000);
		draw_square(x, y, x + 5, y + 35, 0x0000);
		draw_square(x + 15, y, x + 20, y + 35, 0x0000);
	} else if (count == 9) {
		draw_square(x, y, x + 20, y + 5, 0x0000);
		draw_square(x, y, x + 5, y + 20, 0x0000);
		draw_square(x, y + 15, x + 20, y + 20, 0x0000);
		draw_square(x + 15, y, x + 20, y + 35, 0x0000);
	}
}

//==============================================================================
//	사각형을 그리는 함수
//==============================================================================
void draw_square(int x1, int y1, int x2, int y2, short int color)
{
	int x, y;
	for (x = x1; x <= x2; x++)
		for (y = y1; y <= y2; y++)
			plot_pixel(x, y, color);
}

//==============================================================================
//	화살표를 그리는 함수
//==============================================================================
void draw_arrow(int x, int y, short int color)
{
	int i , j ;
	for ( i = 0 ; i < 6 ; i ++ ) {
		for ( j = 6-i ; j >= 0 ; j -- ) {
			plot_pixel( x + 9  - j , y + i * 2 , color);
			plot_pixel( x + 10 + j , y + i * 2 , color);
		}
	}	
}

//==============================================================================
//	픽셀 하나를 그리는 함수
//==============================================================================
void plot_pixel(unsigned int x, unsigned int y, short int line_color) 
{
	*(short int*)(pixel_buffer_start + 
		(((y) & 0xFF) << 10) + (((x) & 0x1FF) << 1)) = line_color;
}




