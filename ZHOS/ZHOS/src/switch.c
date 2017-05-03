#include "stm32f10x.h"
#include "switch.h"
#include <string.h>
#include <stdbool.h>
#include "queue.h"
#include "memory.h"
#include "log.h"

#include "ZHOS.h"

#define N_TICK_IN_SECOND  1000

typedef uint32_t OS_STK;
typedef uint32_t INT32U;

typedef struct TCB
{
  uint32_t *pTaskStack;
  uint16_t Delay;
  uint32_t *freeAddr;
}TCB;

static ZHQueue *TCBQueue;
static unsigned int taskCount;
TCB  *TaskNew, *TaskRuning, *IdleTask;

void TaskIdle() {
	while(1)
	{
		__ASM("WFE");
		SwitchDelay(0);
	}
}

__ASM void TaskSwitch(void)
{
    LDR     R0, =0xE000ED22
    LDR     R1, =0xFF
    STRB    R1, [R0]
    LDR     R0, =0xE000ED04               
    LDR     R1, =0x10000000
    STR     R1, [R0]
    BX      LR
	  ALIGN
}


__ASM void PendSV_Handler(void)
{
		IMPORT  TaskRuning
		IMPORT  TaskNew
	
    CPSID   I                                                   
    MRS     R0, PSP                                             
    CBZ     R0, NoSave                           

    SUBS    R0, R0, #0x20                                       
    STM     R0, {R4-R11}

    LDR     R1, =TaskRuning                                     
    LDR     R1, [R1]
    STR     R0, [R1]                                            
                                                              
NoSave

    LDR     R0, =TaskRuning                                     
    LDR     R1, =TaskNew
    LDR     R2, [R1]
    STR     R2, [R0]

    LDR     R0, [R2]                                            
    LDM     R0, {R4-R11}                                        
    ADDS    R0, R0, #0x20
    MSR     PSP, R0                                             
    ORR     LR, LR, #0x04                                       
    CPSIE   I
    BX      LR                                                 
    ALIGN
}

void  SysTick_Handler (void)
{   
	uint8_t i;
	TCB *task;
	for(i = 0; i < taskCount; i++)
	{
		task = TCBQueue->getConWithIndex(TCBQueue, i);
		if(task->Delay != 0)
		{
			task->Delay--;
		}
    }
}

void SwitchDelay(uint16_t nTick)
{
     uint8_t i;
	 TCB *task;
	 TaskRuning->Delay = nTick;	 
	 for(i = 0; i < taskCount; i++)
	 {
		task = TCBQueue->getConWithIndex(TCBQueue, i);
		if(task->Delay == 0)
		{
			if(task == TaskRuning)
			{
				continue;
			}
			else
			{
				TaskNew = task;
				break;
			}
		}
	 }
	 
	 if(task == TaskRuning)
	 {
		 if (task->Delay != 0)
		 {
			 TaskNew = IdleTask;
		 }
		 else return;
	 }
	 TaskSwitch();
}


void CreateTask(void (*task)(void))
{
	OS_STK *ptos; // �����ջ
	TCB *newTask; // �´�����������
	if(NULL == task)
		return;
		
	ptos = (OS_STK *)mymalloc(sizeof(OS_STK) * TASK_TACK_DEEP);
	if(NULL == ptos)
	{
		ZHLog("����ջ����ʧ�ܡ�\r\n");
		return;
	}
	newTask = (TCB *)mymalloc(sizeof(TCB));
	newTask->freeAddr = ptos;
    ptos += (TASK_TACK_DEEP - 1);                                          
    *(ptos)    = (INT32U)0x01000000L;             
    *(--ptos)  = (INT32U)task;
	newTask->pTaskStack = ptos - 14;
	newTask->Delay = 0;
	// ѹ���������
	TCBQueue->enQueue(TCBQueue, (void *)newTask);
	taskCount = TCBQueue->count;
}

void TaskEnd(void)
{
	TCB *task;
	uint8_t i;
	for(i = 0; i < taskCount; i++)
	{
		task = TCBQueue->deQueue(TCBQueue);
		if(task == TaskRuning)
		{
			myfree(task->freeAddr);
			myfree(task);
			TaskNew = TCBQueue->deQueue(TCBQueue);
			TCBQueue->enQueue(TCBQueue, TaskNew);
			TaskSwitch();
			return;
		}
		TCBQueue->enQueue(TCBQueue, task);
    }
}

void SwitchStart(void)
{
	TCBQueue = ZHQueue_new("ϵͳ�������");
	TCBQueue->init(TCBQueue, MAX_TASK); // �������ֻ�ܴ� MAX_TASK ������ 																 
	__set_PSP(0); 															
	CreateTask(TaskIdle);
	IdleTask = (TCB *)TCBQueue->deQueue(TCBQueue);
	TCBQueue->enQueue(TCBQueue, (void *)IdleTask);
	SysTick_Config((SystemCoreClock / N_TICK_IN_SECOND) - 1);
}

