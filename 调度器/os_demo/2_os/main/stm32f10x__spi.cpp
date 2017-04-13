#include "stm32f10x__spi.h"

namespace periph {
	namespace spi_reg{

		void SPI_Class::Config(const SPI_Config& cfg) {
			u32 temp = CR1;
			temp = assignb(temp,CR1_BIDIMODE|CR1_BIDIOE|CR1_RXONLY, cfg.direction);
			temp = assignf(temp, CR1_MSTR, cfg.mode);
			temp = assignf(temp, CR1_DFF, cfg.dataSize);
			temp = assignf(temp, CR1_CPOL, cfg.clockPolarity);
			temp = assignf(temp, CR1_CPHA, cfg.clockPhase);
			temp = assignf(temp, CR1_SSM, cfg.softwareSlaveManage);
			temp = assignf(temp, CR1_BR, cfg.baudRate);
			temp = assignf(temp, CR1_LSBFIRST, cfg.firstBit);
			if((cfg.softwareSlaveManage==ENABLE) &&
			   (cfg.mode == SPI_MASTER) )
				temp |= CR1_SSI;
			CR1 = temp;
			CRCPR = cfg.CRC_Polynominal;
			SPI_Cmd(ENABLE);
		} /* void SPI_Class::Config(SPI_ConfigStruct const &cfg) */
		
	} /* namespace spi_reg */
}	/* namespace periph */
