#include "sys.h"
#include "usart.h"
#include "IOStream.h"
#include <stdio.h> 
#include "debug.h"

//	<<< Use Configuration Wizard in Context Menu >>> 
//	<e>开启串口接收
//	<i>开启/关闭串口接收功能
#define USART_RECEIVE_EN	1
//	</e>
//	<<< end of configuration section >>>

#if(USART_RECEIVE_EN!=0)
ARMAPI void USART1_IRQHandler(void)
{
	using namespace periph;
	using namespace nstd;
	if(usart1.ReceivedData())	{
		//接收到数据
		textInFIFO<<(char)usart1.Read();
	}
}
#endif

void USART1_Init(u32 baudRate, u32 pclk)
{	using namespace periph;

	nvic.Config(USART1_IRQn, ENABLE, 0, 0);

	rcc.ClockCmd(&usart1, ENABLE);								// 使能串口

	USART_BaseConfig base;
	base.baudRate = baudRate;
	
	usart1.Config(base, pclk);
	
	usart1.TransmitterCmd(ENABLE);		   						// 使能串口发送
	/* 配置完串口后再使能和配置 IO，以免使能 PA9 为 AF输出后不稳定 */
	rcc.ClockCmd(&gpioa, ENABLE);
	pa9.Config(AF_OUTPUT, PUSH_PULL);							// 串口输出脚配置
	pa10.Config(INPUT);											// 串口输入
#if(USART_RECEIVE_EN != 0)
	usart1.ReceiverCmd(ENABLE);									// 使能串口接收
	usart1.InterruptCmd(ReceivedInt_OverRunErrInt, ENABLE);		// 使能接收中断
#endif
}

extern "C" 
int sendchar(int ch){
	if(ch=='\n'){
		while(! (usart1.IsTransmitDataRegisterEmpty())); 
			usart1.Write('\r'); 
	}
	while(! (usart1.IsTransmitDataRegisterEmpty())); 
	usart1.Write(ch); 
	return ch;
}

extern "C"
int getkey(void){
	char r;
	while(textInFIFO.IsEmpty());
	textInFIFO>>r;
	return r;
}

//////////////////////////////////////////////////////////////////
// Retargeting cout and printf
#if 1

#include <stdio.h>
#include <stdlib.h>

#pragma import(__use_no_semihosting_swi)

namespace std{
    
    struct __FILE
    {
        int handle;

        /* Whatever you require here. If the only file you are using is */
        /* standard output using printf() for debugging, no file handling */
        /* is required. */
    };

    FILE __stdout;
    FILE __stdin;
    FILE __stderr;
    
    // You might also have to re-implement fopen() and related functions 
    // if you define your own version of __FILE
    // 有兴趣的可以定义自己的 __FILE，实现自己的 fopen，fwrite, fread
	// 因为库里面的 fopen 是弱符号，你的定义可以覆盖库的定义
	FILE *fopen(const char * __restrict /*filename*/,
                           const char * __restrict /*mode*/)
    {
        usart1<<"\n\r fopen. \n\r";
        return NULL;
    }

    //-----------------------------------------------------------
    int fputc(int ch, std::FILE *f) {
        return sendchar(ch);
    }
    
    //-----------------------------------------------------------
    int fgetc(FILE *f) {
        /* Your implementation of fgetc(). */
        return sendchar(getkey());
    }
    
    /*
	检查是流否有错误，如果没有错误，返回 0
    */
    int ferror(FILE *stream)
    {
        /* Your implementation of ferror(). */
        return 0;
    }

    long int ftell(FILE *stream){
        /* Your implementation of ftell(). */
        usart1<<"ftell\n\r";
        return 0;
    }
    
    int fclose(FILE *f){
        /* Your implementation of fclose(). */
        usart1<<"\n\r fclose \n\r";
        return 0;
    }
    
    int fseek(FILE *f, long nPos, int nMode){
        /* Your implementation of fseek(). */
        usart1<<"fseek\n\r";
        return 0;
    }
    
    /*
	对于输出流，把缓冲区中的内容全部发送出去
	这里用的是自己定义的 FILE 结构体，没有缓冲区，
	所以什么都不做就行了。
    */
    int fflush(FILE *f){
        /* Your implementation of fflush(). */
        return 0;
    }

    
    /*
默认的 _sys_exit 使用了 semihosted calls
	所以必须重新实现它。
	declared in <rt_sys.h>    */
    ARMAPI void  _sys_exit(int) {
        /* declared in <stdlib.h> */
        abort();
        while(1);
    }

    /*
	这个函数是C/C++标准库用来打印必要的调试信息的
	最好重新实现它，把调试信息发送到串口。
	this function is declared in <rt_sys.h>    */
    ARMAPI void _ttywrch(int ch) {
        sendchar(ch); 
        return ; 
    }
    
}

#endif 
//end
//////////////////////////////////////////////////////////////////

