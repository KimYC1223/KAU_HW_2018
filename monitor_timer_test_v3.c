//==============================================================================
//	헤더 파일 인클루드
//==============================================================================
#include "address_map_nios2.h"
#include "nios2_ctrl_reg_macros.h"
#include "monitor_count_bitmap.h"
#include "BufferConst.h"
#include "Page.h"
#include <stdio.h>
#include <string.h>

//==============================================================================
//	상수 정의
//==============================================================================
#define SCREEN_WIDTH 320 	// 모니터의 폭을 픽셀로 320행
#define SCREEN_HEIGHT 240 	// 모니터의 높이를 픽셀로 240열
#define PORT1_TEN 65 		// 멀티탭의 port1의 10의자리 나타낼 자리
							// state를 나타낼 자리
#define PORT1_ONE 95 		// 멀티탭의 port1의 1의자리 나타낼 자리
#define PORT2_TEN 125 		// 멀티탭의 port2의 10의자리 나타낼 자리,
							// state를 나타낼 자리
#define PORT2_ONE 155 		// 멀티탭의 port2의 1의자리 나타낼 자리
#define PORT3_TEN 185 		// 멀티탭의 port3의 10의자리 나타낼 자리,
							// state를 나타낼 자리
#define PORT3_ONE 215 		// 멀티탭의 port3의 1의자리 나타낼 자리
#define NUMBER_y_axis 80 	// 타이머 숫자를 나타낼 자리
#define state_y_axis 125 	// 멀티탭의 on/off를 나타낼 것
#define BUFFER_SIZE 100		// 사용자 버퍼의 크기
#define true 1
#define false 0

//==============================================================================
//	함수 조기 선언
//==============================================================================
void monitor_count_bitmap(int x, int y, int count);
void VGA_text(int x, int y, char *text_ptr);
void texting(int x, int y, char *text_ptr);
void clear_screen();
void wait_for_vsync();
void enable_nios2_interrupts();
void config_GPIO();
void config_timer();
void config_KEYs();
void config_total();
void print_all_port_to_monitor();
void power_control();
void timer_control();
void inputBuffer_write(char item);
char inputBuffer_read();
void Terminal_clear ();
void Terminal_print(char *up , char *down);

//==============================================================================
//	전역 변수 선언 및 초기화
//==============================================================================
volatile int *KEY_ptr = (int *)KEY_BASE;
volatile int *TIMER_ptr = (int *)TIMER_BASE;
volatile int *SW_ptr = (int *)SW_BASE;
volatile int *JP1_ptr = (int *)JP1_BASE;

int count1 = 0, count2 = 0, count3 = 0;		// port들의 시간값
char mode1=true,mode2=true,mode3=true;	// 포트 모드 지정
int PORT1_STATE, PORT2_STATE, PORT3_STATE;
int PORT1_RUN=false, PORT2_RUN=false, PORT3_RUN=false;

int RED = 0xF800,GREEN = 0x07E0;   			// 색 지정
int time = 0x5F5E0FF; 						//100MHz으로 1초에 1씩 감소하도록
											// 초기값을 1초로 설정
volatile int pixel_buffer_start;
volatile int *pixel_ctrl_ptr;
short int front_buffer[512 * 256];
short int back_buffer[512 * 256];

volatile int  ControllerState = 0;
volatile char Message;

//==============================================================================
//	사용자의 입력을 받을 인풋 버퍼
//==============================================================================
volatile char inputBuffer[BUFFER_SIZE] = {0};
volatile int buffer_in  = 0;
volatile int buffer_out = 0;

//==============================================================================
//	캐릭터 버퍼를 사용하기 위한 변수들
//==============================================================================
char erase[50] = "                                                 \0";
char waitFor[5] = "next\0";
char waitFor2[6] = "state\0";

