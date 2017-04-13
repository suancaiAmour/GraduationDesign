#ifndef WWDG_TYPEDEF_H
#define WWDG_TYPEDEF_H

#include "stm32f10x__rcc.h"
#include "stm32f10x__nvic.h"

namespace periph{
	using namespace nstd;
	
	// 用来设置窗口看门狗定时器的时钟
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
			, WDGTB		= BitFromTo(8, 7)	// timer base, 计数器时钟分频
			, W			= BitFromTo(6, 0)	// 7-bit window value
		};
		
		enum WWDG_SR{						// status register
			  EWIF = Bit(0)					// early wackup interrupt flag
		};
		
		// 注意：在使用窗口看门狗之前，先通过RCC使能看窗口看门狗的时钟。

		struct WWDG_Class: protected WWDG_TypeDef
		{
			public:
				
			//说明：
			//	设置看门狗计数器的时钟
			//参数：
			//	tb	参考 WWDG_TimerBase 枚举类的定义。
			void SetTimerBase(WWDG_TimerBase tb)	{ CFR = assignf(CFR, WDGTB, tb); }
			
			//说明：
			// 设置窗口值，如果在计数器的值大于窗口值的时候 Reflash 计数器，看门狗会产生系统复位信号
			//参数：
			//	val	窗口值, 6 bits，如果在计数器的值大于窗口值的时候 Reflash 计数器，看门狗会产生系统复位信号
			//备注：
			//	实际的计数器是7 bit 的，但是 T6 bit被清0的时候就会产生复位信号，
			//	所以可以看成是 6-bit 的，下溢的时候产生复位信号。至于 6 号 bit 的值，由函数内部自动处理。
			void SetWindow(u8 val)	{ CFR = assignf(CFR, W, val | Bit(6)); }
			
			//说明：
			//	系统启动后窗口看门狗默认为失能状态。
			//	这个函数用来启动看窗口门狗，Enable the watchdog，一但启动，除非系统复位，否则无法停止。
			//参数：
			//	val	
			//		一个 6-bit 的，用来设置看门狗计数器的初始值，默认为 0b111111
			void finline Start(u8 val = T)	{ CR = (val | Bit(6) | WGDA); }
			
			//说明：
			//	更新看门狗的计数器
			//参数：
			//	val	一个 6-bit 的值，用来刷新看门狗的计数器的值，默认是 0b111111
			//	计数器从这个值开始递减，递减到0时，再递减一次就会产生系统复位信号。即下溢的时候会产生复位信号。
			void finline Reflash(u8 val = T)	{ CR = val | Bit(6); }
			
			
			//说明：	ewrly wakeup interrupt enable
			//	EWI中断使能，当窗口看门狗的6-bit计数器到达 0 时，会产生中断，
			//	由中断服务程序进行复位前必须的工作，或者喂狗然后进行系统恢复来避免系统复位
			void finline EWI_Enable()	{	CFR = setb(CFR, EWI);}
			
			//说明：
			//	清除EWI中断标志
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
