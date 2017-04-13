#ifndef MMC_SD_H
#define MMC_SD_H
#include "stm32f10x__gpio.h"
#include "stm32f10x__spi.h"
using namespace periph;

// SD传输数据结束后是否释放总线宏定义  
#define NO_RELEASE      0
#define RELEASE         1

// class disk_interface{
// 	virtual Reault ReadSector() = 0;
//	virtual Result WriteSector() = 0;
//  virtual Result ReadSectors(...)
//  virtual Result WriteSectors(...)
//  virtual u32 Capacity()
// };

class SD_Card {		
	typedef u8 R1;	/* 第一个接收到的总是r1 */
	struct R2;
	struct R3;
	struct R7;
	struct R1b;
	union Response;

	template<u8 num>struct SD_CMD{};
	template<u8 num>struct SD_ACMD{};
	
	void CommandBegin(u8 cmd, u32 arg, u8 crc);
	template<u8 cmd>
	void SendCMD(u32 arg, u8 crc, typename SD_CMD<cmd>::Resp* resp);
	
	template<u8 cmd>
	void SendACMD(u32 arg, u8 crc, typename SD_ACMD<cmd>::Resp* resp);
		
	enum{
		  NCS_MIN = 0
		, NCR_MAX = 8
		, NRC_MIN = 1
		, NAC_MIN = 1
		
		/* NAC_MAX is specified in CSD */
		/*
		实测 NAC 居然大于 200，汗，这效率。。。
		*/
		, NAC_MAX = 20000
		
		, NWR_MIN = 1
		, NEC_MIN = 0
		, NDS_MIN = 0
		, NBR_MIN = 0
		, NBR_MAX = 1
		, NCX_MIN = 0
		, NCX_MAX = 8
	};
	
	void GetResponse(R1* r1);
	void GetResponse(R2* r2);
	void GetResponse(R7* r7);
	void GetResponse(R3* p_r3);
	void GetResponse(R1b* p_r1b);
	enum TOKEN{
		  SINGLE_RD_BLOCK_START = 0XFE
		, SINGLE_WR_BLOCK_START = 0XFE
		, MULTI_RD_BLOCK_START = 0XFE
		, MULTI_WR_BLOCK_START = 0XFC
		, MULTI_WR_STOP = 0XFD
	};
		
	PinRename(pa3,cs);
	PinRename(pa5,sck);
	PinRename(pa6,miso);
	PinRename(pa7,mosi);
	finline static void Select()	{ cs = 0; }
	finline static void DeSelect()	{ cs = 1; }
	finline static u8 ReadByte()				{ return spi1.SPI_MasterReadWrite(0xff); }
	finline static void WriteByte(const u8 dat)		{ spi1.SPI_MasterReadWrite(dat); }
	finline static void WriteDummyByte()	{spi1.SPI_MasterReadWrite(0xff);}
	
	finline static void ReadMultiBytes(u8* buf, u16 len) { while(len--) *(buf++)=ReadByte();}
	finline static void WriteMultiBytes(const u8* buf,u16 len) { while(len--) WriteByte(*(buf++)); }
	
	static void IO_Init();

	static void SendCommand(u8 cmd, u32 arg, u8 crc, Response *p_res);

	static u8 WaitReady(void);

	Result ReceiveDataBlocks(u8* dat, u16 len, u8 count);
	
	enum{
		blockLength = 512
	};
	
	class SCR;
	
public:
	
	Result ReadSingleBlock(u32 sector, u8 *buffer);
	Result ReadMultiBlocks(u32 sector, u8 *buffer, u8 count);
	
	Result WriteSingleBlock(u32 sector, const u8 *dat);
	Result WriteMultiBlocks(u32 sector, const u8 *dat, u8 count);

	Result GetCSD(u8 *csd_data);
	Result GetSCR(u8 *buf);

	enum SD_Card_Type{
		  SD_TYPE_MMC		= 0
		, SD_TYPE_V1		= 1
		, SD_TYPE_V2		= 2
		, SD_TYPE_V2HC		= 4
	};
	SD_Card_Type  SD_Type;	//SD卡的类型

	enum InitResult{
		  INIT_SUCCESS			/* 初始化成功　*/
		, VOLTAGE_INCOMPATIBLE	/* 电压不兼容 */
		, NO_CARD				/* 没有插入 SD 卡 */
		, INIT_ERROR			/* 未知错误 */
	};
	
	InitResult Init();
	
	static bool IsBusy() {
		Select();
		u8 r = ReadByte();
		DeSelect();
		return r != 0xff;
	}
};
	
extern SD_Card sd;

#endif	/* MMC_SD_H */