//==============================================================================
//
//	MAIN 함수
//
//==============================================================================
int main(void)
{
	char terminal1[42] = "up\0";
	char terminal2[42] = "down\0";
	int page = PAGE_Idle;
	int selection = 0;
	int selection2 = 0;
	
	config_total();
	//==========================================================================
	//	반복문 
	//==========================================================================
	while (1) // wait for an interrupt
	{
		//======================================================================
		//	그림을 그림
		//======================================================================
		print_all_port_to_monitor();
		
		
		
		//======================================================================
		//	메인 페이지 화면
		//======================================================================
		char check = inputBuffer_read();
		
		switch ( page ) {
			//==================================================================
			//	기본 페이지
			//==================================================================
			case PAGE_Idle :
				printf("Idle Page!\n");
				//==============================================================
				//	Terminal Print
				//==============================================================
				strcpy(terminal1,
						"Multi-Function Power Supply Device\0");
				strcpy(terminal2,
						"Press OK Button or Cancle Button \0" );
				Terminal_print ( terminal1 , terminal2 );
				
				//==============================================================
				//	Input Handling
				//==============================================================
				if ( check == CTR_O || check == KEY_0 )
					page = PAGE_Selection;
				else if ( check == CTR_X || check == KEY_1 )
					page = PAGE_Reset;
			break;
			
			//==================================================================
			//	기기 리셋 페이지
			//==================================================================
			case PAGE_Reset :
				printf("Reset Page!\n");
				//==============================================================
				//	Terminal Print
				//==============================================================
				strcpy(terminal1, "Do you want to reset your Device?\0");
				if ( selection == 0 )
					strcpy(terminal2, "   ->YES      NO\0");
				else if ( selection == 1 )
					strcpy(terminal2, "     YES    ->NO\0");

				Terminal_print ( terminal1 , terminal2 );
				
				//==============================================================
				//	Input Handling
				//==============================================================
				if ( check == CTR_O || check == KEY_0 ) {
					if ( selection == 0 ) {
						count1 = 0; count2 = 0; count3 = 0;
						PORT1_RUN = false;PORT2_RUN = false;PORT3_RUN = false;
						PORT1_STATE = RED;PORT2_STATE = RED;PORT3_STATE = RED;
						mode1 = true; mode2 = true; mode3 = true;
						page = PAGE_Idle;
						selection = 0 ;
					} else {
						page = PAGE_Idle;
						selection = 0 ;
					}
				} else if ( check == CTR_X || check == KEY_1 ) {
					page = PAGE_Idle;
					selection = 0 ;
				} else if ( check == CTR_L || check == KEY_3
						 || check == CTR_R || check == KEY_2 ) {
					selection = (selection == 1 ) ? 0 : 1 ;
				}
					
			break;
			
			//==================================================================
			//	포트 선택 페이지
			//==================================================================
			case PAGE_Selection :
				printf("Selection Page!\n");
				//==============================================================
				//	Terminal Print
				//==============================================================
				strcpy(terminal1, "Please Select Port\0");
				if ( selection == 0 )
					strcpy(terminal2, "   ->PORT1   PORT2   PORT3\0");
				else if ( selection == 1 )
					strcpy(terminal2, "     PORT1 ->PORT2   PORT3\0");
				else if ( selection == 2 )
					strcpy(terminal2, "     PORT1   PORT2 ->PORT3\0");

				Terminal_print ( terminal1 , terminal2 );
				
				//==============================================================
				//	Input Handling
				//==============================================================
				if ( check == CTR_O || check == KEY_0) {
					page = PAGE_funcSelect;
				}else if ( check == CTR_X || check == KEY_1 ) {
					page = PAGE_Idle;
					selection = 0 ;
				} else if ( check == CTR_L || check == KEY_3 ){
					selection = (selection == 0) ? 2 : selection - 1;
				} else if ( check == CTR_R || check == KEY_2 ){
					selection = (selection + 1) % 3;
				}
					
			break;
			
			//==================================================================
			//	기능 선택 페이지
			//==================================================================
			case PAGE_funcSelect :
				printf("funcSelect Page!\n");
				//==============================================================
				//	Terminal Print
				//==============================================================
				strcpy(terminal1, "What do you want?\0");
				if ( selection2 == 0 )
					strcpy(terminal2, " ->Toggle   Run   Stop   Timer\0");
				else if ( selection2 == 1 )
					strcpy(terminal2, "   Toggle ->Run   Stop   Timer\0");
				else if ( selection2 == 2 )
					strcpy(terminal2, "   Toggle   Run ->Stop   Timer\0");
				else if ( selection2 == 3 )
					strcpy(terminal2, "   Toggle   Run   Stop ->Timer\0");

				Terminal_print ( terminal1 , terminal2 );
				
				//==============================================================
				//	Input Handling
				//==============================================================
				if ( check == CTR_O || check == KEY_0 ) {		
					//==========================================================
					//	 기능 구현
					//==========================================================
					switch ( selection2 ) {
						//======================================================
						//	토글을 하고 싶을 때
						//======================================================
						case 0 : 
							if ( selection == 0 ) {
								count1 = 0; PORT1_RUN = true;
							} else if ( selection == 1 ) {
								count2 = 0; PORT2_RUN = true;
							} else if ( selection == 2 ) {
								count3 = 0; PORT3_RUN = true;
							}
						break;
						
						//======================================================
						//	타이머를 시작시키고 싶을 때
						//======================================================
						case 1 : 
							if ( selection == 0 ) {
								PORT1_RUN = true;
							} else if ( selection == 1 ) {
								PORT2_RUN = true;
							} else if ( selection == 2 ) {
								PORT3_RUN = true;
							}
						break;
						
						//======================================================
						//	타이머를 멈추고 싶을 때
						//======================================================
						case 2 : 
							if ( selection == 0 ) {
								PORT1_RUN = false;
							} else if ( selection == 1 ) {
								PORT1_RUN = false;
							} else if ( selection == 2 ) {
								PORT1_RUN = false;
							}
						break;
						
						//======================================================
						//	타이머를 멈추고 싶을 때
						//======================================================
						case 3 : 
							page = PAGE_TimerSet;
						break;
					}
					selection2 = 0;
						
				} else if ( check == CTR_X || check == KEY_1 ) {
					page = PAGE_Selection;
					selection2 = 0 ;
				} else if ( check == CTR_L || check == KEY_3 ){
					selection2 = (selection2 == 0) ? 3 : selection2 - 1;
				} else if ( check == CTR_R || check == KEY_2 ){
					selection2 = (selection2 + 1) % 4;
				}
					
			break;
			
			//==================================================================
			//	포트 선택 페이지
			//==================================================================
			case PAGE_TimerSet :
				printf("TimerSet Page!\n");
				//==============================================================
				//	Terminal Print
				//==============================================================
				strcpy(terminal1, "Timer Setting!\0");
				strcpy(terminal2, "   Input > \0");
				
				switch ( selection2 / 10 ) {
					case 0 : strcat(terminal2,"0"); break;
					case 1 : strcat(terminal2,"1"); break;
					case 2 : strcat(terminal2,"2"); break;
					case 3 : strcat(terminal2,"3"); break;
					case 4 : strcat(terminal2,"4"); break;
					case 5 : strcat(terminal2,"5"); break;
					case 6 : strcat(terminal2,"6"); break;
					case 7 : strcat(terminal2,"7"); break;
					case 8 : strcat(terminal2,"8"); break;
					case 9 : strcat(terminal2,"9"); break;
					default : strcat(terminal2,"0 "); break;
				}
				
				switch ( selection2 % 10 ) {
					case 0 : strcat(terminal2,"0 "); break;
					case 1 : strcat(terminal2,"1 "); break;
					case 2 : strcat(terminal2,"2 "); break;
					case 3 : strcat(terminal2,"3 "); break;
					case 4 : strcat(terminal2,"4 "); break;
					case 5 : strcat(terminal2,"5 "); break;
					case 6 : strcat(terminal2,"6 "); break;
					case 7 : strcat(terminal2,"7 "); break;
					case 8 : strcat(terminal2,"8 "); break;
					case 9 : strcat(terminal2,"9 "); break;
					default : strcat(terminal2,"0 "); break;
				}
				
				Terminal_print ( terminal1 , terminal2 );
				
				//==============================================================
				//	Input Handling
				//==============================================================
				if ( check == CTR_O || check == KEY_0 ) {
					if ( selection == 0 ) count1 = selection2;
					else if ( selection == 1 ) count2 = selection2;
					else if ( selection == 2 ) count3 = selection2;
					selection2 = 0;
					page = PAGE_funcSelect;
				} else if ( check == CTR_X || check == KEY_1 ) {
					selection2 = 0;
					page = PAGE_funcSelect;
				} else if ( check == KEY_3 ){
					selection2 += 10;
				} else if ( check == KEY_2 ){
					selection2 += 1;
				} else if ( check == CTR_0 || check == CTR_1 || check == CTR_2
					     || check == CTR_3 || check == CTR_4 || check == CTR_5
						 || check == CTR_6 || check == CTR_7 || check == CTR_8
						 || check == CTR_9){
					selection2 = (selection2 % 10) * 10 + ( check - 1 );
					
				}
			
			break;
			
			default:
				printf("Page Error!\n");
			break;
		}
		
		power_control();
		
		
		
		//======================================================================
		//	V sync를 기다림
		//======================================================================
		wait_for_vsync(	);
	}
}
//==============================================================================
//	토탈 초기화
//==============================================================================
void config_total() {
	config_timer(); 								// 타이머 초기화
	config_KEYs(); 									// KEY 초기화
	config_GPIO();									// GPIO 초기화
	enable_nios2_interrupts(); 						// NIOS 2 
	
	pixel_ctrl_ptr = (int *)PIXEL_BUF_CTRL_BASE;	// De1-SoC의 픽셀버퍼
	*(pixel_ctrl_ptr + 1) = front_buffer; 			// 포인터 설정
	wait_for_vsync(); 								// tearing effect 제거

	pixel_buffer_start = *pixel_ctrl_ptr; 			// buffer address를 가리킴
	clear_screen(); 								// 바탕을 검은색으로 clear
	*(pixel_ctrl_ptr + 1) = back_buffer;

	PORT1_STATE = RED;								// 포트 1번 초기화
	PORT2_STATE = RED;								// 포트 2번 초기화
	PORT3_STATE = RED;								// 포트 3번 초기화
}

