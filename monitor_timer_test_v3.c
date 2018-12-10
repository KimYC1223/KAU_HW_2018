//==============================================================================
//	��� ���� ��Ŭ���
//==============================================================================
#include "address_map_nios2.h"
#include "nios2_ctrl_reg_macros.h"
#include "monitor_count_bitmap.h"
#include "BufferConst.h"
#include "Page.h"
#include <stdio.h>
#include <string.h>

//==============================================================================
//	��� ����
//==============================================================================
#define SCREEN_WIDTH 320 	// ������� ���� �ȼ��� 320��
#define SCREEN_HEIGHT 240 	// ������� ���̸� �ȼ��� 240��
#define PORT1_TEN 65 		// ��Ƽ���� port1�� 10���ڸ� ��Ÿ�� �ڸ�
							// state�� ��Ÿ�� �ڸ�
#define PORT1_ONE 95 		// ��Ƽ���� port1�� 1���ڸ� ��Ÿ�� �ڸ�
#define PORT2_TEN 125 		// ��Ƽ���� port2�� 10���ڸ� ��Ÿ�� �ڸ�,
							// state�� ��Ÿ�� �ڸ�
#define PORT2_ONE 155 		// ��Ƽ���� port2�� 1���ڸ� ��Ÿ�� �ڸ�
#define PORT3_TEN 185 		// ��Ƽ���� port3�� 10���ڸ� ��Ÿ�� �ڸ�,
							// state�� ��Ÿ�� �ڸ�
#define PORT3_ONE 215 		// ��Ƽ���� port3�� 1���ڸ� ��Ÿ�� �ڸ�
#define NUMBER_y_axis 80 	// Ÿ�̸� ���ڸ� ��Ÿ�� �ڸ�
#define state_y_axis 125 	// ��Ƽ���� on/off�� ��Ÿ�� ��
#define BUFFER_SIZE 100		// ����� ������ ũ��
#define true 1
#define false 0

//==============================================================================
//	�Լ� ���� ����
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
//	���� ���� ���� �� �ʱ�ȭ
//==============================================================================
volatile int *KEY_ptr = (int *)KEY_BASE;
volatile int *TIMER_ptr = (int *)TIMER_BASE;
volatile int *SW_ptr = (int *)SW_BASE;
volatile int *JP1_ptr = (int *)JP1_BASE;

int count1 = 0, count2 = 0, count3 = 0;		// port���� �ð���
char mode1=true,mode2=true,mode3=true;	// ��Ʈ ��� ����
int PORT1_STATE, PORT2_STATE, PORT3_STATE;
int PORT1_RUN=false, PORT2_RUN=false, PORT3_RUN=false;

int RED = 0xF800,GREEN = 0x07E0;   			// �� ����
int time = 0x5F5E0FF; 						//100MHz���� 1�ʿ� 1�� �����ϵ���
											// �ʱⰪ�� 1�ʷ� ����
volatile int pixel_buffer_start;
volatile int *pixel_ctrl_ptr;
short int front_buffer[512 * 256];
short int back_buffer[512 * 256];

volatile int  ControllerState = 0;
volatile char Message;

//==============================================================================
//	������� �Է��� ���� ��ǲ ����
//==============================================================================
volatile char inputBuffer[BUFFER_SIZE] = {0};
volatile int buffer_in  = 0;
volatile int buffer_out = 0;

//==============================================================================
//	ĳ���� ���۸� ����ϱ� ���� ������
//==============================================================================
char erase[50] = "                                                 \0";
char waitFor[5] = "next\0";
char waitFor2[6] = "state\0";

