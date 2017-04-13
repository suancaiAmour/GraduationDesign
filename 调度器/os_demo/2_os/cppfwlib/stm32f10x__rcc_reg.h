
namespace rcc_reg{
	/**	@brief	RCC_CR 寄存器的位定义
	*/
	enum RCC_CR{	
		  PLLRDY	= Bit(25)			// r, PLL clock ready flag
		, PLLON		= Bit(24)			// rw, PLL enable
		, CSSON		= Bit(19)			// rw, Clock security system enable
		, HSEBYP	= Bit(18)			// rw, External high-speed clock bypass
		, HSERDY	= Bit(17)			// r, External high-speed clock ready flag
		, HSEON		= Bit(16)			// rw, HSE clock enable
		, HSICAL	= BitFromTo(15, 8)	// r, Internal high-speed clock calibration
		, HSITRIM	= BitFromTo(7, 3)	// rw, Internal high-speed clock trimming
		, HSIRDY	= Bit(1)			// r, Internal high-speed clock ready flag
		, HSION		= Bit(0)			// rw, Internal high-speed clock enable

		, CR_PLLRDY		= Bit(25)			// r, PLL clock ready flag
		, CR_PLLON		= Bit(24)			// rw, PLL enable
		, CR_CSSON		= Bit(19)			// rw, Clock security system enable
		, CR_HSEBYP		= Bit(18)			// rw, External high-speed clock bypass
		, CR_HSERDY		= Bit(17)			// r, External high-speed clock ready flag
		, CR_HSEON		= Bit(16)			// rw, HSE clock enable
		, CR_HSICAL		= BitFromTo(15, 8)	// r, Internal high-speed clock calibration
		, CR_HSITRIM	= BitFromTo(7, 3)	// rw, Internal high-speed clock trimming
		, CR_HSIRDY		= Bit(1)			// r, Internal high-speed clock ready flag
		, CR_HSION		= Bit(0)			// rw, Internal high-speed clock enable
	};
	
	/**	@brief	RCC_CFGR 寄存器的位定义
	*/
	enum RCC_CFGR{		// configuration register
		  MCO			= BitFromTo(26, 24)	// rw, Microntroller clock output
		, USBPRE		= Bit(22)			// USB prescaler
		, PLLMUL		= BitFromTo(21, 18)	// PLL multiplication factor
		, PLLXTPRE		= Bit(17)			// HSE devider for pll entry
		, PLLSRC		= Bit(16)			// PLL entry clock source,  This bit can be written only when PLL is disabled.
		, ADCPRE		= BitFromTo(15, 14)	// ADC prescaler 
		, PPRE2			= BitFromTo(13, 11)	// APB high-speed prescaler (APB2)
		, PPRE1			= BitFromTo(10, 8)	// APB low-speed prescaler (APB1)
		, HPRE			= BitFromTo(7, 4)	// AHB prescaler
		, SWS			= BitFromTo(3, 2)	// System clock switch status
		, SW			= BitFromTo(1, 0)	// System clock switch
		
		, CFGR_MCO			= BitFromTo(26, 24)	// rw, Microntroller clock output
		, CFGR_USBPRE		= Bit(22)			// USB prescaler
		, CFGR_PLLMUL		= BitFromTo(21, 18)	// PLL multiplication factor
		, CFGR_PLLXTPRE		= Bit(17)			// HSE devider for pll entry
		, CFGR_PLLSRC		= Bit(16)			// PLL entry clock source,  This bit can be written only when PLL is disabled.
		, CFGR_ADCPRE		= BitFromTo(15, 14)	// ADC prescaler 
		, CFGR_PPRE2		= BitFromTo(13, 11)	// APB high-speed prescaler (APB2)
		, CFGR_PPRE1		= BitFromTo(10, 8)	// APB low-speed prescaler (APB1)
		, CFGR_HPRE			= BitFromTo(7, 4)	// AHB prescaler
		, CFGR_SWS			= BitFromTo(3, 2)	// System clock switch status
		, CFGR_SW			= BitFromTo(1, 0)	// System clock switch
	};
	
