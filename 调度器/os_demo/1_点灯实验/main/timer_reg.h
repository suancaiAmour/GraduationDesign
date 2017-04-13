
namespace timer_reg{
	
	/*** 下面的内容是定时器的寄存器的位的定义，用户可不必理会 ***
	***/
	
	/**	@brief CR1寄存器的相关位定义 */
	enum TIM_CR1{
		  CKD		= BitFromTo(9,8)	/* division ratio between timer clock 
										and dead-time sampling clock */
		, ARPE		= Bit(7)			/* Auto-reload preload enable */
		, CMS		= BitFromTo(6,5)	/* Center aligned mode selection */
		, DIR		= Bit(4)	/* Direction */
		, OPM		= Bit(3)	/* one-pulse mode */
		, URS		= Bit(2)	/* update request source */
		, UDIS		= Bit(1)	/* update event disable */
		, CEN		= Bit(0)	/* Counter enable */
	};
	
	enum TIM_CR2{
		/**	@note the OISx and OISxN bits can not be modified as long as LOCK 
		level 1, 2 or 3 has been programmed (LOCK bits in TIMx_BDTR register).
		*/
		  OIS4	= Bit(14)	/* Output Idle state 4 (OC4 output) */
		, OIS3N	= Bit(13) 	/* Output Idle state 3 (OC3N output) */
		, OIS3	= Bit(12)	/* Output Idle state 3 (OC3 output) */
		, OIS2N = Bit(11)	/* Output Idle state 2 (OC2N output) */
		, OIS2	= Bit(10)	/* Output Idle state 2 (OC2 output) */
		, OIS1N	= Bit(9)	/* Output Idle state 1 (OC1N output), 
								the OIS1N state after a dead-time when MOE=0 */
		, OIS1	= Bit(8)	/* Output Idle state 1 (OC1 output) 
								the OIS1 state after a dead-time when MOE=0 */\
		
		, TI1S	= Bit(7)	/* TI1 selection: 0, TI1 = TIMx_CH1; 1, TI1 = CH1^CH2^CH3 */
		, MMS	= BitFromTo(6,4)	/* Master mode selection, trigger output selection */
		, CCDS	= Bit(3)	/* Capture/compare DMA selection -*-*-*-*/
		, CCUS	= Bit(2)	/* Capture/compare control update selection */
		, CCPC	= Bit(0)	/* Capture/compare preloaded control */
	};
	
	enum TIM_SMCR {
		  ETP	= Bit(15)	/* External trigger polarity */
		, ECE	= Bit(14)	/* External Clock Enable */
		, ETPS	= BitFromTo(13,12)	/* External trigger prescaler */
		, ETF	= BitFromTo(11,8)	/* External trigger filter */
		
		, MSM	= Bit(7)	/* master/slave mode */
		, TS	= BitFromTo(6,4)	/* Triger selection */
		, SMS	= BitFromTo(2,0)	/* Slave mode selection */
	};
	
	enum TIM_DIER {
		  TDE	= Bit(14)	/* Trigger DMA request enable */
		, COMDE	= Bit(13)	/*  COM DMA request enable */
		, CC4DE = Bit(12)	/* Capture/Compare 4 DMA request enable */
		, CC3DE = Bit(11)	/* Capture/Compare 3 DMA request enable */
		, CC2DE = Bit(10)	/* Capture/Compare 2 DMA request enable */
		, CC1DE = Bit(9)	/* Capture/Compare 1 DMA request enable */
		, UDE	= Bit(8)	/* Update DMA request enable */
		, BIE	= Bit(7)	/* Break interrupt enable */
		, TIE	= Bit(6)	/* Trigger interrupt enable */
		, COMIE	= Bit(5)	/* COM interrupt enable */
		, CC4IE	= Bit(4)	/* Capture/Compare 4 interrupt enable */
		, CC3IE	= Bit(3)	/* Capture/Compare 3 interrupt enable */
		, CC2IE	= Bit(2)	/* Capture/Compare 2 interrupt enable */
		, CC1IE	= Bit(1)	/* Capture/Compare 1 interrupt enable */
		, UIE	= Bit(0)	/* Update interrupt enable */
	};
	
