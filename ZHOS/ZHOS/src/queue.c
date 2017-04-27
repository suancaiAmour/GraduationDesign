#include "queue.h"
#include "log.h"
#include "memory.h"
#include <stddef.h>

void queueInit(ZHQueue *cthis, int maxCount)
{
	cthis->cache = (void **)mymalloc(sizeof(void *) * maxCount);
	if(cthis->cache == NULL)
	{
		ZHLog("��������ʧ�ܡ�\r\n");
		cthis->maxCount = 0;
	}
	else
	{
		cthis->maxCount = maxCount;
	}
}

void enQueue(ZHQueue *cthis, void *con)
{
	int rear;
	if (cthis->count == cthis->maxCount)
	{
		ZHLog("���������\r\n");
	}
	else
	{
		rear = (cthis->front + cthis->count) % cthis->maxCount;
		rear = (rear + 1) % cthis->maxCount;
		cthis->cache[rear] = con;
		cthis->count++;
	}
}

void *deQueue(ZHQueue *cthis)
{
	if(cthis->count == 0)
	{
		ZHLog("����Ϊ��,��ջΪ�ա�\r\n");
		return NULL;
	}
	else
	{
		cthis->front = (cthis->front + 1) % cthis->maxCount;
		cthis->count--;
		return cthis->cache[cthis->front];
	}
}

void *getConWithIndex(ZHQueue *cthis, unsigned int index)
{
	unsigned int conIndex;
	if(cthis->count == 0)
	{
		ZHLog("����Ϊ��,��ȡ����ʧ�ܡ�\r\n");
		return NULL;
	}
	else
	{
		conIndex = (cthis->front + index + 1) % cthis->maxCount;
		return cthis->cache[conIndex];
	}
}

CTOR(ZHQueue, rootClass)
FUNCTION_SETTING(init, queueInit);
FUNCTION_SETTING(enQueue, enQueue);
FUNCTION_SETTING(deQueue, deQueue);
FUNCTION_SETTING(getConWithIndex, getConWithIndex);
END_CTOR
