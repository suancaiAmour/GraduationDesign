#ifndef TIM_TYPEDEF_H
#define TIM_TYPEDEF_H

#include "stm32f10x__common.h"
#include "stm32f10x__RCC.H"

namespace periph{

	struct TIM_BaseConfig;
	struct TIM_OutputCompareConfig;
	
/* �Ĵ�����λ�����ļ� */
#include "timer_reg.h"
	
	enum TIM_ClockDivision_CKD {
		  CKD_DIV1	= BByte(00)
		, CKD_DIV2	= BByte(01)
		, CKD_DIV4	= BByte(10)
	};
	
	/**	@brief ������ʱ��Դѡ�� */
	enum TIM_ClockSource{
		  INTERNAL_CLOCK				/*!< CK_INT, internal clock */
		, EXTERNAL_INPUT_PIN			
		, EXTERNAL_TRIGGER_INPUT_ETR
		, INTERNAL_TRIGGER_INPUT
	};
	
	/**	@brief �������Ĺ���ģʽ */
	enum TIM_CounterMode {
		UP_COUNTING = BByte(000)			/*!< ���ϼ��� */
		, DOWN_COUNTING = BByte(001) 		/*!< ���¼��� */
		
		, CENTER_ALIGNED_D = BByte(100)		/*!< Output compare interrupt flags of channels 
												configured in output(CCxS=00 in TIMx_CCMRx register) 
												are set only when the counter is counting down. 
												@note for TIM2 - TIM5, TIM1 and TIM8 only */
		, CENTER_ALIGNED_U	= BByte(010)	/*!< Output compare interrupt flags of channels  
												configured in output(CCxS=00 in TIMx_CCMRx register) are  
												set only when the counter is counting up.
												@note for TIM2 - TIM5, TIM1 and TIM8 only */
		, CENTER_ALIGNED_U_D = BByte(110)	/*!< Output compare interrupt flags of channels  
												configured in output(CCxS=00 in TIMx_CCMRx register) are  
												set only when the counter is counting up or down.
												@note for TIM2 - TIM5, TIM1 and TIM8 only */
	};
	
	/**	@brief	�����¼����¼�Դ */
	enum TIM_UpdateRequestSource{
		  UPDATE_SOURCE_GLOBAL		=	0	/*!< ����������/���磬������������¼�����ͨ�� ��ģʽ��������
												�����źž�����Ϊ�����¼����¼�Դ */
		, UPDATE_SOURCE_REGULAR		=	1	/*!< ���Լ���������/������Ϊ�����¼����¼�Դ */
	};
	
	struct TIM_CaptureCompareControl{
		/* BY_SETTING_COMG: When capture/compare control bits are preloaded (CCPC=1), 
		they are updated by setting the COMG bit only 
		BY_SETTING_COMG_OR_POSEDGE_TRGI: When capture/compare control bits are 
		preloaded (CCPC=1), they are updated by setting the COMG bit or when an 
		rising edge occurs on TRGI */
		enum UpdateSelection{							/* TIM_CR2_CCUS */
			  BY_SETTING_COMG					= 0
			, BY_SETTING_COMG_OR_POSEDGE_TRGI	= 1
		}update;
		
		/* DISABLE: CCxE, CCxNE and OCxM bits are not preloaded
		ENABLE: CCxE, CCxNE and OCxM bits are preloaded, after having been written, 
		they are updated only when COM bit is set.
		This mamber acts only on channels that have a complementary output.
		*/
		FunctionalState preload;
	};
		
	/**	@brief	basic timer ���ж�Դ��DMA ����Դ */
	enum TIM_InterruptSource{
		  UpdateEventInt		= timer_reg::UIE		/*!< �����¼��������ж� */
		, BreakInt				= timer_reg::BIE		/* break interrupt */
		, TriggerInt			= timer_reg::TIE		/* trigger interrupt */
		, COM_Int				= timer_reg::COMIE		/* COM interrupt */
		, CaptureCompare4Int	= timer_reg::CC4IE		/* Capture/Compare 4 interrupt */
		, CaptureCompare3Int	= timer_reg::CC3IE		/* Capture/Compare 3 interrupt */
		, CaptureCompare2Int	= timer_reg::CC2IE		/* Capture/Compare 2 interrupt */
		, CaptureCompare1Int	= timer_reg::CC1IE		/* Capture/Compare 1 interrupt */
	};
	BIT_OR_OPERATOR_SUPPORT(TIM_InterruptSource)
	
