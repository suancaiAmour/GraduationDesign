#include "../h/init.h"

void Init(void)
{
	SystemInit();//系统时钟等初始化
	delay_init(72);	     //延时初始化
	NVIC_Configuration();//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);//串口初始化为9600
	LED_Init();	 //LED端口初始化
	KEY_Init();

	
	
	printf("初始化成功\n");
}


void NVIC_Config(void)
{
	/*定义一个NVIC_InitTypeDef 类型的结构体，名字叫NVIC_InitStructure*/
	NVIC_InitTypeDef NVIC_InitStructure;
	/**********************************************
	NVIC_PriorityGroup_0：先占优先级0位从优先级4位
	NVIC_PriorityGroup_1：先占优先级1位从优先级3位
	NVIC_PriorityGroup_2：先占优先级2位从优先级2位
	NVIC_PriorityGroup_3：先占优先级3位从优先级1位
	NVIC_PriorityGroup_4：先占优先级4位从优先级0位
	***********************************************/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	/*配置选中的中断向量*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;; //TIM2捕获比较中断

	/*设置了成员NVIC_IRQChannel中的先占优先级*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;

	/*设置了成员NVIC_IRQChannel中的响应优先级*/
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;

	/*使能中断向量*/
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	/*调用库函数，初始化中断向量*/
	NVIC_Init(&NVIC_InitStructure);


}
void TIM2_GPIO_Config(void)
{
 	/*定义一个GPIO_InitTypeDef 类型的结构体，名字叫GPIO_InitStructure*/
	GPIO_InitTypeDef GPIO_InitStructure;
	/*开启TIM2的时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	/*开启GPIOA的时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
	/*选择PA0引脚*/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 ;
	/*引脚模式设置为下拉输入*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	/*设置引脚速度*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	/*初始化所设置的PA管脚*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
