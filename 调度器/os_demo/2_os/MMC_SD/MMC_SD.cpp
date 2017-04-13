#include "debug.h"
#include "sys.h"
#include "mmc_sd.h"

#include  "stm32f10x__spi.h"

/* MMC_SD 模块将要被弃用的函数 */
#include "mmc_sd_old_functions.h"

struct SD_Card::R2{
	R1	r1;
	u8	status;
};

struct SD_Card::R3{
	R1	r1;
	u32 ocr;
};

struct SD_Card::R7{
	R1	r1;
	u32	voltage_info;	
};


struct SD_Card::R1b{
	R1 r1;
};

union SD_Card::Response{
	R1	r1;
	R2	r2;
	R3	r3;
	R7	r7;
};

#define CmdResp(cmd,resp) \
template<> struct SD_Card::SD_CMD<cmd>{typedef SD_Card::resp Resp;}
/* 建立 CMD 和返回类型的映射关系 */
CmdResp(0,R1);
CmdResp(1,R1);
CmdResp(6,R1);
CmdResp(8,R7);
CmdResp(9,R1);
CmdResp(10,R1);
CmdResp(12,R1b);
CmdResp(13,R2);
CmdResp(16,R1);
CmdResp(17,R1);
CmdResp(18,R1);
CmdResp(24,R1);
CmdResp(25,R1);
CmdResp(27,R1);
CmdResp(28,R1b);
CmdResp(29,R1b);
CmdResp(30,R1);
CmdResp(32,R1);
CmdResp(33,R1);
CmdResp(38,R1b);
CmdResp(42,R1);
CmdResp(55,R1);
CmdResp(56,R1);
CmdResp(58,R3);
CmdResp(59,R1);
#undef CmdResp

#define ACmdResp(cmd,resp) \
template<> struct SD_Card::SD_ACMD<cmd>{typedef SD_Card::resp Resp;}
/* 建立 ACMD 和返回类型的映射关系 */
ACmdResp(13,R2);
ACmdResp(22,R1);
ACmdResp(23,R1);
ACmdResp(41,R1);
ACmdResp(42,R1);
ACmdResp(51,R1);
#undef ACmdResp

#define CMD_ResType(cmd)	SD_CMD<cmd>::Resp
#define ACMD_ResType(cmd)	SD_ACMD<cmd>::Resp

/*
class for SCR register
for Version NO. 1.0
*/
class SD_Card::SCR{
	 u8 Version(u8* buf);
};


void SD_Card::GetResponse(R1* r1){
	u8 retry = 0;
	do{
		retry++;
		if(retry > NCR_MAX)	break;
		*r1 = ReadByte();
	}while( *r1 == 0xff );
}

void SD_Card::GetResponse(R2* r2){
	GetResponse(&r2->r1);
	r2->status = ReadByte();
}

/**	@brief 得到R7类型的返回值 */
void SD_Card::GetResponse(R7* p_r7){
	R7& r7 = *p_r7;
	GetResponse(&r7.r1);
	
	//得到voltage information
	r7.voltage_info = ReadByte();
	r7.voltage_info <<= 8;	/* 右移8位，准备接收下一个字节 */
	r7.voltage_info += ReadByte();
	r7.voltage_info <<= 8;	/* 右移8位，准备接收下一个字节 */
	r7.voltage_info += ReadByte();
	r7.voltage_info <<= 8;	/* 右移8位，准备接收下一个字节 */
	r7.voltage_info += ReadByte();
}

/**	@brief	得到R3类型的返回值 */
void SD_Card::GetResponse(R3* p_r3){
	R3& r3 = *p_r3;
	GetResponse(&r3.r1);
	/* 得到ocr */
	r3.ocr = ReadByte();
	r3.ocr <<= 8;	/* 右移8位，准备接收下一个字节 */
	r3.ocr += ReadByte();
	r3.ocr <<= 8;	/* 右移8位，准备接收下一个字节 */
	r3.ocr += ReadByte();
	r3.ocr <<= 8;	/* 右移8位，准备接收下一个字节 */
	r3.ocr += ReadByte();
}

