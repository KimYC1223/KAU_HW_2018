//==============================================================================
//	헤더 파일 인클루드
//==============================================================================
#include "address_map_nios2.h"
#include "BufferConst.h"
#include <stdio.h>
#define BUFFER_SIZE 100				// 사용자 버퍼의 크기
#define true 1
#define false 0

//==============================================================================
//	전역 변수 선언 및 초기화 & 함수 조기 선언
//==============================================================================
extern volatile int *JP1_ptr;
extern volatile char inputBuffer[BUFFER_SIZE];
extern volatile int buffer_in;
extern volatile int buffer_out;
extern void inputBuffer_write(char item);
extern char inputBuffer_read();

extern volatile int ControllerState;
extern volatile char Message;
int isZeroMessage = true;

//==============================================================================
//	타이머 핸들러
//==============================================================================
void GPIO_ISR(void) {
	//==========================================================================
	//	인터럽트 초기화
	//==========================================================================
	int input = *(JP1_ptr + 3);
	*(JP1_ptr + 3) =  0xFFFFFFFF;

	//==========================================================================
	//	사용자 컨트롤러 프리엠블 확인
	//==========================================================================
	if ( ControllerState == 0 ) {
		if ( *(JP1_ptr) & 0b1 != 0 ) ControllerState ++;
		else ControllerState = 0;
		
	} else if ( ControllerState == 1 ) {
		if ( *(JP1_ptr) & 0b1 != 0 ) ControllerState = 0;
		else 
			if ( isZeroMessage ) isZeroMessage = false;
			else { ControllerState ++; isZeroMessage = true; }
	
	} else if ( ControllerState == 2 ) {
		if ( *(JP1_ptr) & 0b1 != 0 ) ControllerState ++; 
		else ControllerState = 0;
	}
	
	//==========================================================================
	//	사용자 컨트롤러 값 받아오기
	//==========================================================================
	//	4번째 비트
	//==========================================================================
	else if ( ControllerState == 3 ) {
		if ( *(JP1_ptr) & 0b1 != 0 ) { Message += 8; ControllerState ++; }
		else 
			if ( isZeroMessage ) isZeroMessage = false;
			else { ControllerState ++; isZeroMessage = true; }
	}
	
	//==========================================================================
	//	5번째 비트
	//==========================================================================
	else if ( ControllerState == 4 ) {
		if ( *(JP1_ptr) & 0b1 != 0 ) { Message += 4; ControllerState ++; }
		else 
			if ( isZeroMessage ) isZeroMessage = false;
			else { ControllerState ++; isZeroMessage = true; }
	}
	
	//==========================================================================
	//	6번째 비트
	//==========================================================================
	else if ( ControllerState == 5 ) {
		if ( *(JP1_ptr) & 0b1 != 0 ) { Message += 2; ControllerState ++; }
		else 
			if ( isZeroMessage ) isZeroMessage = false;
			else { ControllerState ++; isZeroMessage = true; }
	}
	
	//==========================================================================
	//	7번째 비트
	//==========================================================================
	else if ( ControllerState == 6 ) {
		if ( *(JP1_ptr) & 0b1 != 0 ) { Message += 1; ControllerState ++; }
		else 
			if ( isZeroMessage ) isZeroMessage = false;
			else { ControllerState ++; isZeroMessage = true; }
	}
	
	//==========================================================================
	//	마지막
	//==========================================================================
	else if ( ControllerState == 7 ) {
		//printf("I've got a Message ! : %d \n",Message);
		inputBuffer_write(Message+1);
		ControllerState = 0;
		Message = 0;
	}
	
} 

