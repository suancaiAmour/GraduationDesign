#include "stm32f10x__cpplib.h"
#include "stm32f10x__tim.h"

namespace periph{
	namespace timer_reg{
		
		/************************************  BasicTimer  ************************************
		***/
		
		/**	@brief 定时器的基本配置 */
		void BasicTimer::BaseConfig(const TIM_BaseConfig& cfg) {
			PSC = cfg.prescaler;
			ARR = cfg.autoReloadValue;
		}
		
		/***
		************************************  BasicTimer  ************************************/
		
		/************************************  AdvancedTimer  ************************************
		***/
		
		void AdvancedTimer::BaseConfig(const TIM_BaseConfig& cfg) {
			PSC = cfg.prescaler;
			ARR = cfg.autoReloadValue;
			u16 temp = CR1;
			temp = assignf(temp, CMS|DIR, cfg.counterMode);
			temp = assignf(temp, CKD, cfg.clockDivForFilters);
			CR1 = temp;
		}
	
		void AdvancedTimer::CaptureCompare1Config(const TIM_OutputCompareConfig* const pcfg){
			const TIM_OutputCompareConfig& cfg = *pcfg;
			u32 temp = CCMR1;
			temp = assignf(temp, CC1S, BByte(00));
			temp = assignf(temp, OC1M, cfg.mode);
			CCMR1 = temp;
			
			CCR1 = cfg.compareValue;
		
			*BitBandPtr<OIS1>(CR2) = cfg.OCx_IdleState;
			*BitBandPtr<OIS1N>(CR2) = cfg.OCxN_IdleState;
			
			*BitBandPtr<CC1E>(CCER) = cfg.OCx_State;
			*BitBandPtr<CC1P>(CCER) = cfg.OCx_Polarity;
			*BitBandPtr<CC1NE>(CCER) = cfg.OCxN_State;
			*BitBandPtr<CC1NP>(CCER) = cfg.OCxN_Polarity;
		}
		void AdvancedTimer::CaptureCompare2Config(const TIM_OutputCompareConfig* const pcfg){
			const TIM_OutputCompareConfig& cfg = *pcfg;
			u32 temp = CCMR1;
			temp = assignf(temp, CC2S, BByte(00));
			temp = assignf(temp, OC2M, cfg.mode);
			CCMR1 = temp;
			
			CCR2 = cfg.compareValue;
		
			*BitBandPtr<OIS2>(CR2) = cfg.OCx_IdleState;
			*BitBandPtr<OIS2N>(CR2) = cfg.OCxN_IdleState;
			
			*BitBandPtr<CC2E>(CCER) = cfg.OCx_State;
			*BitBandPtr<CC2P>(CCER) = cfg.OCx_Polarity;
			*BitBandPtr<CC2NE>(CCER) = cfg.OCxN_State;
			*BitBandPtr<CC2NP>(CCER) = cfg.OCxN_Polarity;
		}
		void AdvancedTimer::CaptureCompare3Config(const TIM_OutputCompareConfig* const pcfg){
			const TIM_OutputCompareConfig& cfg = *pcfg;
			u32 temp = CCMR2;
			temp = assignf(temp, CC3S, BByte(00));
			temp = assignf(temp, OC3M, cfg.mode);
			CCMR2 = temp;
			
			CCR3 = cfg.compareValue;
		
			*BitBandPtr<OIS3>(CR2) = cfg.OCx_IdleState;
			*BitBandPtr<OIS3N>(CR2) = cfg.OCxN_IdleState;
			
			*BitBandPtr<CC3E>(CCER) = cfg.OCx_State;
			*BitBandPtr<CC3P>(CCER) = cfg.OCx_Polarity;
			*BitBandPtr<CC3NE>(CCER) = cfg.OCxN_State;
			*BitBandPtr<CC3NP>(CCER) = cfg.OCxN_Polarity;
		}
		void AdvancedTimer::CaptureCompare4Config(const TIM_OutputCompareConfig* const pcfg){
			const TIM_OutputCompareConfig& cfg = *pcfg;
			u32 temp = CCMR2;
			temp = assignf(temp, CC4S, BByte(00));
			temp = assignf(temp, OC4M, cfg.mode);
			CCMR2 = temp;
			
			CCR4 = cfg.compareValue;
		
			*BitBandPtr<OIS3>(CR2) = cfg.OCx_IdleState;
			/* there is no such thing as OC4N */
			//*BitBandPtr<OIS4N>(CR2) = cfg.OCxN_IdleState; 
			
			*BitBandPtr<CC4E>(CCER) = cfg.OCx_State;
			*BitBandPtr<CC4P>(CCER) = cfg.OCx_Polarity;
			//*BitBandPtr<CC4NE>(CCER) = cfg.OCxN_State;
			//*BitBandPtr<CC4NP>(CCER) = cfg.OCxN_Polarity;
		}
		
		/***
		************************************  AdvancedTimer  ************************************/
		
	} /* namespace timer_reg */
} /* namespace periph */
