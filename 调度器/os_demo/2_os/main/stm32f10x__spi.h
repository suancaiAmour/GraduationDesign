#ifndef STM32F10X_SPI_H
#define STM32F10X_SPI_H

#include "stm32f10x__COMMON.h"

namespace periph{

	struct SPI_Config;
	
	#include "stm32f10x__spi_reg.h"
	
	enum SPI_Direction {			/* CR1_BIDIMODE, CR1_BIDIOE, CR1_RXONLY 这个的格式比较特殊点，不直接使用 assignf */
		  UNIDIRECTION_DUPLEX		= 0
		, UNIDIRECTION_RX			= spi_reg::CR1_RXONLY
		, BIDIRECTION_RX			= spi_reg::CR1_BIDIMODE
		, BIDIRECTION_TX			= spi_reg::CR1_BIDIMODE | spi_reg::CR1_BIDIOE
	};
	
	enum SPI_Mode{					/* CR1_MSTR */
		  SPI_SLAVE					= 0
		, SPI_MASTER				= 1
	};
	
	enum SPI_DataFrameFormat{		/* CR1_DFF */
		  DATA_FRAME_8BITS			= 0
		, DATA_FRAME_16BITS			= 1
	};
	
	enum SPI_ClockPolarity {		/* CR1_CPOL */
		  IDLE_CLK_LOW				= 0
		, IDLE_CLK_HIGH				= 1
	};
	
	enum SPI_BaudRateControl{		/* CR1_BR */
		  SPI_PCLK_DIV_2			= BByte(000)
		, SPI_PCLK_DIV_4			= BByte(001)
		, SPI_PCLK_DIV_8			= BByte(010)
		, SPI_PCLK_DIV_16			= BByte(011)
		, SPI_PCLK_DIV_32			= BByte(100)
		, SPI_PCLK_DIV_64			= BByte(101)
		, SPI_PCLK_DIV_128			= BByte(110)
		, SPI_PCLK_DIV_256			= BByte(111)
	};
	
	enum SPI_FirstBit{				/*	CR1_LSBFIRST */
		  SPI_MSB_FIRST				= 0
		, SPI_LSB_FIRST				= 1
	};
	
	enum SPI_ClockPhase {			/* CR1_CPHA */
		  FIRST_EDGE_CAPTURE			= 0
		, SECOND_EDGE_CAPTURE			= 1
	};
	
	namespace spi_reg{
	
		class SPI_Class: public SPI_TypeDef {
		public:
			
			/**	@brief SPI 配置，在函数末尾会自动调用 SPI_Cmd(ENABLE) */
			void Config(const SPI_Config& cfg);
			
			/**	@brief SPI 使能/禁止
			*	@note  <no read-modify-write> 	*/
			finline void SPI_Cmd(const FunctionalState newState) { *PRegBit(CR1,SPE)=newState; }
			
			/**	@brief 设置波特率 */
			void SPI_SetBaudRate(const SPI_BaudRateControl br) { CR1=assignf(CR1,CR1_BR,br); }	
			
			/**	@brief 读写字节 */
			u16 SPI_MasterReadWrite(const u16 data) {
				/* 只要函数退出的时候数据已经传输完成，就不需要去检查 TXE 位 */
				DR=data;			/* 发送一个byte */
				uint32_t const volatile * const pRXNE = PRegBit(SR,RXNE);
				while(*pRXNE==0); /* 等待传输完成 */   
				return DR;			
			}
			
			//void SPI_WriteMultiBytes(const
			
		}; /* SPI_Class */
	
	} /* namespace spi_reg */
	
	struct SPI_Config {
		
		SPI_Direction direction;
		SPI_Mode mode;
		SPI_DataFrameFormat dataSize; 
		SPI_ClockPolarity clockPolarity; 
		SPI_ClockPhase clockPhase;
		FunctionalState softwareSlaveManage;
		SPI_BaudRateControl baudRate;		/*!< SPI 波特率的设置 */
		SPI_FirstBit firstBit;
		u16 CRC_Polynominal;	/* CRC generator polynominal，如果不使用CRC功能，可以忽略它 */
		
		SPI_Config():
			  direction(UNIDIRECTION_DUPLEX)
			, mode(SPI_MASTER)
			, dataSize(DATA_FRAME_8BITS)
			, clockPolarity(IDLE_CLK_HIGH)
			, clockPhase(SECOND_EDGE_CAPTURE)
			, softwareSlaveManage(ENABLE)
			, baudRate(SPI_PCLK_DIV_256) /* 默认最低速 */
			, firstBit(SPI_MSB_FIRST)
			, CRC_Polynominal(0x07){}
	}; /* struct SPI_ConfigStruct */
	
} /* namespace periph */

#define spi1	(*(periph::spi_reg::SPI_Class *)SPI1)

#endif
