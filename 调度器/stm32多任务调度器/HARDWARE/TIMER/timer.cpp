#include "../../h/timer.h"

Task *task ;
unsigned int Address_BASE = 0;

void IRQHandler(void)
{
	
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源
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
			
			/*↑↑↑保存当然运行中的任务现场↑↑↑*/

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
			/*↑↑↑恢复上一个任务的现场↑↑↑*/
		}
		task->Start_Next(); //下一个任务
	}
}
extern "C"
{
	void TIM3_IRQHandler(void)   //TIM3中断
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

//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void Timerx_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源
	}

	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM3, //TIM2
		TIM_IT_Update  |  //TIM 中断源
		TIM_IT_Trigger,   //TIM 触发中断源 
		ENABLE  //使能
		);
		
	TIM3->CNT = 0x01;

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	
	while (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源
	}	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源
	}

	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设 
}

