#include "debug.h"
#include "sys.h"
#include "mmc_sd.h"

#include  "stm32f10x__spi.h"

/* MMC_SD ģ�齫Ҫ�����õĺ��� */
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
/* ���� CMD �ͷ������͵�ӳ���ϵ */
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
/* ���� ACMD �ͷ������͵�ӳ���ϵ */
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

/**	@brief �õ�R7���͵ķ���ֵ */
void SD_Card::GetResponse(R7* p_r7){
	R7& r7 = *p_r7;
	GetResponse(&r7.r1);
	
	//�õ�voltage information
	r7.voltage_info = ReadByte();
	r7.voltage_info <<= 8;	/* ����8λ��׼��������һ���ֽ� */
	r7.voltage_info += ReadByte();
	r7.voltage_info <<= 8;	/* ����8λ��׼��������һ���ֽ� */
	r7.voltage_info += ReadByte();
	r7.voltage_info <<= 8;	/* ����8λ��׼��������һ���ֽ� */
	r7.voltage_info += ReadByte();
}

/**	@brief	�õ�R3���͵ķ���ֵ */
void SD_Card::GetResponse(R3* p_r3){
	R3& r3 = *p_r3;
	GetResponse(&r3.r1);
	/* �õ�ocr */
	r3.ocr = ReadByte();
	r3.ocr <<= 8;	/* ����8λ��׼��������һ���ֽ� */
	r3.ocr += ReadByte();
	r3.ocr <<= 8;	/* ����8λ��׼��������һ���ֽ� */
	r3.ocr += ReadByte();
	r3.ocr <<= 8;	/* ����8λ��׼��������һ���ֽ� */
	r3.ocr += ReadByte();
}

/*	�õ�R1b���͵ķ���ֵ */
void SD_Card::GetResponse(R1b* p_r1b) {
	/* �ȵõ�r1 */
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
    /* �������϶�������8��ʱ�ӣ���SD�����ʣ�µĹ��� */
	WriteDummyByte();

    return;
}

/*
�� SendCMD �� SendACMD ���ã�Ϊ�˽�ʡ�������������ĺ�����
*/
void SD_Card::CommandBegin(u8 cmd, u32 arg, u8 crc){
	/* ����д������ʱ */
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

    /* �������϶�������8��ʱ�ӣ���SD�����ʣ�µĹ��� */
	WriteDummyByte();

    return;
}

#if 0

/*******************************************************************************
* Function Name  : SD_GetCID
* Description    : ��ȡSD����CID��Ϣ��������������Ϣ
* Input          : u8 *cid_data(���CID���ڴ棬����16Byte��
* Output         : None
* Return         : u8 
*                  0��NO_ERR
*                  1��TIME_OUT
*                  other��������Ϣ
*******************************************************************************/
u8 SD_GetCID(u8 *cid_data) {
    R1 r1;
    /* ��CMD10�����CID */
    SendCommand(CMD(10), 0, 0xFF, (Response *)&r1);
    if(r1 != 0x00)return r1;  /* û������ȷӦ�����˳������� */
    SD_ReceiveData(cid_data, 16, RELEASE);	/* ����16���ֽڵ����� */
    return 0;
}

/*******************************************************************************
* Function Name  : SD_GetCSD
* Description    : ��ȡSD����CSD��Ϣ�������������ٶ���Ϣ
* Input          : u8 *cid_data(���CID���ڴ棬����16Byte��
* Output         : None
* Return         : u8 
*                  0��NO_ERR
*                  1��TIME_OUT
*                  other��������Ϣ
*******************************************************************************/
u8 SD_GetCSD(u8 *csd_data){
    R1 r1;	 
    //��CMD9�����CSD
    SendCommand(CMD(9), 0, 0xFF, (Response *)&r1);
    if(r1 != 0x00)return r1;  //û������ȷӦ�����˳�������  
    //����16���ֽڵ�����
    SD_ReceiveData(csd_data, 16, RELEASE); 
    return 0;
}
 
/*******************************************************************************
* Function Name  : SD_GetCapacity
* Description    : ��ȡSD�����������ֽڣ�
* Input          : None
* Output         : None
* Return         : u32 capacity 
*                   0�� ȡ�������� 
*******************************************************************************/
u32 SD_GetCapacity(void){
    u8 csd[16];
    u32 Capacity;
    u8 r1;
    u16 i;
	u16 temp;  
    //ȡCSD��Ϣ������ڼ��������0
    if(SD_GetCSD(csd)!=0) return 0;	    
    //���ΪSDHC�����������淽ʽ����
    if((csd[0]&0xC0)==0x40)
    {									  
	    Capacity=((u32)csd[8])<<8;
		Capacity+=(u32)csd[9]+1;	 
        Capacity = (Capacity)*1024;//�õ�������
		Capacity*=512;//�õ��ֽ���			   
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
    	Capacity *= (u32)temp;//�ֽ�Ϊ��λ 	  
    }
    return (u32)Capacity;
}

