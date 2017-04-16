#ifndef __SWITCH_H__
#define __SWITCH_H__

#include "stdint.h"
#define MAX_TASK          2
#define N_TICK_IN_SECOND  1000


#define IDLE_TASK         MAX_TASK
typedef uint32_t OS_STK;
typedef uint32_t INT32U;

typedef struct TCB
{
  uint32_t *pTaskStack;
	uint16_t Delay;
}TCB;


extern  TCB *TaskRuning;
extern  TCB *TaskNew;
extern  TCB  TCBTask[MAX_TASK];

void    SwitchTaskInt(void (*task)(void), OS_STK *ptos);
void    SwitchDelay(uint16_t Time);
void    SwitchStart(void);

#endif


