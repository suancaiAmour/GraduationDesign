#ifdef MMC_SD_C
//mmc_sd 模块将被弃用的函数


/*******************************************************************************
* Function Name  : SD_SendCommand
* Description    : 向SD卡发送一个命令
* Input          : u8 cmd   命令 
*                  u32 arg  命令参数
*                  u8 crc   crc校验值
* Output         : None
* Return         : u8 r1 SD卡返回的响应
*******************************************************************************/
//u8 SD_SendCommand(u8 cmd, u32 arg, u8 crc)
//{
//    unsigned char r1;
//    unsigned char Retry = 0; 
//	SD_CS=1;//释放	  
//    SPI_ReadWriteByte(0xff);//高速写命令延时	 
//	SPI_ReadWriteByte(0xff);  
//    //片选端置低，选中SD卡
//    SD_CS=0; 
//    //发送
//    SPI_ReadWriteByte(cmd | 0x40);                         //分别写入命令
//    SPI_ReadWriteByte(arg >> 24);
//    SPI_ReadWriteByte(arg >> 16);
//    SPI_ReadWriteByte(arg >> 8);
//    SPI_ReadWriteByte(arg);
//    SPI_ReadWriteByte(crc);
//    
//    //等待响应，或超时退出
//    while((r1 = SPI_ReadWriteByte(0xFF))==0xFF)
//    {
//        Retry++;
//        if(Retry > 200)break; 
//    }   
//    //关闭片选
//    SD_CS=1;
//    //在总线上额外增加8个时钟，让SD卡完成剩下的工作
//    SPI_ReadWriteByte(0xFF);	 
//    //返回状态值
//    return r1;
//}


/*******************************************************************************
* Function Name  : SD_SendCommand_NoDeassert
* Description    : 向SD卡发送一个命令(结束是不失能片选，还有后续数据传来）
* Input          : u8 cmd   命令 
*                  u32 arg  命令参数
*                  u8 crc   crc校验值
* Output         : None
* Return         : u8 r1 SD卡返回的响应
*******************************************************************************/
//u8 SD_SendCommand_NoDeassert(u8 cmd, u32 arg, u8 crc)
//{
//    unsigned char r1;
//    unsigned char Retry = 0;    
//	SD_CS=1;//释放
//    SPI_ReadWriteByte(0xff);//高速写命令延时
//	SPI_ReadWriteByte(0xff);  
//    //片选端置低，选中SD卡
//    SD_CS=0;	    
//    //发送
//    SPI_ReadWriteByte(cmd | 0x40); //分别写入命令
//    SPI_ReadWriteByte(arg >> 24);
//    SPI_ReadWriteByte(arg >> 16);
//    SPI_ReadWriteByte(arg >> 8);
//    SPI_ReadWriteByte(arg);
//    SPI_ReadWriteByte(crc);	 
//    //等待响应，或超时退出
//    while((r1 = SPI_ReadWriteByte(0xFF))==0xFF)
//    {
//        Retry++;
//        if(Retry > 200)break;   
//    }
//    //返回响应值
//    return r1;
//}

//#define CMD0    0       //卡复位
//#define CMD9    9       //命令9 ，读CSD数据
//#define CMD10   10      //命令10，读CID数据
//#define CMD12   12      //命令12，停止数据传输
//#define CMD16   16      //命令16，设置SectorSize 应返回0x00
//#define CMD17   17      //命令17，读sector
//#define CMD18   18      //命令18，读Multi sector
//#define CMD24   24      //命令24，写sector
//#define CMD25   25      //命令25，写Multi sector
//#define CMD55   55      //命令55，应返回0x01
//#define CMD58   58      //命令58，读OCR信息
//#define CMD59   59      //命令59，使能/禁止CRC，应返回0x00


////通信协议上，ACMD命令和CMD的命令不是以BIT6作为区分的，这里只是为了软件区分的方便
//#define ACMD23  (23 | (u8)BIT6)		//命令23，设置多sector写入前预先擦除N个block
//#define ACMD41  (41 | (u8)BIT6)		//命令41，应返回0x00


//SD卡的CMD指令的宏定义
// #define GO_IDLE_STATE 			CMD(0)
// #define SEND_OP_COND 			CMD(1)
// #define SWITCH_FUNC				CMD(6)
// #define SEND_IF_COND			CMD(8)
// #define SEND_CSD				CMD(9)
// #define SEND_CID				CMD(10)
// #define STOP_TRANSMISSION		CMD(12)
// #define SEND_STATUS				CMD(13)
// #define SET_BLOCKLEN			CMD(16)
// #define READ_SINGLE_BLOCK		CMD(17)
// #define READ_MULTIPLE_BLOCK		CMD(18)
// #define WRITE_BLOCK				CMD(24)
// #define WRITE_MULTIPLE_BLOCK	CMD(25)
// #define PROGRAM_CSD				CMD(27)
// #define SET_WRITE_PROT			CMD(28)
// #define CLR_WRITE_PROT			CMD(29)
// #define SEND_WRITE_PROT			CMD(30)
// #define ERASE_WR_BLK_START_ADDR	CMD(32)
// #define ERASE_WR_BLK_END_ADDR	CMD(33)
// #define ERASE					CMD(38)
// #define LOCK_UNLOCK				CMD(42)
// #define APP_CMD					CMD(55)
// #define GEN_CMD					CMD(56)
// #define READ_OCR				CMD(58)
// #define CRC_ON_OFF				CMD(59)
					 

#endif
//
