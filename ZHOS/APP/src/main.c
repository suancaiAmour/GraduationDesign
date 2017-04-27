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

void Task3(void)
{
	while(1)
	{
		ZHLog("任务3\r\n");
		SwitchDelay(1000);
	}
}

void Task2(void)
{
	lw_oopc_report();
	CreateTask(Task3);
	TaskEnd();
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
	uart_init(115200);
	SwitchStart();
	CreateTask(Task0);
	CreateTask(Task1);
	SwitchDelay(0);
	while(1);
}
