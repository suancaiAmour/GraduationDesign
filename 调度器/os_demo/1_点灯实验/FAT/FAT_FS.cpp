//#define FAT_FS_C	//������������ģ��ı���
#ifdef FAT_FS_C
#define NDEBUG	//������

#include "Debug.h"
#include "FAT_FS.h"
#include "utilities.h"
#include "memory.h"
#include "FileInfo.h"
#include "FAT_Struct.h"

//����SD������Ҫ�ṩ���ⲿ����
//����ǻ��������洢�豸���������޸İ�����ͷ�ļ��ͺ궨��
//�ṩ�ĺ���������Ը���Ψһ������ַ���ж�д���������н��ṩ���ⲿ������һ����Ӧ�İ�װ
#include "MMC_SD.h"
//ʧ���˶�Ҫ���ط�0ֵ
finline u8 ReadSector(u32 sector_address,u8* buffer) 	
	{	return SD_ReadSingleBlock(sector_address, buffer);}
finline u8 WriteSector(u32 sector_address,const u8* buffer) 
	{	return SD_WriteSingleBlock(sector_address, buffer);}


namespace nstd{

typedef struct
{
	u16	bytes_per_sector;		//ÿ�����ֽ���
	u8	sectors_per_cluster;	//ÿ��������
	u8 	FAT_num;				//FAT��
	u16 sectors_per_FAT;		//ÿ��FAT���������
	u32	root_cluster;			//��Ŀ¼�غ�
	u32 FAT1_address;			//FAT1�����������ַ
	u32 total_sectors;			//FAT32��������������
	//ɾ��һ���ļ��Ժ�ҲҪ�������ඡ�����
	u32 cur_free_cluster;		//�����ŵ�ǰһ�����дصĴغ�
//NND����51��Ƭ���ϳ�ʼ�����Ҫ�ó�ʱ�䣬�޷����ܣ��Ȳ�֧�����
//	u32 free_clusters_num;		//���дص�����
}FAT32Info;						//��¼FAT32��Ϣ�Ľṹ��

FAT32Info xdata fat32;

enum{
	FAT32_EOC = 0x0fffffff	//FAT32�����ر��, end of cluster
};
//FAT16_EOC

#define FAT32	1
typedef struct{
	u8 type;					//�ļ�ϵͳ������
	u32 DBR_sector;				//DBR ��������ַ
}FilesysInfo;					//��¼�ļ�ϵͳ����Ϣ�Ľṹ��
FilesysInfo xdata filesys;


File xdata cur_dir;								//current directory	��ǰĿ¼

//˵����
//	�ļ�ϵͳ�ڲ��Ķ�д����������������RAM��С���ޣ�������������ڲ���cur_dir�ṹ��
//	���������ֱ��򿪵��ļ���ͬʹ����
unsigned char xdata RWbuffer[BUFF_SIZE]= {0}; 	//read write buffer

#define CONTINUOUS	0							//�������

#define DIRITEM_LENGTH	32						//Ŀ¼��ĳ���

/***************************************�������ڲ���������***************************************/

static u32 GetSectorAddr(u32 cluster,u8 sector_offset);

#define GCL_ERROR		0xffff
static u16 GetClusterList(u32 start, u32* list, u16 max_list_item);

u8 MoveToFirstSector(File* p_file);

u8 SearchItem(u8* name_x, u8 len, File* p_dir);

#define DIRITEM_EMPTY	0		//����Ŀ
#define DIRITEM_SHORT	1		//�Ϸ����ļ���
#define DIRITEM_DELETED	2		//�����ѱ�ɾ��
#define DIRITEM_LONG	3		//���ļ���
#define DIRITEM_ERROR	0xff	//����
u8 CheckDirItem(u8* pDirItem);
char CmpShortFileName_X(const u8* name_x, u8 len,const u8* pDirItem);

#define UFB_SUCCEED			0		//�ɹ�
#define UFB_WRITE_ERROR		1		//д����
#define UFB_READ_ERROR		2		//������
u8 UpdateFileBuffer(u32 sector, File* p_file);

u8 dirseek(File* p_dir, signed long int offset, u8 fromwhere);

//�������Ľ�β�غ�
#define SECTOR_END		0xffffffff
u32 MoveToNextSector( File* p_file );

u8 GoToFileDir(File *p_file, File *p_dir);

static
u8 NewDirItem(const u8* name_x,u8 attribute, File* p_dir);
static
u8 AllocNewCluster(File* p_file);

u8 FillCluster(u32 cluster, u8 val);

//˵����
//	�ж�һ���ļ��ṹ��������ǲ�����Ŀ¼
#define IsSubDir(p_file)		( ((p_file)->attribute & SUB_DIR) != 0 )

//˵����
//	�ж�һ������Ŀ¼���Ե��ļ��ṹ����ļ�ָ���Ƿ��Ѿ������ļ�β
#define IsEndOfFile(p_file)		( (p_file)->ptr >= (p_file)->size)

//˵����
//	�ж�һ����Ŀ¼���Ե��ļ��ṹ���Ƿ��Ѿ������ļ�β������ǣ��Ͳ�Ӧ���ٵ���MoveToNextSector��
#define IsEndOfDir(p_file)		(((p_file)->next_cluster == 0x0fffffff) && ((p_file)->buf.pos >= fat32.bytes_per_sector))

/***************************************�������ڲ���������***************************************/

/******************************���µ����ݿɸ���MCU�ľ�����������޸�******************************/
//ע��
//	��������ṩ3K���ϵ��ڴ�أ�������ݽ��鱣����״

//˵����
//	����һ���µ�File�ṹ�壬�����ؽṹ���ָ��
//���أ�
//	File�ṹ���ָ�룬���ʧ�ܣ�����NULL
//��ע��
//	����������Ը���MCU����������޸�
File* NewFileStruct()
{
	// ע��p_fileһ����ָ��_MALLOC_MEM_����������� malloc ʧ�ܷ��� NULL
	// p_file == NULL Ҳ����Ϊ��
	File* p_file;	
	//PFile p_file;

	//���ṹ�����ռ�
	p_file = (File*)malloc( sizeof(File) );
	if(p_file == NULL)
	{
		DB_SendString("\nMemory allocation for file error.\n");
		return NULL;
	}

	DB_SendString("\nbytes per sector :");DB_SendDec((u32)(fat32.bytes_per_sector));
	MemorySet((u8*)p_file, (u16)sizeof(File), (u8)0);	//����ṹ��
	DB_SendString("\nnow bytes per sector :");DB_SendDec(fat32.bytes_per_sector);
	
	// ������51��Ƭ���ϵ��ڴ�Ƚ�С�������޷�����Ϊһ���ļ���һ�黺��
	// Ҳ������Ϊ���������Ҫ����PrepFileOp(p_file)�ͱ��� p_curFile
	// �������ķ�ʽҲ��Ӱ�쵽 DeleteFileStruct ����Ϊ
	((File*)p_file)->buf.p = RWbuffer;
	
	//��ʼ��buffer�ǷǷ���
	((File*)p_file)->buf.pos = fat32.bytes_per_sector;

	return p_file;
}

//˵����
//	��������RWbuffer�������ĸ��ļ��ı�����NULL��ʾ����ϵͳ
//	��ΪҪʵ���߼���ÿ���ļ�����ռһ������������MUC��RAM�Ƚ�С��
//	���Կ���������ʱ�任�ռ��ԭ����ʵ�֡�
static PFile xdata p_curFile = NULL;

//˵����
//	��ÿ���ļ�����ǰ����������������Ϊ�ļ�����׼��buffer
//	ʹ��ÿ���ļ����߼��϶���ռһ��buffer
//���أ�
//	�ɹ�����0
//	ʧ�ܷ��ط�0
u8 PreFileOp(File* p_newFile)
{
	if( p_curFile != p_newFile)
	{
		if( p_curFile != NULL )
		{	//�ú������ݻ����Ƿ��ѱ�д����и���
				if( p_curFile->buf.is_changed != 0)
				{	//��buffer��is_changedΪ��ʱ����ô�ͻ���д
					if( WriteSector(p_curFile->buf.sector, p_curFile->buf.p) )
					{
						DB_SendString("\nWrite sector error!\n");
						return 1;
					}
					p_curFile->buf.is_changed = 0;		//�����־λ
				}
		}
		if( p_newFile != NULL)	//NULL��������������ʹ�ã���GetClusterList
		{
			//���ﲻ���������жϣ���������
			//if(p_newFile->buf.pos < fat32.bytes_per_sector )	//�������������Ч�ģ��Ÿ���
			if( ReadSector(p_newFile->buf.sector, p_newFile->buf.p) != 0)
			{
				DB_SendString("\nRead sector error!\n");
				return 1;
			}
		}
		p_curFile = p_newFile;		//���浱ǰ�ڲ������ļ�
	}
	return 0;
}

//˵����
//	ɾ��File�ṹ�壬�ͷſռ�
//������
//	p_file	ָ��Ҫɾ����File�ṹ��
void DeleteFileStruct(File* p_file)
{
	free(p_file);
}

//��FileSysInit�����Ҳ������Ҫ����
//	cur_dir.buf.p = RWbuffer;		//���õ�ǰĿ¼�Ļ�����

/******************************���ϵ����ݿɸ���MCU�ľ�����������޸�******************************/

//˵����
//	��һ��������һ��File�ṹ���buffer��
//������
//	sector		Ҫ����������
//	p_file		File�ṹ���ָ��
//��ע��
//	��buffer��is_changedΪ0ʱ��
//		���buffer�����Ӧ������ = sector����ô��������Ͳ����������p_file��buffe
//		���buffer�����Ӧ������ != sector,�һ�����ָ������Ч��
//			����ô��������ͻ��������p_file��buffer 
//			�����ܻ�����ָ����û��Ч��������� p_file->buf.sector ����
//	��buffer��is_changedΪ��0ʱ����ô�ͻ���д�����Ӧ������
//���أ�
//#define UFB_SUCCEED			0		//�ɹ�
//#define UFB_WRITE_ERROR		1		//д����
//#define UFB_READ_ERROR		2		//������
u8 UpdateFileBuffer(u32 sector, File* p_file)
{
	if(p_file->buf.is_changed != 0)
	{	//��buffer��is_changedΪ��ʱ����ô�ͻ���д
		if( WriteSector(p_file->buf.sector, p_file->buf.p) )
		{
			DB_SendString("\nUpdateFileBuffer Write sector error!\n");
			return UFB_WRITE_ERROR;
		}
		p_file->buf.is_changed = 0;		//����
	}
	if( p_file->buf.sector != sector  )
	{
		//���������ָ����Ч����û�б�Ҫ���и���
		if( p_file->buf.pos < fat32.bytes_per_sector)
		{	
			if( ReadSector( sector, p_file->buf.p) != 0)
			{
				DB_SendString("\nUpdateFileBuffer Read sector error!\n");
				p_file->buf.sector = SECTOR_END;	//��ʱbuffer�ǷǷ���
				return UFB_READ_ERROR;
			}
		}
		p_file->buf.sector = sector;	//����buffer��Ӧ������
	}

	//���»�������ӵ����
	p_curFile = p_file;

	return UFB_SUCCEED;	
}


//˵����
//	�ĵ�ǰĿ¼Ϊ��Ŀ¼
//ע�⣺
//	����Ӧ���ļ�ϵͳ��ʼ�������ʹ��
void GoRootDir(File* p_dir)	
{										
	DB_SendString("\nGo root dir, root cluster address :");			
	p_dir->cluster =  fat32.root_cluster;
	DB_SendDec(p_dir->cluster);
	p_dir->dir.cluster = fat32.root_cluster;
	p_dir->dir.pos = 0;
	MoveToFirstSector(p_dir);	//��ʼ���ļ�ָ��
}

//˵����
//	�õ������߼������� DBR ���׵�ַ
//������
//	MBR_buffer	��¼MBR�������ڴ�
//���أ�
//	DBR����������ַ������д��󣬷���0
//��ע��
//	1. �ݲ�֧����չ����
u32 GetDBRAddr(u8* MBR_buffer)
{
	if(MBR_buffer[0x1fe]!=0x55 || MBR_buffer[0x1ff]!=0xaa)	//�ж�SD�������Ƿ����
	{
		DB_SendString("Read MBR error !\n\n");
		return 0;
	}

	//�õ�������ʼ��ַ
	if(GetDWord_S(MBR_buffer, 0x1C6))  		
		return GetDWord_S(MBR_buffer, 0x1C6); 
	else if(GetDWord_S(MBR_buffer, 0x1D6))
		return GetDWord_S(MBR_buffer, 0x1D6); 
	else if(GetDWord_S(MBR_buffer, 0x1E6))
		return GetDWord_S(MBR_buffer, 0x1E6); 
	else		//������չ���������ش���
		return 0;

}

//˵����
//	��ʼ�� FAT32Info �ṹ��
//������
//	DBR_buffer	�������������ʱ���Ǽ�¼DBR �������ڴ棬
//				�����ڲ����������������Ϣ���DBR_bufferָ����ڴ��
//				���н�һ���ĳ�ʼ������
//	p_fat32		ָ�����ʼ���Ľṹ���ָ��
//ע�⣺
//	1.	DBR_bufferָ����ڴ��������ں�����ִ�й����лᱻ����
//		���Ժ���ִ�����DBR_bufferָ����ڴ���������û�������
u8 InitFat32(u8* DBR_buffer, u32 DBR_sector, FAT32Info* p_fat32)
{
//	u32 reserved_sectors;			//FAT����������
	u16 index = 0;

	p_fat32->total_sectors = GetDWord_S(DBR_buffer, 0x20);
	DB_SendString("\nTotal sectors :");
	DB_SendDec( p_fat32->total_sectors ); 
	DB_SendString("\nCapacity :");
	DB_SendDec(p_fat32->total_sectors * 512/1024/1024);
	DB_SendString(" MB\n");

	//��ȡ�ļ�ϵͳ�����ֽ���
	p_fat32->bytes_per_sector = GetWord_S(DBR_buffer,0x0b);
	DB_SendDec(p_fat32->bytes_per_sector);
	DB_SendString(" bytes per sectors \n");
	if(p_fat32->bytes_per_sector != BUFF_SIZE)
		return 1;	//�ݲ�֧�ַ�512�ֽ�ÿ�������ļ�ϵͳ

	//��ȡÿ��������
	p_fat32->sectors_per_cluster = DBR_buffer[0x0d];
	DB_SendDec(p_fat32->sectors_per_cluster);
	DB_SendString(" sectors per cluster \n");

	//��ȡ����������
//	reserved_sectors = GetWord_S(DBR_buffer, 0x0e);
//	DB_SendDec(reserved_sectors);
//	DB_SendString(" reserved sectors \n");

	//��ȡFAT��
	p_fat32->FAT_num = DBR_buffer[0x10];
	DB_SendDec(p_fat32->FAT_num);
	DB_SendString(" FATs \n");

	//��ÿFAT������
	p_fat32->sectors_per_FAT = GetDWord_S(DBR_buffer, 0x24);
	DB_SendDec(p_fat32->sectors_per_FAT);
	DB_SendString(" Sectors per FAT   \n");

	//��ʼ��FAT1_address 
	p_fat32->FAT1_address = DBR_sector + 
					GetWord_S(DBR_buffer, 0x0e);//����������
	DB_SendString("\n FAT1 address:");
	DB_SendDec(p_fat32->FAT1_address);
	
	//��ʼ��
	p_fat32->root_cluster = GetDWord_S(DBR_buffer, 0x2c);	//��¼��Ŀ¼�غ�
	DB_SendString("\nRoot cluster address :");
	DB_SendDec(p_fat32->root_cluster);

	//�������DBR_sector��������Ѿ�û�����ˣ�����������Ϊ����

	//���濪ʼ��ʼ��  cur_free_cluster

	DBR_sector = 0;						//��FAT�����ļ���
	p_fat32->cur_free_cluster = 0;

	while( DBR_sector < p_fat32->sectors_per_FAT )
	{
		//���»������е�����
		if( ReadSector( p_fat32->FAT1_address + DBR_sector, DBR_buffer) != 0)
		{	//��FAT��
			DB_SendString("\nread sector error!\n");
			return 1;
		}
		index = 0;
		while( index < p_fat32->bytes_per_sector )
		{
			if( GetDWord_S(DBR_buffer, index) == 0)
			{	//����������д�
				//���㲢������дصĴغ�
				p_fat32->cur_free_cluster = 
					(DBR_sector * p_fat32->bytes_per_sector + index ) / sizeof(u32);
				goto GET_FREE_CLUSTER_COMPLETE;
			}
			index += sizeof(u32);
		}
		DBR_sector++;
	}
	GET_FREE_CLUSTER_COMPLETE:

	if( p_fat32->cur_free_cluster == 0)
	{	//������дض�û�б���ʼ����˵��û�п��д�
		p_fat32->cur_free_cluster = FAT32_EOC;
		DB_SendString("\nThere is no free cluster!\n");
	}
	else if( p_fat32->cur_free_cluster < p_fat32->FAT_num)
	{	//����
		return 1;
	} 
	DB_SendString("\n cur_free_cluster :\n");
	DB_SendDec(p_fat32->cur_free_cluster);

	//��ʼ�� cur_free_cluster ����

	return 0;
}

//˵����
//	��ʼ���ļ�ϵͳ
//������
//	��
//���أ�
//	0	�ɹ���
//	1	ʧ�ܣ�	
//ע��
//	1. ��ʱ��֧��FAT32��ʽ���ļ�ϵͳ����ÿ������СΪ512�ֽ�
//	2. �ڳ�ʼ���ļ�ϵͳǰҪ�ȳ�ʼ��SD����������ǻ���SD������ô�ͳ�ʼ����Ӧ�Ĵ洢�豸
//	3. �ļ�ϵ�Ĵ��������KEIL�Ķ�̬�ڴ������ƣ������ڵ���FileSysInit֮ǰ����Ҫ����init_mempool����
//	init_mempool��������ϸ˵����ο�KEIL�İ����ĵ�
u8 FileSysInit()
{
	DB_SendString("Read MBR to buffer...\n"); 
	if(ReadSector(0,RWbuffer)) //��MBR,��һ������
	{
		DB_SendString("\nRead MBR error\n");
		return 0;
	}
	filesys.DBR_sector = 0;

	//�������0����û��MBR��DBR����0����
	if(CmpASCArray("MSDOS5.0", RWbuffer+0x03, 8, 0) )	
	{	//�����MBR����ӷ������еõ�DBR�ĵ�ַ
		filesys.DBR_sector = GetDBRAddr(RWbuffer);

		//��ȡDBR��FatBuffer
		DB_SendString("\nRead DBR to buffer...\n");
		if(ReadSector(filesys.DBR_sector, RWbuffer) != 0 )	//�õ�DBR����������Ϣ
		{
			DB_SendString("\nRead DBR Error!\n");
			return 1;
		}
		if(CmpASCArray("MSDOS5.0", RWbuffer+0x03, 8, 0))
		{
			return 1;
		}
	}

	//��ʾ�ļ�ϵͳ����
	DB_SendString("File System: ");	
	if( 0 == CmpASCArray(&(RWbuffer[0x52]), "FAT32", StringLength("FAT32"), 0))
	{
	 	DB_SendString("FAT32\n\n");
		//type = FAT32;
			//��ʼ��fat32
		if(InitFat32(RWbuffer, filesys.DBR_sector ,&fat32) != 0)
		{
			DB_SendString("InitFat32 error!\n");
			return 1;
		}
	}
	else if( 0 == CmpASCArray(&(RWbuffer[0x52]), "FAT16", StringLength("FAT16"), 0))
	{
	  	DB_SendString("FAT16\n\n");
		return 1;	//�ݲ�֧��FAT16
	}
	else
	{
		return 1;	//��֧��FAT32
	}
	
	//��ʼ��cur_dir����
	MemorySet((u8*)&cur_dir, sizeof(File), 0);

	cur_dir.attribute = SUB_DIR;	//����������Ϊ��Ŀ¼
	cur_dir.buf.p = RWbuffer;		//���õ�ǰĿ¼�Ļ�����
				
	GoRootDir(&cur_dir);		//�ѵ�ǰĿ¼����Ϊ��Ŀ¼

	return 0;
}


//˵����
//	��ȡ�������������������ַ
//������
//	cluster	�غ� cluster >= 2
//	sector_offset ��������ƫ�� sector_offset < sectors_per_cluster
//���أ�
//	������ַ �ɹ�
//	1 ʧ��
//��ע��
//	1. ����δ��Ӳ��ķ���
static finline u32 GetSectorAddr(u32 cluster,u8 sector_offset)
{
	cluster = (cluster - 2)*fat32.sectors_per_cluster;
	cluster += fat32.FAT_num*fat32.sectors_per_FAT;
	cluster += fat32.FAT1_address;
	cluster += sector_offset;	//���ϴ�������ƫ��
	return cluster;
}


//˵��
//	��ָ����Ŀ¼���ļ��У�������һ���ļ�������������Ǹ��ļ�����ô��ǰ
//	Ŀ¼�ڲ����ļ�ָ��ͻ�����ָ���ָ�����Ŀ¼��
//������
//	name_x	�ļ���������·���� + '.' + ��չ��
//	len		name_x�ַ����ĳ���
//	p_dir 
//		ָ����Ŀ¼�����Ϊ�գ���ôĬ��Ϊ��ǰĿ¼
//���أ�
//	�ɹ�����0�����򷵻�����ֵ
//ע��
//	1. �����뵱ǰ�ļ�Ŀ¼���ļ��У���Ŀ¼���ļ��У��н�������
//		51��Ƭ���Ķ�ջ���ޣ���XDATAʵ�ֶ�ջ�Ƚ��鷳�����ԾͲ�֧�����������
//	2. �����ִ�Сд
//	3. ��֧��·������
u8 SearchItem(const u8* name_x, u8 len, File*  p_dir)
{
	u8 xdata dirItem = DIRITEM_DELETED;
	DB_SendString("\nIn search item function. searching file:"); DB_SendString((char*)name_x);
	if(p_dir == NULL)
	{
		p_dir = &cur_dir;
	}
	dirseek(p_dir, 0, SEEK_SET);	//��ʼ������һ��Ŀ¼��
	DB_SendString("\nenter searching loop.\n");
	while(1)	//��������	
	{
		dirItem = CheckDirItem( p_dir->buf.p + p_dir->buf.pos );
		
		if( DIRITEM_SHORT == dirItem)
		{	//����Ƕ�Ŀ¼���ô�Ϳ�ʼ�Ƚ��ļ���
			if(0 == CmpShortFileName_X(name_x, len, p_dir->buf.p + p_dir->buf.pos) )
			{
				DB_SendString("\nCmpShortFileName_X :File name is equal! file is found!\n");
				return 0;	//���سɹ�
			}
		}
		else if(DIRITEM_EMPTY == dirItem)
		{	//������Ŀ¼���ǿ�Ŀ¼���ô�����û���ˣ����ؼ�������
			DB_SendString("\nmeet empty item, The file is not found.\n");
			return 1;	//����û�ҵ�
		}

		if( dirseek(p_dir, DIRITEM_LENGTH, SEEK_CUR) != 0 )
		{	//�ƶ��ļ�ָ��ͻ�����ָ��
			DB_SendString("dirseek error. file not found!\n");
			return 2;
		}
		if( IsEndOfDir(p_dir) )	//seek��Ҫ�ж�
		{	//�������Ŀ¼β��
			DB_SendString("reach the end of dir. file not found!\n");
			return 1;
		}
		
	}		
}

//˵��
//	Ŀ¼�ļ�ָ��ͻ���ָ����ָ��һ��Ŀ¼���������������Ŀ¼���
//	��Ϣ������ļ��ṹ��
//������
//	p_file	ָ��һ��Ҫ�������ļ��ṹ��
//	p_dir	ָ��һ��Ŀ¼���ļ��ṹ���ָ��
//			������ֵΪ�գ��ڲ�һ��ʼ���Զ�������ָ��ǰĿ¼cur_dir
//���أ�
//	�ɹ�����0�����򷵻�����ֵ
//ע�⣺
//	1.	��������Ǹ�����SearchItem�ɹ������ʹ�õ�
u8 FillFileStruct(File* p_file, File* p_dir)
{
	if( p_dir == NULL)
	{
		p_dir = &cur_dir;
	}

	//�ȼ��Ŀ¼��ĺϷ���
	if( CheckDirItem( p_dir->buf.p + p_dir->buf.pos) != DIRITEM_SHORT)
	{
		return 1;
	}
	//�ټ�黺��������Ч��
	else if(p_dir->buf.pos + DIRITEM_LENGTH > fat32.bytes_per_sector)
	{
		return 1;
	}

	//������ʼ��
	//��16λ
	p_file->cluster = GetWord_S(p_dir->buf.p, p_dir->buf.pos + 0x14);
	p_file->cluster = (p_file->cluster << 16);
	//��16λ
	p_file->cluster += GetWord_S(p_dir->buf.p, p_dir->buf.pos + 0x1a);
	DB_SendString("\nFiel first Cluster ");
	DB_SendDec(p_file->cluster);
	
	//���ýṹ�����������
	p_file->next_cluster = p_file->cluster;	//��һ��Ҫ���Ĵ�Ϊ��ʼ��
	p_file->next_sector_offset = 0;			//��һ��Ҫ��������

	p_file->size = GetDWord_S( p_dir->buf.p, p_dir->buf.pos + 0x1c );
	DB_SendString("\nSize :");
	DB_SendDec( p_file->size );
	DB_SendString(" bytes.\n");

	p_file->attribute = p_dir->buf.p[p_dir->buf.pos + 0x0b];
	DB_SendString("attribute :");
	DB_SendHex(p_file->attribute);

	p_file->dir.cluster = p_dir->cluster;
	p_file->dir.pos = p_dir->ptr;
	
	MoveToFirstSector(p_file);	//�ļ�ָ��ĳ�ʼ������
	
	return 0;
}

//˵����
//	�ı乤��Ŀ¼��֧��ֱ��ʹ��·��
//������
//	path	Ŀ��Ŀ¼��·��
//	len		path�ĳ���
//	p_dir	Ҫ�ı�Ľṹ�壬���Ϊ�գ�Ĭ���ǵ�ǰĿ¼
//���أ�
//	0		�ɹ�
//	��0		ʧ�ܣ����ҵ�ǰĿ¼���ɸ�Ŀ¼
//ע�⣺																			  
//	�ݲ�֧����һ��Ŀ¼�� .. ����(cmpshortfilename������ܵøĸ�)
u8 ChangeDir(const u8* path, u8 len, File* p_dir)									
{
	u8 idata pos = 0;

	if(p_dir == NULL)
	{
		p_dir = &cur_dir;
	}
	
	if( len == 0 )
	{		//����ַ�������Ϊ�գ�����ʧ��
		GoRootDir(p_dir);
		return 1;
	}

	if( FILE_SEPARATOR == (*path) )	//����ַ�����ĵ�һ�������ļ��ָ���	
	{		//�ĵ�ǰ��ǰĿ¼Ϊ��Ŀ¼�����򣬾��ڵ�ǰĿ¼�½���
		GoRootDir(p_dir);
		path++;						//��λ��ǰ��������Ѿ�û����
		len--;						//����len
		if(len == 0)	
		{	//���ֻ���뵽��Ŀ¼����ô�ͷ���
			return 0;
		}
	}

	while(1)
	{
		pos = Find( path, len, FILE_SEPARATOR);		//�ҵ���һ���ļ��ָ�����λ��
		//��ʱposΪ��һ��Ŀ¼���ĳ���
		if( 0!= SearchItem(path, pos, p_dir) )	//����Ŀ¼
		{
			DB_SendString("\ndir not found, ChangeDir failed.\n");
			return 1;
		}
		if( FillFileStruct(p_dir, p_dir) != 0)
		{
		 	DB_SendString("\nfill file struct error! ChangeDir failed.\n");
			return 1;
		}
		if((cur_dir.attribute & SUB_DIR) == 0)		//����Ƿ�����Ŀ¼
		{	//������Բ�����Ŀ¼����ô�ͷ���ʧ��
			DB_SendString("The path is not a directory.\n");
			GoRootDir(p_dir);
			return 1;
		}
		
		if(pos < len)			
		{
			len -= (pos + 1);	//����len
			if(len == 0)		//�������������������� ��subdir\"
			{	
				return 0;		//��������		
			}	
			path = &(path[ pos + 1] );	//��λ��ǰ��������Ѿ�û������
		}
		else	
		{	//������� ��subdir" ����β��
			return 0;
		}
	}
}

//˵����
//	�ж�Ŀ¼���Ƿ�Ϸ�
//������
//	pDirItem
//		ָ��buffer�е�Ŀ¼����׵�ַ��Ŀ¼��ı�ʾ��ʽ���ļ�ϵͳ�еı�ʾ����һ�£�
//		Ŀ¼��ĳ�����32���ֽ�
//���أ�
//	DIRITEM_EMPTY		����Ŀ
//	DIRITEM_SHORT		�Ϸ����ļ���
//	DIRITEM_DELETED		�����ѱ�ɾ��
//	DIRITEM_LONG		���ļ���
//	DIRITEM_ERROR		����	
//��ע��
//	�������ж��Ƿ��д�������Ч�ʿ��ܻ�Ƚϵ�
#define NOCHECK_ERROR
u8 CheckDirItem( u8* pDirItem )
{
	u8 idata i;
#ifndef NOCHECK_ERROR
	//err_char�е��ַ��ǲ��ܳ�����DirName���κ�λ�õ�
	u8 code err_char[] = { 0x22, 0x2a, 0x2b, 0x2c, 0x2e, 0x2f, 0x3a, 0x3b
		,0x3c, 0x3e, 0x3f, 0x5b, 0x5c, 0x5d, 0x7c, 0x00 /*�Ӹ�0������ֹ���ַ���*/};
#endif
	
	i = pDirItem[0];

	if( i == 0xe5 )	
	{	//�����Ŀ�ѱ�ɾ��
		return DIRITEM_DELETED;
	}
	else if(i == 0)
	{	//����ļ����ĵ�һ���� 0��˵���ǿյ�
		DB_SendString("\nThis item is empty!\n");
		return DIRITEM_EMPTY;
	}
#ifndef NOCHECK_ERROR
	else if(i == 0x20)	
	{	//�ļ����ĵ�һ���ֽ��ǲ���Ϊ0x20��
		return DIRITEM_ERROR;
	}
#endif
	
	else if( ( READ_ONLY | HIDEDDEN | SYSTEM | VOLLABLE ) == pDirItem[0x0b])
	{	//������������ļ����־
		return DIRITEM_LONG;
	}

#ifndef NOCHECK_ERROR
	//��鿴��û�зǷ��ַ�
	for(i = 0; i < 8; i--)
	{
		//��鿴�Ƿ��зǷ��ַ�
		if( pDirItem[i] == err_char[FindChar(pDirItem[i], err_char, 0)] )
		{	//���ȷ���зǷ��ַ������ش���
			return DIRITEM_ERROR;
		}
	}
#endif
	
	//����������˵��û�зǷ��ַ������Է��غϷ��Ķ��ļ���
	return DIRITEM_SHORT;
}

//˵����
//	�Ƚ�һ�� null-terminated string �Ͷ��ļ����е��ļ���(������չ��)����Сд�޹�
//������
//	name_x	�ṩ���ַ���(ASCII���룬GB����Ҳ��)
//	len		name_xָ����ַ����ĳ���
//	pDirItem	�ļ�������ݴ��ļ�ϵͳ�ж����ڴ�buffer�е�
//���أ�
//	0	�ַ������ļ������
//	��0	�ַ������ļ��������
//ע�⣺
//	1.	��������Ƚϵ��Ƕ��ļ������ڵ����������֮ǰ��Ҫ�� CheckDirItem ����ļ���
//��ע��
//	1.	���ڹ������ASCII���ݣ�����Ӧ�þͲ��ø����ǲ��ǹ��������жϱȽϾ����ֽڵ�ʱ
//		��Ҫ��Ҫ������ĸ���Ƚ���
char CmpShortFileName_X(const u8* name_x, u8 len,const u8* pDirItem)
{
	u8 idata i = 0;
	u8 idata j = 0;
	DB_SendString("conpare ");DB_SendString((char*)name_x);
	DB_SendString(" and "); DB_SendString((char*)pDirItem);
	
	//�Ƚ��ļ�������
	while(1)
	{
		if( (i >= len) || ( '.' == name_x[i] ) )
		{	//���name_x���ļ���������������
			if(' ' == pDirItem[i] )
			{	//���pDirItem���ļ�������Ҳ��������
				break;
			}
			else
			{	
				//��� i == 0���ṩ�Ŀ����ǵ�ǰĿ¼��'.'���ϼ�Ŀ¼'..'
				if((i == 0) && (i < len) && (len < 2))
				{
					while(i < len)
					{
						if( (name_x[i] != '.') || (pDirItem[i] != '.'))
							return CmpLetter(name_x[i], pDirItem[i]);
						i++;
					}
					if(pDirItem[i] != ' ')	//�����Ӧ��Ŀ¼����ļ�����������
						return pDirItem[i];	
					return 0;
				}				 
				//���pDirItem���ļ������ֻ�û��������ô�ͷ��ز����
				return pDirItem[i];
			}
		}
		else if( ( i >= 8) ||  (' ' == pDirItem[i] ))
		{	//����ļ����е��ļ���������������ôname_x�е��ļ�������ҲӦ�ý���
			while(1)
			{
				if( (i >= len) || ('.' == name_x[i]) )	
				{	//���name_x���ļ�������Ҳ������
					break;
				}
				else if( ' ' != name_x[i])
				{	//����ļ������ֻ��зǿո��ַ������ز����
					return ' ';
				}
				i++;
			}
			break;
		}
		else if( (0 != CmpLetter(name_x[i], pDirItem[i]) ))// &&
		//	  (0 == j) )	 	//j== 0 ��ʾ����ֽڿ��Ե���ĸ���Ƚ�		//( (name_x[i] != pDirItem[i]))
		{	//�������ȣ�ֱ�ӷ���
			return CmpLetter(name_x[i], pDirItem[i]);		//(name_x[i] - pDirItem[i]);			
		}

		i++;
	}


	//����������˵���ļ����е��ļ������ֺ�name_x���ļ����������
	//��ʱҪô (i == len) ������ (name_x[i] == '.')
	if( i >= len)
	{	//���name_x������������ôĿ¼�����չ��ҲӦ��Ϊ�գ��������
		if(pDirItem[8] == ' ')
		{	//�����չ��ҲΪ��
			return 0;
		}
		else
		{	//�����Ϊ�գ������
			return pDirItem[8];
		}
	}

	j = 0;
	i++;

	//���ڿ�ʼ�Ƚ���չ��
	while(1)
	{
		if( i >= len)
		{	//��� name_x �е���չ����������
			if( ( ' ' != pDirItem[8 + j] ) && ( j < 3) )
			{	//���pDirItem�е���չ������û�н���
				return pDirItem[8 + j];
			}
			else
			{
				return 0;
			}
		}
		else if(j >= 3)
		{	//���Ŀ¼�����չ�������ˣ���ôname_x��Ӧ�õ���\0'�ˣ��������
			if(name_x[++i] == 0)
			{	//���Ҳ��������
				return 0;			
			}
			else
			{	//������ǽ���
				return name_x[i];				
			}
		}
		else if(0 != CmpLetter( name_x[i], pDirItem[8 + j]) )
		{	//��������
			return CmpLetter( name_x[i], pDirItem[8 + j]);
		}
		j++;
		i++;
	}
	
}



//˵����
//	��FAT�õ����б�
//������
//	start			��ʼ�غţ�Ҳ�����б��0����
//	list			ָ���б��ָ��
//	max_list_item	�б�������ɵ�������Ҳ��������ܼ�¼���ٸ���
//����
//	��¼���б������
//	GCL_ERROR ����
static
u16 GetClusterList(u32 start, u32* list, u16 max_list_item)
{
	u8 xdata counter;					//��ʾlist����һ���ɼ�¼��λ��
	u32 xdata  last;					//��һ����¼��List�еĴ���
	//������ʼ����Ϊ��һ��ʼ��(start<<2) / fat32.bytes_per_sector ����� 
	u32 xdata sector_offset = ~((u32)0);	//������¼buffer��Ӧ��������FAT1ƫ�Ƶ�������

	PreFileOp(NULL);

	
	if(start == 0)
	{	//�մ����Ĵ�СΪ0���ļ���Ŀ¼����д����ʼ�غ���0
		list[0] = FAT32_EOC;	//ֱ����0��λд�������־
		return 0;
	}
	else if(start < fat32.FAT_num)
	{							//0�ŵ�(FAT - 1)�Ŵ��Ǳ�����ϵͳ�õ�
		return GCL_ERROR;		//���ش���
	}
	
	list[0] = start;
	last = start;
	counter = 1;	
	while(counter < max_list_item)	
	{	//���ѭ��max_list_item - 1�Σ���һ���Ѿ�����ֵ��
		//start��������б��е���һ����¼������������Ŵ�FAT1ƫ�Ƶ�������
		start = ( (start<<2) / fat32.bytes_per_sector );
		
		if( sector_offset != start)
		{	//����б�Ҫ���»�����
			sector_offset = start;
			//��������buffer
			if(ReadSector( fat32.FAT1_address + sector_offset, RWbuffer) != 0)
			{
				DB_SendString("\nReadSector error!\n");
				return 	GCL_ERROR;
			}
		}		
		//��start��������������ֽ�ƫ��
		start = ((last << 2) % fat32.bytes_per_sector); 

		start = GetDWord_S(RWbuffer, start);			//�ȶ�ȡ�غ�		

		if(start == (last + 1))	
		{	//�������һ����¼��ֵ����������������ܿ��Խ����Ż��������list��������
			last = start;	//������һ����ȡ��ֵ
			if(counter > 2)	
			{	//���ض���λ�÷�Χ���Ż����Լ򻯶�ȡlist�ĺ���
				if(list[counter - 2] == CONTINUOUS)
				{	//�����һ����¼�ڱ��е���ֵ�Ѿ����������
					list[counter - 1] = start;	//������һ��ֵ�����ظ���counter
					continue;	//ֱ������������ж�
				}
				else if(((list[counter - 2] + 2) == start) &&
						((list[counter - 3] + 3) == start))
				{	//����Ѿ����������Σ���ô�Ϳ���д���Ż������
					list[counter - 2] = CONTINUOUS;	//д���Ż����
					list[counter - 1] = start;		//������һ��ֵ�����ظ���counter
					continue;	//ֱ��������������ݣ�����Ҫ����counter
				}
			}
		}

		if(start < fat32.FAT_num)
		{
			return GCL_ERROR;		//���ش���
		}

		list[counter] = start;	//д��list
		counter++;				//����counter
		last = start;

		if(start == FAT32_EOC)	//�������
		{
			return counter;		//��Ϊ���һ��������0x0fffffff,��ʾ����			
		}
		else if(start < fat32.FAT_num)
		{	//���һ��ʼ����start�Ĵغ���û��ʹ�õ�,�Ϳ��ܳ������ִ���
			DB_SendString("\nGetting file cluster list error! cluster < numbers of fat.\n");
			return GCL_ERROR;
		}		
	}
	return (counter + 1);	//���������ѭ����˵��list����
}


//˵����
//	��һ���ļ�����һҪ����λ�ó�ʼ��Ϊ�ļ�ͷ��֧��·������
//������
//	name_x	�ļ������ɺ�·����
//	p_file	�ļ�����Ϣ����������p_fileָ��Ľṹ����
//���أ�
//	0		�ɹ�
//	����	����
//ע�⣺
//	1.	�������ʹ���ļ���һ��Ҫ����fclose�ر��ļ�
//	2.	���������ı��ļ�ϵͳ�ڲ�cur_dir����ǰĿ¼��������
File* fopen(const u8* name_x, u8 mode)
{
	union
	{	struct
		{	u8 H;
			u8 L;
		}bytes;
		u16 word;
	}idata temp;	//�����ʡ�ռ䡣������ʱҪ�����á�����
	File *p_file = NULL;
	
	DB_SendString("\nIn fopen funciton. You want to open file :"); DB_SendString((char*)name_x);
	///////////////////////////////����·����Ϣ��ʼ///////////////////////////////
	//�õ��ַ�����·���ĳ��ȣ���¼�� temp.bytes.L�У����ֵҲ��Ŀ���ļ�������ʼλ�õ�����
	temp.bytes.L = 0;	//��ʼ������Ϊ0
	temp.bytes.H = FindChar(FILE_SEPARATOR, name_x, temp.bytes.L);//�չ�name_xΪ��ǰĿ¼���ļ������
	while(name_x[temp.bytes.H])	//���name_xΪ��ǰĿ¼���ļ�����ô�������while�У�temp.bytes.LΪ0
	{
		temp.bytes.L = temp.bytes.H;
		(temp.bytes.L)++;	//����1
		temp.bytes.H = FindChar(FILE_SEPARATOR, name_x, temp.bytes.L);
	}
	//��ʱ temp.bytes.L Ϊ�õ��ַ�����·���ĳ���
	if( 0 != temp.bytes.L )	
	{	//�����·���ɷ֣���ô�͸���Ŀ¼
		if(0 != ChangeDir(name_x, temp.bytes.L, NULL))	//���ĵ�ǰĿ¼
		{	//���Ŀ¼�л�ʧ��
			DB_SendString("\nChangeDir failed. There may be invalid path.\n");
			goto FAIL;
		}
	}
	/////////////////////////////·����Ϣ�������/////////////////////////////

	//��p_file����ռ�
	p_file = NewFileStruct();
	if(p_file == NULL)
	{
		DB_SendString("\nNewFileStruct error.\n");
		goto FAIL;
	}
	
	temp.bytes.H = StringLength(name_x);
	name_x += temp.bytes.L;					//ת�����򿪵��ļ������׵�ַ

	if( SearchItem( name_x, temp.bytes.H - temp.bytes.L, &cur_dir ) ) //�����ļ�
	{	//����Ҳ����ļ�
		if( checkb(mode , FILE_CREATE ) == 0)
		{
			DB_SendString("\nOpen file failed. File is not found.\n");
			DeleteFileStruct(p_file);	//��ʧ�ܣ��ͷ��ڴ�
			goto FAIL;
		}
		else
		{	//��������Ҳ����ļ��򴴽����ļ��ķ�ʽ�򿪣��ʹ����µ��ļ�
			if(IsEndOfDir(&cur_dir))
			{	//�����Ҫ�����´�
				if(AllocNewCluster(&cur_dir))
				{
					DB_SendString("\nopen file failed. cannot create new file!\n");
					goto FAIL;
				}
				if(FillCluster(cur_dir.next_cluster, 0))	//���㻺����
				{
					DB_SendString("\nfill cluster error! fopen failed\n");
					goto FAIL;
				}
				if(dirseek(&cur_dir, 0, SEEK_CUR) )		//�����´غ�Ҫ���¶�λ�ļ�ָ��
				{
					DB_SendString("\ndirseek failed! fopen failed!\n");
					goto FAIL;
				}
			}
			if(CheckDirItem(cur_dir.buf.p + cur_dir.buf.pos) != DIRITEM_EMPTY)
			{
				goto FAIL;
			}
			if( NewDirItem(name_x, RW,  &cur_dir))
			{
				DB_SendString("\ncreate new	dir item failed. fopen failed.\n");
				goto FAIL;
			}
		}
	}
	if( FillFileStruct( p_file, &cur_dir) != 0)		//�ҵ��ļ���Ϳ�������ļ��ṹ����
	{
		DB_SendString("\nFillFileStruct error!\n");
		DeleteFileStruct(p_file);	//��ʧ�ܣ��ͷ��ڴ�
		return NULL; 
	}
	//����ģʽ
	p_file->mode = mode;
 	DB_SendString("\nSucceed, Exit fopen function!\n");
	return p_file;
	
	FAIL:
		DeleteFileStruct(p_file);
		return NULL;
}

//˵����
//	��File�ṹ���е�next_cluster��next_sector_offsetָ����һ������
//������
//	p_file	ָ��File�ṹ���ָ�룬������ΪNULL
//���أ�
//	�����������֮ǰnext_cluster��next_sector_offsetָ��������ĵ�ַ
//	��� �����������֮ǰnext_cluster��next_sector_offsetָ��������ĵ�ַ ���н����ź�
//	��ô�ͻ᷵�� SECTOR_END
//ע�⣺
//	�ڵ����������֮ǰӦ�ü���Ƿ��Ѿ����ļ�β��
u32 MoveToNextSector(File* p_file)
{
	u32 idata addr;	//���صĵ�ַ
	
	if(p_file->next_cluster != FAT32_EOC)	//����ǲ��ǽ���
	{	//���淵�صĵ�ַ
		addr = GetSectorAddr(p_file->next_cluster, p_file->next_sector_offset);
	}
	else
	{	//����ǽ�������ֱ�ӷ���
		return SECTOR_END;
	}

	if(p_file->next_sector_offset >= (fat32.sectors_per_cluster - 1))		
	{	//��ε��ô˺���Ҫ���ص������Ǵ������һ��������˵������Ҫ����cluster��
		p_file->next_sector_offset = 0;	  	//��һ��0����

		if(p_file->list_index < CLU_LIST_ITEM - 1)	//�����б��Ƿ����---
		{
			if(p_file->cluster_list[p_file->list_index] == CONTINUOUS)
			{	//������������
				++(p_file->next_cluster);	//ֱ���Լ�1
				if((p_file->next_cluster) == (p_file->cluster_list[p_file->list_index + 1]))
				{	//����Ż��������ؽ�����
					//ʹp_file->next_cluster) �� (p_file->cluster_list[p_file->list_index])һ��
					++(p_file->list_index);
				}
			}
			else
			{	//���û��������ǣ���������ȡ
				++(p_file->list_index);
				if(p_file->cluster_list[p_file->list_index] == CONTINUOUS)	
				{	//�������list_index���������������
					//ֱ���Լ�1��������	
					//p_file->next_cluster = p_file->cluster_list[p_file->list_index];
					++(p_file->next_cluster);		
				}
				else	//�������list_index��û�������������
				{
					p_file->next_cluster = p_file->cluster_list[p_file->list_index];
				}
			}
		}
		else	
		{	//������б������
			GetClusterList(p_file->next_cluster, p_file->cluster_list, CLU_LIST_ITEM);
			p_file->next_cluster = p_file->cluster_list[1];
			p_file->list_index = 1;		//0��λΪ���������Ȼ��Ҫ���ص��������ڵĴغ�
		}
	}
	else
	{
		p_file->next_sector_offset++;
	}

	return addr;
}

//˵����
//	ʹFile�ļ��ṹ���е�next_cluster��next_sector_offsetָ���ļ��ĵ�һ������
//	ͬʱҲ�������ļ�ָ�����Ч������ָ�룬���ļ�ָ����г�ʼ������
//������
//	p_file	ָ��File�ṹ���ָ��
//���أ�
//	����ɹ�������0�����򷵻�����ֵ
//ע�⣺
//	1.	File�ṹ���cluster����Ҫ�Ѿ�����ʼ����
u8 MoveToFirstSector(File* p_file)
{
	//�õ����б�
	if(GetClusterList(p_file->cluster, p_file->cluster_list, CLU_LIST_ITEM) == GCL_ERROR)
	{
		DB_SendString("\nGet cluster list error!\n");
		free(p_file);	//��ʧ�ܣ��ͷ��ڴ�
		return 1;	 
	}	
	//��Ϊ p_file->cluster������0�����Բ�ֱ����cluster��next_cluster��ֵ
	p_file->next_cluster = p_file->cluster_list[0];
	p_file->next_sector_offset = 0;
	p_file->list_index = 0;
	p_file->ptr = 0;							//�����ļ�ָ��
	p_file->buf.pos = fat32.bytes_per_sector;	//��Ч������ָ��
	return 0;
}


//˵����
//	�ļ�����һ��������buffer�У�
//	���bufferΪNULL���Ͷ�һ���������ļ��Ļ��������������ļ�ָ�룬����size������
//������
//	p_file	ָ��һ���ļ����ԣ�����Ŀ¼���ԣ����ļ��ṹ���ָ��
//	buffer	����Ŀ¼������
//	size	�������Ĵ�С
//���أ�
//	 �������ֽ������������FR_ERROR���������
//ע�⣺
//	1.	����ͬһ���ļ���������һ�ζ���ʱ��buffer��NULL��һ�β���NULL�������Ľ����δ����ġ�
//		��Ϊ���ַ�ʽ�Ĵ�������ǲ�ͬ�ģ�
//	2.	�����һ���ļ�ʹ����fseek����������fread�Ͳ���ʹ��NULL��buffer
//	3.	p_fileָ��Ľṹ�岻��������Ŀ¼���Ե�
u16 fread(File* p_file,u8* buffer,u16 size)
{
	u16 temp = 0;
	u32 xdata sector;

	if(p_file == NULL)
	{	//���p_file�ǿյ�
		return FR_ERROR;
	}
	else if((p_file->mode & FILE_READ) == 0)	//��������Զ�ģʽ��
	{
		DB_SendString("fread error! file is not open with read mode.\n");
		return FR_ERROR;
	}

	//�ȼ���Ƿ����ļ�β
	if ( IsEndOfFile(p_file) )
		return 0;					//����ǣ��Ͳ�Ӧ�ø���buffer��

	if(buffer == NULL)
	{	//���buffer�ǿգ�Ĭ�϶�һ���������ļ���Ӧ�Ļ�������

		//���»���������һ������
		if(UpdateFileBuffer( MoveToNextSector(p_file), p_file) != UFB_SUCCEED)
		{	//���ʧ����
			DB_SendString("\nfread Reading error!\n");
			return FR_ERROR;
		}

		if( ((p_file->size - p_file->ptr) <= fat32.bytes_per_sector) )	
		{	//��������ļ�β��
			//������һҪ����λ��Ϊ�ļ�β
			temp = p_file->size - p_file->ptr;
			p_file->ptr = p_file->size;					//�����ļ�ָ�뵽�ļ�β
			p_file->buf.pos = temp;						//���»�����ָ��
			return temp;								//���ض���temp�ֽ�
		}
		else 	
		{
			p_file->ptr += fat32.bytes_per_sector;		//�����ļ�ָ��
			p_file->buf.pos = fat32.bytes_per_sector;	//���»�����ָ��
			return fat32.bytes_per_sector;				//���ض���һ������
		}
	}
	else
	{	//���԰�Ҫ���Ĳ��ֳַ������֣�
		//	��һ���� ͷ����һ����СС��������С���ڴ��
		//	�ڶ����� �м䲿�֣�һ��������
		//			��PS����51��Ƭ��Ӧ���ϲ�̫���ܳ�������ˡ��������������а취���Եģ�
		//	�����ܷ� β����Ҳ��һ����СС��������С���ڴ��

		//Ϊ�ļ�����׼��������
		PreFileOp(p_file);
	
		if (size > ( p_file->size - p_file->ptr) )
		{	//�ȸ����ļ�β���н�ȡ
			size = p_file->size - p_file->ptr;
		}
				
		if( p_file->buf.pos < fat32.bytes_per_sector  )
		{	//�Ե�һ���ֵĴ��������ǰ��buffer�������ݿ��Զ�
			//����Ӧ�ö����ֽ������浽temp��
			DB_SendString("\nfread part 1\n");
			if(size <= (fat32.bytes_per_sector - p_file->buf.pos ) )
			{	//��� size ��Χ�������������
				temp = size;
			}
			else
			{	//��� size ��Χ�������������
				temp = fat32.bytes_per_sector - p_file->buf.pos;
			}

			//��ʼ��ȡ����
			MemoryMove(buffer, p_file->buf.p + p_file->buf.pos, temp);
			p_file->ptr += temp;		//�����ļ�ָ��
			p_file->buf.pos += temp;	//���»�������һҪ��д��λ��

			//Ϊ��һ������׼��
			size -= temp;
			buffer += temp;
		}

		while( size >= fat32.bytes_per_sector)
		{	//�Եڶ����ֽ��д�����ʵ��һ���ֿ��Խ����Ż��ģ����Ƕ�������������Ҫ��
			//������51��Ƭ���ϲ��������������������ʱ�����Ż�
			//ֱ�Ӷ����û�buffer��
			DB_SendString("\nfread part 2\n");
			if(ReadSector(MoveToNextSector(p_file), buffer) != 0)
			{	//������»���ʧ�ܣ����ش�����ʵҲ���Է���temp����¼����
				return FR_ERROR;
			}
			size -= fat32.bytes_per_sector;				//����ʣ�µĴ�С
			buffer += fat32.bytes_per_sector;			//�����û�buffer
			p_file->ptr += fat32.bytes_per_sector;		//�����ļ�ָ��
			temp += fat32.bytes_per_sector;				//temp������¼ʵ�ʶ�ȡ���ֽ���
		}

		if( size > 0)
		{	//�Ե������ֽ��д���
			DB_SendString("\nfread part 3\n");
			p_file->buf.pos = 0;	//�����Ż��������
			sector = MoveToNextSector(p_file);
			//��Ϊǰ��Ĳ������ܻ���p_curFile��ΪNULL���������������������
			//�ļ�p_file�ˣ����Ļ�Ӱ�캯��PreFileOp����Ϊ�������
			//p_curFile = p_file;
			if( UpdateFileBuffer(sector, p_file) != UFB_SUCCEED)
			{	//������»���ʧ�ܣ����ش���
				DB_SendString("update ubffer error.\n");
				return FR_ERROR;
			}
			MemoryMove( buffer, p_file->buf.p, size);
			p_file->ptr += size;		//�����ļ�ָ��
			temp += size;				//���¶�ȡ�Ĵ�С
			p_file->buf.pos = size;		//���»�����ָ��
		}

		return temp;	//���ض�ȡ�����ֽ���
	}
}

//˵����
//	�ر�һ���ļ�
//������
//	p_file��Ӧ�ý��ļ���ʶ����
//ע�⣺
//	1.	������ʹ���ļ���ʱ��һ��Ҫ�ر��ļ�
//	2.	�������Ҳ�ǻ�ı��ļ�ϵͳ�ڲ�cur_dir������λ�õ�
//���أ�
//	�ɹ�����0��ʧ�ܷ�������ֵ�����ʱ�����δ֪�����ˣ�
u8 fclose(File* p_file)
{
	u32 temp;
	if(p_file == NULL)
		return 0;
	PreFileOp(p_file);
	//������������б�д������ݣ���Ҫ���µ��洢����
	UpdateFileBuffer(p_file->buf.sector, p_file);

	if( !IsSubDir(p_file) )
	{	//���ڷ�Ŀ¼���Բ���Ҫ�Ĵ��������µ��ļ���С
	 	if(GoToFileDir(p_file, &cur_dir)!= 0)
		{	//����
			DB_SendString("Go to file dir error!");
			return 1;
		}
		//�ȽϺ�Ŀ¼�еĴ�С
		if(p_file->size != GetDWordFromLE( cur_dir.buf.p + cur_dir.buf.pos + 0x1c))
		{	//�����С��ͬ��д���µĴ�С
			PutDWordToLE(cur_dir.buf.p + cur_dir.buf.pos + 0x1c , p_file->size);
			cur_dir.buf.is_changed = 1;	//��λд����
			//д�����ݵ��洢����
		}
		//�õ��ļ��Ĵ�
		temp = (u32)(GetWordFromLE( cur_dir.buf.p + cur_dir.buf.pos + 0x14));	//��2���ֽ�
		temp <<= 16;
		temp += GetWordFromLE( cur_dir.buf.p + cur_dir.buf.pos + 0x1A);	//��2���ֽ�
		if( (temp == 0) && (p_file->cluster != 0) )
		{
			PutWordToLE(cur_dir.buf.p + cur_dir.buf.pos + 0x1A,(u16)(p_file->cluster));
			PutWordToLE( cur_dir.buf.p + cur_dir.buf.pos + 0x14,(u16)((p_file->cluster) >> 16)); 
			cur_dir.buf.is_changed = 1;	//��λд����
		}
		//����cu_dir�Ļ�����
		if(UpdateFileBuffer(cur_dir.buf.sector, &cur_dir) != 0 )
		{
			return 1;
		}
		
	}	
	DeleteFileStruct(p_file);	//�ͷ��ڴ�
	DB_SendString("\n file is closed.\n");
	return 0;
}

//˵����
//	�ƶ��ļ�ָ�뵽ָ����λ��
//������
//	p_file	�ļ��ṹ��ָ�룬ָ��һ���Ѿ����򿪵��ļ�
//	offset	ƫ����
//	fromwhere
//		�ļ�ָ������￪ʼƫ�ƣ����������¼���ֵ֮һ
//		SEEK_SET�� �ļ���ͷ ����SEEK_CUR�� ��ǰλ�� ����SEEK_END�� �ļ���β 
//���أ�
//	�ɹ�����0����������ֵ��ԭ����ʧ�ܺ��ļ�ָ���λ�ò��ı䡣
//	�������disk��д�����Ͳ�һ���ˡ�
//ע�⣺
//	1.	��֧�ֶ��ļ��е�seek
//��ע��
//	fseek������ļ���Ӧ�Ļ������������ڲ�������ʹ�ú�IsEndOfDir
u8 fseek(File* p_file, signed long int offset, u8 fromwhere)
{	//�ȼ�����û�нݾ�������
	u32 xdata pos;

	//��֧�ֶ�Ŀ¼�Ĵ���
	if(p_file == NULL)
	{
		return 1;
	}
	else if(IsSubDir(p_file))
	{
		return 1;
	}

	//�ȼ�������Ҫ�ƶ���������ڿ�ͷ��λ�ã����浽pos��
	if(fromwhere == SEEK_CUR)
	{
		if( offset < 0 )
		{
			if( (u32)(-offset) > p_file->ptr)
			{	//�����ƫ����̫��
				return 1;
			}
		}
		pos = p_file->ptr + offset;
		if( offset > 0)
		{	//�������0���ж�����Ժ�᲻�����
			if(pos < p_file->ptr)
			{	//�������ˣ��ͻ�������
				return 1;
			}
			else if( pos > p_file->size)
			{	//���ƫ����̫��
				return 1;
			}
		}
	}
	else if(fromwhere == SEEK_SET)
	{
		if( offset < 0)
		{	//���ļ�ͷ��ʼ��ƫ����������Ϊ��
			return 1;
		}
		else if( (u32)offset > p_file->size)
		{	//���ƫ����̫��
			return 1;
		}
		pos = offset;
	}
	else if(fromwhere == SEEK_END)
	{
		if(offset > 0)
		{	//���ļ�β��ʼ��ƫ����������Ϊ��
			return 1;
		}
		else if( (u32)(-offset) > p_file->size)
		{	//���ƫ����̫��
			return 1;
		}
		pos = p_file->size + offset; 
	}
	else
	{	//fromwhere ֵ�Ƿ�
		return 1;
	}	//�����������Ϊ���������pos�ǺϷ���
		
	//offset�Ѿ�û�����ˣ�����������������
	//fromwhereҲ�Ѿ�û���ˣ������������Ƿ���»������ı�־
	fromwhere = 1;
	if( pos >= p_file->ptr)
	{	//����ڵ�ǰλ�ú���
		//���������ٸ�������Ҳ���Ǽ���ҪMoveToNextSector���ٴ���~

		offset = ( pos -  (p_file->ptr) + (u32)(p_file->buf.pos)  ) / (u32)(fat32.bytes_per_sector);
		if( offset != 0 )	//��������ڸ���
		{	//MoveToNextSector offset�Σ����һ�θ��»�����
			offset--;
		}
		else
		{	//���seek�����ݾ��ڻ������������Ͳ��ø��»�����
			fromwhere = 0;
		}
	}
	else
	{	//���򣬴ӿ�ͷ��ʼ����
		if(MoveToFirstSector(p_file) != 0)	//��λ�ļ�ָ��
		{
			DB_SendString("\nMoveToFirstSector error!\n");
			return 2;
		}
		offset = pos / fat32.bytes_per_sector; //����Ӧ�ü�1�ģ��������һ��Ҫ���»�����
	}

	//���ƶ���ָ��������
	while(offset--)	
	{
		MoveToNextSector(p_file);
	}

	//���ڿ��ܻ�����ļ��Ļ��棬����Ҫ׼��
	PreFileOp(p_file);
	
	//���»�����ָ��ͻ�����ָ��
	p_file->buf.pos = pos % fat32.bytes_per_sector;	
	p_file->ptr = pos;
	if(fromwhere != 0)			//�ж��Ƿ�Ҫ���»�����
	{							//���һ���ƶ�Ҫ���»�����
		pos = MoveToNextSector(p_file);
		if(pos != SECTOR_END)			//���MoveToNextSector���ص��ǲ��ǽ���
		{					//������ǽ������Ÿ��»�����
			
			if( UpdateFileBuffer( pos, p_file ) != UFB_SUCCEED)
			{	//������»�����ʧ��
				MoveToFirstSector(p_file);	//��λ�ļ�ָ��
				return 2;
			}
		}
		else
		{	//���ʱ���ļ��պ�������������
			DB_SendString("\nfseek end of file\n");
			p_file->buf.pos =  fat32.bytes_per_sector;	//��Ч������
		}
	}
	return 0;
}

//˵����
//	����Ŀ¼���ļ�ָ�뵽ָ��λ��
//������
//	p_dir	�ļ��ṹ��ָ�룬ָ��һ���Ѿ����򿪵��ļ�
//	offset	ƫ����
//	fromwhere
//		�ļ�ָ������￪ʼƫ�ƣ����������¼���ֵ֮һ
//		SEEK_SET�� �ļ���ͷ ����SEEK_CUR�� ��ǰλ��
//���أ�
//	�ɹ�����0��ʧ�ܷ�������ֵ��ʧ�ܺ��ļ�ָ���λ����δ����ġ�
//ע�⣺
//	fromwhere�����ﲻ��Ϊ SEEK_END
u8 dirseek(File* p_dir, signed long int offset, u8 fromwhere)
{
	u32 xdata pos;

	//�ȼ���ǲ���Ŀ¼
	if(!IsSubDir(p_dir))
	{
		return 1;
	}
	else if( fromwhere == SEEK_CUR )
	{	//����Ǵӵ�ǰλ��ƫ��
		if( offset < 0 )
		{
			if( (u32)(-offset) > p_dir->ptr)
			{	//�����ƫ����̫��
				return 1;
			}
		}
		pos = p_dir->ptr + offset;
		if( offset > 0)
		{	//�������0���ж�����Ժ�᲻�����
			if(pos < p_dir->ptr)
			{	//�������ˣ��ͻ�������
				return 1;
			}
		}
	}
	else if( fromwhere == SEEK_SET )
	{
		if( offset < 0)
		{	//���ļ�ͷ��ʼ��ƫ����������Ϊ��
			return 1;
		}
		pos = offset;
	}
	else
	{	//fromwhere��������������ֵ
		return 1;
	}	
		
	//offset�Ѿ�û�����ˣ�����������������
	//fromwhereҲ�Ѿ�û���ˣ������������Ƿ���»������ı�־
	fromwhere = 1;
	if( pos >= p_dir->ptr )
	{	//����ڵ�ǰλ�ú���
		//���������ٸ�������Ҳ���Ǽ���ҪMoveToNextSector���ٴ���~
		offset = ( pos -  p_dir->ptr + p_dir->buf.pos  ) / fat32.bytes_per_sector;
		if( offset != 0 )	//��������ڸ���
		{	//MoveToNextSector offset�Σ����һ�θ��»�����
			offset--;
		}
		else
		{	//���seek�����ݾ��ڻ������������Ͳ��ø��»�����
			fromwhere = 0;
		}
	}
	else
	{	//���򣬴ӿ�ͷ��ʼ����
		if(MoveToFirstSector(p_dir) != 0)
		{
			DB_SendString("\nMoveToFirstSector error!\n");
			return 1;
		}
		offset = pos / fat32.bytes_per_sector; //����Ӧ�ü�1�ģ��������һ��Ҫ���»�����
	}
	//���ƶ���ָ��������
	while(offset--)	
	{
		if( MoveToNextSector(p_dir) == SECTOR_END )
		{	//����������������³�ʼ���ļ�ָ�룬�ٷ���ʧ��
			MoveToFirstSector(p_dir);
			return 1;
		}
	}
	//�������ļ�ָ��ͻ�����ָ��
	p_dir->ptr = pos;
	p_dir->buf.pos = pos % fat32.bytes_per_sector;
	
	//����ҪUpdateFilebuffer������Ҫ׼��
	PreFileOp(p_dir);
	
	if( fromwhere != 0 )	//�ж��Ƿ�Ҫ���»�����
	{	//���һ���ƶ�Ҫ���»�����
		pos = MoveToNextSector(p_dir);
		if(pos != SECTOR_END)
		{	//������ǽ���
			if( UpdateFileBuffer( pos,  p_dir ) != UFB_SUCCEED)
			{	//������»�����ʧ��
				MoveToFirstSector(p_dir); 	//��λ�ļ�ָ��
				return 1;
			}
		}
		else
		{
			if(p_dir->buf.pos == 0)
			{	//����Ŀ¼β��
				p_dir->buf.pos = fat32.bytes_per_sector;	//��Ч������
				return 0;
			}
			else
			{	//���������λ�ļ�ָ��
				MoveToFirstSector(p_dir); 	//��λ�ļ�ָ��
				return 1;
			} 
		}
	}
	return 0;
}

//˵����
//	��Ŀ¼���Ե��ļ��ṹ���ڲ�ά����ָ��Ŀ¼���ָ�룬���������������
//	�ƶ�ָ�뵽��һ���Ϸ��Ķ�Ŀ¼��
//	���Ϸ��ĳ�Ŀ¼��ֻ���ļ����йأ�����ֻ���ڻ�ȡ�ļ�����ʱ��Ż������ó���
//������
//	p_dir	ָ��һ����Ŀ¼���Ե��ļ��ṹ�壬������ֵΪ�գ�Ĭ���ǵ�ǰĿ¼
//���أ�
//	�ɹ�����0��ֻ�з���0�ſ��Զ�ȡĿ¼��
//	����Ŀ¼β���� 1�����ʱ��Ͳ��ܶ�ȡĿ¼���ˣ�
//	���󷵻�����ֵ
u8 MoveToNextFile(File* p_dir)
{
	u8 temp;

	if(p_dir == NULL)
	{	//�������Ϊ�գ���ô��Ĭ���ǵ�ǰĿ¼
		p_dir = &cur_dir;
	}

	if(IsEndOfDir(p_dir))
	{	//�����β�ˣ���ô�ͷ���
		return 1;
	}
	else if( p_dir->buf.pos + DIRITEM_LENGTH > fat32.bytes_per_sector)
	{	//��黺�����ĺϷ���
		DB_SendString("dirseek error. file not found!\n");
		return 2;
	}
	temp = 	CheckDirItem(p_dir->buf.p + p_dir->buf.pos); 
	if(temp == DIRITEM_EMPTY)
	{	//��鿴��ǰ���ǲ��ǿգ�����ǣ�Ҳ����
		return 1;
	}
	else if(temp == DIRITEM_ERROR)
	{
		return 2;
	}

	while(1)
	{
		if( dirseek(&cur_dir, DIRITEM_LENGTH, SEEK_CUR) != 0 )
		{	//�ƶ��ļ�ָ��ͻ�����ָ��
			DB_SendString("dirseek error. file not found!\n");
			return 2;
		}
		else if( IsEndOfDir(p_dir) )	//seek��Ҫ�ж�
		{	//�������Ŀ¼β��
			DB_SendString("reach the end of dir.\n");
			return 1;
		}
		//��黺��������Ч��
		else if( p_dir->buf.pos + DIRITEM_LENGTH > fat32.bytes_per_sector)
		{
			DB_SendString("dirseek error. file not found!\n");
			return 2;
		}

		temp = CheckDirItem(p_dir->buf.p + p_dir->buf.pos);

		if( temp == DIRITEM_SHORT)
		{	//����ҵ���
			return 0;
		}
		else if( temp == DIRITEM_EMPTY)
		{	//����ǿգ�Ҳ��ΪĿ¼β
			return 1;
		}
		else if(temp == DIRITEM_ERROR)
		{	//������ִ���
			DB_SendString("\nDIRITEM_ERROR!\n");
			return 2;
		}
	}
}

//˵����
//	��Ŀ¼���Ե��ļ��ṹ���ڲ�ά����ָ��Ŀ¼���ָ�룬���������������
//	�ƶ�ָ�뵽��һ���Ϸ��Ķ�Ŀ¼��
//	���Ϸ��ĳ�Ŀ¼��ֻ���ļ����йأ�����ֻ���ڻ�ȡ�ļ�����ʱ��Ż������ó��� 
//������
//	p_dir	ָ��һ����Ŀ¼���Ե��ļ��ṹ�壬������ֵΪ�գ�Ĭ���ǵ�ǰĿ¼
//���أ�
//	�ɹ�����0��ֻ�з���0�ſ��Զ�ȡĿ¼��
//	����Ŀ¼β���� 1��˵�����Ŀ¼����û�����ݣ�
//	���󷵻�����ֵ
u8 MoveToFirstFile(File* p_dir)
{
	u8 temp;
	if(p_dir == NULL)
	{	//�������Ϊ�գ���ô��Ĭ���ǵ�ǰĿ¼
		p_dir = &cur_dir;
	}
	if( MoveToFirstSector(p_dir) != 0)	//�ȸ�λָ��
	{	//�����λ�ļ�ָ��ʧ�ܣ�Ҳ���ش���
		return 2;
	}

	if( dirseek(&cur_dir, 0, SEEK_SET) != 0 )
	{	//�ƶ��ļ�ָ�뵽�ļ���ʼ
		DB_SendString("dirseek error. file not found!\n");
		return 2;
	}
	while(1)
	{	//���Ŀ¼��
		temp = CheckDirItem(p_dir->buf.p + p_dir->buf.pos);
		
		if( IsEndOfDir(p_dir) )	//seek��Ҫ�ж�
		{	//�������Ŀ¼β��
			DB_SendString("reach the end of dir.\n");
			return 1;
		}
//		//��黺��������Ч��
//		else if( p_dir->buf.pos + DIRITEM_LENGTH > fat32.bytes_per_sector)
//		{
//			DB_SendString("dirseek error. file not found!\n");
//			return 2;
//		}
		else if(temp == DIRITEM_SHORT)
		{	//����ҵ��ˣ��ͷ��سɹ�
			return 0;
		}
		else if( temp == DIRITEM_EMPTY)
		{	//������Ŀ¼�˵������û��������
			return 1;
		}
//		else if( temp == DIRITEM_ERROR)
//		{	//�������˵���ļ�ϵͳ����������
//			DB_SendString("\nDIRITEM_ERROR!\n");
//			return 2;
//		}

		if( dirseek(&cur_dir, DIRITEM_LENGTH, SEEK_CUR) != 0 )
		{	//�ƶ��ļ�ָ��ͻ�����ָ��
			DB_SendString("dirseek error. file not found!\n");
			return 2;
		}
	}
}


//˵����
//	Ŀ¼���Ե��ļ����и�ָ��Ŀ¼����ļ�ָ�룬��������������Եõ�Ŀ¼��Ķ��ļ���
//������
//	p_dir	ָ��һ����Ŀ¼���Ե��ļ��ṹ�壬���Ϊ�գ�Ĭ��Ϊ��ǰĿ¼
//	buf		���ص��ļ�����ŵ�λ�ã���������Ϊ12���ֽ�
//���أ�
//	�ɹ�����0�����򷵻�����ֵ
//ע�⣺
//	1.	�������ֻ�ܽ��������¼���������ʹ�ã���������δ�����
//		MoveToFirstFile 
//		MoveToNextFile	
u8 GetShortFileNameX(File* p_dir, u8* buf)
{
	u8 i, j;	//, k;	//��������
	if(p_dir == NULL)
	{	//���p_dirΪ�գ�Ĭ���ǵ�ǰĿ¼
		p_dir = &cur_dir;
	}
	//����ǲ���Ŀ¼����
	if(!IsSubDir(p_dir))
	{
		return 1;
	}
	//���Ŀ¼���Ƿ�Ϸ�
	if(CheckDirItem(p_dir->buf.p + p_dir->buf.pos) != DIRITEM_SHORT)
	{
		return 1;
	}

	//��ȡ���ļ���
	for(i = 0; i < 8; i++)
	{
		buf[i] = p_dir->buf.p[ p_dir->buf.pos + i];

		if( buf[i] == 0x20 )	
			break;
	}

	buf[i] = '.';

	for(i++,j = 8; j < 8 + 3; i++, j++)
	{
		buf[i] = p_dir->buf.p[p_dir->buf.pos + j];
		if(buf[i] == 0x20)	//����ո�����ѭ��
		{
			break;
		}
	}
	buf[i] = 0;
	if( j == 8 )	//������չ��Ϊ�յ�����£�ȥ��'.'
	{
		buf[i - 1] = 0;
	}

	return 0;
}


//˵����
//	Ϊһ���ļ�����Ŀ¼���Ե�Ҳ�У���β�ط���һ���µĴ�
//������
//	p_file	ָ��һ��Ҫ�����´ص��ļ��ṹ��
//			�������ļ����ԣ�Ҫ������ļ���MoveToNextSector����SECTOR_END�������ʹ��
//			�����سɹ���MoveToNextSector������ʹ�ã����᷵��SECTOR_END��
//			������Ŀ¼���ԣ�Ҫ��IsEndOfDir�õ������ʱ��ʹ��
//			���سɹ��󣬿���ԭ��fseek���»�������Ȼ��Ͳ���IsEndOfDir��
//��ע��
//	����û�з���ռ���ļ������������״أ����ʱ����Ҫfcloseȥ���д��Ŀ¼��Ĺ���
static
u8 AllocNewCluster(File* p_file)
{
	u16 sector_count;
	u16 index;
	u16 temp;
	u32 free;

	PreFileOp(NULL);	//��Ϊ�����������ܻ��д����

	if( p_file->cluster_list[p_file->list_index] == FAT32_EOC )
	{	
		//���ҵ�һ���µĿ��д�ȥ���� fat32.cur_free_cluster
		if( fat32.cur_free_cluster == FAT32_EOC )
		{	//���ʾ�Ѿ�û�п��д���
			return 1;
		}
		free = fat32.cur_free_cluster;	//����ԭ����ֵ

		//��free�Ŵط�����ļ�
		p_file->next_cluster = p_file->cluster_list[p_file->list_index] = free;
		if( p_file->list_index < CLU_LIST_ITEM - 1)	//������б�������д��
		{	//�ڴ��б���д�������־
			p_file->cluster_list[p_file->list_index + 1] = FAT32_EOC;
		}

		if(p_file->list_index != 0)
		{
			//���ļ�ԭ�������һ���ض�Ӧ��FAT����д��free�Ŵ�
			temp = ( p_file->cluster_list[p_file->list_index - 1] * sizeof(u32) )
						 / fat32.bytes_per_sector;
			index = (  p_file->cluster_list[p_file->list_index - 1] * sizeof(u32) )
						 % fat32.bytes_per_sector;
			if( ReadSector( fat32.FAT1_address + temp, RWbuffer) != 0)
			{
				DB_SendString("\nread sector error!\n");
				return 1;	
			}
			PutDWordToLE(  RWbuffer + index, free);	//������д��free�Ŵ�
		}
		else
		{	
			p_file->cluster = free;			//���ļ������״�
			temp = fat32.sectors_per_FAT;	//Ŀ������ sector_count != temp;
		}

		//����free�Ŵص�FAT�����λ�ã�������д�������־
		sector_count = ( free * sizeof(u32) ) / fat32.bytes_per_sector;
		index = ( free * sizeof(u32) ) % fat32.bytes_per_sector;

		if ( sector_count != temp)
		{	//��������������ͬһ��������
			//����ոո��ļ�ԭ�������һ���ص�FAT����д������ݣ��ٸ��»�����
			// p_file->list_index == 0 ʱǰ��û�ж�FAT���ͱ�̸ʲô������
			if(p_file->list_index != 0)
			{	
				if( WriteSector( fat32.FAT1_address + temp, RWbuffer) != 0)
				{
					DB_SendString("\nread sector error!\n");
					return 1;	
				}
			}

			//���»�����Ϊfree�Ŵر������ڵ�����
			if( ReadSector( fat32.FAT1_address + sector_count, RWbuffer) != 0)
			{
				DB_SendString("\nread sector error!\n");
				return 1;	
			}
		}
		PutDWordToLE(RWbuffer + index  ,FAT32_EOC );	//��free�Ŵر���д�������־
		if( WriteSector(fat32.FAT1_address + sector_count, RWbuffer) != 0)
		{	//����д�������
			return 1;
		}

		//�����µĿ��д��Ѿ�������ļ��ˣ����ǻ���Ҫ�� fat32.cur_free_cluster ���и���
		//Ѱ���µĿ��д�

		index += sizeof(u32);	//����һ��FAT���ʼ�����µĿ��д�
		while(1)
		{
			while( index < fat32.bytes_per_sector )
			{	//��һ������������
				if( GetDWord_S(RWbuffer, index) == 0)
				{	//����������д�
					//���㲢������дصĴغ�
					fat32.cur_free_cluster = 
						(sector_count * fat32.bytes_per_sector + index ) / sizeof(u32);
					goto GET_FREE_CLUSTER_COMPLETE;
				}
				index += sizeof(u32);	// ��һ��FAT����
			}
			index = 0;		//������������
			sector_count++;	//������һ������
			if( sector_count >= fat32.sectors_per_FAT )
			{	//�ж��ǲ���Ӧ���˳�
				break;
			}
			//���»������е����ݣ�����һ����������
			if( ReadSector( fat32.FAT1_address + sector_count, RWbuffer) != 0)
			{	//��FAT��
				DB_SendString("\nread sector error!\n");
				return 1;
			}	
		}
		GET_FREE_CLUSTER_COMPLETE:

		if( fat32.cur_free_cluster == free)
		{	//ֵû�иı䣬˵��û���ҵ��µĿ��д�
			fat32.cur_free_cluster = FAT32_EOC;
		}

		return 0;
	}
	else
	{	//����list_index����ָ����β�����
		return 1;
	}	
}

//˵����
//	���ڴ���ָ����С�����ݿ�д���ļ�����һ���ļ�����д�����
//������
//	p_file	ָ��һ���ļ��ṹ��
//	buffer	ָ��һ�����ݿ飬��֧��buffer == NULL�����
//	size	���ݿ�Ĵ�С
//ע�⣺
//	���ﲻ֧��buffer == NULL��ֱ������д���
//��ע��
//	д�������ܻ�ı��ļ��Ĵ�С����fcloseȥ��ɸı��С�Ĳ���
u16 fwrite(File* p_file,const u8* buffer,u16 size)
{
	u16 temp;
	u32 sector = SECTOR_END;

	if(p_file == NULL)
	{	//�������ǿ�
		return FW_ERROR;
	}
	else if((p_file->mode & FILE_WRITE) == 0)	//���������дģʽ��
	{
		DB_SendString("fwrite error! file is not open with write mode.\n");
		return FW_ERROR;
	}

	//���԰�Ҫд�Ĳ��ֳַ������֣�
	//	��һ���� ͷ����һ����СС��������С���ڴ��
	//	�ڶ����� �м䲿�֣�һ��������
	//			��PS����51��Ƭ��Ӧ���ϲ����ܳ�������ˡ��������������а취���Եģ�
	//	�����ܷ� β����Ҳ��һ����СС��������С���ڴ��
	
	//Ϊ�ļ�����׼��������
	PreFileOp(p_file);

	temp = 0;
	if( p_file->buf.pos < fat32.bytes_per_sector  )
	{	//�Ե�һ���ֵĴ��������ǰ��buffer�������ݿ��Զ�
		//����Ӧ�ö����ֽ������浽temp��

		//����Ӧ��д�����
		if( size < (fat32.bytes_per_sector - p_file->buf.pos))
		{
			temp = size;
		}
		else
		{
			temp = (fat32.bytes_per_sector - p_file->buf.pos);
		}

		//��ʼд������
		MemoryMove(p_file->buf.p + p_file->buf.pos, buffer ,temp);

		p_file->ptr += temp;		//�����ļ�ָ��
		p_file->buf.pos += temp;	//���»�������һҪ��д��λ��
		p_file->buf.is_changed = 1;	//д�������ѱ��ı�ı��

		//������÷��أ�Ϊ��һ������׼��
		size -= temp;
		buffer += temp;
	}

	while( size >= fat32.bytes_per_sector)
	{	//�Եڶ����ֽ��д�����ʵ��һ���ֿ��Խ����Ż��ģ����Ƕ�������������Ҫ��
		//������51��Ƭ���ϲ��������������������ʱ�����Ż�
		//ֱ�Ӷ����û�buffer��
		sector = MoveToNextSector(p_file);
		if( sector == SECTOR_END )
		{	//�����Ҫ�����´�
			if( AllocNewCluster(p_file) != 0)
			{	//���ʧ�ܣ��ͷ���
				goto RETURN;
			}
			sector = MoveToNextSector(p_file);
		}
		if( WriteSector( sector, buffer) != 0 )
		{
			goto RETURN;
		}
		size -= fat32.bytes_per_sector;				//����ʣ�µĴ�С
		buffer += fat32.bytes_per_sector;			//�����û�buffer
		p_file->ptr += fat32.bytes_per_sector;		//�����ļ�ָ��
		temp += fat32.bytes_per_sector;				//temp������¼ʵ�ʶ�ȡ���ֽ���
	}

	if( size > 0)
	{	//�Ե������ֽ��д���
		sector = MoveToNextSector(p_file);
		if( sector == SECTOR_END )
		{	//�����Ҫ�����´�
			if( AllocNewCluster(p_file) != 0)
			{	//���ʧ�ܣ��ͷ���
				goto RETURN;
			}

			sector = MoveToNextSector(p_file);
		}	
		if(p_file->ptr < p_file->size)
		{	//������Ƕ��ļ�׷�����ݣ���ֻ���޸��ļ������ݣ���ô����Ч������ָ��
			p_file->buf.pos = 0;	//����update��ʱ��Ż���»�����������
		}
		if( UpdateFileBuffer(sector, p_file) != UFB_SUCCEED)
		{	//������»���ʧ�ܣ����ش���
			DB_SendString("update ubffer error.\n");
			return FR_ERROR;
		}
		MemoryMove( p_file->buf.p, buffer , size);	//д��������
		p_file->ptr += size;						//�����ļ�ָ��
		temp += size;								//���¶�ȡ�Ĵ�С
		p_file->buf.pos = size;						//���»�����ָ��
		p_file->buf.is_changed = 1;					//д�뻺�������ı���
	}

	RETURN:
	if( p_file->ptr > p_file->size )
	{	//���д������ݳ�����С�����޸��ļ��Ĵ�С
		p_file->size = p_file->ptr;
	}
	return temp;	//���ض�ȡ�����ֽ���	
}

//˵����
//	����һ�����򿪵��ļ����ڵ�Ŀ¼��������Ŀ¼���Ե��ļ��ṹ���ڵ�ָ��
//	ָ��һ���ļ����ڵ��ļ���
//������
//	p_file	ָ��һ�����򿪵��ļ�
//	p_dir	Ŀ¼���ԣ�ָ��һ��Ҫ����λ�õ�Ŀ¼�����Ϊ�գ�Ĭ��ָ���ļ�
//			ϵͳ�ڲ���cur_dir
//���أ�
//	�ɹ�����0�����򷵻�����ֵ��
u8 GoToFileDir(File *p_file, File *p_dir)
{
	if(p_dir == NULL)
	{	// ���Ϊ�գ�Ĭ��ָ���ļ�ϵͳ�ڲ���cur_dir
		p_dir = &cur_dir;
	}
	if( !IsSubDir(p_dir))
	{
		return 1;
	}

	if((p_file->dir.cluster == fat32.root_cluster)
		 || p_file->dir.cluster == 0 ) //���ڸ�Ŀ¼�µ��ļ����ϼ�Ŀ¼�غ�д�Ŀ��ܻ���0
	{	//����ļ����ڸ�Ŀ¼
		GoRootDir(p_dir);
	}
	else
	{	
		p_dir->cluster = p_file->dir.cluster;
		MoveToFirstSector(p_dir);				//��λ�ļ�ָ��
		//�ƶ�����ʼ���õ��ϼ�Ŀ¼����Ϣ�����ṹ��� �ڵ�dir�ṹ����Ϣ
		if( dirseek(p_dir, DIRITEM_LENGTH, SEEK_SET)!= 0)
		{	//�ƶ����ϼ�Ŀ¼��Ŀ¼��
			return 1;
		}
		if( CmpShortFileName_X("..", 2, p_dir->buf.p + p_dir->buf.pos) != 0)
		{	//�����һ���Ŀ¼������ֲ���".."������ʧ��
			return 1;
		}
		//�õ��ϼ�Ŀ¼��ʼ�صĸ��ֽ�
		p_dir->dir.cluster = GetWordFromLE(p_dir->buf.p + p_dir->buf.pos + 0x14);
		p_dir->dir.cluster <<= 16;
		p_dir->dir.cluster += GetWordFromLE(p_dir->buf.p + p_dir->buf.pos + 0x1A);
		p_dir->dir.pos = 0;	//��ʵ������Ŀ¼���ԵĽṹ���У���һ��û������
	}
	return dirseek(p_dir, p_file->dir.pos, SEEK_SET);
}



//˵����
//	����һ���µ�Ŀ¼�Ҫ��Ŀ¼�����ļ���ָ���Ѿ��ڿ�Ŀ¼����
//������
//	name_x	a null-terminated string
//			Ҫ�����ļ�����������8�ֽڣ� + '.' + ��չ����������3�ڣ��ĸ�ʽ
//			������ֲ��Ϸ����᷵��ʧ��
//	attribute
//			����
//	p_dir	ָ����Ŀ¼�����ΪNULL��Ĭ��Ϊ��ǰĿ¼
//���أ�
//	�ɹ�����0�����򷵻�����ֵ
//��ע��
//	��ʱ��֧��д�봴��ʱ�䣬������ʱ��
static
u8 NewDirItem(const u8* name_x,u8 attribute, File* p_dir)
{
	u8 name_len;	//�ļ������ֳ���
	u8 ext_len = 0;	//��չ�����ֵĳ���

	while(*name_x == ' ')	//��Ҫ�ո�
		name_x++;

	name_len = FindChar('.', name_x, 0);
	if((name_len > 8) ||( name_len == 0))
	{
		DB_SendString("\nNewDirItem failed, length of name is invalid!\n");
		return 1;
	}
	if( name_x[name_len] == '.')
	{
		ext_len = StringLength(name_x);
	 	ext_len = ext_len - name_len - 1;
		if(ext_len > 3)
		{
			DB_SendString("\nNewDirItem failed, length of extention name is invalid!\n");
			return 1;
		}
	}
	MemorySet(p_dir->buf.p + p_dir->buf.pos, 11,' ' );	//����Ŀ¼���е�����
	MemorySet(p_dir->buf.p + p_dir->buf.pos + 11, DIRITEM_LENGTH - 11, 0);

	p_dir->buf.p[p_dir->buf.pos + 0x0b] = attribute;	//д������
	//attribute�����Ѿ�û���ˣ�������������
	attribute = 0;
	while(name_len--)	//д���ļ���������
	{
		p_dir->buf.p[ p_dir->buf.pos + attribute] = getupper(*name_x);
		attribute++;
		name_x++;
	}
	name_x++;	//����'.'�ַ�
	attribute = 8;
	while(ext_len--)	//д����չ��������
	{
		p_dir->buf.p[p_dir->buf.pos + attribute] = getupper(*name_x);
		attribute++;
		name_x++;	
	}
	p_dir->buf.is_changed = 1;		//��λ���ı���
	if( UpdateFileBuffer(p_dir->buf.sector, p_dir) )	//���»�����
	{
		DB_SendString("\nfatal error! NewDirItem failed! UpdateFileBuffer error!");
		return 1;
	}
	return 0;
}

//˵����
//	��ָ����ֵ��һ��Ϊ0�����һ��ָ���Ĵ�
//������
//	cluster	ָ���Ĵ�
//	val		ָ����ֵ
//���أ�
//	�ɹ�����0�����󷵻�����ֵ����д�洢�������ˣ�
u8 FillCluster(u32 cluster, u8 val)
{
	PreFileOp(NULL);	//׼��������
	MemorySet(RWbuffer, fat32.bytes_per_sector, val);
	cluster = GetSectorAddr(cluster,0);	//�õ���Ӧ����������ַ
	//val�Ѿ�û���ˣ�������������
	val = fat32.sectors_per_cluster;
	while(val--)
	{
		if(WriteSector(cluster, RWbuffer))
		{
			DB_SendString("\nwrite sector error!\n");
			return 1;
		}
		cluster++;
	}
	return 0; 
}

}	// namespace nstd;

/*
//˵����
//	��ָ����Ŀ¼�´���һ���ļ���������������������ⲿ����
//������
//	name_x		�ļ�����������չ��
//	��ע������ֻ֧�ֶ��ļ����������ļ������ֲ�Ҫ����8�ֽڣ������ʧ�ܣ�
//	attribute	�ļ�����
//u8 CreateFile(u8* name_x,u8 attribute, File* p_dir)
//{
//	
//}

//��p_fat32�ڲ���cur_free_cluster���µ���һ�����д�
NextFreeCluster(p_fat32)

//���ڸ�Ŀ¼�·���Ĵأ���Ҫȫ������
*/
#endif
