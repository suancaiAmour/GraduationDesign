#ifndef EXTI_TYPEDEF_H
#define EXTI_TYPEDEF_H

namespace periph{
	enum ExtiLine{
		  Line0 = Bit(0), Line1 = Bit(1), Line2 = Bit(2), Line3 = Bit(3), Line4 = Bit(4), Line5 = Bit(5), Line6 = Bit(6)
		, Line7 = Bit(7), Line8 = Bit(8), Line9 = Bit(9), Line10 = Bit(10), Line11 = Bit(11), Line12 = Bit(12)
		, Line13 = Bit(13), Line14 = Bit(14), Line15 = Bit(15), Line16 = Bit(16), Line17 = Bit(17), Line18 = Bit(18)
		, Line19 = Bit(19)
	};

	namespace exti_reg{

		class EXTI_Class: protected EXTI_TypeDef
		{
			public:
			// 外部中断下降沿触发使能/失能
			finline void RisingTrigger(u16 lines, FunctionalState e)
			{
				RTSR = (e == DISABLE? clearb(RTSR, lines) : setb(RTSR, lines));	
			}
			// 外部中断上升沿触发使能/失能
			finline void FallingTrigger(u16 lines, FunctionalState e)
			{
				FTSR = (e == DISABLE? clearb(FTSR, lines) : setb(FTSR, lines));	
			}	
			// 中断触发配置
			void Trigger(u16 lines,FunctionalState fallingTrig , FunctionalState risingTrig)
			{
				 RisingTrigger(lines, risingTrig);
				 FallingTrigger(lines, fallingTrig);
			}

			/**	@brief	使能或失能外部中断
			*	@param	lines: 中断号，参见 ExtiLine 的定义，可以由多个成员相或得到参数，
			*			但是需要进行强制类型转换。
			*/
			void InterruptConfig(ExtiLine lines,const FunctionalState e)	
					{IMR =((e == DISABLE)?(clearb(IMR,lines)):(setb(IMR,lines)));}
			
			// 清除中断标志，注意在中断处理函数退出前一定要清楚中断标志
			finline void ClearPending(u32 lines)	{ PR = lines;}		// 写1清零

			// 读中断标志
			finline u16 ReadPending(u32 lines)	{return checkb(PR, lines);}		//
		};
		
	}
}

#define exti	(*(periph::exti_reg::EXTI_Class*)EXTI)

#endif
