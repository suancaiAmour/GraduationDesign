#include "IOStream.h"
#include <new>

//	<<< Use Configuration Wizard in Context Menu >>> 
//	<e>使用input/output文本输入输出流
//	<i>不使用输出流的话就无法使用输入流了。
#define	OUTPUT_EN				1

//	    <e>使用input文本输入流
//  	<i>使用input文本输入流会占用一定的内存，
//	    <i>注意input输入流的缓冲区需要配合其它模块一起使用
//	    <i>如果这里不使用input输入流，那么其它模块就无法使用输入缓冲区
#define INPUT_EN				1
//	    <o>文本输入流接收缓冲区大小
#define TEXT_IN_BUFF_ISZE		64

//	    </e>

//	</e>

//	<<< end of configuration section >>>
	
namespace nstd{
	
	#if (OUTPUT_EN != 0)
	/*************************以下是基于串口的输出流的实现************************/
	TextOutStream output;
	/*************************以上是基于串口的输出流的实现************************/
	#endif

	#if(INPUT_EN != 0) 
	/*************************以下是基于串口的输入流的实现************************/
	char TextInBuffer[TEXT_IN_BUFF_ISZE];
	 FIFO<char> textInFIFO;
	TextInStream input;
	/*************************以上是基于串口的输入流的实现************************/
	#endif
	
	void IOStream_Init(){
		#if (OUTPUT_EN != 0)
        /* 由于这个函数调用的时候 C/C++ 运行时库还没有初始化，所以需要手动调用
        placement new 来进行 vptr 的初始化 */
        new (&output) TextOutStream();
		#endif
		#if(INPUT_EN != 0) 
        /* 由于这个函数调用的时候 C/C++ 运行时库还没有初始化，所以需要手动调用
        placement new 来进行 vptr 的初始化 */
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
		u8 count;	//用来记录位数
		u8 i;
		u8 j;	//循环变量

		//计算十进制需要的位数
		temp = number;
		count = 0;
		while(temp = temp/10, temp)
			count++;		//跳出后count = 位数 - 1
		
		for(i = count; i > 0; i--) {
			temp = number;
			for(j = i ; j > 0; j--)
				temp = temp/10;
			temp = temp%10;
			temp = temp + '0';
			*this<<(char)(temp);		
		}
		temp = number%10;  //发送个位
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

/** @brief 为了在移植时不直接产生错误, 定义一个弱符号.
  * 当然, 如果需要使用 output 对象, 就需要有自己实现
  * sendchar 函数了.
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