/*	得到R1b类型的返回值 */
void SD_Card::GetResponse(R1b* p_r1b) {
	/* 先得到r1 */
	GetResponse(&p_r1b->r1);
	WriteDummyByte();
	return;
}

template<u8 cmd>
tinline void SD_Card::SendCMD
		(u32 arg, u8 crc, typename SD_CMD<cmd>::Resp* resp){
	CommandBegin( cmd,  arg,  crc);
	GetResponse(resp);
	
    DeSelect();	
    /* 在总线上额外增加8个时钟，让SD卡完成剩下的工作 */
	WriteDummyByte();

    return;
}

/*
供 SendCMD 和 SendACMD 调用，为了节省代码量而创建的函数。
*/
void SD_Card::CommandBegin(u8 cmd, u32 arg, u8 crc){
	/* 高速写命令延时 */
	WriteDummyByte();
    WriteDummyByte();
	WriteDummyByte();

    Select();
	
    WriteByte(cmd | 0x40);
    WriteByte(arg >> 24);
    WriteByte(arg >> 16);
    WriteByte(arg >> 8);
    WriteByte(arg);
    WriteByte(crc);
}

template<u8 cmd>
tinline void SD_Card::SendACMD
		(u32 arg, u8 crc, typename SD_ACMD<cmd>::Resp* resp){
	
	CommandBegin( cmd,  arg,  crc);
	GetResponse(resp);
	
    DeSelect();	

    /* 在总线上额外增加8个时钟，让SD卡完成剩下的工作 */
	WriteDummyByte();

    return;
}

#if 0

/*******************************************************************************
* Function Name  : SD_GetCID
* Description    : 获取SD卡的CID信息，包括制造商信息
* Input          : u8 *cid_data(存放CID的内存，至少16Byte）
* Output         : None
* Return         : u8 
*                  0：NO_ERR
*                  1：TIME_OUT
*                  other：错误信息
*******************************************************************************/
u8 SD_GetCID(u8 *cid_data) {
    R1 r1;
    /* 发CMD10命令，读CID */
    SendCommand(CMD(10), 0, 0xFF, (Response *)&r1);
    if(r1 != 0x00)return r1;  /* 没返回正确应答，则退出，报错 */
    SD_ReceiveData(cid_data, 16, RELEASE);	/* 接收16个字节的数据 */
    return 0;
}

/*******************************************************************************
* Function Name  : SD_GetCSD
* Description    : 获取SD卡的CSD信息，包括容量和速度信息
* Input          : u8 *cid_data(存放CID的内存，至少16Byte）
* Output         : None
* Return         : u8 
*                  0：NO_ERR
*                  1：TIME_OUT
*                  other：错误信息
*******************************************************************************/
u8 SD_GetCSD(u8 *csd_data){
    R1 r1;	 
    //发CMD9命令，读CSD
    SendCommand(CMD(9), 0, 0xFF, (Response *)&r1);
    if(r1 != 0x00)return r1;  //没返回正确应答，则退出，报错  
    //接收16个字节的数据
    SD_ReceiveData(csd_data, 16, RELEASE); 
    return 0;
}
 
