#include "../h/init.h"

void Init(void)
{
	SystemInit();//ϵͳʱ�ӵȳ�ʼ��
	delay_init(72);	     //��ʱ��ʼ��
	NVIC_Configuration();//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);//���ڳ�ʼ��Ϊ9600
	LED_Init();	 //LED�˿ڳ�ʼ��
	KEY_Init();

	
	
	printf("��ʼ���ɹ�\n");
}


void NVIC_Config(void)
{
	/*����һ��NVIC_InitTypeDef ���͵Ľṹ�壬���ֽ�NVIC_InitStructure*/
	NVIC_InitTypeDef NVIC_InitStructure;
	/**********************************************
	NVIC_PriorityGroup_0����ռ���ȼ�0λ�����ȼ�4λ
	NVIC_PriorityGroup_1����ռ���ȼ�1λ�����ȼ�3λ
	NVIC_PriorityGroup_2����ռ���ȼ�2λ�����ȼ�2λ
	NVIC_PriorityGroup_3����ռ���ȼ�3λ�����ȼ�1λ
	NVIC_PriorityGroup_4����ռ���ȼ�4λ�����ȼ�0λ
	***********************************************/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	/*����ѡ�е��ж�����*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;; //TIM2����Ƚ��ж�

	/*�����˳�ԱNVIC_IRQChannel�е���ռ���ȼ�*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;

	/*�����˳�ԱNVIC_IRQChannel�е���Ӧ���ȼ�*/
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;

	/*ʹ���ж�����*/
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	/*���ÿ⺯������ʼ���ж�����*/
	NVIC_Init(&NVIC_InitStructure);


}
void TIM2_GPIO_Config(void)
{
 	/*����һ��GPIO_InitTypeDef ���͵Ľṹ�壬���ֽ�GPIO_InitStructure*/
	GPIO_InitTypeDef GPIO_InitStructure;
	/*����TIM2��ʱ��*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	/*����GPIOA��ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
	/*ѡ��PA0����*/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 ;
	/*����ģʽ����Ϊ��������*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	/*���������ٶ�*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	/*��ʼ�������õ�PA�ܽ�*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
