#ifndef WWDG_TYPEDEF_H
#define WWDG_TYPEDEF_H

#include "stm32f10x__rcc.h"
#include "stm32f10x__nvic.h"

namespace periph{
	using namespace nstd;
	
	// �������ô��ڿ��Ź���ʱ����ʱ��
	enum WWDG_TimerBase{
		  PCLK1_DIV_4096_DIV_1 	= BByte(00)		// pclk1 / 4096 / 1
		, PCLK1_DIV_4096_DIV_2 	= BByte(01)		// pclk1 / 4096 / 2
		, PCLK1_DIV_4096_DIV_4 	= BByte(10)		// pclk1 / 4096 / 4
		, PCLK1_DIV_4096_DIV_8 	= BByte(11)		// pclk1 / 4096 / 8
	};
	
	namespace wwdg_reg{
		using namespace periph::rcc_reg;
		
		enum WWDG_CR{					// control register
			  WGDA	= Bit(7)			// activation bit, this bit is set by software and clear by hardware after reset
			, T		= BitFromTo(6, 0)	// 7-bit counter
		};
		
		enum WWDG_CFR{						// configuration register
			  EWI		= Bit(9)			// early wackup interrupt enable, set by software and clear by hardware after reset
			, WDGTB		= BitFromTo(8, 7)	// timer base, ������ʱ�ӷ�Ƶ
			, W			= BitFromTo(6, 0)	// 7-bit window value
		};
		
		enum WWDG_SR{						// status register
			  EWIF = Bit(0)					// early wackup interrupt flag
		};
		
		// ע�⣺��ʹ�ô��ڿ��Ź�֮ǰ����ͨ��RCCʹ�ܿ����ڿ��Ź���ʱ�ӡ�

		struct WWDG_Class: protected WWDG_TypeDef
		{
			public:
				
			//˵����
			//	���ÿ��Ź���������ʱ��
			//������
			//	tb	�ο� WWDG_TimerBase ö����Ķ��塣
			void SetTimerBase(WWDG_TimerBase tb)	{ CFR = assignf(CFR, WDGTB, tb); }
			
			//˵����
			// ���ô���ֵ������ڼ�������ֵ���ڴ���ֵ��ʱ�� Reflash �����������Ź������ϵͳ��λ�ź�
			//������
			//	val	����ֵ, 6 bits������ڼ�������ֵ���ڴ���ֵ��ʱ�� Reflash �����������Ź������ϵͳ��λ�ź�
			//��ע��
			//	ʵ�ʵļ�������7 bit �ģ����� T6 bit����0��ʱ��ͻ������λ�źţ�
			//	���Կ��Կ����� 6-bit �ģ������ʱ�������λ�źš����� 6 �� bit ��ֵ���ɺ����ڲ��Զ�����
			void SetWindow(u8 val)	{ CFR = assignf(CFR, W, val | Bit(6)); }
			
			//˵����
			//	ϵͳ�����󴰿ڿ��Ź�Ĭ��Ϊʧ��״̬��
			//	����������������������Ź���Enable the watchdog��һ������������ϵͳ��λ�������޷�ֹͣ��
			//������
			//	val	
			//		һ�� 6-bit �ģ��������ÿ��Ź��������ĳ�ʼֵ��Ĭ��Ϊ 0b111111
			void finline Start(u8 val = T)	{ CR = (val | Bit(6) | WGDA); }
			
			//˵����
			//	���¿��Ź��ļ�����
			//������
			//	val	һ�� 6-bit ��ֵ������ˢ�¿��Ź��ļ�������ֵ��Ĭ���� 0b111111
			//	�����������ֵ��ʼ�ݼ����ݼ���0ʱ���ٵݼ�һ�ξͻ����ϵͳ��λ�źš��������ʱ��������λ�źš�
			void finline Reflash(u8 val = T)	{ CR = val | Bit(6); }
			
			
			//˵����	ewrly wakeup interrupt enable
			//	EWI�ж�ʹ�ܣ������ڿ��Ź���6-bit���������� 0 ʱ��������жϣ�
			//	���жϷ��������и�λǰ����Ĺ���������ι��Ȼ�����ϵͳ�ָ�������ϵͳ��λ
			void finline EWI_Enable()	{	CFR = setb(CFR, EWI);}
			
			//˵����
			//	���EWI�жϱ�־
			void finline ClearPending()	{ SR = clearb(SR, EWIF); }
			
			bool finline IsInWindow()
			{
				u8 wr,tr; 
				wr= CFR&0X7F; 
				tr= CR&0X7F; 
				return tr<wr;
			}
			
			finline operator RCC_APB1_PERIPHERAL()	{return PERIPH_WWDG;}
			
			operator IRQn_Type()	{return WWDG_IRQn;}
		};
	}
}

#define wwdg	(*(periph::wwdg_reg::WWDG_Class*)WWDG)

#endif
