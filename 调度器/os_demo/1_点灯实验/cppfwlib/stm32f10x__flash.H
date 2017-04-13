#ifndef FLASH_TYPEDEF_H
#define FLASH_TYPEDEF_H
#include "stm32f10x__COMMON.h"
namespace periph{
	namespace flash_reg{
		/**	@brief	represent the ratio of the SYSCLK (system clock) 
		*			period to the Flash access time.
		*/
		enum FLASH_LATENCY{
			  ZERO_WAIT_STATE	= BByte(000)
			, ONE_WAIT_STATE	= BByte(001)
			, TWO_WAIT_STATE	= BByte(010)
		};
		
		/**	@brief	Flash access control register
		*/
		enum FLASH_ACR{
			  PREFTBS		= Bit(5)			// Prefetch buffer status
			, PREFTBE		= Bit(4)			// Prefetch buffer enable
			, HLFCYA		= Bit(3)			// Flash half cycle access enable
			, LATENCY		= BitFromTo(2, 0)	// Latency
		};
		
		class FLASH_Class: protected FLASH_TypeDef
		{
			public:
			void SetLatency(u32 sysclk)
			{
				if(sysclk <= 24000000)
					ACR = assignf(ACR, LATENCY, ZERO_WAIT_STATE);
				else if(sysclk <= 48000000)
					ACR = assignf(ACR, LATENCY, ONE_WAIT_STATE);
				else
					ACR = assignf(ACR, LATENCY, TWO_WAIT_STATE);
			}
		};
	}
}

#define flash	(*(periph::flash_reg::FLASH_Class*)FLASH)

#endif
