#include "sys.h"
#include "usart.h"
#include "IOStream.h"
#include <stdio.h> 
#include "debug.h"

//	<<< Use Configuration Wizard in Context Menu >>> 
//	<e>�������ڽ���
//	<i>����/�رմ��ڽ��չ���
#define USART_RECEIVE_EN	1
//	</e>
//	<<< end of configuration section >>>

#if(USART_RECEIVE_EN!=0)
ARMAPI void USART1_IRQHandler(void)
{
	using namespace periph;
	using namespace nstd;
	if(usart1.ReceivedData())	{
		//���յ�����
		textInFIFO<<(char)usart1.Read();
	}
}
#endif

void USART1_Init(u32 baudRate, u32 pclk)
{	using namespace periph;

	nvic.Config(USART1_IRQn, ENABLE, 0, 0);

	rcc.ClockCmd(&usart1, ENABLE);								// ʹ�ܴ���

	USART_BaseConfig base;
	base.baudRate = baudRate;
	
	usart1.Config(base, pclk);
	
	usart1.TransmitterCmd(ENABLE);		   						// ʹ�ܴ��ڷ���
	/* �����괮�ں���ʹ�ܺ����� IO������ʹ�� PA9 Ϊ AF������ȶ� */
	rcc.ClockCmd(&gpioa, ENABLE);
	pa9.Config(AF_OUTPUT, PUSH_PULL);							// �������������
	pa10.Config(INPUT);											// ��������
#if(USART_RECEIVE_EN != 0)
	usart1.ReceiverCmd(ENABLE);									// ʹ�ܴ��ڽ���
	usart1.InterruptCmd(ReceivedInt_OverRunErrInt, ENABLE);		// ʹ�ܽ����ж�
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
    // ����Ȥ�Ŀ��Զ����Լ��� __FILE��ʵ���Լ��� fopen��fwrite, fread
	// ��Ϊ������� fopen �������ţ���Ķ�����Ը��ǿ�Ķ���
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
	����������д������û�д��󣬷��� 0
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
	������������ѻ������е�����ȫ�����ͳ�ȥ
	�����õ����Լ������ FILE �ṹ�壬û�л�������
	����ʲô�����������ˡ�
    */
    int fflush(FILE *f){
        /* Your implementation of fflush(). */
        return 0;
    }

    
    /*
Ĭ�ϵ� _sys_exit ʹ���� semihosted calls
	���Ա�������ʵ������
	declared in <rt_sys.h>    */
    ARMAPI void  _sys_exit(int) {
        /* declared in <stdlib.h> */
        abort();
        while(1);
    }

    /*
	���������C/C++��׼��������ӡ��Ҫ�ĵ�����Ϣ��
	�������ʵ�������ѵ�����Ϣ���͵����ڡ�
	this function is declared in <rt_sys.h>    */
    ARMAPI void _ttywrch(int ch) {
        sendchar(ch); 
        return ; 
    }
    
}

#endif 
//end
//////////////////////////////////////////////////////////////////

