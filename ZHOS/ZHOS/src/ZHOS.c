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
FUNCTION_SETTING(endTask, TaskEnd);
FUNCTION_SETTING(switchTask, SwitchTask);
FUNCTION_SETTING(delay, SwitchDelay);
END_CTOR