/*******************************************************************************
* Function Name  : SD_GetCapacity
* Description    : 获取SD卡的容量（字节）
* Input          : None
* Output         : None
* Return         : u32 capacity 
*                   0： 取容量出错 
*******************************************************************************/
u32 SD_GetCapacity(void){
    u8 csd[16];
    u32 Capacity;
    u8 r1;
    u16 i;
	u16 temp;  
    //取CSD信息，如果期间出错，返回0
    if(SD_GetCSD(csd)!=0) return 0;	    
    //如果为SDHC卡，按照下面方式计算
    if((csd[0]&0xC0)==0x40)
    {									  
	    Capacity=((u32)csd[8])<<8;
		Capacity+=(u32)csd[9]+1;	 
        Capacity = (Capacity)*1024;//得到扇区数
		Capacity*=512;//得到字节数			   
    }
    else
    {		    
    	i = csd[6]&0x03;
    	i<<=8;
    	i += csd[7];
    	i<<=2;
    	i += ((csd[8]&0xc0)>>6);
    
        //C_SIZE_MULT
    	r1 = csd[9]&0x03;
    	r1<<=1;
    	r1 += ((csd[10]&0x80)>>7);	 
    	r1+=2;//BLOCKNR
    	temp = 1;
    	while(r1)
    	{
    		temp*=2;
    		r1--;
    	}
    	Capacity = ((u32)(i+1))*((u32)temp);	 
        // READ_BL_LEN
    	i = csd[5]&0x0f;
        //BLOCK_LEN
    	temp = 1;
    	while(i)
    	{
    		temp*=2;
    		i--;
    	}
        //The final result
    	Capacity *= (u32)temp;//字节为单位 	  
    }
    return (u32)Capacity;
}

/*******************************************************************************
* Function Name  : SD_WriteSingleBlock
* Description    : 写入SD卡的一个block
* Input          : u32 sector 扇区地址（sector值，非物理地址） 
*                  u8 *buffer 数据存储地址（大小至少512byte） 
* Output         : None
* Return         : u8 r1 
*                   0： 成功
*                   other：失败
*******************************************************************************/
u8 SD_WriteSingleBlock(u32 sector, const u8 *dat)
{
    R1 r1;
    u16 retry;

    //设置为高速模式
    //SPI_SetSpeed(SPI_SPEED_HIGH);	   
    //如果不是SDHC，给定的是sector地址，将其转换成byte地址
    if(SD_Type!=SD_TYPE_V2HC)
    {
        sector = sector<<9;
    }   
    SendCommand(CMD(24), sector, 0x00,  (Response*)&r1);
    if(r1 != 0x00)
    {
        return r1;  //应答不正确，直接返回
    }
    
    //开始准备数据传输
    cs=0;
    //先放3个空数据，等待SD卡准备好
    SD_WriteByte(0xff);
    SD_WriteByte(0xff);
    SD_WriteByte(0xff);
    //放起始令牌0xFE
    SD_WriteByte(0xFE);

    //放一个sector的数据
	SD_WriteMultiBytes(dat, 512);

    //发2个Byte的dummy CRC
    SD_WriteByte(0xff);
    SD_WriteByte(0xff);
    
    //等待SD卡应答
    r1 = SD_ReadByte();
    if((r1&0x1F)!=0x05)
    {
        cs=1;
        return r1;
    }
    //等待操作完成
    retry = 0;
    while(!SD_ReadByte())
    {
        retry++;
        if(retry>0xfffe)        //如果长时间写入没有完成，报错退出
        {
            cs=1;
            return 1;           //写入超时返回1
        }
    }
    //写入完成，片选置1
    cs=1;
    SD_WriteByte(0xff);

    return 0;
}

#endif

SD_Card sd;

void SD_Card::IO_Init() {
	rcc.ClockCmd(&gpioa, ENABLE);
	
	cs.Config(OUTPUT, PUSH_PULL);
	cs = 1;
	
	rcc.ClockCmd(&spi1, ENABLE);
	SPI_Config cfg;	/* 默认配置即可 */
	spi1.Config(cfg);
	
	sck.Config(AF_OUTPUT, PUSH_PULL, SPEED_50M);
	mosi.Config(AF_OUTPUT, PUSH_PULL, SPEED_50M);
	miso.Config(INPUT, PULL_UP);
}

