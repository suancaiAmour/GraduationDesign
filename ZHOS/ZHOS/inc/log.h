#ifndef __LOG_H
#define __LOG_H	
#include "sys.h" 

void uart_init(u32 bound);

// ע�ⷢ�͵����ݲ��ܳ��� 100 byte
void ZHLog(char *fmt, ...);

#endif


