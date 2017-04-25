#include "../h/main.h"


//extern Task *task;
//int main(void)
//{

//	task = new Task();
//	
//	Init();
//	Timerx_Init(20,7199);
//	TIM3->CNT = 0x01;
//	__asm
//	{
//		bl Task0
//	}
////	while (true);
////	delete task;
//}
//int temp = 0;
//void Task0(void)
//{
//	while (true)
//	{
//		LED0 = !LED0;
//	}
//}

//void Task1(void)
//{
//	while (true)
//	{
//		LED1 = !LED1;
//	}
//}

u32 TaskStack[2][40];

void Task0(void);
void Task1(void);

int main(void)
{
	uart_init(115200);
	SwitchTaskInt(Task0, &TaskStack[0][39]);
	SwitchTaskInt(Task1, &TaskStack[1][39]);
	SwitchStart();
	
	printf("张鸿\r\n");
	while(1);
}


void Task0(void)
{
   u8 i;
   for(;;)
   {
	   printf("任务1\r\n");
	 SwitchDelay(2000);

   }
}
void Task1(void)
{
   u8 i;
   for(;;)
   {
	   printf("任务2\r\n");
	 SwitchDelay(3000);
	 i = i;
   }
}