SD_Card::InitResult SD_Card::Init() {
	IO_Init();
	
	Response  res;
	R1& r1 = res.r1;
	R3& r3 = res.r3;
    CMD_ResType(8)& cmd8 = res.r7;
	
	//先产生 > 74 个脉冲，让SD卡自己初始化完成
    for(u16 retry=0;retry<12;retry++)	WriteByte(0xFF);	    

    //-----------------SD卡复位到idle开始-----------------

    //循环连续发送CMD0，直到SD卡返回0x01,进入IDLE状态
    //超时则直接退出

	for(u16 retry = 250; 1; ){
		SendCMD<0>(0,0x95,&r1);
		/* 如果初始化成功，那么r1就是0x01 */
		if(r1==0x01)
			break;
		if(--retry==0){
			return INIT_ERROR;
		}
    }
	
	u16 retry;
	//-----------------SD卡复位到idle结束-----------------
	SendCMD<8>(0x1aa,0x87,&cmd8);
    //如果卡片版本信息是v1.0版本的，即r1=0x05，则进行以下初始化
    if(cmd8.r1 == 0x05) { /* 未测试。。。 */
        //设置卡类型为SDV1.0，如果后面检测到为MMC卡，再修改为MMC
        SD_Type = SD_TYPE_V1; 
        
		/********************SD卡、MMC卡初始化开始*********************/	 
        /* 发卡初始化指令CMD55+ACMD41
         * 如果有应答，说明是SD卡，且初始化完成
		 * 没有回应，说明是MMC卡，额外进行相应初始化
		 */
        retry = 400;
		while(retry) {
			SendCMD<55>(0,0,&r1);
			if(r1 == 0xff)	return INIT_ERROR;
			SendACMD<41>(0,0,&r1);
			if(r1 == 0)	//得到正确响应后，发ACMD41，应得到返回值0x00
				break;
			retry--;
		}
		// 判断是超时还是得到正确回应
		// 若有回应：是SD卡；没有回应：是MMC卡
		//----------MMC卡额外初始化操作开始------------
		if(retry == 0) {
            retry = 400;
            //发送MMC卡初始化命令（没有测试）
			while(retry){
				SendCMD<1>(0,0,&r1);
				if(r1 == 0x00)	//正确回应
					break;
				retry--;
            }
            if(retry==0)return INIT_ERROR;   //MMC卡初始化超时		    
            //写入卡类型
			SD_Type = SD_TYPE_MMC;
        }
        //----------MMC卡额外初始化操作结束------------	    

		/* 设置SPI为高速模式 */
		spi1.SPI_SetBaudRate(SPI_PCLK_DIV_4);   

		WriteByte(0xFF);
		
		SendCMD<59>(0,0x95,&r1);	/* 禁止CRC校验 */
		if(r1 != 0x00) return INIT_ERROR;  /* 命令错误，返回r1 */

		/* 设置Sector Size */
		SendCMD<16>(512,0x95,&r1);
		if(r1 != 0x00)return INIT_ERROR;//命令错误，返回r1		 
		//-----------------SD卡、MMC卡初始化结束-----------------
    } 
	/* SD卡为V1.0版本的初始化结束 */
	/* 下面是V2.0卡的初始化
     * 其中需要读取OCR数据，判断是SD2.0还是SD2.0HC卡
	 */
	else if(cmd8.r1 == 0x01)
	{
		dout<<"\nSD2.0\n"<<endl;			 
        
		/* 判断该卡是否支持2.7V-3.6V的电压范围 */
		if( ((u16)(cmd8.voltage_info)) == 0x000001aa)	
		{
			dout<<"Support Voltage 2.7V - 3.6V."<<endl;

			retry = 250;
		    while(retry) {
				SendCMD<55>(0,0,&r1);
				if(r1 != 0x01) { /* 如果CMD55命令无效，返回错误 */
					dout<<"CMD55 failed!"<<endl;
					return INIT_ERROR;
				}
				SendACMD<41>(0x40000000,0,&r1);
				if(r1 == 0) {
					dout<<"ACMD41 success.\n"<<endl;
					break;
				}
				retry--;
			}
		 	/* 跳出循环后，检查原因：初始化成功？or 重试超时？ */
			if(retry == 0){
				dout<<"\nCMD55 + ACMD41 timeout."<<endl;
				return INIT_ERROR; //TIME OUT 
			}
					     
            //初始化指令发送完成，接下来获取OCR信息
			
            //-----------鉴别SD2.0卡版本开始-----------
			
            SendCMD<58>(0,0,&r3);
			//检查接收到的OCR中的bit30位（CCS），确定其为SD2.0还是SDHC
            //如果CCS=1则为SDHC		如果CCS=0则为SD2.0
			if( checkb(r3.ocr, BIT(30))) {	/* SDHC */
				SD_Type = SD_TYPE_V2HC;    //检查CCS
				dout<<"type:SDHC\n"<<endl;
            }
			else{ /* SD2.0 */
				dout<<"type:SD2.0"<<endl;
				SendCMD<16>(512,0x95,&r1);
				if(r1){
					dout<<"Set block size error!"<<endl;
					return INIT_ERROR;
				}
				SD_Type = SD_TYPE_V2;	    
            }
			
			//-----------鉴别SD2.0卡版本结束----------- 
			
            /* 设置SPI为高速模式 */
			spi1.SPI_SetBaudRate(SPI_PCLK_DIV_4);
        }
		else {
			dout<<"\nCurrent voltage is not supported!\n"<<endl;
			return VOLTAGE_INCOMPATIBLE;	//不支持现在的电压范围
		}	    
    }
	else {
		dout<<"Unknown error!"<<endl;
		return INIT_ERROR;
	}
	
	if(res.r1)
		return INIT_ERROR;
	else
		return INIT_SUCCESS;
}