	/**	@brief DMA ����Դ */
	enum TIM_DMA_RequestSource {
		  UpdateEventDMA		= timer_reg::UDE		/* Update DMA Request */
		, TriggerDMA			= timer_reg::TDE		/* Trigger DMA request */
		, COM_DMA				= timer_reg::COMDE		/* COM DMA request */
		, CaptureCompare4DMA	= timer_reg::CC4DE		/* Capture/Compare 4 DMA request */
		, CaptureCompare3DMA	= timer_reg::CC3DE		/* Capture/Compare 3 DMA request */
		, CaptureCompare2DMA	= timer_reg::CC2DE		/* Capture/Compare 2 DMA request */
		, CaptureCompare1DMA	= timer_reg::CC1DE		/* Capture/Compare 1 DMA request */
	};
	BIT_OR_OPERATOR_SUPPORT(TIM_DMA_RequestSource)

	enum TIM_InterruptFlag{
		UpdateEventInterruptFlag	= timer_reg::UIF	/*!< �����¼��������ж� */
	};
	
	enum TIM_OutputCompareMode{
		 FROZEN					=	BByte(000)		/* Frozen - The comparison between the output compare 
													register TIMx_CCR1 and the counter TIMx_CNT has no
													effect on the outputs.(this mode is used to generate 
													a timing base). */
		, SET_ACTIVE_ON_MACH	= BByte(001)
		, SET_INACTIVE_ON_MACH	= BByte(010)
		, TOGGLE_ON_MACH		= BByte(011)
		, FORCE_INACTIVE		= BByte(100)
		, FORCE_ACTIVE			= BByte(101)
		, PWM1					= BByte(110)
		, PWM2					= BByte(111)
	};
	
	enum TIM_CC_OutputPolarity{
		  OC_ACTIVE_HIGH	= 0
		, OC_ACTIVE_LOW		= 1
	};
	enum TIM_CC_OutputIdleState{
		  OC_IDLE_RESET		= 0
		, OC_IDLE_SET		= 1
	};
	
	enum TIM_TriggerOutputSelection{			/* Trigger output selection */
		  RESET_AO_TRGO			= BByte(000)
		, UPDATE_AO_TRGO		= BByte(001)
		, ENABLE_AO_TRGO		= BByte(010)
		/* COMPARE_PULSE_AO_TRGO: The trigger output send a positive pulse when 
		the CC1IF flag is to be set (even if it was already high), as soon as a 
		capture or a compare match occurred. */
		, COMPARE_PULSE_AO_TRGO	= BByte(011)	
		, OC1REF_AS_TRGO		= BByte(100)
		, OC2REF_AS_TRGO		= BByte(101)
		, OC3REF_AS_TRGO		= BByte(110)
		, OC4REF_AS_TRGO		= BByte(111)
	};

	enum TIM_EventGeneration {
		UpdateEvent		= timer_reg::UG
	};
	
	namespace timer_reg{
		
		using namespace periph::rcc_reg;
	
		struct TimerCommon: public TIM_TypeDef {
			
			/**	@brief	���ʹ���� OnePulseMode����ô�´θ����¼�������ʱ��ʱ��ֹͣ����
			*	�ϵ�Ĭ��״̬Ϊ DISABLE
			*	@note  <no read-modify-write> */
			finline void OnePulseModeCmd(FunctionalState s)		{ *BitBandPtr<OPM>(CR1)=s;}
			
			/**	@brief	�Զ���װ�Ĵ���Ԥװ�ص�ʹ��/��ֹ 
			*	@parameter e: ���ʹ�ܣ���ô�Զ�ʱ���� ReloadValue �ĸ���ֻ���ڸ����¼���������
			*	@note  <no read-modify-write> 	*/
			finline void AutoReloadPreloadCmd(FunctionalState s)	{  *BitBandPtr<ARPE>(CR1)=s; }
			
			/**	@brief	���ø����¼����¼�Դ
			*	@note  <no read-modify-write> */
			finline void SetUpdateRequestSource(TIM_UpdateRequestSource s)		{ *BitBandPtr<URS>(CR1)=s; }
			
			/**	@brief	�����¼��Ŀ���״̬
			  *	@note	Ĭ��״̬��Ϊ����״̬
			  *	@param	cmd:
			  *	  @arg true ��ֹ�����¼�
			  *	  @arg false ʹ�ܸ����¼�
			  *	@note  <no read-modify-write> 	*/
			finline void UpdateEventCmd(FunctionalState cmd)		{ *BitBandPtr<UDIS>(CR1)=!cmd;}
			finline FunctionalState UpdateEventCmd()			{ return (FunctionalState)!*BitBandPtr<UDIS>(CR1); }
			
