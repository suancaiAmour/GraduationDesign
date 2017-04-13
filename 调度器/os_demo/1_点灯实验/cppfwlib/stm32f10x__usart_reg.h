#ifndef STM32F10X_USART_REG_H
#define STM32F10X_USART_REG_H

	namespace usart_reg {
		using namespace periph::rcc_reg;
		
		enum USART_SR{				//status register
			  CTS	= Bit(9)
			, LBD	= Bit(8)		// LIN Break detection flag
			, TXE	= Bit(7)		// Transmit data empty
			, TC	= Bit(6)		// Transmit complete
			, RXNE	= Bit(5)		// Read data register not empty
			, ILDE	= Bit(4)		// IDLE line dected
			, ORE	= Bit(3)		// Overrun error
			, NE	= Bit(2)		// noise error flag
			, FE	= Bit(1)		// framing error
			, PE	= Bit(0)		// parity error
		};		
		
		enum USART_BRR{				// Baud rate register
			DIV_Mantissa = BitFromTo(15, 4)
			, DIV_Fraction = BitFromTo(3, 0)
		};
		
		enum USART_CR1{				// control register 1
			  UE	= Bit(13)		// USART enable
			// Word length, 0: 1 Start bit, 8 Data bits, n Stop bit	1: 1 Start bit, 9 Data bits, n Stop bit
			, M		= Bit(12)		
			, WAKE	= Bit(11)	// Wakeup method, 0: Idle Line, 1: Address Mark
			, PCE	= Bit(10)		// parity control enable
			, PS	= Bit(9)		// Parity selection
			, PEIE	= Bit(8)		// PE interrupt enable
			, TXEIE	= Bit(7)		// TXE interrupt enable
			, TCIE	= Bit(6)		// transmit complete interrupt enable
			, RXNEIE= Bit(5)		// RXNE interrupt enable
			, IDLEIE= Bit(4)		// IDLE interrupt enable
			, TE	= Bit(3)		// Transmit enable
			, RE	= Bit(2)		// Receiver enable
			, RWU	= Bit(1)		// Receiver wakeup
			, SBK	= Bit(0)		// SendBreck
		};
		
		enum USART_CR2{
			LINEN 	= Bit(13)				// LIN mode enable
			, STOP 	= BitFromTo(13, 12)	// stop bits
			, CLKEN = Bit(11)			// clock enable, allows the user to enable the CK pin.
			, CPOL 	= Bit(10)			// clock polarity
			, CPHA 	= Bit(9)				// Clock phase	
		
			//	This bit allows the user to select whether the clock pulse associated with the last data bit 
			//	transmitted (MSB) has to be output on the CK pin in synchronous mode. 
			, LBCL 	= Bit(8)				// last bit clock pulse
		
			, LBDIE = Bit(6)			// lin break detection enable
			, LBDL 	= Bit(5)				// lin break detection length
			, ADD 	= BitFromTo(3, 0)		// address of the usart node
		};
		
		enum USART_CR3{
			CTSIE 	= Bit(10)				// CTS interrupt enable, This bit is not available for UART4 & UART5.
			//if CTS mode enabled, data is only transmitted when the nCTS input is asserted (tied to 0)
			, CTSE	= Bit(9)				// CTS enable, This bit is not available for UART4 & UART5.
			//if RTS interrupt enabled, data is only reques ted when there is space in the receive buffer.
			, RTSE 	= Bit(8)				// RTS enable
			, DMAT 	= Bit(7)				// DMA enable transmitter
			, DMAR 	= Bit(6)				// DMA enable receiver
			, SCEN 	= Bit(5)				// Smartcard mode enable
			, NACK 	= Bit(4)				// Smartcard NACK enable
			, HDSEL = Bit(3)			// Half-duplex selection, Selection of Single-wire Half-duplex mode 
			, IRLP 	= Bit(2)				// IrDA low-power
			, IREN 	= Bit(1)				// IrDA mode enable
			, EIE	= Bit(0)				//
		};
		
		enum USART_GTPR{
			  GT	= BitFromTo(15, 8)		// Guard time value
			, PSC	= BitFromTo(7, 0)		// Prescaler value
		};
	} /* namespace usart_reg */

#endif