/*******************************************************************************
* Function Name  : SD_WaitReady
* Description    : 等待SD卡Ready
* Input          : None
* Output         : None
* Return         : u8 
*                   0： 成功
*                   other：失败
*******************************************************************************/
u8 SD_Card::WaitReady(void) {
    u8 r1;
    u32 retry;
    retry=0;
    do {
        r1 = ReadByte();
        if(retry>=0x8ffff)return 1; 
		retry++;
    } while(r1!=0xFF); 
    return 0;
}

Result SD_Card::ReceiveDataBlocks(u8* dat, u16 len, u8 count){
	/*
	在函数中建立封装类，限定函数 ReceiveDataBlock 只能给这个
	函数使用。
	*/
	struct CReceiveDataBlock{
		finline
		Result ReceiveDataBlock(u8 *dat, u16 len){
			//等待SD卡发回数据起始令牌0xFE
			u16 retry = 0;
			R1 r1;
			do{
				if(retry>NAC_MAX){
					dout<<"Receive data block wait too long! ";
					return FAILED;
				}
				r1 = ReadByte();
				retry++;
			}while(r1 != SINGLE_RD_BLOCK_START);

			/* 接收数据 */
			ReadMultiBytes(dat, len);

			//下面是2个伪CRC（dummy CRC）
			WriteByte(0xFF);
			WriteByte(0xFF);
																		
			return SUCCEEDED;
		}
	}fun;
	
	Select();
    do{ /* 开始接收数据 */
        if(fun.ReceiveDataBlock(dat, len) == FAILED){
			dout<<"Receive Data block failed"<<endl;
			break;
		}
        dat += 512;
    } while(--count);
	DeSelect();
	WriteDummyByte();
	if(count) return FAILED;
	else return SUCCEEDED;
}

/*******************************************************************************
* Function Name  : SD_ReadSingleBlock
* Description    : 读SD卡的一个block
* Input          : u32 sector 取地址（sector值，非物理地址） 
*                  u8 *buffer 数据存储地址（大小至少512byte） 
*******************************************************************************/
Result SD_Card::ReadSingleBlock(u32 sector, u8 *buffer){
	R1 r1;	    
		   
    /* 如果不是SDHC，给定的是sector地址，将其转换成byte地址 */
	if(SD_Type != SD_TYPE_V2HC){
        sector *= blockLength;
    }
	/* 读命令 */
	SendCMD<17>(sector,0,&r1);
	if(r1 != 0x00) {
		return FAILED; 		   							  
	}
	
	Result r = ReceiveDataBlocks(buffer, blockLength, 1);
	
	return r;
}

