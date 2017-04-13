#ifndef STM32F10X__SPI_REG_H
#define STM32F10X__SPI_REG_H

namespace spi_reg{

	enum SPI_CR1 {
		  CR1_BIDIMODE		= Bit(15)
		, CR1_BIDIOE		= Bit(14)
		, CR1_CRCEN			= Bit(13)
		, CR1_CRCNEXT		= Bit(12)
		, CR1_DFF			= Bit(11)
		, CR1_RXONLY		= Bit(10)
		, CR1_SSM			= Bit(9)
		, CR1_SSI			= Bit(8)
		, CR1_LSBFIRST		= Bit(7)
		, CR1_SPE			= Bit(6)
		, CR1_BR			= BitFromTo(5,3)
		, CR1_MSTR			= Bit(2)
		, CR1_CPOL			= Bit(1)
		, CR1_CPHA			= Bit(0)
	};
	
	enum SPI_CR2 {
		  CR2_TXEIE		= Bit(7)
		, CR2_RXNEIE	= Bit(6)
		, CR2_ERRIE		= Bit(5)
		, CR2_SSOE		= Bit(2)
		, CR2_TXDMAEN	= Bit(1)
		, CR2_RXDMAEN	= Bit(0)
	};
	
	enum SPI_SR {
		  SR_BSY			= Bit(7)
		, SR_OVR			= Bit(6)
		, SR_MODF			= Bit(5)
		, SR_CRCERR			= Bit(4)
		, SR_UDR			= Bit(3)
		, SR_CHSIDE			= Bit(2)
		, SR_TXE			= Bit(1)
		, SR_RXNE			= Bit(0)
	};
	
} /* namespace spi_reg */

#endif
