#ifndef __TIMER_H
#define __TIMER_H

#define Task_Count 2//任务数量
extern "C"
{
#include "sys.h"
#include "led.h"

void Timerx_Init(u16 arr, u16 psc); //不用C方式编译会出未定义的问题
void IRQHandler(void);

}

#include "main.h"

class Task;

class Task
{
public:Task(void)
	{
		Reg_Buff[0][6]=((unsigned int)&Task0) ;
		Reg_Buff[1][6]=((unsigned int)&Task1) ;
		Reg_Buff[0][6]=Reg_Buff[1][7]=0x61000000 ;

		Current = 0;
		Next = 1;
	}
	public: static const unsigned char Count = Task_Count;
	public:	unsigned char Current; //当前任务
	public:	unsigned char Next ;	//下一个任务
	public: volatile unsigned int Reg_Buff[Task_Count][8];
	public:	void	Start_Next() //更新至下一个任务
	{
		(Current + 1 < Count) ? Current++ : Current = 0;
		(Next + 1 < Count) ? Next++ : Next = 0;

//		if (Next != 0 && (Next - Current) != 1)
//		{
//			while (true)
//			{

//			}
//		}
	}

};

#endif
