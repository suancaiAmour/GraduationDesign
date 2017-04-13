#pragma once
#ifndef AFIO_TYPEDEF_H
#define AFIO_TYPEDEF_H

#include "stm32f10x__RCC.h"

namespace periph{
		
	enum AFIO_ExtiLineSource {
		  EXTI_LINE0 = 0
		, EXTI_LINE1
		, EXTI_LINE2
		, EXTI_LINE3
		, EXTI_LINE4
		, EXTI_LINE5
		, EXTI_LINE6
		, EXTI_LINE7
		, EXTI_LINE8
		, EXTI_LINE9
		, EXTI_LINE10
		, EXTI_LINE11
		, EXTI_LINE12
		, EXTI_LINE13
		, EXTI_LINE14
		, EXTI_LINE15
	};
	
	enum AFIO_EXTI_PortSelect
	{	PORTA = 0,PORTB, PORTC, PORTD, PORTE, PORTF, PORTG 	};
	
/** @brief 
		Configuration			| Available debug ports | PA13    | PA14    | PA15	| PB3       | PB4      |
		Selections				|                       | (JTMS)  | (JTCK)  | (JTDI)| (JTDO)    | (NJTRST) |
								|                       | (SWDIO) |	(SWCLK) |       |(TRACE SWO)|          |
	1.	SWD_JTAG_ENABLE			| Full SWJ              |         |			|       |           |          |
								| (JTAG-DP + SW-DP)     | X       |	X		| X     | X         | X        |
								| (Reset state)         |         |			|       |           |          |
	2.	SWD_JTAG_WITHOUT_NJTRST	| Full SWJ              | X       |	X		| X		| X         | free     |
								| (JTAG-DP + SW-DP)     |         |			|       |           |          |
								| but without NJTRST    |         |			|       |           |          |
	3.	SWD_ENABLE_JTAG_DISABLE	| JTAG-DP Disabled and	| X       |	X		| free  | free(1)   | free     |
								| SW-DP Enabled         |         |         |       |           |          |
	4.	SWD_JTAG_DISABLE		| JTAG-DP Disabled and	| free    |	free    | free  | free	    | free     |
								| SW-DP Disabled        |         |         |       |           |          |
	(1). Released only if not using asynchronous trace.
*/
	enum SWD_JTAG_CONFIG{
		SWD_EN_JTAG_EN          = BByte(000), 	/*!< Full SWJ (JTAG-DP + SW-DP): Reset State */
		SWD_EN_JTAG_WITHOUT_NJTRST = BByte(001),	/*!< Full SWJ (JTAG-DP + SW-DP) but without NJTRST */
		SWD_EN_JTAG_DIS         = BByte(010),	/*!< JTAG-DP Disabled and SW-DP Enabled */
		SWD_DIS_JTAG_DIS        = BByte(100)	/*!< JTAG-DP Disabled and SW-DP Disabled */
	};
	
	namespace afio_reg{
		using namespace periph::rcc_reg;
		
		class AFIO_class: protected AFIO_TypeDef
		{
			public:
			/**	@brief	外部中断映射
			*	@param	pin: 外部中断号，从 @ref AFIO_ExtiLineSource 的成员中选择一个作为参数
			*	@param	port: 端口号，可以从 AFIO_EXTI_PortSelect 的成员中选择一个作为参数，
			*			也可以直接以 gpioa, gpiob, ..., gpioe 作为参数，推荐后者的用法。
			*/
			void Map(AFIO_ExtiLineSource pin, AFIO_EXTI_PortSelect port)	{(this->EXTICR[(pin)/4] = AssignField( EXTICR[(pin)/4], Bit(((pin)%4)*4), 4,port ) );}

            /**	@brief 调试相关的 IO 配置
              * @param cfg: 可以是枚举 SWD_JTAG_CONFIG 的成员之一，参考 @ref SWD_JTAG_CONFIG 的定义
              */
			inline void DebugConfig(SWD_JTAG_CONFIG cfg)	{MAPR = AssignField(MAPR, Bit(24), 3, cfg);}
			
			protected:
				AFIO_class(){}			//禁止用户建立对象
		};
	}
}

#define afio	(*(periph::afio_reg::AFIO_class*)AFIO)
#endif
