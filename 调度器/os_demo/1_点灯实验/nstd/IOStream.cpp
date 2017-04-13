#include "IOStream.h"
#include <new>

//	<<< Use Configuration Wizard in Context Menu >>> 
//	<e>ʹ��input/output�ı����������
//	<i>��ʹ��������Ļ����޷�ʹ���������ˡ�
#define	OUTPUT_EN				1

//	    <e>ʹ��input�ı�������
//  	<i>ʹ��input�ı���������ռ��һ�����ڴ棬
//	    <i>ע��input�������Ļ�������Ҫ�������ģ��һ��ʹ��
//	    <i>������ﲻʹ��input����������ô����ģ����޷�ʹ�����뻺����
#define INPUT_EN				1
//	    <o>�ı����������ջ�������С
#define TEXT_IN_BUFF_ISZE		64

//	    </e>

//	</e>

//	<<< end of configuration section >>>
	
namespace nstd{
	
	#if (OUTPUT_EN != 0)
	/*************************�����ǻ��ڴ��ڵ��������ʵ��************************/
	TextOutStream output;
	/*************************�����ǻ��ڴ��ڵ��������ʵ��************************/
	#endif

	#if(INPUT_EN != 0) 
	/*************************�����ǻ��ڴ��ڵ���������ʵ��************************/
	char TextInBuffer[TEXT_IN_BUFF_ISZE];
	 FIFO<char> textInFIFO;
	TextInStream input;
	/*************************�����ǻ��ڴ��ڵ���������ʵ��************************/
	#endif
	
	void IOStream_Init(){
		#if (OUTPUT_EN != 0)
        /* ��������������õ�ʱ�� C/C++ ����ʱ�⻹û�г�ʼ����������Ҫ�ֶ�����
        placement new ������ vptr �ĳ�ʼ�� */
        new (&output) TextOutStream();
		#endif
		#if(INPUT_EN != 0) 
        /* ��������������õ�ʱ�� C/C++ ����ʱ�⻹û�г�ʼ����������Ҫ�ֶ�����
        placement new ������ vptr �ĳ�ʼ�� */
        new (&textInFIFO) FIFO<char>();
        new (&input) TextInStream();
		textInFIFO.Init( TextInBuffer, TEXT_IN_BUFF_ISZE);
		input.Init(textInFIFO);
		#endif
	}
	
     extern "C" int putchar(int);
     int TextOutStream::SendChar(char ch){
        return putchar(ch);
    }
    
    TextOutStream& TextOutStream::operator <<(const void* const p)
    {
		long l = (long)p;
		for(int i=sizeof(l)*2; i; ){
			*this<< NumToASCII((l>>(4*(--i)))&0x0F);
		}
		return *this;
	}
    
    TextOutStream& TextOutStream::operator <<(u32 number)
	{
		u32 temp;
		u8 count;	//������¼λ��
		u8 i;
		u8 j;	//ѭ������

		//����ʮ������Ҫ��λ��
		temp = number;
		count = 0;
		while(temp = temp/10, temp)
			count++;		//������count = λ�� - 1
		
		for(i = count; i > 0; i--) {
			temp = number;
			for(j = i ; j > 0; j--)
				temp = temp/10;
			temp = temp%10;
			temp = temp + '0';
			*this<<(char)(temp);		
		}
		temp = number%10;  //���͸�λ
		temp = temp + '0';
		*this<<(char)(temp);
		return *this;
	}
    
    TextOutStream& TextOutStream::operator <<(const char* str)
    {
		while(*str){
			SendChar(*str);
			str++;
		}
		return *this;
	}
}

/** @brief Ϊ������ֲʱ��ֱ�Ӳ�������, ����һ��������.
  * ��Ȼ, �����Ҫʹ�� output ����, ����Ҫ���Լ�ʵ��
  * sendchar ������.
  */
extern "C"
#ifndef _MSC_VER
__attribute((weak))
#endif
int sendchar(int ch){
	extern int putchar(int);
    putchar(ch);
    return ch;
}
