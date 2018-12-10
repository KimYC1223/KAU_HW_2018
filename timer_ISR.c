//==============================================================================
//	헤더 파일 인클루드
//==============================================================================
#include "address_map_nios2.h"

//==============================================================================
//	전역 변수 선언 및 초기화 & 함수 조기 선언
//==============================================================================
extern volatile int *TIMER_ptr;
extern void timer_control();

//==============================================================================
//	타이머 핸들러
//==============================================================================
void timer_ISR(void) {
	*TIMER_ptr = 0;
	timer_control();
}

