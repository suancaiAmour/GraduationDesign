#ifndef STM32F10X_SPI_H
#define STM32F10X_SPI_H

#include "stm32f10x__COMMON.h"

namespace periph{

	struct SPI_Config;
	
	#include "stm32f10x__spi_reg.h"
	
	enum SPI_Direction {			/* CR1_BIDIMODE, CR1_BIDIOE, CR1_RXONLY ����ĸ�ʽ�Ƚ�����㣬��ֱ��ʹ�� assignf */
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
			
			/**	@brief SPI ���ã��ں���ĩβ���Զ����� SPI_Cmd(ENABLE) */
			void Config(const SPI_Config& cfg);
			
			/**	@brief SPI ʹ��/��ֹ
			*	@note  <no read-modify-write> 	*/
			finline void SPI_Cmd(const FunctionalState newState) { *PRegBit(CR1,SPE)=newState; }
			
			/**	@brief ���ò����� */
			void SPI_SetBaudRate(const SPI_BaudRateControl br) { CR1=assignf(CR1,CR1_BR,br); }	
			
			/**	@brief ��д�ֽ� */
			u16 SPI_MasterReadWrite(const u16 data) {
				/* ֻҪ�����˳���ʱ�������Ѿ�������ɣ��Ͳ���Ҫȥ��� TXE λ */
				DR=data;			/* ����һ��byte */
				uint32_t const volatile * const pRXNE = PRegBit(SR,RXNE);
				while(*pRXNE==0); /* �ȴ�������� */   
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
		SPI_BaudRateControl baudRate;		/*!< SPI �����ʵ����� */
		SPI_FirstBit firstBit;
		u16 CRC_Polynominal;	/* CRC generator polynominal�������ʹ��CRC���ܣ����Ժ����� */
		
		SPI_Config():
			  direction(UNIDIRECTION_DUPLEX)
			, mode(SPI_MASTER)
			, dataSize(DATA_FRAME_8BITS)
			, clockPolarity(IDLE_CLK_HIGH)
			, clockPhase(SECOND_EDGE_CAPTURE)
			, softwareSlaveManage(ENABLE)
			, baudRate(SPI_PCLK_DIV_256) /* Ĭ������� */
			, firstBit(SPI_MSB_FIRST)
			, CRC_Polynominal(0x07){}
	}; /* struct SPI_ConfigStruct */
	
} /* namespace periph */

#define spi1	(*(periph::spi_reg::SPI_Class *)SPI1)

#endif