	/**	@brief	RCC_CIR 寄存器的位定义
	*/
	enum RCC_CIR{		// Clock interrupt register
		  CSSC			= Bit(23)			// Clock security system interrupt clear
		, PLLRDYC		= Bit(20)			// PLL ready interrupt clear
		, HSERDYC		= Bit(19)			// HSE ready interrupt clear
		, HSIRDYC		= Bit(18)			// HSI ready interrupt clear
		, LSERDYC		= Bit(17)			// LSE ready interrupt clear
		, LSIRDYC		= Bit(16)			// LSI ready interrupt clear
		, PLLRDYIE		= Bit(12)			// PLL ready interrupt enable
		, HSERDYIE		= Bit(11)			// HSE ready interrupt enable
		, HSIRDYIE		= Bit(10)			// HSI ready interrupt enable
		, LSERDYIE		= Bit(9)			// LSE ready interrupt enable
		, LSIRDYIE		= Bit(8)			// LSI ready interrupt enable
		, CSSF			= Bit(7)			// clock security system interrupt flag
		, PLLRDYF		= Bit(4)			// PLL ready interrupt flag
		, HSERDYF		= Bit(3)			// HSE ready interrupt flag
		, HSIRDF		= Bit(2)			// HSI ready interrupt flag
		, LSERDYF		= Bit(1)			// LSE ready interrupt flag
		, LSIRDYF		= Bit(0)			// LSI ready interrupt flag
		
		, CIR_CSSC			= Bit(23)			// Clock security system interrupt clear
		, CIR_PLLRDYC		= Bit(20)			// PLL ready interrupt clear
		, CIR_HSERDYC		= Bit(19)			// HSE ready interrupt clear
		, CIR_HSIRDYC		= Bit(18)			// HSI ready interrupt clear
		, CIR_LSERDYC		= Bit(17)			// LSE ready interrupt clear
		, CIR_LSIRDYC		= Bit(16)			// LSI ready interrupt clear
		, CIR_PLLRDYIE		= Bit(12)			// PLL ready interrupt enable
		, CIR_HSERDYIE		= Bit(11)			// HSE ready interrupt enable
		, CIR_HSIRDYIE		= Bit(10)			// HSI ready interrupt enable
		, CIR_LSERDYIE		= Bit(9)			// LSE ready interrupt enable
		, CIR_LSIRDYIE		= Bit(8)			// LSI ready interrupt enable
		, CIR_CSSF			= Bit(7)			// clock security system interrupt flag
		, CIR_PLLRDYF		= Bit(4)			// PLL ready interrupt flag
		, CIR_HSERDYF		= Bit(3)			// HSE ready interrupt flag
		, CIR_HSIRDF		= Bit(2)			// HSI ready interrupt flag
		, CIR_LSERDYF		= Bit(1)			// LSE ready interrupt flag
		, CIR_LSIRDYF		= Bit(0)			// LSI ready interrupt flag
	};
	
	/** @brief	RCC_AHB_PERIPHERAL 寄存器的位定义
	*/
	enum RCC_AHB_PERIPHERAL
	{
		  PERIPH_SDIO	= Bit(10)
		, PERIPH_FSMC	= Bit(8)
		, PERIPH_CRC	= Bit(6)
		, PERIPH_FLITF	= Bit(4)
		, PERIPH_SRAM	= Bit(2)
		, PERIPH_DMA2	= Bit(1)
		, PERIPH_DMA1	= Bit(0)
	};
	
	/** @brief	APB2 外设，用来作ClockCmd函数的参数
	*	@note	大部分外设结构体都已经重载了 RCC_APB2_PERIPHERAL() 操作符，
	*			所以不必直接使用这里的 identifier作为参数，
	*			例如，使能GPIOA的时钟: rcc.PeriphControl(gpioa, ENABLE);
	*/
	enum RCC_APB2_PERIPHERAL{
	  PERIPH_TIM11		= Bit(21)
	, PERIPH_TIM10		= Bit(20)
	, PERIPH_TIM9		= Bit(19)
	, PERIPH_USART1		= Bit(14)		// USART1 clock enable
	, PERIPH_TIM8		= Bit(13)
	, PERIPH_SPI1		= Bit(12)
	, PERIPH_TIM1		= Bit(11)
	, PERIPH_GPIOG		= Bit(8)
	, PERIPH_GPIOF		= Bit(7)
	, PERIPH_GPIOE		= Bit(6)
	, PERIPH_GPIOD		= Bit(5)
	, PERIPH_GPIOC		= Bit(4)
	, PERIPH_GPIOB		= Bit(3)
	, PERIPH_GPIOA		= Bit(2)
	, PERIPH_AFIO		= Bit(0)	// Alternate function I/O clock enable
	};
	