//==============================================================================
//
//	MAIN �Լ�
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
	//	�ݺ��� 
	//==========================================================================
	while (1) // wait for an interrupt
	{
		//======================================================================
		//	�׸��� �׸�
		//======================================================================
		print_all_port_to_monitor();
		
		
		
		//======================================================================
		//	���� ������ ȭ��
		//======================================================================
		char check = inputBuffer_read();
		
		switch ( page ) {
			//==================================================================
			//	�⺻ ������
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
			//	��� ���� ������
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
			//	��Ʈ ���� ������
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
			//	��� ���� ������
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
					//	 ��� ����
					//==========================================================
					switch ( selection2 ) {
						//======================================================
						//	����� �ϰ� ���� ��
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
						//	Ÿ�̸Ӹ� ���۽�Ű�� ���� ��
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
						//	Ÿ�̸Ӹ� ���߰� ���� ��
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
						//	Ÿ�̸Ӹ� ���߰� ���� ��
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
			//	��Ʈ ���� ������
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
		//	V sync�� ��ٸ�
		//======================================================================
		wait_for_vsync(	);
	}
}
//==============================================================================
//	��Ż �ʱ�ȭ
//==============================================================================
void config_total() {
	config_timer(); 								// Ÿ�̸� �ʱ�ȭ
	config_KEYs(); 									// KEY �ʱ�ȭ
	config_GPIO();									// GPIO �ʱ�ȭ
	enable_nios2_interrupts(); 						// NIOS 2 
	
	pixel_ctrl_ptr = (int *)PIXEL_BUF_CTRL_BASE;	// De1-SoC�� �ȼ�����
	*(pixel_ctrl_ptr + 1) = front_buffer; 			// ������ ����
	wait_for_vsync(); 								// tearing effect ����

	pixel_buffer_start = *pixel_ctrl_ptr; 			// buffer address�� ����Ŵ
	clear_screen(); 								// ������ ���������� clear
	*(pixel_ctrl_ptr + 1) = back_buffer;

	PORT1_STATE = RED;								// ��Ʈ 1�� �ʱ�ȭ
	PORT2_STATE = RED;								// ��Ʈ 2�� �ʱ�ȭ
	PORT3_STATE = RED;								// ��Ʈ 3�� �ʱ�ȭ
}

//==============================================================================
//	Ÿ�̸� ����
//==============================================================================
void timer_control(){
	//==============================================================================
	//	��Ʈ 1�� ���� 
	//==============================================================================
		//	���� ��尡 Wait for ON �̶��,
		//==========================================================================
		if ( mode1 == true ) {
			if ( PORT1_STATE == RED ) {
				if ( count1 > 0 ) { if ( PORT1_RUN == true )count1 --; }
				else if ( count1 == 0 && PORT1_RUN == true )
					{ PORT1_STATE = GREEN; PORT1_RUN = false; mode1 = false;}
			}
		//==========================================================================
		//	���� ��尡 Wait for OFF ���,
		//==========================================================================
		} else if ( mode1 == false ) {
			if ( PORT1_STATE == GREEN ) {
				if ( count1 > 0 ) { if ( PORT1_RUN == true ) count1 --; }
				else if( count1 == 0 && PORT1_RUN == true )
					{ PORT1_STATE = RED;  PORT1_RUN = false; mode1 = true;} 
			}
		}	
	//==============================================================================
	//	��Ʈ 2�� ���� 
	//==============================================================================
		//	���� ��尡 Wait for ON �̶��,
		//==========================================================================
		if ( mode2 == true ) {
			if ( PORT2_STATE == RED ) {
				if ( count2 > 0 ) { if ( PORT2_RUN == true ) count2 --; }
				else if ( count2 == 0 && PORT2_RUN == true )
					{ PORT2_STATE = GREEN;  PORT2_RUN = false; mode2 = false;} 
			}
		//==========================================================================
		//	���� ��尡 Wait for OFF ���,
		//==========================================================================
		} else if ( mode2 == false ) {
			if ( PORT2_STATE == GREEN ) {
				if ( count2 > 0 ) { if ( PORT2_RUN == true ) count2 --; }
				else if ( count2 == 0 && PORT2_RUN == true )
					{ PORT2_STATE = RED;  PORT2_RUN = false; mode2 = true;} 
			}
		}
		
	//==============================================================================
	//	��Ʈ 3�� ���� 
	//==============================================================================
		//	���� ��尡 Wait for ON �̶��,
		//==========================================================================
		if ( mode3 == true ) {
			if ( PORT3_STATE == RED ) {
				if ( count3 > 0 ) { if ( PORT3_RUN == true ) count3 --; }
				else if ( count3 == 0 && PORT3_RUN == true )
					{ PORT3_STATE = GREEN;  PORT3_RUN = false; mode3 = false;} 
			}
		//==========================================================================
		//	���� ��尡 Wait for OFF ���,
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
//	�ܼ�Ʈ �Ŀ� ����
//==============================================================================
void power_control() {
	char result = 0 ;
	if ( PORT1_STATE == GREEN ) result += 4;
	if ( PORT2_STATE == GREEN ) result += 8;
	if ( PORT3_STATE == GREEN ) result += 16;
	
	*(JP1_ptr) = (int)result;
}

//==============================================================================
//	����Ϳ� ��Ʈ ���
//==============================================================================
void print_all_port_to_monitor() {
	pixel_buffer_start = *(pixel_ctrl_ptr + 1);
	// back buffer ���� front buffer�� ���� �������� back buffer�� ��´�.
	clear_screen();
		
	//==========================================================================
	//	��Ʈ 1�� ǥ��
	//==========================================================================
	int ten = count1 / 10; 					// port1�� 10���ڸ�
	int one = count1 % 10; 					// port1�� 1���ڸ�
	
	monitor_count_bitmap(PORT1_TEN, NUMBER_y_axis, ten);
	monitor_count_bitmap(PORT1_ONE, NUMBER_y_axis, one);
	// port1�� Ÿ�̸Ӹ� ����Ϳ� ǥ��

	draw_square(PORT1_TEN, state_y_axis, PORT1_ONE + 20,
										state_y_axis + 35, PORT1_STATE);
											
	//==========================================================================
	//	��Ʈ 2�� ǥ��
	//==========================================================================
	ten = count2 / 10; 						// port2�� 10���ڸ�
	one = count2 % 10; 						// port2�� 1���ڸ�
	monitor_count_bitmap(PORT2_TEN, NUMBER_y_axis, ten);
	monitor_count_bitmap(PORT2_ONE, NUMBER_y_axis, one); 
	// port2�� Ÿ�̸Ӹ� ����Ϳ� ǥ��

	draw_square(PORT2_TEN, state_y_axis, PORT2_ONE + 20, 
										state_y_axis + 35, PORT2_STATE);
											
	//==========================================================================
	//	��Ʈ 3�� ǥ��
	//==========================================================================
	ten = count3 / 10; 						// port3�� 10���ڸ�
	one = count3 % 10; 						// port3�� 1���ڸ�
	monitor_count_bitmap(PORT3_TEN, NUMBER_y_axis, ten); 
	monitor_count_bitmap(PORT3_ONE, NUMBER_y_axis, one);
	// port3�� Ÿ�̸Ӹ� ����Ϳ� ǥ��
	
	draw_square(PORT3_TEN, state_y_axis, PORT3_ONE + 20, 
										state_y_axis + 35, PORT3_STATE);
	
	//==========================================================================
	// ĳ���� ���۸� ����� ����� �׸�
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
	//	��Ʈ 1���� ��� ǥ��
	//==========================================================================
	if ( mode1 == true ) {
		draw_square(PORT1_TEN, NUMBER_y_axis - 32,
						PORT1_TEN + 16, NUMBER_y_axis - 16, GREEN );
	} else {
		draw_square(PORT1_TEN, NUMBER_y_axis - 32,
						PORT1_TEN + 16, NUMBER_y_axis - 16, RED );
	}
	
	//==========================================================================
	//	��Ʈ 2���� ��� ǥ��
	//==========================================================================
	if ( mode2 == true ) {
		draw_square(PORT2_TEN, NUMBER_y_axis - 32,
						PORT2_TEN + 16, NUMBER_y_axis - 16, GREEN );
	} else {
		draw_square(PORT2_TEN, NUMBER_y_axis - 32,
						PORT2_TEN + 16, NUMBER_y_axis - 16, RED );
	}
	
	//==========================================================================
	//	��Ʈ 3���� ��� ǥ��
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
//	���ۿ� �ִ� �Լ�
//==============================================================================
void inputBuffer_write(char item) {
	if ( ( buffer_in + 1 ) % BUFFER_SIZE  == buffer_out )
		return;
	
	inputBuffer [ buffer_in ] = item;
	buffer_in = ( buffer_in + 1 ) % BUFFER_SIZE ;
	return;
}

//==============================================================================
//	����� ���۸� �о���� ģ��
//==============================================================================
char inputBuffer_read() {
	if ( buffer_in == buffer_out )
		return 0;
	
	int result = inputBuffer[buffer_out];
	buffer_out = (buffer_out + 1) % BUFFER_SIZE;
	return result;
}



//==============================================================================
//	Ÿ�̸� �ʱ�ȭ
//==============================================================================
void config_timer() {
	*(TIMER_ptr + 2) = time;
	*(TIMER_ptr + 3) = time >> 16;
	*(TIMER_ptr + 1) = 0b0111; //start�� continue�� ito�� 1�� ����
}

//==============================================================================
//	Ǫ�� ��ư �ʱ�ȭ
//==============================================================================
void config_KEYs(void) {
	*(KEY_ptr + 2) = 0b1111;
}

//==============================================================================
//	GPIO �ʱ�ȭ
//==============================================================================
void config_GPIO(void) {
	*(JP1_ptr + 1) = 0b11100;
	*(JP1_ptr + 2) = 0b00010;
	*(JP1_ptr + 2) = 0xFFFFFFFF;
	*(  JP1_ptr  ) = 0b0;
}

//==============================================================================
//	Nios II ���ͷ�Ʈ �ʱ�ȭ
//==============================================================================
void enable_nios2_interrupts() {
	NIOS2_WRITE_IENABLE(0b100000000011);
	NIOS2_WRITE_STATUS(1);
}

//==============================================================================
//	V sync ���ߴ� �Լ�
//==============================================================================
void wait_for_vsync() /*vsync ���߱�*/ {
	register int status;
	*pixel_ctrl_ptr = 1;

	status = *(pixel_ctrl_ptr + 3);
	while ((status & 0x01) != 0)
		status = *(pixel_ctrl_ptr + 3);
}

//==============================================================================
//	ȭ�鿡 �ؽ�Ʈ�� ����ϴ� �Լ�
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
//	��ġ�� �°� ������ִ� �Լ�
//==============================================================================
void texting(int x, int y, char *text_ptr) 
{
	VGA_text(x/4, y/4,text_ptr);
}

//==============================================================================
//	�͹̳� ���� ���
//==============================================================================
void Terminal_print (char *text_up,char *text_down ) 
{
	Terminal_clear ();
	VGA_text( ( PORT1_TEN+4 ) /4, ( state_y_axis + 57 ) / 4, text_up);
	VGA_text( ( PORT1_TEN+4 ) /4, ( state_y_axis + 63 ) / 4, text_down);
}

//==============================================================================
//	�͹̳� �����
//==============================================================================
void Terminal_clear () 
{
	VGA_text( ( PORT1_TEN+4 ) /4, ( state_y_axis + 57 ) / 4, erase);
	VGA_text( ( PORT1_TEN+4 ) /4, ( state_y_axis + 63 ) / 4, erase);
}


//==============================================================================
//	ȭ���� ����� �Լ�
//==============================================================================
void clear_screen() /* ȭ�� Ŭ���� �κ� */ {
	draw_square(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, 0xffff);
}
