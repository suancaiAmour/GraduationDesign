#include "delay.h"
#include "sys.h" 
#include "usart.h"
#include "string.h"
#include "lw_oopc.h"

CLASS(zhanghong, rootClass)
char *name;
int age;
void (*init)(zhanghong *cthis,char *name, int age);
END_CLASS

void zhanghongInit(zhanghong *cthis, char *name, int age)
{
	cthis->age = age;
	cthis->name = name;
}

void zhanghongDealloc(zhanghong *cthis)
{
	printf("�ź豻�ͷ���\r\n");
}

CTOR(zhanghong, rootClass)
FUNCTION_SETTING(init, zhanghongInit);
FUNCTION_SETTING(dealloc, zhanghongDealloc);
END_CTOR
 
 int main(void)
 {	 
	zhanghong *zh; 
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200

  	while(1)
	{	
		zh = zhanghong_new("hahah");
		zh->init(zh,"�ź�",13);
		lw_oopc_report();
		printf("�ҵ����� %d, �ҵ����� %s\r\n", zh->age, zh->name);
		zhanghong_release(zh);
		lw_oopc_report();
		delay_ms(1000);
	}	   
}