//==============================================================================
//	타이머 제어
//==============================================================================
void timer_control(){
	//==============================================================================
	//	포트 1번 제어 
	//==============================================================================
		//	만약 모드가 Wait for ON 이라면,
		//==========================================================================
		if ( mode1 == true ) {
			if ( PORT1_STATE == RED ) {
				if ( count1 > 0 ) { if ( PORT1_RUN == true )count1 --; }
				else if ( count1 == 0 && PORT1_RUN == true )
					{ PORT1_STATE = GREEN; PORT1_RUN = false; mode1 = false;}
			}
		//==========================================================================
		//	만약 모드가 Wait for OFF 라면,
		//==========================================================================
		} else if ( mode1 == false ) {
			if ( PORT1_STATE == GREEN ) {
				if ( count1 > 0 ) { if ( PORT1_RUN == true ) count1 --; }
				else if( count1 == 0 && PORT1_RUN == true )
					{ PORT1_STATE = RED;  PORT1_RUN = false; mode1 = true;} 
			}
		}	
	//==============================================================================
	//	포트 2번 제어 
	//==============================================================================
		//	만약 모드가 Wait for ON 이라면,
		//==========================================================================
		if ( mode2 == true ) {
			if ( PORT2_STATE == RED ) {
				if ( count2 > 0 ) { if ( PORT2_RUN == true ) count2 --; }
				else if ( count2 == 0 && PORT2_RUN == true )
					{ PORT2_STATE = GREEN;  PORT2_RUN = false; mode2 = false;} 
			}
		//==========================================================================
		//	만약 모드가 Wait for OFF 라면,
		//==========================================================================
		} else if ( mode2 == false ) {
			if ( PORT2_STATE == GREEN ) {
				if ( count2 > 0 ) { if ( PORT2_RUN == true ) count2 --; }
				else if ( count2 == 0 && PORT2_RUN == true )
					{ PORT2_STATE = RED;  PORT2_RUN = false; mode2 = true;} 
			}
		}
		
	//==============================================================================
	//	포트 3번 제어 
	//==============================================================================
		//	만약 모드가 Wait for ON 이라면,
		//==========================================================================
		if ( mode3 == true ) {
			if ( PORT3_STATE == RED ) {
				if ( count3 > 0 ) { if ( PORT3_RUN == true ) count3 --; }
				else if ( count3 == 0 && PORT3_RUN == true )
					{ PORT3_STATE = GREEN;  PORT3_RUN = false; mode3 = false;} 
			}
		//==========================================================================
		//	만약 모드가 Wait for OFF 라면,
		//==========================================================================
		} else if ( mode3 == false ) {
			if ( PORT3_STATE == GREEN ) {
				if ( count3 > 0 ) { if ( PORT3_RUN == true ) count3 --; }
				else if ( count3 == 0 && PORT3_RUN == true )
					{ PORT3_STATE = RED;  PORT3_RUN = false; mode3 = true;} 
			}
		}
}

