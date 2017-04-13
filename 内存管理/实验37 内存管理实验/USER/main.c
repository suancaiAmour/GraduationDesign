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
	printf("张鸿被释放了\r\n");
}

CTOR(zhanghong, rootClass)
FUNCTION_SETTING(init, zhanghongInit);
FUNCTION_SETTING(dealloc, zhanghongDealloc);
END_CTOR
 
 int main(void)
 {	 
	zhanghong *zh; 
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200

  	while(1)
	{	
		zh = zhanghong_new("hahah");
		zh->init(zh,"张鸿",13);
		lw_oopc_report();
		printf("我的年龄 %d, 我的名字 %s\r\n", zh->age, zh->name);
		zhanghong_release(zh);
		lw_oopc_report();
		delay_ms(1000);
	}	   
}



