#include "../../h/timer.h"

Task *task ;
unsigned int Address_BASE = 0;

void IRQHandler(void)
{
	
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //���TIMx���жϴ�����λ:TIM �ж�Դ
		__asm
		{
			ldr r5, [Address_BASE]  
			str r5, [&task->Reg_Buff[task->Current][0]]//R0

			ldr r5, [Address_BASE , 0x4] 
			str r5, [&task->Reg_Buff[task->Current][1]]//R1

			ldr r5, [Address_BASE , 0x8] 
			str r5, [&task->Reg_Buff[task->Current][2]]//R2

			ldr r5, [Address_BASE , 0xc] 
			str r5, [&task->Reg_Buff[task->Current][3]]//R3

			ldr r5, [Address_BASE , 0x10]
			str r5, [&task->Reg_Buff[task->Current][4]]//R12

//			ldr r5, [Address_BASE , 0x14]
//			str r5, [&task->Reg_Buff[task->Current][5]]//R13 LR

			ldr r5, [Address_BASE , 0x18]

			str r5, [&task->Reg_Buff[task->Current][6]]//R14 PC

			ldr r5, [Address_BASE , 0x1c]
			str r5, [&task->Reg_Buff[task->Current][7]]//R15 xRSP
			
			/*���������浱Ȼ�����е������ֳ�������*/

			ldr r5, [&task->Reg_Buff[task->Next][0]]//R0
			str r5, [Address_BASE]

			ldr r5, [&task->Reg_Buff[task->Next][1]]//R1
			str r5, [Address_BASE, 0x4]

			ldr r5, [&task->Reg_Buff[task->Next][2]]//R2
			str r5, [Address_BASE, 0x8]

			ldr r5, [&task->Reg_Buff[task->Next][3]]//R3
			str r5, [Address_BASE, 0xc]

			ldr r5, [&task->Reg_Buff[task->Next][4]]//R12
			str r5, [Address_BASE, 0x10]

//			ldr r5, [&task->Reg_Buff[task->Next][5]]//R13 LR
//			str r5, [Address_BASE, 0x14]

			ldr r5, [&task->Reg_Buff[task->Next][6]]//R14 PC
			//orr r5, 0x01
			str r5, [Address_BASE, 0x18]

			ldr r5, [&task->Reg_Buff[task->Next][7]]//R15 xRSP
			str r5, [Address_BASE, 0x1c]
			/*�������ָ���һ��������ֳ�������*/
		}
		task->Start_Next(); //��һ������
	}
}
extern "C"
{
	void TIM3_IRQHandler(void)   //TIM3�ж�
	{
		__ASM
		{
			mrs r4, msp
			add r4, 0x08
			str r4, [&Address_BASE]
			bl 	  IRQHandler
		}
	}

}

//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void Timerx_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //���TIMx���жϴ�����λ:TIM �ж�Դ
	}

	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM3, //TIM2
		TIM_IT_Update  |  //TIM �ж�Դ
		TIM_IT_Trigger,   //TIM �����ж�Դ 
		ENABLE  //ʹ��
		);
		
	TIM3->CNT = 0x01;

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	
	while (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //���TIMx���жϴ�����λ:TIM �ж�Դ
	}	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //���TIMx���жϴ�����λ:TIM �ж�Դ
	}

	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx���� 
}

