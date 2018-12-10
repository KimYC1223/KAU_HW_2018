#include "address_map_nios2.h"
#include "nios2_ctrl_reg_macros.h"
#include "monitor_count_bitmap.h"

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
void monitor_count_bitmap(int x, int y, int count);
void clear_screen();
void wait_for_vsync();

volatile int *TIMER_ptr = (int *)TIMER_BASE;
int count1 = 30; /* port1의 시간값*/
int count2 = 20; /* port2의 시간값*/
int count3 = 50; /* port3의 시간값*/
int time = 0x5F5E0FF; /*100MHz으로 1초에 1씩 감소하도록 초기값을 1초로 설정*/

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
	*(TIMER_ptr + 1) = 0b0111; //start와 continue와 ito를 1로 설정
}
/* Turn on interrupts in the Nios II processor */
void enable_nios2_interrupts()
{
	NIOS2_WRITE_IENABLE(0x1);
	NIOS2_WRITE_STATUS(1);
}

void wait_for_vsync() /*vsync 맞추기*/
{
	register int status;
	*pixel_ctrl_ptr = 1;

	status = *(pixel_ctrl_ptr + 3);
	while ((status & 0x01) != 0)
		status = *(pixel_ctrl_ptr + 3);
}

int main(void)
{
	config_timer(); // configure interval timer
	enable_nios2_interrupts(); // enable interrupts in the Nios II processor
	pixel_ctrl_ptr = (int *)PIXEL_BUF_CTRL_BASE; // De1-SoC의 픽셀버퍼 포인터 설정
	*(pixel_ctrl_ptr + 1) = front_buffer; //  
	wait_for_vsync(); // tearing effect 제거

	pixel_buffer_start = *pixel_ctrl_ptr; // buffer address를 가리킴
	clear_screen(); // 바탕을 검은색으로 clear
	*(pixel_ctrl_ptr + 1) = back_buffer;

	while (1) // wait for an interrupt
	{
		pixel_buffer_start = *(pixel_ctrl_ptr + 1); // back buffer 값을 front buffer에 써줘 시작점을 back buffer로 잡는다.
		clear_screen();
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
		wait_for_vsync();
	}
}

void clear_screen() /* 화면 클리어 부분 */
{
	draw_square(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, 0xffff);
}

void wait_for_vsync()
{
	register int status;
	*pixel_ctrl_ptr = 1;

	status = *(pixel_ctrl_ptr + 3);
	while ((status & 0x01) != 0)
		status = *(pixel_ctrl_ptr + 3);
}
