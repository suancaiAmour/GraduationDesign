#ifndef __INIT_H
#define __INIT_H	 
extern "C"
{
	#include "../h/led.h"
	#include "../h/delay.h"
	#include "../h/sys.h"
	#include "../h/key.h"
	#include "../h/usart.h"

	#include "../h/stm32f10x.h"
	#include "../h/misc.h"
	#include "../h/timer.h"

}


#define  HEX__(n)  0x##n##UL
#define  B8__(x)   ( ((x) & 0x0000000FUL) ? 0x01:0 )\
									|( ((x) & 0x000000F0UL) ? 0x02:0 )\
									|( ((x) & 0x00000F00UL) ? 0x04:0 )\
									|( ((x) & 0x0000F000UL) ? 0x08:0 )\
									|( ((x) & 0x000F0000UL) ? 0x10:0 )\
									|( ((x) & 0x00F00000UL) ? 0x20:0 )\
									|( ((x) & 0x0F000000UL) ? 0x40:0 )\
									|( ((x) & 0xF0000000UL) ? 0x80:0 )
#define  b8(x)                      ((unsigned char)B8__(HEX__(x)))
#define  b16(x_msb,x_lsb)           (((unsigned int)B8(x_msb)<<8) + B8(x_lsb))
#define  b32(x_msb,x_2,x_3,x_lsb)   (((unsigned long)B8(x_msb)<<24) + ((unsigned long)B8(x_2)<<16) + ((unsigned long)B8(x_3)<<8) + B8(x_lsb))

/*
	a = B8(10011001)    ¼´ a=0x99
	a = B16(10011001,10011001)  ¼´ a=0x9999
	a = B32(10011001,10011001,10011001,10011001)  ¼´ a=0x99999999
*/
 
 
 
 
void Init(void);
void Timer_Configuration(void);
void NVIC_Config(void);
void TIM2_GPIO_Config(void);
#endif
