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
			// �ⲿ�ж��½��ش���ʹ��/ʧ��
			finline void RisingTrigger(u16 lines, FunctionalState e)
			{
				RTSR = (e == DISABLE? clearb(RTSR, lines) : setb(RTSR, lines));	
			}
			// �ⲿ�ж������ش���ʹ��/ʧ��
			finline void FallingTrigger(u16 lines, FunctionalState e)
			{
				FTSR = (e == DISABLE? clearb(FTSR, lines) : setb(FTSR, lines));	
			}	
			// �жϴ�������
			void Trigger(u16 lines,FunctionalState fallingTrig , FunctionalState risingTrig)
			{
				 RisingTrigger(lines, risingTrig);
				 FallingTrigger(lines, fallingTrig);
			}

			/**	@brief	ʹ�ܻ�ʧ���ⲿ�ж�
			*	@param	lines: �жϺţ��μ� ExtiLine �Ķ��壬�����ɶ����Ա���õ�������
			*			������Ҫ����ǿ������ת����
			*/
			void InterruptConfig(ExtiLine lines,const FunctionalState e)	
					{IMR =((e == DISABLE)?(clearb(IMR,lines)):(setb(IMR,lines)));}
			
			// ����жϱ�־��ע�����жϴ������˳�ǰһ��Ҫ����жϱ�־
			finline void ClearPending(u32 lines)	{ PR = lines;}		// д1����

			// ���жϱ�־
			finline u16 ReadPending(u32 lines)	{return checkb(PR, lines);}		//
		};
		
	}
}

#define exti	(*(periph::exti_reg::EXTI_Class*)EXTI)

#endif
