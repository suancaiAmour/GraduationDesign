#include "ZHOS.h"
#include "log.h"
#include "sys.h"
#include "switch.h"
#include "string.h"

OSClass *pZHOS;

void ZHOSInit(void)
{
	NVIC_Configuration();
	uart_init(LOG_RATE);
	pZHOS = OSClass_new("ϵͳ�ں�");
	SwitchStart();
	pZHOS->log("ZHOS��ʼ���ɹ�\r\n");
}

void SwitchTask(void)
{
	SwitchDelay(0);
}

CTOR(OSClass, rootClass)
FUNCTION_SETTING(log, ZHLog);
FUNCTION_SETTING(createTask, CreateTask);
FUNCTION_SETTING(createTaskWithTackDeep, CreateTaskWithTackDeep);
FUNCTION_SETTING(endTask, TaskEnd);
FUNCTION_SETTING(switchTask, SwitchTask);
FUNCTION_SETTING(delay, SwitchDelay);
FUNCTION_SETTING(getOSTime, getOSTickTime);

FUNCTION_SETTING(createCond, createCond);
FUNCTION_SETTING(releaseCond, releaseCond);
FUNCTION_SETTING(condSignal, condSignal);
FUNCTION_SETTING(condWait, condWait);
END_CTOR
