#include "../../h/stm32f10x.h"
#include "../../h/led.h"

/////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/5/27
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved											  
////////////////////////////////////////////////////////////////////////////////// 	   

//��ʼ��PA8��PD2Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��PA�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //LED0-->PA.8 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOA, &GPIO_InitStructure);
 GPIO_SetBits(GPIOA,GPIO_Pin_8);						 //PA.8 �����


 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);   //ʹ��PD�˿�ʱ��

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	    		 //LED1-->PD.2 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
 GPIO_Init(GPIOD, &GPIO_InitStructure);	
 GPIO_SetBits(GPIOD,GPIO_Pin_2); 						 //PD.2 ����� 
 
 //delay_ms(3000);
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   //ʹ��PD�˿�ʱ��

 //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //��ֹJTAG����

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8
	| GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 ;	    		 //LED1-->PD.2 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOB, &GPIO_InitStructure);
 
 
}
 
