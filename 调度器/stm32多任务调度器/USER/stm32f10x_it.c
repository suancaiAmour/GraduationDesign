/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 
#include "usart.h"
#include "stm32f10x_tim.h"


 
void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
 
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
}
 
void PendSV_Handler(void)
{
}
 
void SysTick_Handler(void)
{
//	while(1)
//	{
//		printf("触发到");
//	}
}
unsigned int Tim_RIsing_count = 0; //上升沿计数
unsigned int Tim_Falling_count = 0;//下降沿计数

static char Time_State = RIsing;

void TIM2_IRQHandler(void)		 //中断函数
{
	/*检测中断标志位*/
	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) == SET)    	/*重置中断标志位*/
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);

	if (Time_State == RIsing)
	{
		Time_State = Falling;
		TIM_OC1PolarityConfig(TIM2, TIM_ICPolarity_Falling);
		TIM2->CNT = 0x04;
		Tim_RIsing_count = TIM_GetCapture1(TIM2);
	}
	else 
	{
		Time_State = RIsing;
		TIM_OC1PolarityConfig(TIM2, TIM_ICPolarity_Rising);
		Tim_Falling_count = TIM_GetCapture1(TIM2);
	}
	//TIM_UpdateRequestConfig(TIM2, TIM_UpdateSource_Global); 中断源

}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
/*
	u8 TM5_CH1_CAPTURE_STA = 0;//8位0x00~0x80
	u16 TM5_CH1_CAPTURE_VAL;  //捕获高电平后定时器溢出的次数

	void TIM5_IRQHandler(void)
	{
		if ((TM5_CH1_CAPTURE_STA & 0x80) == 0)  //未成功捕获，0x80捕获完成
		{
			if (TIM_GetITStatus(TIM5, TIM_IT_Update) == SET)  //数据更新中断产生
			{
				if (TM5_CH1_CAPTURE_STA & 0x40)     //已经捕获到高电平
				{
					if ((TM5_CH1_CAPTURE_STA & 0x3f) == 0x3f)   //­溢出
					{
						TM5_CH1_CAPTURE_STA |= 0x80;  //强制捕获成功
						TM5_CH1_CAPTURE_VAL = 0xffff;  //此时的计数值
					}
					else
					{
						TM5_CH1_CAPTURE_STA++;
					}

				}
			}
			if (TIM_GetITStatus(TIM5, TIM_IT_CC1) == SET)//发生捕获
			{
				if (TM5_CH1_CAPTURE_STA & 0x40)  //成功捕获到一次下降沿，但不是第一次捕获
				{
					TM5_CH1_CAPTURE_STA |= 0x80;  //捕获成功
					TM5_CH1_CAPTURE_VAL = TIM_GetCapture1(TIM5);//获取捕获值
					TIM_OC1PolarityConfig(TIM5, TIM_ICPolarity_Rising);//要设置为上升沿，等待下降沿的来临
				}
				else  //第一次捕获
				{
					TM5_CH1_CAPTURE_STA = 0;
					TM5_CH1_CAPTURE_VAL = 0;
					TIM_SetCounter(TIM5, 0);  //还没等到下降沿来时把所有的都清零
					TM5_CH1_CAPTURE_STA |= 0x40;
					TIM_OC1PolarityConfig(TIM5, TIM_ICPolarity_Falling);//要设置成下降沿，等到上升沿的来临
				}

			}
		}
		TIM_ClearITPendingBit(TIM5, TIM_IT_CC1 | TIM_IT_Update);

	}
}*/