//==============================================================================
//	콘센트 파워 제어
//==============================================================================
void power_control() {
	char result = 0 ;
	if ( PORT1_STATE == GREEN ) result += 4;
	if ( PORT2_STATE == GREEN ) result += 8;
	if ( PORT3_STATE == GREEN ) result += 16;
	
	*(JP1_ptr) = (int)result;
}

//==============================================================================
//	모니터에 포트 출력
//==============================================================================
void print_all_port_to_monitor() {
	pixel_buffer_start = *(pixel_ctrl_ptr + 1);
	// back buffer 값을 front buffer에 써줘 시작점을 back buffer로 잡는다.
	clear_screen();
		
	//==========================================================================
	//	포트 1번 표현
	//==========================================================================
	int ten = count1 / 10; 					// port1의 10의자리
	int one = count1 % 10; 					// port1의 1의자리
	
	monitor_count_bitmap(PORT1_TEN, NUMBER_y_axis, ten);
	monitor_count_bitmap(PORT1_ONE, NUMBER_y_axis, one);
	// port1의 타이머를 모니터에 표현

	draw_square(PORT1_TEN, state_y_axis, PORT1_ONE + 20,
										state_y_axis + 35, PORT1_STATE);
											
	//==========================================================================
	//	포트 2번 표현
	//==========================================================================
	ten = count2 / 10; 						// port2의 10의자리
	one = count2 % 10; 						// port2의 1의자리
	monitor_count_bitmap(PORT2_TEN, NUMBER_y_axis, ten);
	monitor_count_bitmap(PORT2_ONE, NUMBER_y_axis, one); 
	// port2의 타이머를 모니터에 표현

	draw_square(PORT2_TEN, state_y_axis, PORT2_ONE + 20, 
										state_y_axis + 35, PORT2_STATE);
											
	//==========================================================================
	//	포트 3번 표현
	//==========================================================================
	ten = count3 / 10; 						// port3의 10의자리
	one = count3 % 10; 						// port3의 1의자리
	monitor_count_bitmap(PORT3_TEN, NUMBER_y_axis, ten); 
	monitor_count_bitmap(PORT3_ONE, NUMBER_y_axis, one);
	// port3의 타이머를 모니터에 표현
	
	draw_square(PORT3_TEN, state_y_axis, PORT3_ONE + 20, 
										state_y_axis + 35, PORT3_STATE);
	
	//==========================================================================
	// 캐릭터 버퍼를 사용할 배경을 그림
	//==========================================================================
	draw_square(PORT1_TEN, state_y_axis + 45,
							PORT3_ONE + 20, state_y_axis + 77, 0 );	
	
	draw_square(PORT1_TEN, NUMBER_y_axis - 32,
						PORT1_ONE + 20, NUMBER_y_axis - 16, 0 );
	texting ( PORT1_TEN+24 , NUMBER_y_axis - 28, waitFor );
	texting ( PORT1_TEN+24 , NUMBER_y_axis - 24, waitFor2 );
	texting ( PORT2_TEN+24 , NUMBER_y_axis - 28, waitFor );
	texting ( PORT2_TEN+24 , NUMBER_y_axis - 24, waitFor2 );
	texting ( PORT3_TEN+24 , NUMBER_y_axis - 28, waitFor );
	texting ( PORT3_TEN+24 , NUMBER_y_axis - 24, waitFor2 );
	
	draw_square(PORT2_TEN, NUMBER_y_axis - 32,
						PORT2_ONE + 20, NUMBER_y_axis - 16, 0 );
	draw_square(PORT3_TEN, NUMBER_y_axis - 32,
						PORT3_ONE + 20, NUMBER_y_axis - 16, 0 );
	
	//==========================================================================
	//	포트 1번의 모드 표현
	//==========================================================================
	if ( mode1 == true ) {
		draw_square(PORT1_TEN, NUMBER_y_axis - 32,
						PORT1_TEN + 16, NUMBER_y_axis - 16, GREEN );
	} else {
		draw_square(PORT1_TEN, NUMBER_y_axis - 32,
						PORT1_TEN + 16, NUMBER_y_axis - 16, RED );
	}
	
	//==========================================================================
	//	포트 2번의 모드 표현
	//==========================================================================
	if ( mode2 == true ) {
		draw_square(PORT2_TEN, NUMBER_y_axis - 32,
						PORT2_TEN + 16, NUMBER_y_axis - 16, GREEN );
	} else {
		draw_square(PORT2_TEN, NUMBER_y_axis - 32,
						PORT2_TEN + 16, NUMBER_y_axis - 16, RED );
	}
	
	//==========================================================================
	//	포트 3번의 모드 표현
	//==========================================================================
	if ( mode3 == true ) {
		draw_square(PORT3_TEN, NUMBER_y_axis - 32,
						PORT3_TEN + 16, NUMBER_y_axis - 16, GREEN );
	} else {
		draw_square(PORT3_TEN, NUMBER_y_axis - 32,
						PORT3_TEN + 16, NUMBER_y_axis - 16, RED );
	}
}


