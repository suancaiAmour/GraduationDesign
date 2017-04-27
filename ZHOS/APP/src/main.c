#include "log.h"
#include "switch.h"
#include "sys.h"
#include "queue.h"

#include "lw_oopc.h"

void Task0(void)
{
   while(1)
   {
	 ZHLog("任务0\r\n");
	 SwitchDelay(2000);
   }
}

void Task2(void)
{
	while(1)
	{
		lw_oopc_report();
		SwitchDelay(10000);
		TaskEnd();
	}
}

void Task1(void)
{
   CreateTask(Task2);
   while(1)
   {
	 ZHLog("任务1\r\n");
	 SwitchDelay(5000);
   }
}


int main(void)
{
	NVIC_Configuration();
	uart_init(9600);
	SwitchStart();
	CreateTask(Task0);
	CreateTask(Task1);
	SwitchDelay(0);
	while(1);
}
