#include "stm32f10x__cpplib.h"

namespace core{


	void NVIC_Class::Config(IRQn_Type n, FunctionalState e, u8 groupPri, u8 subPri)
	{
		if(e == DISABLE)
			ICER[n/32] = Bit(n%32);
		else
			ISER[n/32] = Bit(n%32);
		
		PriorityGroupingStyle s = scb.GetPriorityGrouping();
		// 下面计算并用groupPri来保存最终要写到IP寄存器的值
		switch(s)
		{
		case GRP16_SUB1:{
			groupPri <<= 4;
		}break;
		case GRP8_SUB2:{
			groupPri <<= 5;
			groupPri |= ((subPri & BByte(1)) << 4);
		}break;
		case GRP4_SUB4:{
			groupPri <<= 6;
			groupPri |= ((subPri & BByte(11)) << 4);
		}break;
		case GRP2_SUB8:{
			groupPri <<= 7;
			groupPri |= ((subPri & BByte(111)) << 4);
		}break;
		case GRP1_SUB16:{
			groupPri = (subPri << 4);
		}break;
		default:{
			// 出错了?
			return ;	
		}
		}
		IP[n] = groupPri;
		return ;
	}

}
