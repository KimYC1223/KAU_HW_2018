//==============================================================================
//	��� ���� ��Ŭ���
//==============================================================================
#include "address_map_nios2.h"
#include "BufferConst.h"
#include "stdio.h"
#define BUFFER_SIZE 100				// ����� ������ ũ��

//==============================================================================
//	���� ���� ���� �� �ʱ�ȭ
//==============================================================================
extern volatile int *KEY_ptr;
extern volatile int *SW_ptr;
extern int PORT1_STATE; 			//��Ʈ 1�� �ʱ����_RED
extern int PORT2_STATE; 			//��Ʈ 2�� �ʱ����_RED
extern int PORT3_STATE; 			//��Ʈ 3�� �ʱ����_RED
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
//	Ǫ�� ��ư ���ͷ�Ʈ �ڵ鷯
//
//==============================================================================
void pushbutton_ISR(void)
{
	//==========================================================================
	//	Ǫ�� ��ư �� �������� & �ʱ�ȭ
	//==========================================================================
	int press = *(KEY_ptr + 3);
	*(KEY_ptr + 3) = press;
	int reset_flag;
	
	//==========================================================================
	//	���� ���� �ڵ鸵 �ϴ� �κ�
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
