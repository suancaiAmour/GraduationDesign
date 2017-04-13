#ifndef __cplusplus
#define USART_TYPEDEF_H
#endif
#ifndef USART_TYPEDEF_H
#define USART_TYPEDEF_H

#include "stm32f10x__rcc.h"

namespace	periph{
	struct USART_BaseConfig;
	
	#include "stm32f10x__usart_reg.h"
	
	/** @brief 停止位选择 */
	enum USART_StopBits{
		  STOP_BITS_1 = BByte(00)				// 1 stop bit
		, STOP_BITS_0D5 = BByte(01)				// 0.5 stop bits
		, STOP_BITS_2 = BByte(10)				// 2 stop bits
		, STOP_BITS_1D5 = BByte(11)				// 1.5 stop bits
	};

	/**	@brief 数据长度设置，注意校验位也属于 data bit */
	enum USART_WordLength{
		  DATA_BITS_8 = 0 		// 8 个数据位
		, DATA_BITS_9 = 1		// 9 个数据位
	};
	
	/** @brief 奇偶校验选择 */
	enum USART_Parity {			/* CR1_PCE CR1_PS */
		  EVEN 				= BByte(10)	
		, ODD 				= BByte(11)
		, NO_PARITY_CHECK	= BByte(00)
	};
	
	/** @brief USART 的状态位 */
	enum USART_State {
		  Received = Bit(5)
		, TransmitComplete = Bit(6)
	};
	
	/** @brief USART 的中断源 */
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
			/**	@param 设置数据位的长度，参考 @ref USART_WordLength 的定义。*/
			void SetWordLen(USART_WordLength l)					{*BitBandPtr<M>(CR1)=l; }
			
			/**	@param 设置停止位 */
			void SetStopBits(USART_StopBits s)					{ CR2 = assignf(CR2, STOP, s); }
			
			/**	@brief 设置波特率。
			*	@param br: 希望设置的波对率。以 bps 为单位。
			*	@param pclk: USART 的工作时钟频率。以 Hz 为单位。
			*/
			void finline SetBaudRate(u32 br,u32 pclk)	{ BRR = pclk / br; }
			
			public:
							
			/** @brief USART使能/失能
			*	@note 通过 RCC 使能 USART 后还需要通过这个函数使能，才能使 USART 工作
			*	@note 使能 USART 后还得按照需要使用函数 TransmitterCmd 和 ReceiverCmd 相应的使能 USART 的发送器和接收器 */
			void finline USART_Cmd(FunctionalState e)	{ *BitBandPtr<UE>(CR1)=e; }
			
			/**	@brief 串口发送器使能/失能
			*	@note  <no read-modify-write> 	*/
			void finline TransmitterCmd(FunctionalState e)	{ *BitBandPtr<TE>(CR1)=e; }
			/** @brief 串口接收器的使能/失能
			*	@note  <no read-modify-write> 	*/
			void ReceiverCmd(FunctionalState e)				{ *BitBandPtr<RE>(CR1)=e; }
			
			public:
		
			/** @brief 设置串口的中断
			*	uarttInt	由 enum USART_Interrupt 中的成员相或得到。
			*	e			使能(ENABLE)/失能(DISABLE)
			*/
			void InterruptCmd(USART_InterruptSource usartInt, FunctionalState e = ENABLE)	
				{ CR1 = (e==DISABLE)?clearb(CR1, usartInt):setb(CR1, usartInt);}

			/** @brief  向数据寄存器写一个字节的数据 */
			void finline Write(u8 dat)			{DR = dat;}
			/** @brief 从数据寄存器读一个字节的数据 */
			u8 finline Read()					{ u8 temp = DR; return temp; }

			/**	@brief 在关闭串口功能的时候，必须用这个函数检查到数据已经发送完毕后，才能关闭串口！
			*	@note  <no read-modify-write> 	*/
			bool finline TransmitComplete()				{return  *BitBandPtr<TC>(SR); }
			/**	@brief 检测USART的状态，如果返回真，则不能使用Write写入数据
			*	@note  <no read-modify-write> 	*/
			bool finline IsTransmitDataRegisterEmpty()	{ return *BitBandPtr<TXE>(SR); }
			/*  检测是否有接收到数据 */
			bool ReceivedData()				{return checkb(SR, RXNE);}

			/* 这几个函数已经比较少使用了。 */
			void Write(char const* str);
			finline USART1_Class&  operator << (char c)	{ while(!IsTransmitDataRegisterEmpty());Write(c);return *this; }
			USART1_Class&  operator << (Manipulator m);
			USART1_Class& operator << (char const* str);
			
			finline operator RCC_APB2_PERIPHERAL()	{return PERIPH_USART1;}
			
			/**	@brief USART 基本参数配置，配置完成后，还要使能相应的
			*	接收器/发送器才能使用串口 ( TransmitterCmd 和 ReceiverCmd 函数)
			*	@note 这个函数最后会自动调用 USART_Cmd 函数，但是在调用这个函数之前
			*	请先通过 RCC 使能相应的串口。
			*	@param base: 参见 USART_BaseConfig 结构体。
			*	pclk: USART 使用的时钟频率 */
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
