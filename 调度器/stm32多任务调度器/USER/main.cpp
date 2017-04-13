#include "../h/main.h"

extern Task *task;
int main(void)
{

	task = new Task();
	
	Init();
	Timerx_Init(20,7199);
	TIM3->CNT = 0x01;
	__asm
	{
		bl Task0
	}
//	while (true);
//	delete task;
}
int temp = 0;
void Task0(void)
{
	while (true)
	{
		LED0 = !LED0;
	}
}

void Task1(void)
{
	while (true)
	{
		LED1 = !LED1;
	}
}