//==============================================================================
//	버퍼에 넣는 함수
//==============================================================================
void inputBuffer_write(char item) {
	if ( ( buffer_in + 1 ) % BUFFER_SIZE  == buffer_out )
		return;
	
	inputBuffer [ buffer_in ] = item;
	buffer_in = ( buffer_in + 1 ) % BUFFER_SIZE ;
	return;
}

//==============================================================================
//	사용자 버퍼를 읽어오는 친구
//==============================================================================
char inputBuffer_read() {
	if ( buffer_in == buffer_out )
		return 0;
	
	int result = inputBuffer[buffer_out];
	buffer_out = (buffer_out + 1) % BUFFER_SIZE;
	return result;
}



//==============================================================================
//	타이머 초기화
//==============================================================================
void config_timer() {
	*(TIMER_ptr + 2) = time;
	*(TIMER_ptr + 3) = time >> 16;
	*(TIMER_ptr + 1) = 0b0111; //start와 continue와 ito를 1로 설정
}

//==============================================================================
//	푸쉬 버튼 초기화
//==============================================================================
void config_KEYs(void) {
	*(KEY_ptr + 2) = 0b1111;
}

//==============================================================================
//	GPIO 초기화
//==============================================================================
void config_GPIO(void) {
	*(JP1_ptr + 1) = 0b11100;
	*(JP1_ptr + 2) = 0b00010;
	*(JP1_ptr + 2) = 0xFFFFFFFF;
	*(  JP1_ptr  ) = 0b0;
}

