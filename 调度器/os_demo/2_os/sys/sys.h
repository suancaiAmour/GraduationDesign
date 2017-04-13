#ifndef __SYS_H
#define __SYS_H

#include "define.h"
#include "nstd_type.h"

#include "stm32f10x.h"	/*!< 官方库的头文件 */
#include "stm32f10x__cpplib.h"	/*!< c++ 库的头文件 */

class System
{
	u32 sysclk;
public:
	u32 GetSysclk()	{return sysclk;}
	void Init();
	void SoftReset()	{scb.SoftReset();}
};
extern System sys;

void Sys_Standby(void);         //待机模式 	

void delay_init(u8 SYSCLK);
#define Delay_Init	delay_init
void delay_ms(u16 nms);
#define Delay_ms	delay_ms	
void delay_us(u32 Nus);
#define Delay_us	delay_us


#endif