/*******************************************************************************
* Function Name  : SD_WriteSingleBlock
* Description    : д��SD����һ��block
* Input          : u32 sector ������ַ��sectorֵ���������ַ�� 
*                  u8 *buffer ���ݴ洢��ַ����С����512byte�� 
* Output         : None
* Return         : u8 r1 
*                   0�� �ɹ�
*                   other��ʧ��
*******************************************************************************/
u8 SD_WriteSingleBlock(u32 sector, const u8 *dat)
{
    R1 r1;
    u16 retry;

    //����Ϊ����ģʽ
    //SPI_SetSpeed(SPI_SPEED_HIGH);	   
    //�������SDHC����������sector��ַ������ת����byte��ַ
    if(SD_Type!=SD_TYPE_V2HC)
    {
        sector = sector<<9;
    }   
    SendCommand(CMD(24), sector, 0x00,  (Response*)&r1);
    if(r1 != 0x00)
    {
        return r1;  //Ӧ����ȷ��ֱ�ӷ���
    }
    
    //��ʼ׼�����ݴ���
    cs=0;
    //�ȷ�3�������ݣ��ȴ�SD��׼����
    SD_WriteByte(0xff);
    SD_WriteByte(0xff);
    SD_WriteByte(0xff);
    //����ʼ����0xFE
    SD_WriteByte(0xFE);

    //��һ��sector������
	SD_WriteMultiBytes(dat, 512);

    //��2��Byte��dummy CRC
    SD_WriteByte(0xff);
    SD_WriteByte(0xff);
    
    //�ȴ�SD��Ӧ��
    r1 = SD_ReadByte();
    if((r1&0x1F)!=0x05)
    {
        cs=1;
        return r1;
    }
    //�ȴ��������
    retry = 0;
    while(!SD_ReadByte())
    {
        retry++;
        if(retry>0xfffe)        //�����ʱ��д��û����ɣ������˳�
        {
            cs=1;
            return 1;           //д�볬ʱ����1
        }
    }
    //д����ɣ�Ƭѡ��1
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
	SPI_Config cfg;	/* Ĭ�����ü��� */
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
	
	//�Ȳ��� > 74 �����壬��SD���Լ���ʼ�����
    for(u16 retry=0;retry<12;retry++)	WriteByte(0xFF);	    

    //-----------------SD����λ��idle��ʼ-----------------

    //ѭ����������CMD0��ֱ��SD������0x01,����IDLE״̬
    //��ʱ��ֱ���˳�

	for(u16 retry = 250; 1; ){
		SendCMD<0>(0,0x95,&r1);
		/* �����ʼ���ɹ�����ôr1����0x01 */
		if(r1==0x01)
			break;
		if(--retry==0){
			return INIT_ERROR;
		}
    }
	
	u16 retry;
	//-----------------SD����λ��idle����-----------------
	SendCMD<8>(0x1aa,0x87,&cmd8);
    //�����Ƭ�汾��Ϣ��v1.0�汾�ģ���r1=0x05����������³�ʼ��
    if(cmd8.r1 == 0x05) { /* δ���ԡ����� */
        //���ÿ�����ΪSDV1.0����������⵽ΪMMC�������޸�ΪMMC
        SD_Type = SD_TYPE_V1; 
        
		/********************SD����MMC����ʼ����ʼ*********************/	 
        /* ������ʼ��ָ��CMD55+ACMD41
         * �����Ӧ��˵����SD�����ҳ�ʼ�����
		 * û�л�Ӧ��˵����MMC�������������Ӧ��ʼ��
		 */
        retry = 400;
		while(retry) {
			SendCMD<55>(0,0,&r1);
			if(r1 == 0xff)	return INIT_ERROR;
			SendACMD<41>(0,0,&r1);
			if(r1 == 0)	//�õ���ȷ��Ӧ�󣬷�ACMD41��Ӧ�õ�����ֵ0x00
				break;
			retry--;
		}
		// �ж��ǳ�ʱ���ǵõ���ȷ��Ӧ
		// ���л�Ӧ����SD����û�л�Ӧ����MMC��
		//----------MMC�������ʼ��������ʼ------------
		if(retry == 0) {
            retry = 400;
            //����MMC����ʼ�����û�в��ԣ�
			while(retry){
				SendCMD<1>(0,0,&r1);
				if(r1 == 0x00)	//��ȷ��Ӧ
					break;
				retry--;
            }
            if(retry==0)return INIT_ERROR;   //MMC����ʼ����ʱ		    
            //д�뿨����
			SD_Type = SD_TYPE_MMC;
        }
        //----------MMC�������ʼ����������------------	    

		/* ����SPIΪ����ģʽ */
		spi1.SPI_SetBaudRate(SPI_PCLK_DIV_4);   

		WriteByte(0xFF);
		
		SendCMD<59>(0,0x95,&r1);	/* ��ֹCRCУ�� */
		if(r1 != 0x00) return INIT_ERROR;  /* ������󣬷���r1 */

		/* ����Sector Size */
		SendCMD<16>(512,0x95,&r1);
		if(r1 != 0x00)return INIT_ERROR;//������󣬷���r1		 
		//-----------------SD����MMC����ʼ������-----------------
    } 
	/* SD��ΪV1.0�汾�ĳ�ʼ������ */
	/* ������V2.0���ĳ�ʼ��
     * ������Ҫ��ȡOCR���ݣ��ж���SD2.0����SD2.0HC��
	 */
	else if(cmd8.r1 == 0x01)
	{
		dout<<"\nSD2.0\n"<<endl;			 
        
		/* �жϸÿ��Ƿ�֧��2.7V-3.6V�ĵ�ѹ��Χ */
		if( ((u16)(cmd8.voltage_info)) == 0x000001aa)	
		{
			dout<<"Support Voltage 2.7V - 3.6V."<<endl;

			retry = 250;
		    while(retry) {
				SendCMD<55>(0,0,&r1);
				if(r1 != 0x01) { /* ���CMD55������Ч�����ش��� */
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
		 	/* ����ѭ���󣬼��ԭ�򣺳�ʼ���ɹ���or ���Գ�ʱ�� */
			if(retry == 0){
				dout<<"\nCMD55 + ACMD41 timeout."<<endl;
				return INIT_ERROR; //TIME OUT 
			}
					     
            //��ʼ��ָ�����ɣ���������ȡOCR��Ϣ
			
            //-----------����SD2.0���汾��ʼ-----------
			
            SendCMD<58>(0,0,&r3);
			//�����յ���OCR�е�bit30λ��CCS����ȷ����ΪSD2.0����SDHC
            //���CCS=1��ΪSDHC		���CCS=0��ΪSD2.0
			if( checkb(r3.ocr, BIT(30))) {	/* SDHC */
				SD_Type = SD_TYPE_V2HC;    //���CCS
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
			
			//-----------����SD2.0���汾����----------- 
			
            /* ����SPIΪ����ģʽ */
			spi1.SPI_SetBaudRate(SPI_PCLK_DIV_4);
        }
		else {
			dout<<"\nCurrent voltage is not supported!\n"<<endl;
			return VOLTAGE_INCOMPATIBLE;	//��֧�����ڵĵ�ѹ��Χ
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
* Description    : �ȴ�SD��Ready
* Input          : None
* Output         : None
* Return         : u8 
*                   0�� �ɹ�
*                   other��ʧ��
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
	�ں����н�����װ�࣬�޶����� ReceiveDataBlock ֻ�ܸ����
	����ʹ�á�
	*/
	struct CReceiveDataBlock{
		finline
		Result ReceiveDataBlock(u8 *dat, u16 len){
			//�ȴ�SD������������ʼ����0xFE
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

			/* �������� */
			ReadMultiBytes(dat, len);

			//������2��αCRC��dummy CRC��
			WriteByte(0xFF);
			WriteByte(0xFF);
																		
			return SUCCEEDED;
		}
	}fun;
	
	Select();
    do{ /* ��ʼ�������� */
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
* Description    : ��SD����һ��block
* Input          : u32 sector ȡ��ַ��sectorֵ���������ַ�� 
*                  u8 *buffer ���ݴ洢��ַ����С����512byte�� 
*******************************************************************************/
Result SD_Card::ReadSingleBlock(u32 sector, u8 *buffer){
	R1 r1;	    
		   
    /* �������SDHC����������sector��ַ������ת����byte��ַ */
	if(SD_Type != SD_TYPE_V2HC){
        sector *= blockLength;
    }
	/* ������ */
	SendCMD<17>(sector,0,&r1);
	if(r1 != 0x00) {
		return FAILED; 		   							  
	}
	
	Result r = ReceiveDataBlocks(buffer, blockLength, 1);
	
	return r;
}

Result SD_Card::ReadMultiBlocks(u32 sector, u8 *buffer, u8 count) {

 	/* �������SDHC����sector��ַת��byte��ַ */
    if(SD_Type!=SD_TYPE_V2HC)
        sector *= blockLength;
	
	R1b r1b;
	/* ����������� */
	SendCMD<18>(sector,0,&r1b.r1);
	if(r1b.r1 != 0x00)	{
		dout<<"SD_Card::ReadMultiBlock-cmd18 failed!"<<endl;
		return FAILED;
	}
	Result r = ReceiveDataBlocks(buffer, blockLength, count);
	
	/* ����ֹͣ�������û�д����顣���� -*-*-*-*-*-*-*-*-*-*-*-*-*-*/
	SendCMD<12>(0,0,&r1b);
	
	return r;
}

Result SD_Card::WriteSingleBlock(u32 sector, const u8 *dat) {
	/*
	�������SDHC����������sector��ַ������ת����byte��ַ
	*/
	if(SD_Type!=SD_TYPE_V2HC)
        sector *= blockLength;
	
    R1 r1;
	SendCMD<24>(sector,0x00,&r1);
	/* Ӧ����ȷ��ֱ�ӷ��� */
	if(r1 != 0x00){
		dout<<"SD_Card::WriteSingleBlock cmd 24 failed"<<endl;
		return FAILED;
	}
	
    //��ʼ׼�����ݴ���
    Select();
	WriteDummyByte();
	WriteDummyByte();
	WriteDummyByte();
	
    /* ����ʼ���� */
	WriteByte(SINGLE_WR_BLOCK_START);

    /* ��һ��sector������ */
	WriteMultiBytes(dat, blockLength);

    //��2��Byte��dummy CRC
    WriteDummyByte();
	WriteDummyByte();
    
    /* �ȴ�SD��Ӧ�� */
    r1 = ReadByte();
    if((r1&0x1F)!=0x05){
		DeSelect();
		dout<<"SD_Card::WriteSingleBlock card invalid response"<<endl;
        return FAILED;
	}
	
    /* �ȴ�������� */
	u32 retry = 0;
    while(!ReadByte()){
        retry++;
        if(retry>0x3ffff) { /* �����ʱ��д��û����ɣ������˳� */
            DeSelect();
			dout<<"SD_Card::WriteSingleBlock wait too long"<<endl;
            return FAILED; /* д�볬ʱ */
		}
    }
	
	DeSelect();
    WriteDummyByte();

    return SUCCEEDED;
}

Result SD_Card::WriteMultiBlocks(u32 sector, const u8 *dat, u8 count) {
    R1 r1;
	
	/* �������SDHC����������sector��ַ������ת����byte��ַ */
	if(SD_Type != SD_TYPE_V2HC)sector *= blockLength;
	/* ���Ŀ�꿨����MMC��������ACMD23ָ��ʹ��Ԥ���� */
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
	SendCMD<25>(sector, 0x00,&r1); /*�����д��ָ�� */
    if(r1 != 0x00){
		dout<<"SD_Card::WriteMultiBlock CMD25 failed."<<endl;
		return FAILED;  /* Ӧ����ȷ��ֱ�ӷ��� */
	}
	
	Select();
    WriteDummyByte();
	WriteDummyByte();
	WriteDummyByte();
    //--------������N��sectorд���ѭ������
    do{
        /* ����ʼ���� */
        WriteByte(MULTI_WR_BLOCK_START);
		
		WriteMultiBytes(dat,blockLength);
        
		//��2��Byte��dummy CRC
        WriteDummyByte();
        WriteDummyByte();
        
        //�ȴ�SD��Ӧ��
        r1 = ReadByte();
        
		if((r1&0x1F)!=0x05){
            DeSelect();
			dout<<"SD_Card::WriteMultiBlock bad card response."<<endl;
            return FAILED;
		}
		
		/* �ȴ�������� */
		u32 retry = 0;
		while(!ReadByte()){
			retry++;
			if(retry>0x3ffff) { /* �����ʱ��д��û����ɣ������˳� */
				DeSelect();
				dout<<"SD_Card::WriteMultiBlock wait too long"<<endl;
				return FAILED; /* д�볬ʱ */
			}
		}

    }while(--count);//��sector���ݴ������
    
    /* �������������� */
	WriteByte(MULTI_WR_STOP);
	for(int i=NBR_MAX; i;--i)
		WriteDummyByte();
    if(WaitReady()){ /* �ȴ�׼���� */
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
* Description    : ��ȡSD����CSD��Ϣ�������������ٶ���Ϣ
* Input          : u8 *cid_data(���CID���ڴ棬����16Byte��
* Output         : None
*****************************************/
Result SD_Card::GetCSD(u8 *csd_data){
    R1 r1;	 
	//��CMD9�����CSD
	SendCMD<9>(0, 0xFF, &r1);
	if(r1 != 0x00){
		dout<<"SD_Card::GetCSD CMD<9> faild!"<<endl;
		return FAILED;
	}
	//����16���ֽڵ�����
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
	output<<"��ʼ���ɹ�"<<endl;

	Result r = sd.ReadMultiBlocks(0,buf,2);
	if(r==SUCCEEDED){
		output<<"���ɹ�"<<endl;
		PrintBlock(buf, 1024);
	}
	
	r = sd.GetSCR(buf);
	if(r==SUCCEEDED){
		output<<"�� SCR �ɹ�"<<endl;
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
