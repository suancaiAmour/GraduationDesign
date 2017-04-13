#pragma once
#ifndef RCC_TYPEDEF
#define RCC_TYPEDEF

#include "stm32f10x__COMMON.h"

namespace periph{
    
	using namespace nstd;
    
	struct RCC_ClocksFreq;
	
	#include "stm32f10x__rcc_reg.h"
	
	/**	@brief 时钟频率定义 */
	enum{
		  HSI_FREQ			= 8000000	/* HIS 频率 */
		/*
		HSE_VALUE 定义在 stm32f10x.h 里面
		*/
		, HSE_FREQ			= HSE_VALUE
		, APB1_MAX			= 36000000	/* APB1 外设最大时钟频率 */
		, SYSCLK_MAX		= 72000000	/* 系统时钟最大工作频率 */
	};
	
	/** @brief	时钟输出选择。 micorcontroler clock output select. */
	enum RCC_MCO_Selection{
		  NO_CLOCK			= 0
		, OUTPUT_SYSCLK		= BByte(100)	// System clock (SYSCLK) selected
		, OUTPUT_HSI		= BByte(101)	// HSI clock selected
		, OUTPUT_HSE		= BByte(110)	// HSE clock selected
		, OUTPUT_PLL_DIV_2	= BByte(111)	// PLL clock divided by 2 selected
	};
	
	/** @brief	系统时钟选择 */
	enum SysclkSourceSelect{
		  HSI_AS_SYSCLK		= BByte(00)
		, HSE_AS_SYSCLK		= BByte(01)
		, PLL_AS_SYSCLK		= BByte(10)
	};
	
	/** @brief	USB 时钟分频数 */
	enum USB_Prescaler{
		  PLL_DIV_1D5		= 0		// PLL clock is divided by 1.5
		, PLL_DIV_1			= 1		// PLL is not divided
	};
	
	/**	@brief PLL的时钟源 */
	enum PLL_SourceSelect{
		  HSI_DIV_2			= BByte(00)
		, HSE_DIV_1			= BByte(01)
		, HSE_DIV_2			= BByte(11)
	};
	/**	@brief pll 的乘数 */
	enum RCC_PLL_Mul{
		  PLL_MUL_2
		, PLL_MUL_3
		, PLL_MUL_4
		, PLL_MUL_5
		, PLL_MUL_6
		, PLL_MUL_7
		, PLL_MUL_8
		, PLL_MUL_9
		, PLL_MUL_10
		, PLL_MUL_11
		, PLL_MUL_12
		, PLL_MUL_13
		, PLL_MUL_14
		, PLL_MUL_15
		, PLL_MUL_16
	};
	
	enum PeriphReset{
		  ENTER_RESET	= 1		// 使外设进入复位状态
		, EXIT_RESET	= 0		// 使外设退出复位状态
	};
	
	/**	@brief	由 RCC_CR 寄存器控制的时钟
	*/
	enum RCC_CR_Cmd
	{
		  PLL					= rcc_reg::PLLON
		, CLOCK_SECURITY_SYSTEM	= rcc_reg::CSSON
		, HSE_BYPASS			= rcc_reg::HSEBYP
		, HSE					= rcc_reg::HSEON
		, HSI					= rcc_reg::HSION
	};
	
	typedef struct SysclkConfigStruct{
			/*
			sysclkSource 是选择的系统时钟源，如果
			没有选择 PLL，那么 PLL 相关配置信息会被忽略
			*/
			SysclkSourceSelect		sysclkSource; 
			PLL_SourceSelect		PLL_Source;
			u8						PLL_Mul;
	} SysclkConfig;
	

	namespace rcc_reg{
			
		class RCC_Class : protected RCC_TypeDef
		{
			protected:
			RCC_Class(){}	// 禁止外部建立对象
			
			/**	@brief	设置 PLL 的倍频数
			*	@param	mul: 可以是2, 3, 4, 5, ..., 16
			*	@return	none
			*	@note	只有在 PLL 为失能状态时这个函数的配置才有效。
			*/
			finline void SetPLL_Mul(u8 mul)		{ CFGR = assignf(CFGR, PLLMUL, mul-2);}
			
			finline void SetPLL(PLL_SourceSelect s,RCC_PLL_Mul mul) 
						{CFGR=assignf(assignf((u32)CFGR,PLLMUL,mul),PLLXTPRE|PLLSRC,s); }
						
			finline RCC_PLL_Mul GetPLL_Mul()    {return (RCC_PLL_Mul)readf(CFGR,PLLMUL);}
			
			/**	@brief	设置 PLL 的时钟源
			*	@param	s: 参考 PLL_Source 枚举类的定义。
			*	@note	只有在 PLL 为失能状态时这个函数的配置才有效。	
			*/
			finline void SetPLL_Source(PLL_SourceSelect s)	{ CFGR = assignf(CFGR, PLLXTPRE | PLLSRC, s);}
			finline PLL_SourceSelect GetPLL_Source() {return (PLL_SourceSelect)readf(CFGR,PLLXTPRE | PLLSRC);}
			
			finline bool IsPLL_Ready()		{ return checkb(CR, PLLRDY)!=0;}
			finline bool IsHSE_Ready()		{ return checkb(CR, HSERDY)!=0;}
			finline bool IsHSI_Ready()		{ return checkb(CR, HSIRDY)!=0; }
			
