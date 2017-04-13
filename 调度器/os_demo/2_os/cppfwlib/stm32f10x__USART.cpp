#include "stm32f10x__cpplib.h"
namespace periph{
	namespace usart_reg{
		
		/**	@brief	串口发送字符串函数
		*/
		void USART1_Class::Write(char const * str) {
			while(*str) {
				while(!(IsTransmitDataRegisterEmpty()) );
				Write(*str);
				str++;
			}
		}

		/**	@brief	串口发送字符串函数*/
		USART1_Class& USART1_Class::operator << (char const* str) {
			Write(str);
			return *this;
		}
		
		USART1_Class& USART1_Class::operator << (Manipulator m) {
			(*m)(*this);
			return *this;
		}

		void  USART1_Class::Config(USART_BaseConfig& base, u32 pclk) {
			SetBaudRate(base.baudRate, pclk);
			*BitBandPtr<PS>(CR1) = (0x01)&base.parity;
			*BitBandPtr<PCE>(CR1) = base.parity>>1;
			SetWordLen(base.wordLength);	
			USART_Cmd(ENABLE);			// 使能串口
		}
		
	}	/* namespace usart_reg */

		/**	@brief	end line */
		void endl(USART1_Class& u)
		{ u << "\r\n"; }

}	/* namespace periph */
