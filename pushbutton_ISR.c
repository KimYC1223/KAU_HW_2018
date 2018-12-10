//==============================================================================
//	헤더 파일 인클루드
//==============================================================================
#include "address_map_nios2.h"
#include "BufferConst.h"
#include "stdio.h"
#define BUFFER_SIZE 100				// 사용자 버퍼의 크기

//==============================================================================
//	전역 변수 선언 및 초기화
//==============================================================================
extern volatile int *KEY_ptr;
extern volatile int *SW_ptr;
extern int PORT1_STATE; 			//포트 1의 초기상태_RED
extern int PORT2_STATE; 			//포트 2의 초기상태_RED
extern int PORT3_STATE; 			//포트 3의 초기상태_RED
extern int count1;
extern int count2;
extern int count3;
extern int RED;
extern int GREEN;

extern volatile char inputBuffer[BUFFER_SIZE];
extern volatile int buffer_in;
extern volatile int buffer_out;
extern void inputBuffer_write(char item);
extern char inputBuffer_read();

//==============================================================================
//
//	푸쉬 버튼 인터럽트 핸들러
//
//==============================================================================
void pushbutton_ISR(void)
{
	//==========================================================================
	//	푸쉬 버튼 값 가져오기 & 초기화
	//==========================================================================
	int press = *(KEY_ptr + 3);
	*(KEY_ptr + 3) = press;
	int reset_flag;
	
	//==========================================================================
	//	값에 따라 핸들링 하는 부분
	//==========================================================================
	if ((press & 0b01) != 0) 
		inputBuffer_write(KEY_0);
	else if ((press & 0b10) != 0)
		inputBuffer_write(KEY_1);
	else if ((press & 0b100) != 0)
		inputBuffer_write(KEY_2);
	else if ((press & 0b100) != 0)
		inputBuffer_write(KEY_3);
}