//==============================================================================
//	Nios II 인터럽트 초기화
//==============================================================================
void enable_nios2_interrupts() {
	NIOS2_WRITE_IENABLE(0b100000000011);
	NIOS2_WRITE_STATUS(1);
}

//==============================================================================
//	V sync 맞추는 함수
//==============================================================================
void wait_for_vsync() /*vsync 맞추기*/ {
	register int status;
	*pixel_ctrl_ptr = 1;

	status = *(pixel_ctrl_ptr + 3);
	while ((status & 0x01) != 0)
		status = *(pixel_ctrl_ptr + 3);
}

//==============================================================================
//	화면에 텍스트를 출력하는 함수
//==============================================================================
void VGA_text(int x, int y, char * text_ptr) 
{
	int offset;
	volatile char * character_buffer = (char *)FPGA_CHAR_BASE;	

	offset = (y << 7) + x;
	while (*(text_ptr)) {
		*(character_buffer + offset) = *(text_ptr);	
		++text_ptr;
		++offset;
	}
}

//==============================================================================
//	위치에 맞게 출력해주는 함수
//==============================================================================
void texting(int x, int y, char *text_ptr) 
{
	VGA_text(x/4, y/4,text_ptr);
}

//==============================================================================
//	터미널 윗쪽 출력
//==============================================================================
void Terminal_print (char *text_up,char *text_down ) 
{
	Terminal_clear ();
	VGA_text( ( PORT1_TEN+4 ) /4, ( state_y_axis + 57 ) / 4, text_up);
	VGA_text( ( PORT1_TEN+4 ) /4, ( state_y_axis + 63 ) / 4, text_down);
}

//==============================================================================
//	터미널 지우기
//==============================================================================
void Terminal_clear () 
{
	VGA_text( ( PORT1_TEN+4 ) /4, ( state_y_axis + 57 ) / 4, erase);
	VGA_text( ( PORT1_TEN+4 ) /4, ( state_y_axis + 63 ) / 4, erase);
}


//==============================================================================
//	화면을 지우는 함수
//==============================================================================
void clear_screen() /* 화면 클리어 부분 */ {
	draw_square(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, 0xffff);
}