			/**	@brief ʹ��/��ֹ��������ʹ�ܼ������󣬼������Ϳ�ʼ������
			*	@note  <no read-modify-write> 	*/
			finline void CounterCmd(FunctionalState s)		{ *BitBandPtr<CEN>(CR1)=s; }
			
			/**	@brief	���ü������������Զ���װֵ
			*	@note  <no read-modify-write> 	*/
			finline void SetAutoReloadValue(u16 value)			{ ARR = value; }
			
			/**	@brief ���ü�������ֵ
			*	@note  <no read-modify-write> 	*/
			finline void SetCounter(u16 value)		{ (CNT = value); }
			
			/**	@brief	����Ԥ��Ƶ�ķ�Ƶϵ��
			*	@param	div: ��Ƶϵ����APB2ʱ�� / div �õ���ʱ��ʱ�ӡ�div��ֵ������ 1 - 0xff ֮�䡣
			*	@note  <no read-modify-write> 	*/
			finline void SetPrescaler(u16 div)		{(PSC = ((div!=0)?(div - 1):(div)));}
			
			/**	@brief	ʹ��/��ֹ��ʱ�����ж�Դ��DMA������Դ */
			//void InterruptCmd(TIM_InterruptSource interruptSource, FunctionalState newState)	
			//		{ DIER = (newState==DISABLE)?clearb(DIER,interruptSource):setb(DIER,interruptSource); }
			
			/**	@brief DMA ����Դʹ�� */
			//void DMA_Cmd(TIM_DMA_RequestSource dmaReq, FunctionalState newState)
			//		{ DIER = (newState==DISABLE)?clearb(DIER,dmaReq):setb(DIER,dmaReq); }
					
			/**	@brief	����жϱ�־���жϴ�����������ɺ���������־�� */
			//void ClearFlag(TIM_InterruptFlag flag)	{ SR = clearb(SR, flag); }
			
		};	/* struct TimerCommon */
		
		struct BasicTimer: public TimerCommon {
		/************************************  �¼�/�ж�/DMA����  ************************************
		***/
			finline void UpdateEventGenerate()		{ *BitBandPtr<UG>(EGR) = 1; }
			
			/**	@brief �����¼��жϣ���λ��Ϊʧ��״̬
			*	@note  <no read-modify-write> 	*/
			finline void UpdateEventIntCmd(FunctionalState newState)	{ *BitBandPtr<UpdateEventInt>(DIER) = newState; }
			finline FunctionalState UpdateEventIntCmd()		{ return  (FunctionalState)*BitBandPtr<UpdateEventInt>(DIER); }
		/***
		************************************  �¼�/�ж�/DMA����  ************************************/
			
			
			/**	@brief ��ʱ���Ļ�������
			*	@param cfg: �������ֻʹ�������³�Ա 
			*			prescaler
			*			autoReloadValue
			*	@note  <no read-modify-write> 	*/
			void BaseConfig(const TIM_BaseConfig& cfg);

		};	/* struct BasicTimer */
	
		struct AdvancedTimer: public TimerCommon {
		
		/************************************  �¼�/�ж�/DMA����  ************************************
		***/
			finline void UpdateEventGenerate()		{ *BitBandPtr<UG>(EGR) = 1; }
			
			/**	@brief �����¼��жϣ���λ��Ϊʧ��״̬
			*	@note  <no read-modify-write> 	*/
			finline void UpdateEventIntCmd(FunctionalState newState)	{ *BitBandPtr<UpdateEventInt>(DIER) = newState; }
			finline FunctionalState UpdateEventIntCmd()		{ return  (FunctionalState)*BitBandPtr<UpdateEventInt>(DIER); }
			
			/**	@brief ��������¼��жϱ�־ 
			*	@note  <no read-modify-write> 	*/
			finline void ClearUpdateEventIntFlag()		{ *BitBandPtr<UIF>(SR) = 0; }
		/***
		************************************  �¼�/�ж�/DMA����  ************************************/
			
			/**	@brief ���� TRGO ��� */
			void SetTriggerOutput(TIM_TriggerOutputSelection sel)		{ CR2 = assignf(CR2, MMS, sel); }
			
			/**	@brief ���� RepetitionCounter
			*	@note  <no read-modify-write> 	*/
			finline void SetRepetitionCounter(u8 rcr)	{ RCR = rcr; }
			
			/**	@brief ���ü�����ģʽ 
			*	@example tim1.SetCounterMode(UP_COUNTING); */
			void SetCounterMode(TIM_CounterMode mode) { CR1 = assignf(CR1,CMS|DIR,mode); }
			