Result SD_Card::ReadMultiBlocks(u32 sector, u8 *buffer, u8 count) {

 	/* 如果不是SDHC，将sector地址转成byte地址 */
    if(SD_Type!=SD_TYPE_V2HC)
        sector *= blockLength;
	
	R1b r1b;
	/* 发读多块命令 */
	SendCMD<18>(sector,0,&r1b.r1);
	if(r1b.r1 != 0x00)	{
		dout<<"SD_Card::ReadMultiBlock-cmd18 failed!"<<endl;
		return FAILED;
	}
	Result r = ReceiveDataBlocks(buffer, blockLength, count);
	
	/* 发送停止命令。。。没有错误检查。。。 -*-*-*-*-*-*-*-*-*-*-*-*-*-*/
	SendCMD<12>(0,0,&r1b);
	
	return r;
}

Result SD_Card::WriteSingleBlock(u32 sector, const u8 *dat) {
	/*
	如果不是SDHC，给定的是sector地址，将其转换成byte地址
	*/
	if(SD_Type!=SD_TYPE_V2HC)
        sector *= blockLength;
	
    R1 r1;
	SendCMD<24>(sector,0x00,&r1);
	/* 应答不正确，直接返回 */
	if(r1 != 0x00){
		dout<<"SD_Card::WriteSingleBlock cmd 24 failed"<<endl;
		return FAILED;
	}
	
    //开始准备数据传输
    Select();
	WriteDummyByte();
	WriteDummyByte();
	WriteDummyByte();
	
    /* 放起始令牌 */
	WriteByte(SINGLE_WR_BLOCK_START);

    /* 放一个sector的数据 */
	WriteMultiBytes(dat, blockLength);

    //发2个Byte的dummy CRC
    WriteDummyByte();
	WriteDummyByte();
    
    /* 等待SD卡应答 */
    r1 = ReadByte();
    if((r1&0x1F)!=0x05){
		DeSelect();
		dout<<"SD_Card::WriteSingleBlock card invalid response"<<endl;
        return FAILED;
	}
	
    /* 等待操作完成 */
	u32 retry = 0;
    while(!ReadByte()){
        retry++;
        if(retry>0x3ffff) { /* 如果长时间写入没有完成，报错退出 */
            DeSelect();
			dout<<"SD_Card::WriteSingleBlock wait too long"<<endl;
            return FAILED; /* 写入超时 */
		}
    }
	
	DeSelect();
    WriteDummyByte();

    return SUCCEEDED;
}

