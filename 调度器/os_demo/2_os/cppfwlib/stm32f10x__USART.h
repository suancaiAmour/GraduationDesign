#ifndef __cplusplus
#define USART_TYPEDEF_H
#endif
#ifndef USART_TYPEDEF_H
#define USART_TYPEDEF_H

#include "stm32f10x__rcc.h"

namespace	periph{
	struct USART_BaseConfig;
	
	#include "stm32f10x__usart_reg.h"
	
	/** @brief ֹͣλѡ�� */
	enum USART_StopBits{
		  STOP_BITS_1 = BByte(00)				// 1 stop bit
		, STOP_BITS_0D5 = BByte(01)				// 0.5 stop bits
		, STOP_BITS_2 = BByte(10)				// 2 stop bits
		, STOP_BITS_1D5 = BByte(11)				// 1.5 stop bits
	};

	/**	@brief ���ݳ������ã�ע��У��λҲ���� data bit */
	enum USART_WordLength{
		  DATA_BITS_8 = 0 		// 8 ������λ
		, DATA_BITS_9 = 1		// 9 ������λ
	};
	
	/** @brief ��żУ��ѡ�� */
	enum USART_Parity {			/* CR1_PCE CR1_PS */
		  EVEN 				= BByte(10)	
		, ODD 				= BByte(11)
		, NO_PARITY_CHECK	= BByte(00)
	};
	
	/** @brief USART ��״̬λ */
	enum USART_State {
		  Received = Bit(5)
		, TransmitComplete = Bit(6)
	};
	
	/** @brief USART ���ж�Դ */
	enum USART_InterruptSource {
		  ParityErrorInt = Bit(8)
		, TransMitDataRegEmptyInt = Bit(7)
		, TransmitCompleteInt = Bit(6)
		, ReceivedInt_OverRunErrInt = Bit(5)
		, IdleInt = Bit(4)
	};
	
	namespace usart_reg{
		
		typedef void (*Manipulator)(struct USART1_Class& u );
		
		struct USART1_Class: public USART_TypeDef
		{
			private:
			/**	@param ��������λ�ĳ��ȣ��ο� @ref USART_WordLength �Ķ��塣*/
			void SetWordLen(USART_WordLength l)					{*BitBandPtr<M>(CR1)=l; }
			
			/**	@param ����ֹͣλ */
			void SetStopBits(USART_StopBits s)					{ CR2 = assignf(CR2, STOP, s); }
			
			/**	@brief ���ò����ʡ�
			*	@param br: ϣ�����õĲ����ʡ��� bps Ϊ��λ��
			*	@param pclk: USART �Ĺ���ʱ��Ƶ�ʡ��� Hz Ϊ��λ��
			*/
			void finline SetBaudRate(u32 br,u32 pclk)	{ BRR = pclk / br; }
			
			public:
							
			/** @brief USARTʹ��/ʧ��
			*	@note ͨ�� RCC ʹ�� USART ����Ҫͨ���������ʹ�ܣ�����ʹ USART ����
			*	@note ʹ�� USART �󻹵ð�����Ҫʹ�ú��� TransmitterCmd �� ReceiverCmd ��Ӧ��ʹ�� USART �ķ������ͽ����� */
			void finline USART_Cmd(FunctionalState e)	{ *BitBandPtr<UE>(CR1)=e; }
			
			/**	@brief ���ڷ�����ʹ��/ʧ��
			*	@note  <no read-modify-write> 	*/
			void finline TransmitterCmd(FunctionalState e)	{ *BitBandPtr<TE>(CR1)=e; }
			/** @brief ���ڽ�������ʹ��/ʧ��
			*	@note  <no read-modify-write> 	*/
			void ReceiverCmd(FunctionalState e)				{ *BitBandPtr<RE>(CR1)=e; }
			
			public:
		
			/** @brief ���ô��ڵ��ж�
			*	uarttInt	�� enum USART_Interrupt �еĳ�Ա���õ���
			*	e			ʹ��(ENABLE)/ʧ��(DISABLE)
			*/
			void InterruptCmd(USART_InterruptSource usartInt, FunctionalState e = ENABLE)	
				{ CR1 = (e==DISABLE)?clearb(CR1, usartInt):setb(CR1, usartInt);}

			/** @brief  �����ݼĴ���дһ���ֽڵ����� */
			void finline Write(u8 dat)			{DR = dat;}
			/** @brief �����ݼĴ�����һ���ֽڵ����� */
			u8 finline Read()					{ u8 temp = DR; return temp; }

			/**	@brief �ڹرմ��ڹ��ܵ�ʱ�򣬱��������������鵽�����Ѿ�������Ϻ󣬲��ܹرմ��ڣ�
			*	@note  <no read-modify-write> 	*/
			bool finline TransmitComplete()				{return  *BitBandPtr<TC>(SR); }
			/**	@brief ���USART��״̬����������棬����ʹ��Writeд������
			*	@note  <no read-modify-write> 	*/
			bool finline IsTransmitDataRegisterEmpty()	{ return *BitBandPtr<TXE>(SR); }
			/*  ����Ƿ��н��յ����� */
			bool ReceivedData()				{return checkb(SR, RXNE);}

			/* �⼸�������Ѿ��Ƚ���ʹ���ˡ� */
			void Write(char const* str);
			finline USART1_Class&  operator << (char c)	{ while(!IsTransmitDataRegisterEmpty());Write(c);return *this; }
			USART1_Class&  operator << (Manipulator m);
			USART1_Class& operator << (char const* str);
			
			finline operator RCC_APB2_PERIPHERAL()	{return PERIPH_USART1;}
			
			/**	@brief USART �����������ã�������ɺ󣬻�Ҫʹ����Ӧ��
			*	������/����������ʹ�ô��� ( TransmitterCmd �� ReceiverCmd ����)
			*	@note ������������Զ����� USART_Cmd �����������ڵ����������֮ǰ
			*	����ͨ�� RCC ʹ����Ӧ�Ĵ��ڡ�
			*	@param base: �μ� USART_BaseConfig �ṹ�塣
			*	pclk: USART ʹ�õ�ʱ��Ƶ�� */
			void Config(USART_BaseConfig& base, u32 pclk);
		};
	}	
	using periph::usart_reg::USART1_Class;
	void endl(USART1_Class& u);

	struct USART_BaseConfig{
		
		uint32_t baudRate;  

		USART_WordLength wordLength;

		USART_StopBits stopBits;

		USART_Parity parity;
		
		USART_BaseConfig(): baudRate(9600), wordLength(DATA_BITS_8), stopBits(STOP_BITS_1), parity(NO_PARITY_CHECK){}
	};
	
} /* namespace usart_reg */

#define usart1				(*((periph::usart_reg::USART1_Class*)USART1 ))


#endif