			public:
			
			/**	@brief	选择STM32的时钟输出
			*	@param	s: 参考RCC_MCO_Select枚举类的定义。
			*/
			void finline SetClockOutput(RCC_MCO_Selection s)		{CFGR = assignf(CFGR, MCO, s); }
			
			/**	@brief	选择系统时钟
			*	@param	s: 参考SystemClockSelection枚举类的定义。
			*	@note	选择系统时钟后，需要调用函数 GetCurSystemClockSwitch，判断其返回值
			*			一直到 GetCurSystemClockSwitch 返回设置的值为止，才算成功。
			*/
			void finline  SysclkSwitch(SysclkSourceSelect s)		{CFGR = assignf(CFGR, SW, s); }
			
			void finline SetSysclkSource(SysclkSourceSelect s) { SysclkSwitch(s); }
			
			/**	@brief	得到当前的系统时钟源
			*	@param	none
			*	@return 当前的系统时钟源。
			*/
			SysclkSourceSelect GetSysclkSwitchStatus()	{return (SysclkSourceSelect)readf(CFGR, SWS);}
			
			SysclkSourceSelect GetSysclkSource()	{return (SysclkSourceSelect)readf(CFGR, SWS);}
			
			/**	@brief	设置 AHB 时钟预分频器
			*	@param	div: 设置分频系数，参数可以是1, 2, 4, 8, 16, ..., 512
			*/
			void SetAPB1_Prescaler(u8 div);
			
			u8 GetAPB1_Prescaler();
			
			/**	@brief	设置 APB2 时钟预分频器
			*	@param	div:	设置分频系数，参数可以是1, 2, 4, 8, 16
			*/
			void SetAPB2_Prescaler(u8 div);
			
			/**	@brief	设置 AHB 时钟预分频器
			*	@param	div: 设置分频系数，参数可以是1, 2, 4, 8, 16, ..., 512
			*/
			void SetAHB_Prescaler(u8 div);
			
			/**	@brief	设置 ADC 时钟预分频器
			*	@param	div: 设置分频系数，参数可以是 2, 4, 6, 8
			*/
			void SetADC_Prescaler(u8 div)			{ CFGR = assignf(CFGR, ADCPRE, (div>>1) - 1); }
			
			/**	@brief	设置 USB 时钟预分频器
			*	@param	p: 参考 USB_Prescaler 枚举类的定义
			*	@return	none
			*/
			void SetUSB_Prescaler(USB_Prescaler p)	{CFGR = assignf(CFGR, USBPRE, p);}
			
			/**	@brief	配置PLL
			*	@param	s: PLL时钟源，参考 PLL_Source 的定义。
			*	@param	mul: PLL 倍频数
			*	@note	只有在 PLL 为失能状态时这个函数的配置才有效。
			*/
			finline void PLL_Config(PLL_SourceSelect s, u8 mul)	{SetPLL_Source(s);SetPLL_Mul(mul); }
			
			
			
			/** @brief	以默认的方式初始化RCC，其实就是把RCC的寄存器设置回复位值，
			*	建议只在系统初始化的时候使用
			*/
			void DeInit();
			
			/**	@brief	使能/禁止时钟
			*	@param	c: 参考RCC_CR_Cmd的定义
			*		@note	如果 c 直接或间接的被用作系统时钟，那么这个函数无法将它禁止。
			*		@arg	PLL: pll 的开启或关闭
			*		@arg	CLOCK_SECURITY_SYSTEM
			*		@arg	HSE: 高速外部时钟
			*		@arg	HSE_BYPASS: 高速外部时钟旁路
			*		@arg	HSI: 高速内部时钟
			*	@param	s: 使能或禁止，参考FunctionalState的定义。
			*/
			finline void ClockCmd(RCC_CR_Cmd c, FunctionalState s)		{ CR = (s==DISABLE)? clearb(CR, c): setb(CR, c);}
						
			u32 Config(const SysclkConfigStruct&  c);
	
			/**	@brief 使能/禁止相应的外设
			*	@example rcc.ClockCmd(&gpioa, ENABLE);	// 使能 GPIOA
			*	@note 暂未添加对 USB 的处理 */
			void ClockCmd(const void* const peri, FunctionalState newState);
			
			void GetClocksFreq(RCC_ClocksFreq* );
		};

	} /* namespace rcc_reg */
	
	struct RCC_ClocksFreq{
		uint32_t SYSCLK;  /*!< returns SYSCLK clock frequency expressed in Hz */
		uint32_t HCLK;    /*!< returns HCLK clock frequency expressed in Hz */
		uint32_t PCLK1;   /*!< returns PCLK1 clock frequency expressed in Hz */
		uint32_t PCLK2;   /*!< returns PCLK2 clock frequency expressed in Hz */
		uint32_t ADCCLK;  /*!< returns ADCCLK clock frequency expressed in Hz */
	};

} /* namespace periph */
using periph::rcc_reg::RCC_Class;
#define rcc					(*((RCC_Class*)RCC))
#define pRCC				((RCC_Class*)RCC)		// 编辑器对这种指针定义才有智能提示，囧

#endif
