#include "stm32f10x.h"
#include "switch.h"
#include <string.h>
#include <stdbool.h>


TCB      TaskTCB[MAX_TASK + 1] = {0};
TCB      *TaskNew, *TaskRuning;
uint32_t IdleStack[20];
uint8_t  TaskCnt = 0;

void TaskIdle() {
	while(1){
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
	  bool bOneSwitch;
	  bOneSwitch = false;
	  for(i = 0; i < MAX_TASK; i++){
			if(TaskTCB[i].Delay != 0){
				TaskTCB[i].Delay--;			
      }
			if(bOneSwitch == false){
  			TaskCnt %= MAX_TASK;
				if(0 == TaskTCB[TaskCnt].Delay){
					bOneSwitch = true;
					TaskNew = &TaskTCB[TaskCnt++];
				}else{
					 TaskCnt++;
				}
		  }
    }
		if(bOneSwitch == false)
			TaskNew = &TaskTCB[IDLE_TASK];
		TaskSwitch();
}

void SwitchDelay(uint16_t nTick)
{
	  uint8_t i;
	  if(0 == nTick)
			return;
		
	  TaskRuning->Delay = nTick;             
	  for(i = 0; i < MAX_TASK; i++){
			TaskCnt %= MAX_TASK;	
			if(0 == TaskTCB[TaskCnt].Delay){
				TaskNew = &TaskTCB[TaskCnt++];
				break;
			}else TaskCnt++;
    }
		if(TaskRuning == TaskNew)
			TaskNew = &TaskTCB[IDLE_TASK];
	  TaskSwitch();
}


void SwitchTaskInt(void (*task)(void), OS_STK *ptos)
{
	  if(MAX_TASK + 1 <= TaskCnt){
			TaskCnt = 0;
			return;
    }
	  if(NULL == task)
			return;
		if(NULL == ptos)
			return;
                                                  
    *(ptos)    = (INT32U)0x01000000L;             
    *(--ptos)  = (INT32U)task;                     
		TaskTCB[TaskCnt]. pTaskStack =  ptos -14;
		TaskTCB[TaskCnt++]. Delay =  0;	
}


void SwitchStart(void)
{
	SystemInit(); 																 
	__set_PSP(0); 															
	SwitchTaskInt(TaskIdle, IdleStack+19);	 
	SysTick_Config((SystemCoreClock / N_TICK_IN_SECOND) - 1);  
}

