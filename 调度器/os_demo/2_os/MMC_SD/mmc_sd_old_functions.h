#ifdef MMC_SD_C
//mmc_sd ģ�齫�����õĺ���


/*******************************************************************************
* Function Name  : SD_SendCommand
* Description    : ��SD������һ������
* Input          : u8 cmd   ���� 
*                  u32 arg  �������
*                  u8 crc   crcУ��ֵ
* Output         : None
* Return         : u8 r1 SD�����ص���Ӧ
*******************************************************************************/
//u8 SD_SendCommand(u8 cmd, u32 arg, u8 crc)
//{
//    unsigned char r1;
//    unsigned char Retry = 0; 
//	SD_CS=1;//�ͷ�	  
//    SPI_ReadWriteByte(0xff);//����д������ʱ	 
//	SPI_ReadWriteByte(0xff);  
//    //Ƭѡ���õͣ�ѡ��SD��
//    SD_CS=0; 
//    //����
//    SPI_ReadWriteByte(cmd | 0x40);                         //�ֱ�д������
//    SPI_ReadWriteByte(arg >> 24);
//    SPI_ReadWriteByte(arg >> 16);
//    SPI_ReadWriteByte(arg >> 8);
//    SPI_ReadWriteByte(arg);
//    SPI_ReadWriteByte(crc);
//    
//    //�ȴ���Ӧ����ʱ�˳�
//    while((r1 = SPI_ReadWriteByte(0xFF))==0xFF)
//    {
//        Retry++;
//        if(Retry > 200)break; 
//    }   
//    //�ر�Ƭѡ
//    SD_CS=1;
//    //�������϶�������8��ʱ�ӣ���SD�����ʣ�µĹ���
//    SPI_ReadWriteByte(0xFF);	 
//    //����״ֵ̬
//    return r1;
//}


/*******************************************************************************
* Function Name  : SD_SendCommand_NoDeassert
* Description    : ��SD������һ������(�����ǲ�ʧ��Ƭѡ�����к������ݴ�����
* Input          : u8 cmd   ���� 
*                  u32 arg  �������
*                  u8 crc   crcУ��ֵ
* Output         : None
* Return         : u8 r1 SD�����ص���Ӧ
*******************************************************************************/
//u8 SD_SendCommand_NoDeassert(u8 cmd, u32 arg, u8 crc)
//{
//    unsigned char r1;
//    unsigned char Retry = 0;    
//	SD_CS=1;//�ͷ�
//    SPI_ReadWriteByte(0xff);//����д������ʱ
//	SPI_ReadWriteByte(0xff);  
//    //Ƭѡ���õͣ�ѡ��SD��
//    SD_CS=0;	    
//    //����
//    SPI_ReadWriteByte(cmd | 0x40); //�ֱ�д������
//    SPI_ReadWriteByte(arg >> 24);
//    SPI_ReadWriteByte(arg >> 16);
//    SPI_ReadWriteByte(arg >> 8);
//    SPI_ReadWriteByte(arg);
//    SPI_ReadWriteByte(crc);	 
//    //�ȴ���Ӧ����ʱ�˳�
//    while((r1 = SPI_ReadWriteByte(0xFF))==0xFF)
//    {
//        Retry++;
//        if(Retry > 200)break;   
//    }
//    //������Ӧֵ
//    return r1;
//}

//#define CMD0    0       //����λ
//#define CMD9    9       //����9 ����CSD����
//#define CMD10   10      //����10����CID����
//#define CMD12   12      //����12��ֹͣ���ݴ���
//#define CMD16   16      //����16������SectorSize Ӧ����0x00
//#define CMD17   17      //����17����sector
//#define CMD18   18      //����18����Multi sector
//#define CMD24   24      //����24��дsector
//#define CMD25   25      //����25��дMulti sector
//#define CMD55   55      //����55��Ӧ����0x01
//#define CMD58   58      //����58����OCR��Ϣ
//#define CMD59   59      //����59��ʹ��/��ֹCRC��Ӧ����0x00


////ͨ��Э���ϣ�ACMD�����CMD���������BIT6��Ϊ���ֵģ�����ֻ��Ϊ��������ֵķ���
//#define ACMD23  (23 | (u8)BIT6)		//����23�����ö�sectorд��ǰԤ�Ȳ���N��block
//#define ACMD41  (41 | (u8)BIT6)		//����41��Ӧ����0x00


//SD����CMDָ��ĺ궨��
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
