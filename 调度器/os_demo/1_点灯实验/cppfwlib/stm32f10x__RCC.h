#pragma once
#ifndef RCC_TYPEDEF
#define RCC_TYPEDEF

#include "stm32f10x__COMMON.h"

namespace periph{
    
	using namespace nstd;
    
	struct RCC_ClocksFreq;
	
	#include "stm32f10x__rcc_reg.h"
	
	/**	@brief ʱ��Ƶ�ʶ��� */
	enum{
		  HSI_FREQ			= 8000000	/* HIS Ƶ�� */
		/*
		HSE_VALUE ������ stm32f10x.h ����
		*/
		, HSE_FREQ			= HSE_VALUE
		, APB1_MAX			= 36000000	/* APB1 �������ʱ��Ƶ�� */
		, SYSCLK_MAX		= 72000000	/* ϵͳʱ�������Ƶ�� */
	};
	
	/** @brief	ʱ�����ѡ�� micorcontroler clock output select. */
	enum RCC_MCO_Selection{
		  NO_CLOCK			= 0
		, OUTPUT_SYSCLK		= BByte(100)	// System clock (SYSCLK) selected
		, OUTPUT_HSI		= BByte(101)	// HSI clock selected
		, OUTPUT_HSE		= BByte(110)	// HSE clock selected
		, OUTPUT_PLL_DIV_2	= BByte(111)	// PLL clock divided by 2 selected
	};
	
	/** @brief	ϵͳʱ��ѡ�� */
	enum SysclkSourceSelect{
		  HSI_AS_SYSCLK		= BByte(00)
		, HSE_AS_SYSCLK		= BByte(01)
		, PLL_AS_SYSCLK		= BByte(10)
	};
	
	/** @brief	USB ʱ�ӷ�Ƶ�� */
	enum USB_Prescaler{
		  PLL_DIV_1D5		= 0		// PLL clock is divided by 1.5
		, PLL_DIV_1			= 1		// PLL is not divided
	};
	