	enum RCC_APB2ENR{
		  APB2ENR_TIM11EN	= Bit(21)
		, APB2ENR_TIM10EN	= Bit(20)
		, APB2ENR_TIM9EN	= Bit(19)
		, APB2ENR_ADC3EN	= Bit(15)
		, APB2ENR_USART1EN	= Bit(14)		// USART1 clock enable
		, APB2ENR_TIM8EN	= Bit(13)
		, APB2ENR_SPI1EN	= Bit(12)
		, APB2ENR_TIM1EN	= Bit(11)
		, APB2ENR_ADC2EN	= Bit(10)
		, APB2ENR_ADC1EN	= Bit(9)
		, APB2ENR_GPIOGEN	= Bit(8)
		, APB2ENR_GPIOFEN	= Bit(7)
		, APB2ENR_GPIOEEN	= Bit(6)
		, APB2ENR_GPIODEN	= Bit(5)
		, APB2ENR_GPIOCEN	= Bit(4)
		, APB2ENR_GPIOBEN	= Bit(3)
		, APB2ENR_GPIOAEN	= Bit(2)
		, APB2ENR_AFIOEN	= Bit(0)	// Alternate function I/O clock enable
	};
	
	/** @brief	APB1 外设，用来作PeriphControl函数的参数
	*	@note	大部分外设结构体都已经重载了 RCC_APB1_PERIPHERAL() 操作符，
	*			所以不必直接使用这里的 identifier作为参数，
	*			例如，使能WWDG的时钟: rcc.PeriphControl(wwdg, ENABLE);
	*/
	enum RCC_APB1_PERIPHERAL{
		  PERIPH_WWDG	= Bit(11)		// window watch dog
		, PERIPH_TIM7	= Bit(5)
		, PERIPH_TIM6	= Bit(4)
		, PERIPH_TIM5	= Bit(3)
		, PERIPH_TIM4	= Bit(2)
		, PERIPH_TIM3	= Bit(1)
		, PERIPH_TIM2	= Bit(0)
	};
	
	enum RCC_APB1ENR{
		  APB1ENR_DACEN		= Bit(29)
		, APB1ENR_PWREN		= Bit(28)
		, APB1ENR_BKPEN		= Bit(27)
		, APB1ENR_CAN1EN	= Bit(25)
		, APB1ENR_USBEN		= Bit(23)
		, APB1ENR_I2C2EN	= Bit(22)
		, APB1ENR_I2C1EN	= Bit(21)
		, APB1ENR_UART5EN	= Bit(20)
		, APB1ENR_UART4EN	= Bit(19)
		, APB1ENR_USART3EN	= Bit(18)
		, APB1ENR_USART2EN	= Bit(17)
		, APB1ENR_SPI3EN	= Bit(15)
		, APB1ENR_SPI2EN	= Bit(14)
		, APB1ENR_WWDGEN	= Bit(11)		// window watch dog
		, APB1ENR_TIM14EN	= Bit(8)
		, APB1ENR_TIM13EN	= Bit(7)
		, APB1ENR_TIM12EN	= Bit(6)
		, APB1ENR_TIM7EN	= Bit(5)
		, APB1ENR_TIM6EN	= Bit(4)
		, APB1ENR_TIM5EN	= Bit(3)
		, APB1ENR_TIM4EN	= Bit(2)
		, APB1ENR_TIM3EN	= Bit(1)
		, APB1ENR_TIM2EN	= Bit(0)
	};
}
