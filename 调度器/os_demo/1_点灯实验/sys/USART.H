#ifndef __USART_H
#define __USART_H

#include "stm32f10x__cpplib.h"
	  	
void USART1_Init(u32 baudRate, u32 pclk = 72000000);

class USART1_SEND_CHAR
{
public:
	static void Send(char ch)
	{
		while(! (usart1.IsTransmitDataRegisterEmpty())); 
		usart1.Write(ch);
		return; 
	}
};

#define SEND_CHAR_CLASS USART1_SEND_CHAR

#endif