	enum TIM_SR{
		  CC4OF		= Bit(12)	/* Capture/Compare 4 overcapture flag */
		, CC3OF		= Bit(11)	/* Capture/Compare 3 overcapture flag */
		, CC2OF		= Bit(10)	/* Capture/Compare 2 overcapture flag */
		, CC1OF		= Bit(9)	/* Capture/Compare 1 overcapture flag */
		, BIF		= Bit(7)	/* Break interrupt flag */
		, TIF		= Bit(6)	/* Trigger interrupt flag */
		, COMIF		= Bit(5)	/* COM interrupt flag */
		, CC4IF		= Bit(4)	/* Capture/compare 4 interrupt flag */
		, CC3IF		= Bit(3)	/* Capture/compare 3 interrupt flag */
		, CC2IF		= Bit(2)	/* Capture/compare 2 interrupt flag */
		, CC1IF		= Bit(1)	/* Capture/compare 1 interrupt flag */
		, UIF		= Bit(0)	/* Update interrupt flag */
	};
	
	/**	@brief Event generation register */
	enum TIM_EGR{
		  BG		= Bit(7)	/* Break generation */
		, TG		= Bit(6)	/* Trigger generation */
		, COMG		= Bit(5)	/* Capture/compare control update generation */
		, CC4G		= Bit(4)	/* Capture/compare 4 generation */
		, CC3G		= Bit(3)	/* Capture/compare 3 generation */
		, CC2G		= Bit(2)	/* Capture/compare 2 generation */
		, CC1G		= Bit(1)	/* Capture/compare 1 generation */
		, UG		= Bit(0)	/* Update generation */
	};
	
	/**	@brief Capture/compare mode register 1 */
	enum TIM_CCMR1 {
		  CC2S		= BitFromTo(9,8)	/* Capture/Compare 2 selection */
		, CC1S		= BitFromTo(1,0)	/* Capture/Compare 1 selection */
		
		/* Output compare mode */
		, OC2CE		= Bit(15)			/* Output Compare 2 clear enable */
		, OC2M		= BitFromTo(14,12)	/* Output Compare 2 mode */
		, OC2PE		= Bit(11)			/* Output Compare 2 preload enable */
		, OC2FE		= Bit(10)			/* Output Compare 2 fast enable */
		, OC1CE		= Bit(7)			/* Output Compare 1 clear enable */
		, OC1M		= BitFromTo(6,4)	/* Output Compare 1 mode */
		, OC1PE		= Bit(3)			/* Output Compare 1 preload enable */
		, OC1FE		= Bit(2)			/* Output Compare 1 fast enable */
		
		/* Input compare mode */
		, IC2F		= BitFromTo(15,12)	/* Input capture 2 filter */
		, IC2PSC	= BitFromTo(11,10)	/* Input capture 2 prescaler */
		, IC1F		= BitFromTo(7,4)	/* Input capture 1 filter */
		, IC1PSC	= BitFromTo(3,2)	/* Input capture 1 prescaler */
	};
	
	/**	@brief Capture/compare mode register 2 */
	enum TIM_CCMR2 {
		  CC4S		= BitFromTo(9,8)	/* Capture/Compare 4 selection */
		, CC3S		= BitFromTo(1,0)	/* Capture/Compare 3 selection */
		
		/* Output compare mode */
		, OC4CE		= Bit(15)			/* Output Compare 4 clear enable */
		, OC4M		= BitFromTo(14,12)	/* Output Compare 4 mode */
		, OC4PE		= Bit(11)			/* Output Compare 4 preload enable */
		, OC4FE		= Bit(10)			/* Output Compare 4 fast enable */
		, OC3CE		= Bit(7)			/* Output Compare 3 clear enable */
		, OC3M		= BitFromTo(6,4)	/* Output Compare 3 mode */
		, OC3PE		= Bit(3)			/* Output Compare 3 preload enable */
		, OC3FE		= Bit(2)			/* Output Compare 3 fast enable */
		