			/**	@brief This cmd is disabled asynchronously by hardware assoon as the break 
				input is active. It is set by software or automatically depending on the AOE 
				bit. It is acting only on the channels which are configured in output.
			*	@note  <no read-modify-write> 	*/
			finline void MainOutputCmd(FunctionalState s)	{ *BitBandPtr<MOE>(BDTR)=s; }
			
			/**	@brief ��ʱ���Ļ������� */
			void BaseConfig(const TIM_BaseConfig& cfg);
			
			/**	@note ���úú󣬻���Ҫ���� CounterCmd �� MainOutputCmd ����������� */
			void CaptureCompare1Config(const TIM_OutputCompareConfig* const pcfg);
			void CaptureCompare2Config(const TIM_OutputCompareConfig* const pcfg);
			void CaptureCompare3Config(const TIM_OutputCompareConfig* const pcfg);
			void CaptureCompare4Config(const TIM_OutputCompareConfig* const pcfg);
		
		};	/* struct AdvancedTimer */ 
		
	}	/* namespace timer_reg */
	
	/**	@brief TIM Time Base Init structure definition
	  *	@note This structure is used with all TIMx except for TIM6 and 
      * TIM7.  
      */
	struct TIM_BaseConfig
	{
		uint16_t prescaler;				/*!< prescaler for counter clock, division factor = prescaler + 1 */

		TIM_CounterMode counterMode;	/*!< @note the basic timer only works on up counting mode */

		uint16_t autoReloadValue;		/*!< Specifies the value to be loaded into the active
											Auto-Reload Register at the next update event.
											This parameter must be a number between 0x0000 and 0xFFFF.  */ 
		
		TIM_ClockDivision_CKD clockDivForFilters;	/*!< timer clock frequency / clockDivForCounter
														= counter frequency 
														@note the filters and the dead-time generator 
														(of advanced timer) share the same clock */
		
		u8 repetitionCounter;		/*!< Specifies the repetition counter value. Each time the RCR downcounter
										reaches zero, an update event is generated and counting restarts
										from the RCR value (N).
										This means in PWM mode that (N+1) corresponds to:
										  - the number of PWM periods in edge-aligned mode
										  - the number of half PWM period in center-aligned mode
										This parameter must be a number between 0x00 and 0xFF. 
										@note This parameter is valid only for TIM1 and TIM8. */
		TIM_BaseConfig():
		  counterMode(UP_COUNTING)
		, clockDivForFilters(CKD_DIV1)
		, repetitionCounter(0){}
	};	/* struct TIM_BaseConfig */
	
	/**	@brief TIM Output Compare Config structure definition */
	struct TIM_OutputCompareConfig
	{
		TIM_OutputCompareMode mode;			/*!< (CCMRx_OCyM) */

		uint16_t compareValue;  			/*!< (CCRx) */

		FunctionalState OCx_State;			/*!< (CCER_CCxE) */

		FunctionalState OCxN_State;			/*!< if this value is DISABLE, then OCxN will keep inactive
												@note This parameter is valid only for TIM1 and TIM8. (CCER_CCxNE) */

		TIM_CC_OutputPolarity OCx_Polarity;	/*!< (CCER_CCxP) */

		TIM_CC_OutputPolarity OCxN_Polarity; /*!< @note This parameter is valid only for TIM1 and TIM8. (CCER_CCxNP) 
												@note this parameter dose not apply to Capture/Compare 4. */

		TIM_CC_OutputIdleState OCx_IdleState;				/*!< @note This parameter is valid only for TIM1 and TIM8. (CR2_OISx) */

		TIM_CC_OutputIdleState OCxN_IdleState;			/*!< @note This parameter is valid only for TIM1 and TIM8. (CR2_OISxN) */
		
		
		TIM_OutputCompareConfig():
			  OCx_State(DISABLE)
			, OCxN_State(DISABLE)
			, OCx_Polarity(OC_ACTIVE_HIGH)
			, OCxN_Polarity(OC_ACTIVE_HIGH)
			, OCx_IdleState(OC_IDLE_RESET)
			, OCxN_IdleState(OC_IDLE_RESET)
			{}
	};	/* struct TIM_OutputCompareConfig */
	
}	/* namespace periph */

#define tim1	(*(periph::timer_reg::AdvancedTimer*)TIM1)
#define tim8	(*(periph::timer_reg::AdvancedTimer*)TIM8)

#define tim6	(*(periph::timer_reg::BasicTimer*)TIM6)
#define tim7	(*(periph::timer_reg::BasicTimer*)TIM7)

#endif
