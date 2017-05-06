#include "ZHOS.h"

void producer(void *args)
{
	ZHOSCond *cond = (ZHOSCond *)args;
	while (true)
	{
		ZHOS.condSignal(cond);
		ZHOS.log("����������һ�Ρ�\r\n");
		ZHOS.delay(10000);
	}
}

void consumer(void *args)
{
	int result;
	ZHOSCond *cond = (ZHOSCond *)args;
	while (true)
	{
		ZHOS.log("�����ߵȴ�������\r\n");
		result = ZHOS.condWait(cond, 50000);
		if(result)
		{
			ZHOS.log("��ʱ��\r\n");
		}
		else
		{
			ZHOS.log("����������һ�Ρ�\r\n");
		}
	}
}


int main(void)
{
	ZHOSCond *cond;
	ZHOSInit();
	cond = ZHOS.createCond(1);
	ZHOS.createTask(consumer, cond);
	ZHOS.createTask(producer, cond);
	ZHOS.switchTask();
	while(1);
}
