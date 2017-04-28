#ifndef _ZHOS_H
#define _ZHOS_H

#include "lw_oopc.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define LOG_RATE        115200 // ������
#define MAX_TASK        20     // ���������
#define TASK_TACK_DEEP  100    // �����ջ�������� ��λ byte

#define MEM_MAX_SIZE	40*1024 // ϵͳ��֧�� 40K �ֽ� RAM

CLASS(OSClass, rootClass)
void (*log)(char *fmt, ...);
void (*createTask)(void (*task)(void));
void (*endTask)(void);
void (*switchTask)(void);
void (*delay)(uint16_t nTick);
END_CLASS

extern OSClass *pZHOS;

#define ZHOS (*pZHOS)

extern void ZHOSInit(void);

#endif