Result SD_Card::WriteMultiBlocks(u32 sector, const u8 *dat, u8 count) {
    R1 r1;
	
	/* 如果不是SDHC，给定的是sector地址，将其转换成byte地址 */
	if(SD_Type != SD_TYPE_V2HC)sector *= blockLength;
	/* 如果目标卡不是MMC卡，启用ACMD23指令使能预擦除 */
    if(SD_Type != SD_TYPE_MMC){
		SendCMD<55>(0,0,&r1);
		if(r1){
			dout<<"SD_Card::WriteMultiBlock CMD 55 failed."<<endl;
			return FAILED;
		}
		SendACMD<23>(count, 0x00,&r1 );
		if(r1){
			dout<<"SD_Card::WriteMultiBlock ACMD23 failed."<<endl;
			return FAILED;
		}
	}
	SendCMD<25>(sector, 0x00,&r1); /*发多块写入指令 */
    if(r1 != 0x00){
		dout<<"SD_Card::WriteMultiBlock CMD25 failed."<<endl;
		return FAILED;  /* 应答不正确，直接返回 */
	}
	
	Select();
    WriteDummyByte();
	WriteDummyByte();
	WriteDummyByte();
    //--------下面是N个sector写入的循环部分
    do{
        /* 放起始令牌 */
        WriteByte(MULTI_WR_BLOCK_START);
		
		WriteMultiBytes(dat,blockLength);
        
		//发2个Byte的dummy CRC
        WriteDummyByte();
        WriteDummyByte();
        
        //等待SD卡应答
        r1 = ReadByte();
        
		if((r1&0x1F)!=0x05){
            DeSelect();
			dout<<"SD_Card::WriteMultiBlock bad card response."<<endl;
            return FAILED;
		}
		
		/* 等待操作完成 */
		u32 retry = 0;
		while(!ReadByte()){
			retry++;
			if(retry>0x3ffff) { /* 如果长时间写入没有完成，报错退出 */
				DeSelect();
				dout<<"SD_Card::WriteMultiBlock wait too long"<<endl;
				return FAILED; /* 写入超时 */
			}
		}

    }while(--count);//本sector数据传输完成
    
    /* 发结束传输令牌 */
	WriteByte(MULTI_WR_STOP);
	for(int i=NBR_MAX; i;--i)
		WriteDummyByte();
    if(WaitReady()){ /* 等待准备好 */
		dout<<"SD_Card::WriteMultiBlock write time out."<<endl;
		return FAILED;  
	}
	DeSelect();
    WriteDummyByte();

	if(count) return FAILED;
	else return SUCCEEDED;
}

/*******************************************************************************
* Function Name  : SD_GetCSD
* Description    : 获取SD卡的CSD信息，包括容量和速度信息
* Input          : u8 *cid_data(存放CID的内存，至少16Byte）
* Output         : None
*****************************************/
Result SD_Card::GetCSD(u8 *csd_data){
    R1 r1;	 
	//发CMD9命令，读CSD
	SendCMD<9>(0, 0xFF, &r1);
	if(r1 != 0x00){
		dout<<"SD_Card::GetCSD CMD<9> faild!"<<endl;
		return FAILED;
	}
	//接收16个字节的数据
	Result r = ReceiveDataBlocks(csd_data, 16, 1); 
    return r;
}

Result SD_Card::GetSCR(u8 *buf){
	R1 r1;
	SendCMD<55>(0,0,&r1);
	if(r1 != 0x00) {
		dout<<"SD_Card::GetSCR CMD<55> failed!"<<endl;
		return FAILED;
	}
	SendACMD<51>(0,0,&r1);
	if(r1 != 0x00) {
		dout<<"SD_Card::GetSCR ACMD<51> failed!"<<endl;
		return FAILED;
	}
	Result r = ReceiveDataBlocks(buf,16,1);
	if(r != SUCCEEDED){
		dout<<"SD_Card::GetSCR failed!"<<endl;
	}
	return r;
}

/*
u8 buf[1024];
void PrintBlock(u8* buf, u16 size);
int main(void) {
	
	
	
	RCC_ClocksFreq clocks;
	rcc.GetClocksFreq(&clocks);
	
	while(sd.Init() != sd.INIT_SUCCESS);
	output<<"初始化成功"<<endl;

	Result r = sd.ReadMultiBlocks(0,buf,2);
	if(r==SUCCEEDED){
		output<<"读成功"<<endl;
		PrintBlock(buf, 1024);
	}
	
	r = sd.GetSCR(buf);
	if(r==SUCCEEDED){
		output<<"读 SCR 成功"<<endl;
		PrintBlock(buf, 16);
	}
	
	while(1);
}

void PrintBlock(u8* buf, u16 size){
	for(int i=0; i<size; i++){
		output<<NumToASCII(buf[i]>>4)
			<<NumToASCII(buf[i]);
		if(i%16==15) output<<endl;
		else output<<"  ";
	}
	output<<endl;
}
*/