		/* Input compare mode */
		, IC4F		= BitFromTo(15,12)	/* Input capture 4 filter */
		, IC4PSC	= BitFromTo(11,10)	/* Input capture 4 prescaler */
		, IC3F		= BitFromTo(7,4)	/* Input capture 3 filter */
		, IC3PSC	= BitFromTo(3,2)	/* Input capture 3 prescaler */
	};
	
	/**	@brief capture/compare enable register */
	enum TIM_CCER {
		  CC4P		= Bit(13)	/* Capture/compare 4 output polarity */
		, CC4E		= Bit(12)	/* Capture/Compare 4 output enable */
		, CC3NP		= Bit(11)	/* Capture/Compare 3 complementary output polarity */
		, CC3NE		= Bit(10)	/* Capture/Compare 3 complementary output enabl */
		, CC3P		= Bit(9)	/* Capture/Compare 3 output polarity */
		, CC3E		= Bit(8)	/* Capture/Compare 3 output enable */
		, CC2NP		= Bit(7)	/* Capture/Compare 2 complementary output polarity */
		, CC2NE		= Bit(6)	/* Capture/Compare 2 complementary output enabl */
		, CC2P		= Bit(5)	/* Capture/Compare 2 output polarity */
		, CC2E		= Bit(4)	/* Capture/Compare 2 output enable */
		, CC1NP		= Bit(3)	/* Capture/Compare 1 complementary output polarity */
		, CC1NE		= Bit(2)	/* Capture/Compare 1 complementary output enabl */
		, CC1P		= Bit(1)	/* Capture/Compare 1 output polarity */
		, CC1E		= Bit(0)	/* Capture/Compare 1 output enable */
	};
	
	/**	@brief break and dead-time register */
	enum TIM_BDTR {
		  MOE		= Bit(15)	/* Main output enable */
		, AOE		= Bit(14)	/* Automatic output enable */
		, BRKP		= Bit(13)	/* Break polarity */
		, BRKE		= Bit(12)	/* Break enable */
		, OSSR		= Bit(11)	/* Off-state selection for Run mode */
		, OSSI		= Bit(10)	/* Off-state selection for Idle mode */
		, LOCK		= BitFromTo(9,8)	/* Lock configuration */
		, DTG		= BitFromTo(7,0)	/* Dead-time generator setup */
	};
	
	/***
	**** 上面的内容是定时器的寄存器的位的定义，用户可不必理会 **/
	
} /* namespace timer_reg */	

// 		struct GeneralTimer: public BasicTimer
// 		{
// 			public:

// 		};
		
// 		struct TIM_2_TO_5: public GeneralTimer {	/* 只是为了 RCC_APB1 和 RCC_APB2 的区分 */
// 			public:
// 			/**	@brief	public, 方便 RCC_Typedef::PeriphControl 函数的控制 */
// 			finline operator RCC_APB1_PERIPHERAL() {
// 				switch((u32)this) {
// 					case (u32)TIM2:return PERIPH_TIM2;
// 					case (u32)TIM3:return PERIPH_TIM3;
// 					case (u32)TIM4:return PERIPH_TIM4;
// 					case (u32)TIM5:return PERIPH_TIM5;
// 					default :return PERIPH_TIM2;
// 				}
// 			}
// 		};
// 		
// 		/**	@brief	advanced timer, TIM1, TIM8 */
// 		struct AdvancedTimer: public GeneralTimer {
// 			public:
// 			finline operator RCC_APB2_PERIPHERAL() {
// 				switch((u32)this) {
// 					case (u32)TIM1:return PERIPH_TIM1;
// 					case (u32)TIM8:return PERIPH_TIM8;
// 					default :return PERIPH_TIM1;
// 				}
// 			}
// 		};