	/**	@brief PLL��ʱ��Դ */
	enum PLL_SourceSelect{
		  HSI_DIV_2			= BByte(00)
		, HSE_DIV_1			= BByte(01)
		, HSE_DIV_2			= BByte(11)
	};
	/**	@brief pll �ĳ��� */
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
		  ENTER_RESET	= 1		// ʹ������븴λ״̬
		, EXIT_RESET	= 0		// ʹ�����˳���λ״̬
	};
	
	/**	@brief	�� RCC_CR �Ĵ������Ƶ�ʱ��
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
			sysclkSource ��ѡ���ϵͳʱ��Դ�����
			û��ѡ�� PLL����ô PLL ���������Ϣ�ᱻ����
			*/
			SysclkSourceSelect		sysclkSource; 
			PLL_SourceSelect		PLL_Source;
			u8						PLL_Mul;
	} SysclkConfig;
	

	namespace rcc_reg{
			
		class RCC_Class : protected RCC_TypeDef
		{
			protected:
			RCC_Class(){}	// ��ֹ�ⲿ��������
			
			/**	@brief	���� PLL �ı�Ƶ��
			*	@param	mul: ������2, 3, 4, 5, ..., 16
			*	@return	none
			*	@note	ֻ���� PLL Ϊʧ��״̬ʱ������������ò���Ч��
			*/
			finline void SetPLL_Mul(u8 mul)		{ CFGR = assignf(CFGR, PLLMUL, mul-2);}
			
			finline void SetPLL(PLL_SourceSelect s,RCC_PLL_Mul mul) 
						{CFGR=assignf(assignf((u32)CFGR,PLLMUL,mul),PLLXTPRE|PLLSRC,s); }
						
			finline RCC_PLL_Mul GetPLL_Mul()    {return (RCC_PLL_Mul)readf(CFGR,PLLMUL);}
			
			/**	@brief	���� PLL ��ʱ��Դ
			*	@param	s: �ο� PLL_Source ö����Ķ��塣
			*	@note	ֻ���� PLL Ϊʧ��״̬ʱ������������ò���Ч��	
			*/
			finline void SetPLL_Source(PLL_SourceSelect s)	{ CFGR = assignf(CFGR, PLLXTPRE | PLLSRC, s);}
			finline PLL_SourceSelect GetPLL_Source() {return (PLL_SourceSelect)readf(CFGR,PLLXTPRE | PLLSRC);}
			
			finline bool IsPLL_Ready()		{ return checkb(CR, PLLRDY)!=0;}
			finline bool IsHSE_Ready()		{ return checkb(CR, HSERDY)!=0;}
			finline bool IsHSI_Ready()		{ return checkb(CR, HSIRDY)!=0; }
			
			public:
			
			/**	@brief	ѡ��STM32��ʱ�����
			*	@param	s: �ο�RCC_MCO_Selectö����Ķ��塣
			*/
			void finline SetClockOutput(RCC_MCO_Selection s)		{CFGR = assignf(CFGR, MCO, s); }
			
			/**	@brief	ѡ��ϵͳʱ��
			*	@param	s: �ο�SystemClockSelectionö����Ķ��塣
			*	@note	ѡ��ϵͳʱ�Ӻ���Ҫ���ú��� GetCurSystemClockSwitch���ж��䷵��ֵ
			*			һֱ�� GetCurSystemClockSwitch �������õ�ֵΪֹ������ɹ���
			*/
			void finline  SysclkSwitch(SysclkSourceSelect s)		{CFGR = assignf(CFGR, SW, s); }
			
			void finline SetSysclkSource(SysclkSourceSelect s) { SysclkSwitch(s); }
			
			/**	@brief	�õ���ǰ��ϵͳʱ��Դ
			*	@param	none
			*	@return ��ǰ��ϵͳʱ��Դ��
			*/
			SysclkSourceSelect GetSysclkSwitchStatus()	{return (SysclkSourceSelect)readf(CFGR, SWS);}
			
			SysclkSourceSelect GetSysclkSource()	{return (SysclkSourceSelect)readf(CFGR, SWS);}
			
			/**	@brief	���� AHB ʱ��Ԥ��Ƶ��
			*	@param	div: ���÷�Ƶϵ��������������1, 2, 4, 8, 16, ..., 512
			*/
			void SetAPB1_Prescaler(u8 div);
			
			u8 GetAPB1_Prescaler();
			
			/**	@brief	���� APB2 ʱ��Ԥ��Ƶ��
			*	@param	div:	���÷�Ƶϵ��������������1, 2, 4, 8, 16
			*/
			void SetAPB2_Prescaler(u8 div);
			
			/**	@brief	���� AHB ʱ��Ԥ��Ƶ��
			*	@param	div: ���÷�Ƶϵ��������������1, 2, 4, 8, 16, ..., 512
			*/
			void SetAHB_Prescaler(u8 div);
			
			/**	@brief	���� ADC ʱ��Ԥ��Ƶ��
			*	@param	div: ���÷�Ƶϵ�������������� 2, 4, 6, 8
			*/
			void SetADC_Prescaler(u8 div)			{ CFGR = assignf(CFGR, ADCPRE, (div>>1) - 1); }
			
			/**	@brief	���� USB ʱ��Ԥ��Ƶ��
			*	@param	p: �ο� USB_Prescaler ö����Ķ���
			*	@return	none
			*/
			void SetUSB_Prescaler(USB_Prescaler p)	{CFGR = assignf(CFGR, USBPRE, p);}
			
			/**	@brief	����PLL
			*	@param	s: PLLʱ��Դ���ο� PLL_Source �Ķ��塣
			*	@param	mul: PLL ��Ƶ��
			*	@note	ֻ���� PLL Ϊʧ��״̬ʱ������������ò���Ч��
			*/
			finline void PLL_Config(PLL_SourceSelect s, u8 mul)	{SetPLL_Source(s);SetPLL_Mul(mul); }
			
			
			
			/** @brief	��Ĭ�ϵķ�ʽ��ʼ��RCC����ʵ���ǰ�RCC�ļĴ������ûظ�λֵ��
			*	����ֻ��ϵͳ��ʼ����ʱ��ʹ��
			*/
			void DeInit();
			
			/**	@brief	ʹ��/��ֹʱ��
			*	@param	c: �ο�RCC_CR_Cmd�Ķ���
			*		@note	��� c ֱ�ӻ��ӵı�����ϵͳʱ�ӣ���ô��������޷�������ֹ��
			*		@arg	PLL: pll �Ŀ�����ر�
			*		@arg	CLOCK_SECURITY_SYSTEM
			*		@arg	HSE: �����ⲿʱ��
			*		@arg	HSE_BYPASS: �����ⲿʱ����·
			*		@arg	HSI: �����ڲ�ʱ��
			*	@param	s: ʹ�ܻ��ֹ���ο�FunctionalState�Ķ��塣
			*/
			finline void ClockCmd(RCC_CR_Cmd c, FunctionalState s)		{ CR = (s==DISABLE)? clearb(CR, c): setb(CR, c);}
						
			u32 Config(const SysclkConfigStruct&  c);
	
			/**	@brief ʹ��/��ֹ��Ӧ������
			*	@example rcc.ClockCmd(&gpioa, ENABLE);	// ʹ�� GPIOA
			*	@note ��δ��Ӷ� USB �Ĵ��� */
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
#define pRCC				((RCC_Class*)RCC)		// �༭��������ָ�붨�����������ʾ����

#endif
