//#define FAT_FS_C	//用来控制整个模块的编译
#ifdef FAT_FS_C
#define NDEBUG	//不调试

#include "Debug.h"
#include "FAT_FS.h"
#include "utilities.h"
#include "memory.h"
#include "FileInfo.h"
#include "FAT_Struct.h"

//基于SD卡，需要提供的外部函数
//如果是基于其它存储设备，请自行修改包含的头文件和宏定义
//提供的函数必须可以根据唯一扇区地址进行读写，否则，自行将提供的外部函数加一层相应的包装
#include "MMC_SD.h"
//失败了都要返回非0值
finline u8 ReadSector(u32 sector_address,u8* buffer) 	
	{	return SD_ReadSingleBlock(sector_address, buffer);}
finline u8 WriteSector(u32 sector_address,const u8* buffer) 
	{	return SD_WriteSingleBlock(sector_address, buffer);}


namespace nstd{

typedef struct
{
	u16	bytes_per_sector;		//每扇区字节数
	u8	sectors_per_cluster;	//每簇扇区数
	u8 	FAT_num;				//FAT数
	u16 sectors_per_FAT;		//每个FAT表的扇区数
	u32	root_cluster;			//根目录簇号
	u32 FAT1_address;			//FAT1首扇区物理地址
	u32 total_sectors;			//FAT32分区的总扇区数
	//删除一个文件以后也要更新这个喽。。。
	u32 cur_free_cluster;		//保存着当前一个空闲簇的簇号
//NND，在51单片机上初始化这个要好长时间，无法忍受，先不支持这个
//	u32 free_clusters_num;		//空闲簇的数量
}FAT32Info;						//记录FAT32信息的结构体

FAT32Info xdata fat32;

enum{
	FAT32_EOC = 0x0fffffff	//FAT32结束簇标记, end of cluster
};
//FAT16_EOC

#define FAT32	1
typedef struct{
	u8 type;					//文件系统的类型
	u32 DBR_sector;				//DBR 的扇区地址
}FilesysInfo;					//记录文件系统的信息的结构体
FilesysInfo xdata filesys;


File xdata cur_dir;								//current directory	当前目录

//说明：
//	文件系统内部的读写缓冲区，但是由于RAM大小所限，这个缓冲区被内部的cur_dir结构体
//	和其它各种被打开的文件共同使用着
unsigned char xdata RWbuffer[BUFF_SIZE]= {0}; 	//read write buffer

#define CONTINUOUS	0							//连续标记

#define DIRITEM_LENGTH	32						//目录项的长度

/***************************************以下是内部函数声明***************************************/

static u32 GetSectorAddr(u32 cluster,u8 sector_offset);

#define GCL_ERROR		0xffff
static u16 GetClusterList(u32 start, u32* list, u16 max_list_item);

u8 MoveToFirstSector(File* p_file);

u8 SearchItem(u8* name_x, u8 len, File* p_dir);

#define DIRITEM_EMPTY	0		//空项目
#define DIRITEM_SHORT	1		//合法短文件项
#define DIRITEM_DELETED	2		//该项已被删除
#define DIRITEM_LONG	3		//长文件名
#define DIRITEM_ERROR	0xff	//错误
u8 CheckDirItem(u8* pDirItem);
char CmpShortFileName_X(const u8* name_x, u8 len,const u8* pDirItem);

#define UFB_SUCCEED			0		//成功
#define UFB_WRITE_ERROR		1		//写错误
#define UFB_READ_ERROR		2		//读错误
u8 UpdateFileBuffer(u32 sector, File* p_file);

u8 dirseek(File* p_dir, signed long int offset, u8 fromwhere);

//软件定义的结尾簇号
#define SECTOR_END		0xffffffff
u32 MoveToNextSector( File* p_file );

u8 GoToFileDir(File *p_file, File *p_dir);

static
u8 NewDirItem(const u8* name_x,u8 attribute, File* p_dir);
static
u8 AllocNewCluster(File* p_file);

u8 FillCluster(u32 cluster, u8 val);

//说明：
//	判断一个文件结构体的属性是不是子目录
#define IsSubDir(p_file)		( ((p_file)->attribute & SUB_DIR) != 0 )

//说明：
//	判断一个非子目录属性的文件结构体的文件指针是否已经到了文件尾
#define IsEndOfFile(p_file)		( (p_file)->ptr >= (p_file)->size)

//说明：
//	判断一个子目录属性的文件结构体是否已经到了文件尾，如果是，就不应该再调用MoveToNextSector了
#define IsEndOfDir(p_file)		(((p_file)->next_cluster == 0x0fffffff) && ((p_file)->buf.pos >= fat32.bytes_per_sector))

/***************************************以上是内部函数声明***************************************/

/******************************以下的内容可根据MCU的具体情况进行修改******************************/
//注：
//	如果有能提供3K以上的内存池，这段内容建议保存现状

//说明：
//	创建一个新的File结构体，并返回结构体的指针
//返回：
//	File结构体的指针，如果失败，返回NULL
//备注：
//	这个函数可以根据MCU的情况进行修改
File* NewFileStruct()
{
	// 注意p_file一定是指向_MALLOC_MEM_区，否则就算 malloc 失败返回 NULL
	// p_file == NULL 也不会为真
	File* p_file;	
	//PFile p_file;

	//给结构体分配空间
	p_file = (File*)malloc( sizeof(File) );
	if(p_file == NULL)
	{
		DB_SendString("\nMemory allocation for file error.\n");
		return NULL;
	}

	DB_SendString("\nbytes per sector :");DB_SendDec((u32)(fat32.bytes_per_sector));
	MemorySet((u8*)p_file, (u16)sizeof(File), (u8)0);	//清零结构体
	DB_SendString("\nnow bytes per sector :");DB_SendDec(fat32.bytes_per_sector);
	
	// 由于在51单片机上的内存比较小，所以无法单独为一个文件打开一块缓冲
	// 也就是因为这个，才需要函数PrepFileOp(p_file)和变量 p_curFile
	// 这个分配的方式也会影响到 DeleteFileStruct 的行为
	((File*)p_file)->buf.p = RWbuffer;
	
	//初始化buffer是非法的
	((File*)p_file)->buf.pos = fat32.bytes_per_sector;

	return p_file;
}

//说明：
//	用来表明RWbuffer是属于哪个文件的变量，NULL表示属于系统
//	因为要实现逻辑上每个文件都独占一个扇区，但是MUC的RAM比较小，
//	所以可以利用以时间换空间的原理来实现。
static PFile xdata p_curFile = NULL;

//说明：
//	在每次文件操作前，会调用这个函数，为文件操作准备buffer
//	使得每个文件在逻辑上都独占一块buffer
//返回：
//	成功返回0
//	失败返回非0
u8 PreFileOp(File* p_newFile)
{
	if( p_curFile != p_newFile)
	{
		if( p_curFile != NULL )
		{	//让函数根据缓冲是否已被写入进行更新
				if( p_curFile->buf.is_changed != 0)
				{	//当buffer的is_changed为零时，那么就会先写
					if( WriteSector(p_curFile->buf.sector, p_curFile->buf.p) )
					{
						DB_SendString("\nWrite sector error!\n");
						return 1;
					}
					p_curFile->buf.is_changed = 0;		//清零标志位
				}
		}
		if( p_newFile != NULL)	//NULL参数给其它操作使用，如GetClusterList
		{
			//这里不可以这样判断，否则会出错！
			//if(p_newFile->buf.pos < fat32.bytes_per_sector )	//如果缓冲区是有效的，才更新
			if( ReadSector(p_newFile->buf.sector, p_newFile->buf.p) != 0)
			{
				DB_SendString("\nRead sector error!\n");
				return 1;
			}
		}
		p_curFile = p_newFile;		//保存当前在操作的文件
	}
	return 0;
}

//说明：
//	删除File结构体，释放空间
//参数：
//	p_file	指向要删除的File结构体
void DeleteFileStruct(File* p_file)
{
	free(p_file);
}

//在FileSysInit的这个也可能需要更改
//	cur_dir.buf.p = RWbuffer;		//设置当前目录的缓冲区

/******************************以上的内容可根据MCU的具体情况进行修改******************************/

//说明：
//	读一个扇区到一个File结构体的buffer中
//参数：
//	sector		要读的扇区号
//	p_file		File结构体的指针
//备注：
//	当buffer的is_changed为0时，
//		如果buffer本身对应的扇区 = sector，那么这个函数就不会读扇区到p_file的buffe
//		如果buffer本身对应的扇区 != sector,且缓冲区指针是有效的
//			，那么这个函数就会读扇区到p_file的buffer 
//			（不管缓冲区指针有没有效，都会更新 p_file->buf.sector 变量
//	当buffer的is_changed为非0时，那么就会先写本身对应的扇区
//返回：
//#define UFB_SUCCEED			0		//成功
//#define UFB_WRITE_ERROR		1		//写错误
//#define UFB_READ_ERROR		2		//读错误
u8 UpdateFileBuffer(u32 sector, File* p_file)
{
	if(p_file->buf.is_changed != 0)
	{	//当buffer的is_changed为零时，那么就会先写
		if( WriteSector(p_file->buf.sector, p_file->buf.p) )
		{
			DB_SendString("\nUpdateFileBuffer Write sector error!\n");
			return UFB_WRITE_ERROR;
		}
		p_file->buf.is_changed = 0;		//清零
	}
	if( p_file->buf.sector != sector  )
	{
		//如果缓冲区指针无效，就没有必要进行更新
		if( p_file->buf.pos < fat32.bytes_per_sector)
		{	
			if( ReadSector( sector, p_file->buf.p) != 0)
			{
				DB_SendString("\nUpdateFileBuffer Read sector error!\n");
				p_file->buf.sector = SECTOR_END;	//此时buffer是非法的
				return UFB_READ_ERROR;
			}
		}
		p_file->buf.sector = sector;	//更新buffer对应的扇区
	}

	//更新缓冲区的拥有者
	p_curFile = p_file;

	return UFB_SUCCEED;	
}


//说明：
//	改当前目录为根目录
//注意：
//	在相应的文件系统初始化后才能使用
void GoRootDir(File* p_dir)	
{										
	DB_SendString("\nGo root dir, root cluster address :");			
	p_dir->cluster =  fat32.root_cluster;
	DB_SendDec(p_dir->cluster);
	p_dir->dir.cluster = fat32.root_cluster;
	p_dir->dir.pos = 0;
	MoveToFirstSector(p_dir);	//初始化文件指针
}

//说明：
//	得到基本逻辑分区的 DBR 的首地址
//参数：
//	MBR_buffer	记录MBR扇区的内存
//返回：
//	DBR的首扇区地址。如果有错误，返回0
//备注：
//	1. 暂不支持拓展分区
u32 GetDBRAddr(u8* MBR_buffer)
{
	if(MBR_buffer[0x1fe]!=0x55 || MBR_buffer[0x1ff]!=0xaa)	//判断SD卡分区是否出错
	{
		DB_SendString("Read MBR error !\n\n");
		return 0;
	}

	//得到分区起始地址
	if(GetDWord_S(MBR_buffer, 0x1C6))  		
		return GetDWord_S(MBR_buffer, 0x1C6); 
	else if(GetDWord_S(MBR_buffer, 0x1D6))
		return GetDWord_S(MBR_buffer, 0x1D6); 
	else if(GetDWord_S(MBR_buffer, 0x1E6))
		return GetDWord_S(MBR_buffer, 0x1E6); 
	else		//对于拓展分区，返回错误
		return 0;

}

//说明：
//	初始化 FAT32Info 结构体
//参数：
//	DBR_buffer	调用这个函数的时候是记录DBR 扇区的内存，
//				函数内部利用完这里面的信息后会DBR_buffer指向的内存块
//				进行进一步的初始化工作
//	p_fat32		指向待初始化的结构体的指针
//注意：
//	1.	DBR_buffer指向的内存块的内容在函数的执行过程中会被更改
//		所以函数执行完后DBR_buffer指向的内存块的内容是没有意义的
u8 InitFat32(u8* DBR_buffer, u32 DBR_sector, FAT32Info* p_fat32)
{
//	u32 reserved_sectors;			//FAT保留扇区数
	u16 index = 0;

	p_fat32->total_sectors = GetDWord_S(DBR_buffer, 0x20);
	DB_SendString("\nTotal sectors :");
	DB_SendDec( p_fat32->total_sectors ); 
	DB_SendString("\nCapacity :");
	DB_SendDec(p_fat32->total_sectors * 512/1024/1024);
	DB_SendString(" MB\n");

	//读取文件系统扇区字节数
	p_fat32->bytes_per_sector = GetWord_S(DBR_buffer,0x0b);
	DB_SendDec(p_fat32->bytes_per_sector);
	DB_SendString(" bytes per sectors \n");
	if(p_fat32->bytes_per_sector != BUFF_SIZE)
		return 1;	//暂不支持非512字节每扇区的文件系统

	//读取每簇扇区数
	p_fat32->sectors_per_cluster = DBR_buffer[0x0d];
	DB_SendDec(p_fat32->sectors_per_cluster);
	DB_SendString(" sectors per cluster \n");

	//读取保留扇区数
//	reserved_sectors = GetWord_S(DBR_buffer, 0x0e);
//	DB_SendDec(reserved_sectors);
//	DB_SendString(" reserved sectors \n");

	//读取FAT数
	p_fat32->FAT_num = DBR_buffer[0x10];
	DB_SendDec(p_fat32->FAT_num);
	DB_SendString(" FATs \n");

	//读每FAT扇区数
	p_fat32->sectors_per_FAT = GetDWord_S(DBR_buffer, 0x24);
	DB_SendDec(p_fat32->sectors_per_FAT);
	DB_SendString(" Sectors per FAT   \n");

	//初始化FAT1_address 
	p_fat32->FAT1_address = DBR_sector + 
					GetWord_S(DBR_buffer, 0x0e);//保留扇区数
	DB_SendString("\n FAT1 address:");
	DB_SendDec(p_fat32->FAT1_address);
	
	//初始化
	p_fat32->root_cluster = GetDWord_S(DBR_buffer, 0x2c);	//记录根目录簇号
	DB_SendString("\nRoot cluster address :");
	DB_SendDec(p_fat32->root_cluster);

	//到了这里，DBR_sector这个变量已经没有用了，下面用来作为计数

	//下面开始初始化  cur_free_cluster

	DBR_sector = 0;						//读FAT扇区的计数
	p_fat32->cur_free_cluster = 0;

	while( DBR_sector < p_fat32->sectors_per_FAT )
	{
		//更新缓冲区中的内容
		if( ReadSector( p_fat32->FAT1_address + DBR_sector, DBR_buffer) != 0)
		{	//读FAT表
			DB_SendString("\nread sector error!\n");
			return 1;
		}
		index = 0;
		while( index < p_fat32->bytes_per_sector )
		{
			if( GetDWord_S(DBR_buffer, index) == 0)
			{	//如果遇到空闲簇
				//计算并保存空闲簇的簇号
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
	{	//如果空闲簇都没有被初始化，说明没有空闲簇
		p_fat32->cur_free_cluster = FAT32_EOC;
		DB_SendString("\nThere is no free cluster!\n");
	}
	else if( p_fat32->cur_free_cluster < p_fat32->FAT_num)
	{	//错误
		return 1;
	} 
	DB_SendString("\n cur_free_cluster :\n");
	DB_SendDec(p_fat32->cur_free_cluster);

	//初始化 cur_free_cluster 结束

	return 0;
}

//说明：
//	初始化文件系统
//参数：
//	无
//返回：
//	0	成功；
//	1	失败；	
//注意
//	1. 暂时仅支持FAT32格式的文件系统，且每扇区大小为512字节
//	2. 在初始化文件系统前要先初始化SD卡，如果不是基于SD卡，那么就初始化相应的存储设备
//	3. 文件系的代码采用了KEIL的动态内存管理机制，所以在调用FileSysInit之前还需要调用init_mempool函数
//	init_mempool函数的详细说明请参考KEIL的帮助文档
u8 FileSysInit()
{
	DB_SendString("Read MBR to buffer...\n"); 
	if(ReadSector(0,RWbuffer)) //读MBR,第一个扇区
	{
		DB_SendString("\nRead MBR error\n");
		return 0;
	}
	filesys.DBR_sector = 0;

	//如果返回0，则没有MBR，DBR就在0扇区
	if(CmpASCArray("MSDOS5.0", RWbuffer+0x03, 8, 0) )	
	{	//如果有MBR，则从分区表中得到DBR的地址
		filesys.DBR_sector = GetDBRAddr(RWbuffer);

		//读取DBR到FatBuffer
		DB_SendString("\nRead DBR to buffer...\n");
		if(ReadSector(filesys.DBR_sector, RWbuffer) != 0 )	//得到DBR，即分区信息
		{
			DB_SendString("\nRead DBR Error!\n");
			return 1;
		}
		if(CmpASCArray("MSDOS5.0", RWbuffer+0x03, 8, 0))
		{
			return 1;
		}
	}

	//显示文件系统类型
	DB_SendString("File System: ");	
	if( 0 == CmpASCArray(&(RWbuffer[0x52]), "FAT32", StringLength("FAT32"), 0))
	{
	 	DB_SendString("FAT32\n\n");
		//type = FAT32;
			//初始化fat32
		if(InitFat32(RWbuffer, filesys.DBR_sector ,&fat32) != 0)
		{
			DB_SendString("InitFat32 error!\n");
			return 1;
		}
	}
	else if( 0 == CmpASCArray(&(RWbuffer[0x52]), "FAT16", StringLength("FAT16"), 0))
	{
	  	DB_SendString("FAT16\n\n");
		return 1;	//暂不支持FAT16
	}
	else
	{
		return 1;	//仅支持FAT32
	}
	
	//初始化cur_dir变量
	MemorySet((u8*)&cur_dir, sizeof(File), 0);

	cur_dir.attribute = SUB_DIR;	//把属性设置为子目录
	cur_dir.buf.p = RWbuffer;		//设置当前目录的缓冲区
				
	GoRootDir(&cur_dir);		//把当前目录设置为根目录

	return 0;
}


//说明：
//	获取数据区的扇区的物理地址
//参数：
//	cluster	簇号 cluster >= 2
//	sector_offset 簇内扇区偏移 sector_offset < sectors_per_cluster
//返回：
//	扇区地址 成功
//	1 失败
//备注：
//	1. 函数未添加查错的方法
static finline u32 GetSectorAddr(u32 cluster,u8 sector_offset)
{
	cluster = (cluster - 2)*fat32.sectors_per_cluster;
	cluster += fat32.FAT_num*fat32.sectors_per_FAT;
	cluster += fat32.FAT1_address;
	cluster += sector_offset;	//加上簇内扇区偏移
	return cluster;
}


//说明
//	在指定的目录（文件夹）中搜索一个文件，如果搜索到那个文件，那么当前
//	目录内部的文件指针和缓冲区指针会指向这个目录项
//参数：
//	name_x	文件名（不含路径） + '.' + 扩展名
//	len		name_x字符串的长度
//	p_dir 
//		指定的目录，如果为空，那么默认为当前目录
//返回：
//	成功返回0，否则返回其它值
//注意
//	1. 不进入当前文件目录（文件夹）的目录（文件夹）中进行搜索
//		51单片机的堆栈有限，用XDATA实现堆栈比较麻烦，所以就不支持这个功能了
//	2. 不区分大小写
//	3. 不支持路径搜索
u8 SearchItem(const u8* name_x, u8 len, File*  p_dir)
{
	u8 xdata dirItem = DIRITEM_DELETED;
	DB_SendString("\nIn search item function. searching file:"); DB_SendString((char*)name_x);
	if(p_dir == NULL)
	{
		p_dir = &cur_dir;
	}
	dirseek(p_dir, 0, SEEK_SET);	//初始化到第一个目录项
	DB_SendString("\nenter searching loop.\n");
	while(1)	//进行搜索	
	{
		dirItem = CheckDirItem( p_dir->buf.p + p_dir->buf.pos );
		
		if( DIRITEM_SHORT == dirItem)
		{	//如果是短目录项，那么就开始比较文件名
			if(0 == CmpShortFileName_X(name_x, len, p_dir->buf.p + p_dir->buf.pos) )
			{
				DB_SendString("\nCmpShortFileName_X :File name is equal! file is found!\n");
				return 0;	//返回成功
			}
		}
		else if(DIRITEM_EMPTY == dirItem)
		{	//如果这个目录项是空目录项，那么后面就没有了，不必继续搜索
			DB_SendString("\nmeet empty item, The file is not found.\n");
			return 1;	//返回没找到
		}

		if( dirseek(p_dir, DIRITEM_LENGTH, SEEK_CUR) != 0 )
		{	//移动文件指针和缓冲区指针
			DB_SendString("dirseek error. file not found!\n");
			return 2;
		}
		if( IsEndOfDir(p_dir) )	//seek完要判断
		{	//如果到了目录尾了
			DB_SendString("reach the end of dir. file not found!\n");
			return 1;
		}
		
	}		
}

//说明
//	目录文件指针和缓冲指针所指向一个目录项，这个函数利用这个目录项的
//	信息来填充文件结构体
//参数：
//	p_file	指向一个要被填充的文件结构体
//	p_dir	指向一个目录的文件结构体的指针
//			如果这个值为空，内部一开始会自动调整到指向当前目录cur_dir
//返回：
//	成功返回0，否则返回其它值
//注意：
//	1.	这个函数是跟随在SearchItem成功后才能使用的
u8 FillFileStruct(File* p_file, File* p_dir)
{
	if( p_dir == NULL)
	{
		p_dir = &cur_dir;
	}

	//先检查目录项的合法性
	if( CheckDirItem( p_dir->buf.p + p_dir->buf.pos) != DIRITEM_SHORT)
	{
		return 1;
	}
	//再检查缓冲区的有效性
	else if(p_dir->buf.pos + DIRITEM_LENGTH > fat32.bytes_per_sector)
	{
		return 1;
	}

	//设置起始簇
	//高16位
	p_file->cluster = GetWord_S(p_dir->buf.p, p_dir->buf.pos + 0x14);
	p_file->cluster = (p_file->cluster << 16);
	//低16位
	p_file->cluster += GetWord_S(p_dir->buf.p, p_dir->buf.pos + 0x1a);
	DB_SendString("\nFiel first Cluster ");
	DB_SendDec(p_file->cluster);
	
	//设置结构体的其他变量
	p_file->next_cluster = p_file->cluster;	//下一将要读的簇为起始簇
	p_file->next_sector_offset = 0;			//下一将要读的扇区

	p_file->size = GetDWord_S( p_dir->buf.p, p_dir->buf.pos + 0x1c );
	DB_SendString("\nSize :");
	DB_SendDec( p_file->size );
	DB_SendString(" bytes.\n");

	p_file->attribute = p_dir->buf.p[p_dir->buf.pos + 0x0b];
	DB_SendString("attribute :");
	DB_SendHex(p_file->attribute);

	p_file->dir.cluster = p_dir->cluster;
	p_file->dir.pos = p_dir->ptr;
	
	MoveToFirstSector(p_file);	//文件指针的初始化工作
	
	return 0;
}

//说明：
//	改变工作目录，支持直接使用路径
//参数：
//	path	目标目录的路径
//	len		path的长度
//	p_dir	要改变的结构体，如果为空，默认是当前目录
//返回：
//	0		成功
//	非0		失败，并且当前目录会变成根目录
//注意：																			  
//	暂不支持上一级目录的 .. 符号(cmpshortfilename那里可能得改改)
u8 ChangeDir(const u8* path, u8 len, File* p_dir)									
{
	u8 idata pos = 0;

	if(p_dir == NULL)
	{
		p_dir = &cur_dir;
	}
	
	if( len == 0 )
	{		//如果字符串内容为空，返回失败
		GoRootDir(p_dir);
		return 1;
	}

	if( FILE_SEPARATOR == (*path) )	//如果字符串里的第一个就是文件分隔符	
	{		//改当前当前目录为根目录，否则，就在当前目录下进行
		GoRootDir(p_dir);
		path++;						//进位，前面的内容已经没用了
		len--;						//更新len
		if(len == 0)	
		{	//如果只是想到根目录，那么就返回
			return 0;
		}
	}

	while(1)
	{
		pos = Find( path, len, FILE_SEPARATOR);		//找到下一个文件分隔符的位置
		//此时pos为下一个目录名的长度
		if( 0!= SearchItem(path, pos, p_dir) )	//更换目录
		{
			DB_SendString("\ndir not found, ChangeDir failed.\n");
			return 1;
		}
		if( FillFileStruct(p_dir, p_dir) != 0)
		{
		 	DB_SendString("\nfill file struct error! ChangeDir failed.\n");
			return 1;
		}
		if((cur_dir.attribute & SUB_DIR) == 0)		//检查是否是子目录
		{	//如果属性不是子目录，那么就返回失败
			DB_SendString("The path is not a directory.\n");
			GoRootDir(p_dir);
			return 1;
		}
		
		if(pos < len)			
		{
			len -= (pos + 1);	//更新len
			if(len == 0)		//如果遇到结束，类似情况 “subdir\"
			{	
				return 0;		//结束返回		
			}	
			path = &(path[ pos + 1] );	//进位，前面的内容已经没有用了
		}
		else	
		{	//类似情况 “subdir" 到结尾啦
			return 0;
		}
	}
}

//说明：
//	判断目录项是否合法
//参数：
//	pDirItem
//		指向buffer中的目录项的首地址（目录项的表示格式与文件系统中的表示方法一致）
//		目录项的长度是32个字节
//返回：
//	DIRITEM_EMPTY		空项目
//	DIRITEM_SHORT		合法短文件项
//	DIRITEM_DELETED		该项已被删除
//	DIRITEM_LONG		长文件项
//	DIRITEM_ERROR		错误	
//备注：
//	由于有判断是否有错误，所以效率可能会比较低
#define NOCHECK_ERROR
u8 CheckDirItem( u8* pDirItem )
{
	u8 idata i;
#ifndef NOCHECK_ERROR
	//err_char中的字符是不能出现在DirName的任何位置的
	u8 code err_char[] = { 0x22, 0x2a, 0x2b, 0x2c, 0x2e, 0x2f, 0x3a, 0x3b
		,0x3c, 0x3e, 0x3f, 0x5b, 0x5c, 0x5d, 0x7c, 0x00 /*加个0，空中止的字符串*/};
#endif
	
	i = pDirItem[0];

	if( i == 0xe5 )	
	{	//如果项目已被删除
		return DIRITEM_DELETED;
	}
	else if(i == 0)
	{	//如果文件名的第一项是 0，说明是空的
		DB_SendString("\nThis item is empty!\n");
		return DIRITEM_EMPTY;
	}
#ifndef NOCHECK_ERROR
	else if(i == 0x20)	
	{	//文件名的第一个字节是不能为0x20的
		return DIRITEM_ERROR;
	}
#endif
	
	else if( ( READ_ONLY | HIDEDDEN | SYSTEM | VOLLABLE ) == pDirItem[0x0b])
	{	//如果是遇到长文件项标志
		return DIRITEM_LONG;
	}

#ifndef NOCHECK_ERROR
	//检查看有没有非法字符
	for(i = 0; i < 8; i--)
	{
		//检查看是否有非法字符
		if( pDirItem[i] == err_char[FindChar(pDirItem[i], err_char, 0)] )
		{	//如果确认有非法字符，返回错误
			return DIRITEM_ERROR;
		}
	}
#endif
	
	//如果到了这里，说明没有非法字符，所以返回合法的短文件项
	return DIRITEM_SHORT;
}

//说明：
//	比较一个 null-terminated string 和短文件项中的文件名(包括拓展名)，大小写无关
//参数：
//	name_x	提供的字符串(ASCII编码，GB编码也行)
//	len		name_x指向的字符串的长度
//	pDirItem	文件项的内容从文件系统中读到内存buffer中的
//返回：
//	0	字符串和文件名相等
//	非0	字符串和文件名不相等
//注意：
//	1.	这个函数比较的是短文件名，在调用这个函数之前需要用 CheckDirItem 检查文件项
//备注：
//	1.	由于国标码和ASCII兼容，所以应该就不用根据是不是国标码来判断比较具体字节的时
//		候要不要当成字母来比较了
char CmpShortFileName_X(const u8* name_x, u8 len,const u8* pDirItem)
{
	u8 idata i = 0;
	u8 idata j = 0;
	DB_SendString("conpare ");DB_SendString((char*)name_x);
	DB_SendString(" and "); DB_SendString((char*)pDirItem);
	
	//比较文件名部分
	while(1)
	{
		if( (i >= len) || ( '.' == name_x[i] ) )
		{	//如果name_x的文件名部分遇到结束
			if(' ' == pDirItem[i] )
			{	//如果pDirItem的文件名部分也遇到结束
				break;
			}
			else
			{	
				//如果 i == 0，提供的可能是当前目录的'.'或上级目录'..'
				if((i == 0) && (i < len) && (len < 2))
				{
					while(i < len)
					{
						if( (name_x[i] != '.') || (pDirItem[i] != '.'))
							return CmpLetter(name_x[i], pDirItem[i]);
						i++;
					}
					if(pDirItem[i] != ' ')	//如果对应的目录项的文件名还不结束
						return pDirItem[i];	
					return 0;
				}				 
				//如果pDirItem的文件名部分还没结束，那么就返回不相等
				return pDirItem[i];
			}
		}
		else if( ( i >= 8) ||  (' ' == pDirItem[i] ))
		{	//如果文件项中的文件名遇到结束，那么name_x中的文件名部分也应该结束
			while(1)
			{
				if( (i >= len) || ('.' == name_x[i]) )	
				{	//如果name_x的文件名部分也结束了
					break;
				}
				else if( ' ' != name_x[i])
				{	//如果文件名部分还有非空格字符，返回不相等
					return ' ';
				}
				i++;
			}
			break;
		}
		else if( (0 != CmpLetter(name_x[i], pDirItem[i]) ))// &&
		//	  (0 == j) )	 	//j== 0 表示这个字节可以当字母来比较		//( (name_x[i] != pDirItem[i]))
		{	//如果不相等，直接返回
			return CmpLetter(name_x[i], pDirItem[i]);		//(name_x[i] - pDirItem[i]);			
		}

		i++;
	}


	//如果到了这里，说明文件项中的文件名部分和name_x的文件名部分相等
	//此时要么 (i == len) 或者是 (name_x[i] == '.')
	if( i >= len)
	{	//如果name_x遇到结束，那么目录项的拓展名也应该为空，否则不相等
		if(pDirItem[8] == ' ')
		{	//如果拓展名也为空
			return 0;
		}
		else
		{	//如果不为空，则不相等
			return pDirItem[8];
		}
	}

	j = 0;
	i++;

	//现在开始比较拓展名
	while(1)
	{
		if( i >= len)
		{	//如果 name_x 中的拓展名遇到结束
			if( ( ' ' != pDirItem[8 + j] ) && ( j < 3) )
			{	//如果pDirItem中的拓展名部分没有结束
				return pDirItem[8 + j];
			}
			else
			{
				return 0;
			}
		}
		else if(j >= 3)
		{	//如果目录项的拓展名结束了，那么name_x那应该到‘\0'了，否则不相等
			if(name_x[++i] == 0)
			{	//如果也遇到结束
				return 0;			
			}
			else
			{	//如果不是结束
				return name_x[i];				
			}
		}
		else if(0 != CmpLetter( name_x[i], pDirItem[8 + j]) )
		{	//如果不相等
			return CmpLetter( name_x[i], pDirItem[8 + j]);
		}
		j++;
		i++;
	}
	
}



//说明：
//	从FAT得到簇列表
//参数：
//	start			开始簇号，也会是列表的0号项
//	list			指向列表的指针
//	max_list_item	列表最多容纳的项数，也就是最多能记录多少个簇
//返回
//	记录到列表的项数
//	GCL_ERROR 错误
static
u16 GetClusterList(u32 start, u32* list, u16 max_list_item)
{
	u8 xdata counter;					//表示list中下一个可记录的位置
	u32 xdata  last;					//上一个记录在List中的簇数
	//这样初始化是为了一开始和(start<<2) / fat32.bytes_per_sector 不相等 
	u32 xdata sector_offset = ~((u32)0);	//用来记录buffer对应的扇区从FAT1偏移的扇区数

	PreFileOp(NULL);

	
	if(start == 0)
	{	//刚创建的大小为0的文件的目录项中写的起始簇号是0
		list[0] = FAT32_EOC;	//直接在0号位写入结束标志
		return 0;
	}
	else if(start < fat32.FAT_num)
	{							//0号到(FAT - 1)号簇是保留作系统用的
		return GCL_ERROR;		//返回错误
	}
	
	list[0] = start;
	last = start;
	counter = 1;	
	while(counter < max_list_item)	
	{	//最多循环max_list_item - 1次，第一项已经被赋值了
		//start存放着上列表中的上一个记录，现在用来存放从FAT1偏移的扇区数
		start = ( (start<<2) / fat32.bytes_per_sector );
		
		if( sector_offset != start)
		{	//如果有必要更新缓冲区
			sector_offset = start;
			//读扇区到buffer
			if(ReadSector( fat32.FAT1_address + sector_offset, RWbuffer) != 0)
			{
				DB_SendString("\nReadSector error!\n");
				return 	GCL_ERROR;
			}
		}		
		//将start用来存放扇区内字节偏移
		start = ((last << 2) % fat32.bytes_per_sector); 

		start = GetDWord_S(RWbuffer, start);			//先读取簇号		

		if(start == (last + 1))	
		{	//如果和上一个记录的值物理上连续，则可能可以进行优化，以提高list的利用率
			last = start;	//更新上一个读取的值
			if(counter > 2)	
			{	//在特定的位置范围才优化，以简化读取list的函数
				if(list[counter - 2] == CONTINUOUS)
				{	//如果上一个记录在表中的数值已经是连续标记
					list[counter - 1] = start;	//覆盖上一个值，不必更新counter
					continue;	//直接跳过后面的判断
				}
				else if(((list[counter - 2] + 2) == start) &&
						((list[counter - 3] + 3) == start))
				{	//如果已经连续了三次，那么就可以写入优化标记了
					list[counter - 2] = CONTINUOUS;	//写入优化标记
					list[counter - 1] = start;		//覆盖上一个值，不必更新counter
					continue;	//直接跳过后面的内容，不需要更新counter
				}
			}
		}

		if(start < fat32.FAT_num)
		{
			return GCL_ERROR;		//返回错误
		}

		list[counter] = start;	//写入list
		counter++;				//更新counter
		last = start;

		if(start == FAT32_EOC)	//如果结束
		{
			return counter;		//因为最后一项内容是0x0fffffff,表示结束			
		}
		else if(start < fat32.FAT_num)
		{	//如果一开始参数start的簇号是没有使用的,就可能出现这种错误
			DB_SendString("\nGetting file cluster list error! cluster < numbers of fat.\n");
			return GCL_ERROR;
		}		
	}
	return (counter + 1);	//如果跳出了循环，说明list满了
}


//说明：
//	打开一个文件，下一要读的位置初始化为文件头，支持路径搜索
//参数：
//	name_x	文件名（可含路径）
//	p_file	文件的信息将被储存在p_file指向的结构体中
//返回：
//	0		成功
//	其它	错误
//注意：
//	1.	如果不再使用文件，一定要调用fclose关闭文件
//	2.	这个函数会改变文件系统内部cur_dir（当前目录）的内容
File* fopen(const u8* name_x, u8 mode)
{
	union
	{	struct
		{	u8 H;
			u8 L;
		}bytes;
		u16 word;
	}idata temp;	//额。。。省空间。。。有时要拆开来用。。。
	File *p_file = NULL;
	
	DB_SendString("\nIn fopen funciton. You want to open file :"); DB_SendString((char*)name_x);
	///////////////////////////////处理路径信息开始///////////////////////////////
	//得到字符串中路径的长度，记录在 temp.bytes.L中，这个值也是目标文件名的起始位置的索引
	temp.bytes.L = 0;	//初始化长度为0
	temp.bytes.H = FindChar(FILE_SEPARATOR, name_x, temp.bytes.L);//照顾name_x为当前目录下文件的情况
	while(name_x[temp.bytes.H])	//如果name_x为当前目录下文件，那么不会进入while中，temp.bytes.L为0
	{
		temp.bytes.L = temp.bytes.H;
		(temp.bytes.L)++;	//自增1
		temp.bytes.H = FindChar(FILE_SEPARATOR, name_x, temp.bytes.L);
	}
	//此时 temp.bytes.L 为得到字符串中路径的长度
	if( 0 != temp.bytes.L )	
	{	//如果有路径成分，那么就更改目录
		if(0 != ChangeDir(name_x, temp.bytes.L, NULL))	//更改当前目录
		{	//如果目录切换失败
			DB_SendString("\nChangeDir failed. There may be invalid path.\n");
			goto FAIL;
		}
	}
	/////////////////////////////路径信息处理结束/////////////////////////////

	//给p_file分配空间
	p_file = NewFileStruct();
	if(p_file == NULL)
	{
		DB_SendString("\nNewFileStruct error.\n");
		goto FAIL;
	}
	
	temp.bytes.H = StringLength(name_x);
	name_x += temp.bytes.L;					//转到最后打开的文件名的首地址

	if( SearchItem( name_x, temp.bytes.H - temp.bytes.L, &cur_dir ) ) //搜索文件
	{	//如果找不到文件
		if( checkb(mode , FILE_CREATE ) == 0)
		{
			DB_SendString("\nOpen file failed. File is not found.\n");
			DeleteFileStruct(p_file);	//打开失败，释放内存
			goto FAIL;
		}
		else
		{	//如果是以找不到文件则创建新文件的方式打开，就创建新的文件
			if(IsEndOfDir(&cur_dir))
			{	//如果需要分配新簇
				if(AllocNewCluster(&cur_dir))
				{
					DB_SendString("\nopen file failed. cannot create new file!\n");
					goto FAIL;
				}
				if(FillCluster(cur_dir.next_cluster, 0))	//清零缓冲区
				{
					DB_SendString("\nfill cluster error! fopen failed\n");
					goto FAIL;
				}
				if(dirseek(&cur_dir, 0, SEEK_CUR) )		//分配新簇后要重新定位文件指针
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
	if( FillFileStruct( p_file, &cur_dir) != 0)		//找到文件后就可以填充文件结构体了
	{
		DB_SendString("\nFillFileStruct error!\n");
		DeleteFileStruct(p_file);	//打开失败，释放内存
		return NULL; 
	}
	//设置模式
	p_file->mode = mode;
 	DB_SendString("\nSucceed, Exit fopen function!\n");
	return p_file;
	
	FAIL:
		DeleteFileStruct(p_file);
		return NULL;
}

//说明：
//	让File结构体中的next_cluster和next_sector_offset指向下一个扇区
//参数：
//	p_file	指向File结构体的指针，不可以为NULL
//返回：
//	调用这个函数之前next_cluster和next_sector_offset指向的扇区的地址
//	如果 调用这个函数之前next_cluster和next_sector_offset指向的扇区的地址 里有结束信号
//	那么就会返回 SECTOR_END
//注意：
//	在调用这个函数之前应该检查是否已经到文件尾了
u32 MoveToNextSector(File* p_file)
{
	u32 idata addr;	//返回的地址
	
	if(p_file->next_cluster != FAT32_EOC)	//检查是不是结束
	{	//保存返回的地址
		addr = GetSectorAddr(p_file->next_cluster, p_file->next_sector_offset);
	}
	else
	{	//如果是结束，就直接返回
		return SECTOR_END;
	}

	if(p_file->next_sector_offset >= (fat32.sectors_per_cluster - 1))		
	{	//这次调用此函数要返回的扇区是簇内最后一个扇区，说明可能要更新cluster了
		p_file->next_sector_offset = 0;	  	//下一簇0扇区

		if(p_file->list_index < CLU_LIST_ITEM - 1)	//检查簇列表是否读完---
		{
			if(p_file->cluster_list[p_file->list_index] == CONTINUOUS)
			{	//如果有连续标记
				++(p_file->next_cluster);	//直接自加1
				if((p_file->next_cluster) == (p_file->cluster_list[p_file->list_index + 1]))
				{	//如果优化的连续簇结束了
					//使p_file->next_cluster) 和 (p_file->cluster_list[p_file->list_index])一致
					++(p_file->list_index);
				}
			}
			else
			{	//如果没有连续标记，就正常读取
				++(p_file->list_index);
				if(p_file->cluster_list[p_file->list_index] == CONTINUOUS)	
				{	//如果自增list_index后又遇到连续标记
					//直接自加1，而不是	
					//p_file->next_cluster = p_file->cluster_list[p_file->list_index];
					++(p_file->next_cluster);		
				}
				else	//如果自增list_index后没有遇到连续标记
				{
					p_file->next_cluster = p_file->cluster_list[p_file->list_index];
				}
			}
		}
		else	
		{	//如果簇列表读完了
			GetClusterList(p_file->next_cluster, p_file->cluster_list, CLU_LIST_ITEM);
			p_file->next_cluster = p_file->cluster_list[1];
			p_file->list_index = 1;		//0号位为这个函数里等会就要返回的扇区所在的簇号
		}
	}
	else
	{
		p_file->next_sector_offset++;
	}

	return addr;
}

//说明：
//	使File文件结构体中的next_cluster和next_sector_offset指向文件的第一个扇区
//	同时也会清零文件指针和无效缓冲区指针，对文件指针进行初始化工作
//参数：
//	p_file	指向File结构体的指针
//返回：
//	如果成功，返回0，否则返回其它值
//注意：
//	1.	File结构体的cluster变量要已经被初始化了
u8 MoveToFirstSector(File* p_file)
{
	//得到簇列表
	if(GetClusterList(p_file->cluster, p_file->cluster_list, CLU_LIST_ITEM) == GCL_ERROR)
	{
		DB_SendString("\nGet cluster list error!\n");
		free(p_file);	//打开失败，释放内存
		return 1;	 
	}	
	//因为 p_file->cluster可能是0，所以不直接用cluster给next_cluster赋值
	p_file->next_cluster = p_file->cluster_list[0];
	p_file->next_sector_offset = 0;
	p_file->list_index = 0;
	p_file->ptr = 0;							//清零文件指针
	p_file->buf.pos = fat32.bytes_per_sector;	//无效缓冲区指针
	return 0;
}


//说明：
//	文件的下一个扇区到buffer中，
//	如果buffer为NULL，就读一个扇区到文件的缓冲区，并更新文件指针，参数size被忽略
//参数：
//	p_file	指向一个文件属性（非子目录属性）的文件结构体的指针
//	buffer	读到目录缓冲区
//	size	缓冲区的大小
//返回：
//	 读到的字节数，如果返回FR_ERROR，则出错了
//注意：
//	1.	对于同一个文件，不可以一次读的时候buffer用NULL，一次不用NULL，这样的结果是未定义的。
//		因为两种方式的处理机制是不同的！
//	2.	如果对一个文件使用了fseek函数，调用fread就不能使用NULL的buffer
//	3.	p_file指向的结构体不可以是子目录属性的
u16 fread(File* p_file,u8* buffer,u16 size)
{
	u16 temp = 0;
	u32 xdata sector;

	if(p_file == NULL)
	{	//如果p_file是空的
		return FR_ERROR;
	}
	else if((p_file->mode & FILE_READ) == 0)	//如果不是以读模式打开
	{
		DB_SendString("fread error! file is not open with read mode.\n");
		return FR_ERROR;
	}

	//先检查是否在文件尾
	if ( IsEndOfFile(p_file) )
		return 0;					//如果是，就不应该更新buffer了

	if(buffer == NULL)
	{	//如果buffer是空，默认读一个扇区到文件对应的缓冲区中

		//更新缓冲区到下一个扇区
		if(UpdateFileBuffer( MoveToNextSector(p_file), p_file) != UFB_SUCCEED)
		{	//如果失败了
			DB_SendString("\nfread Reading error!\n");
			return FR_ERROR;
		}

		if( ((p_file->size - p_file->ptr) <= fat32.bytes_per_sector) )	
		{	//如果读到文件尾了
			//更新下一要读的位置为文件尾
			temp = p_file->size - p_file->ptr;
			p_file->ptr = p_file->size;					//更新文件指针到文件尾
			p_file->buf.pos = temp;						//更新缓冲区指针
			return temp;								//返回读到temp字节
		}
		else 	
		{
			p_file->ptr += fat32.bytes_per_sector;		//更新文件指针
			p_file->buf.pos = fat32.bytes_per_sector;	//更新缓冲区指针
			return fat32.bytes_per_sector;				//返回读到一个扇区
		}
	}
	else
	{	//可以把要读的部分分成三部分：
		//	第一部分 头部，一个大小小于扇区大小的内存块
		//	第二部分 中间部分，一个个扇区
		//			（PS：在51单片机应用上不太可能出现这个了。。。不过还是有办法测试的）
		//	第三总分 尾部，也是一个大小小于扇区大小的内存块

		//为文件操作准备缓冲区
		PreFileOp(p_file);
	
		if (size > ( p_file->size - p_file->ptr) )
		{	//先根据文件尾进行截取
			size = p_file->size - p_file->ptr;
		}
				
		if( p_file->buf.pos < fat32.bytes_per_sector  )
		{	//对第一部分的处理，如果当前的buffer中有内容可以读
			//计算应该读的字节数保存到temp中
			DB_SendString("\nfread part 1\n");
			if(size <= (fat32.bytes_per_sector - p_file->buf.pos ) )
			{	//如果 size 范围在这个缓冲区内
				temp = size;
			}
			else
			{	//如果 size 范围在这个缓冲区外
				temp = fat32.bytes_per_sector - p_file->buf.pos;
			}

			//开始读取内容
			MemoryMove(buffer, p_file->buf.p + p_file->buf.pos, temp);
			p_file->ptr += temp;		//更新文件指针
			p_file->buf.pos += temp;	//更新缓冲区下一要读写的位置

			//为下一部分作准备
			size -= temp;
			buffer += temp;
		}

		while( size >= fat32.bytes_per_sector)
		{	//对第二部分进行处理，其实这一部分可以进行优化的，但是对驱动函数有新要求
			//而且在51单片机上不会有这种情况，所以暂时不做优化
			//直接读到用户buffer中
			DB_SendString("\nfread part 2\n");
			if(ReadSector(MoveToNextSector(p_file), buffer) != 0)
			{	//如果更新缓存失败，返回错误，其实也可以返回temp，记录错误
				return FR_ERROR;
			}
			size -= fat32.bytes_per_sector;				//更新剩下的大小
			buffer += fat32.bytes_per_sector;			//更新用户buffer
			p_file->ptr += fat32.bytes_per_sector;		//更新文件指针
			temp += fat32.bytes_per_sector;				//temp用来记录实际读取的字节数
		}

		if( size > 0)
		{	//对第三部分进行处理
			DB_SendString("\nfread part 3\n");
			p_file->buf.pos = 0;	//这样才会更新扇区
			sector = MoveToNextSector(p_file);
			//因为前面的操作可能会让p_curFile变为NULL，到了这里，缓冲区又属于
			//文件p_file了，不改会影响函数PreFileOp的行为，会出错
			//p_curFile = p_file;
			if( UpdateFileBuffer(sector, p_file) != UFB_SUCCEED)
			{	//如果更新缓存失败，返回错误
				DB_SendString("update ubffer error.\n");
				return FR_ERROR;
			}
			MemoryMove( buffer, p_file->buf.p, size);
			p_file->ptr += size;		//更新文件指针
			temp += size;				//更新读取的大小
			p_file->buf.pos = size;		//更新缓冲区指针
		}

		return temp;	//返回读取到的字节数
	}
}

//说明：
//	关闭一个文件
//参数：
//	p_file，应该叫文件标识符吧
//注意：
//	1.	当不再使用文件的时候一定要关闭文件
//	2.	这个函数也是会改变文件系统内部cur_dir变量的位置的
//返回：
//	成功返回0；失败返回其它值（这个时候出了未知错误了）
u8 fclose(File* p_file)
{
	u32 temp;
	if(p_file == NULL)
		return 0;
	PreFileOp(p_file);
	//如果缓冲区中有被写入的内容，需要更新到存储器中
	UpdateFileBuffer(p_file->buf.sector, p_file);

	if( !IsSubDir(p_file) )
	{	//对于非目录属性才需要的处理，保存新的文件大小
	 	if(GoToFileDir(p_file, &cur_dir)!= 0)
		{	//错误
			DB_SendString("Go to file dir error!");
			return 1;
		}
		//比较和目录中的大小
		if(p_file->size != GetDWordFromLE( cur_dir.buf.p + cur_dir.buf.pos + 0x1c))
		{	//如果大小不同，写入新的大小
			PutDWordToLE(cur_dir.buf.p + cur_dir.buf.pos + 0x1c , p_file->size);
			cur_dir.buf.is_changed = 1;	//置位写入标记
			//写入内容到存储器里
		}
		//得到文件的簇
		temp = (u32)(GetWordFromLE( cur_dir.buf.p + cur_dir.buf.pos + 0x14));	//高2个字节
		temp <<= 16;
		temp += GetWordFromLE( cur_dir.buf.p + cur_dir.buf.pos + 0x1A);	//低2个字节
		if( (temp == 0) && (p_file->cluster != 0) )
		{
			PutWordToLE(cur_dir.buf.p + cur_dir.buf.pos + 0x1A,(u16)(p_file->cluster));
			PutWordToLE( cur_dir.buf.p + cur_dir.buf.pos + 0x14,(u16)((p_file->cluster) >> 16)); 
			cur_dir.buf.is_changed = 1;	//置位写入标记
		}
		//更新cu_dir的缓冲区
		if(UpdateFileBuffer(cur_dir.buf.sector, &cur_dir) != 0 )
		{
			return 1;
		}
		
	}	
	DeleteFileStruct(p_file);	//释放内存
	DB_SendString("\n file is closed.\n");
	return 0;
}

//说明：
//	移动文件指针到指定的位置
//参数：
//	p_file	文件结构体指针，指向一个已经被打开的文件
//	offset	偏移量
//	fromwhere
//		文件指针从哪里开始偏移，可以是以下几个值之一
//		SEEK_SET： 文件开头 　　SEEK_CUR： 当前位置 　　SEEK_END： 文件结尾 
//返回：
//	成功返回0，否则其它值。原则上失败后文件指针的位置不改变。
//	但是如果disk读写出错后就不一定了。
//注意：
//	1.	不支持对文件夹的seek
//备注：
//	fseek会更新文件对应的缓冲区，函数内部不可以使用宏IsEndOfDir
u8 fseek(File* p_file, signed long int offset, u8 fromwhere)
{	//先计算有没有捷径。。。
	u32 xdata pos;

	//不支持对目录的处理
	if(p_file == NULL)
	{
		return 1;
	}
	else if(IsSubDir(p_file))
	{
		return 1;
	}

	//先计算最终要移动到的相对于开头的位置，保存到pos中
	if(fromwhere == SEEK_CUR)
	{
		if( offset < 0 )
		{
			if( (u32)(-offset) > p_file->ptr)
			{	//如果负偏移量太大
				return 1;
			}
		}
		pos = p_file->ptr + offset;
		if( offset > 0)
		{	//如果大于0，判断相加以后会不会溢出
			if(pos < p_file->ptr)
			{	//如果溢出了，就会跳进来
				return 1;
			}
			else if( pos > p_file->size)
			{	//如果偏移量太大
				return 1;
			}
		}
	}
	else if(fromwhere == SEEK_SET)
	{
		if( offset < 0)
		{	//从文件头开始，偏移量不可以为负
			return 1;
		}
		else if( (u32)offset > p_file->size)
		{	//如果偏移量太大
			return 1;
		}
		pos = offset;
	}
	else if(fromwhere == SEEK_END)
	{
		if(offset > 0)
		{	//从文件尾开始，偏移量不可以为正
			return 1;
		}
		else if( (u32)(-offset) > p_file->size)
		{	//如果偏移量太大
			return 1;
		}
		pos = p_file->size + offset; 
	}
	else
	{	//fromwhere 值非法
		return 1;
	}	//到了这里，就认为计算出来的pos是合法的
		
	//offset已经没有用了，下面用来做计数器
	//fromwhere也已经没用了，下面用来做是否更新缓冲区的标志
	fromwhere = 1;
	if( pos >= p_file->ptr)
	{	//如果在当前位置后面
		//计算相差多少个扇区，也就是计算要MoveToNextSector多少次啦~

		offset = ( pos -  (p_file->ptr) + (u32)(p_file->buf.pos)  ) / (u32)(fat32.bytes_per_sector);
		if( offset != 0 )	//如果不是在附近
		{	//MoveToNextSector offset次，最后一次更新缓冲区
			offset--;
		}
		else
		{	//如果seek的内容就在缓冲区附近，就不用更新缓冲区
			fromwhere = 0;
		}
	}
	else
	{	//否则，从开头开始算起
		if(MoveToFirstSector(p_file) != 0)	//复位文件指针
		{
			DB_SendString("\nMoveToFirstSector error!\n");
			return 2;
		}
		offset = pos / fat32.bytes_per_sector; //本来应该加1的，不过最后一次要更新缓冲区
	}

	//先移动到指定的扇区
	while(offset--)	
	{
		MoveToNextSector(p_file);
	}

	//由于可能会更新文件的缓存，所以要准备
	PreFileOp(p_file);
	
	//更新缓冲区指针和缓冲区指针
	p_file->buf.pos = pos % fat32.bytes_per_sector;	
	p_file->ptr = pos;
	if(fromwhere != 0)			//判断是否要更新缓冲区
	{							//最后一次移动要更新缓冲区
		pos = MoveToNextSector(p_file);
		if(pos != SECTOR_END)			//检查MoveToNextSector返回的是不是结束
		{					//如果不是结束，才更新缓冲区
			
			if( UpdateFileBuffer( pos, p_file ) != UFB_SUCCEED)
			{	//如果更新缓冲区失败
				MoveToFirstSector(p_file);	//复位文件指针
				return 2;
			}
		}
		else
		{	//这个时候文件刚好填满整数个簇
			DB_SendString("\nfseek end of file\n");
			p_file->buf.pos =  fat32.bytes_per_sector;	//无效缓冲区
		}
	}
	return 0;
}

//说明：
//	更改目录的文件指针到指定位置
//参数：
//	p_dir	文件结构体指针，指向一个已经被打开的文件
//	offset	偏移量
//	fromwhere
//		文件指针从哪里开始偏移，可以是以下几个值之一
//		SEEK_SET： 文件开头 　　SEEK_CUR： 当前位置
//返回：
//	成功返回0，失败返回其它值。失败后文件指针的位置是未定义的。
//注意：
//	fromwhere在这里不能为 SEEK_END
u8 dirseek(File* p_dir, signed long int offset, u8 fromwhere)
{
	u32 xdata pos;

	//先检查是不是目录
	if(!IsSubDir(p_dir))
	{
		return 1;
	}
	else if( fromwhere == SEEK_CUR )
	{	//如果是从当前位置偏移
		if( offset < 0 )
		{
			if( (u32)(-offset) > p_dir->ptr)
			{	//如果负偏移量太大
				return 1;
			}
		}
		pos = p_dir->ptr + offset;
		if( offset > 0)
		{	//如果大于0，判断相加以后会不会溢出
			if(pos < p_dir->ptr)
			{	//如果溢出了，就会跳进来
				return 1;
			}
		}
	}
	else if( fromwhere == SEEK_SET )
	{
		if( offset < 0)
		{	//从文件头开始，偏移量不可以为负
			return 1;
		}
		pos = offset;
	}
	else
	{	//fromwhere不可以是其它的值
		return 1;
	}	
		
	//offset已经没有用了，下面用来做计数器
	//fromwhere也已经没用了，下面用来做是否更新缓冲区的标志
	fromwhere = 1;
	if( pos >= p_dir->ptr )
	{	//如果在当前位置后面
		//计算相差多少个扇区，也就是计算要MoveToNextSector多少次啦~
		offset = ( pos -  p_dir->ptr + p_dir->buf.pos  ) / fat32.bytes_per_sector;
		if( offset != 0 )	//如果不是在附近
		{	//MoveToNextSector offset次，最后一次更新缓冲区
			offset--;
		}
		else
		{	//如果seek的内容就在缓冲区附近，就不用更新缓冲区
			fromwhere = 0;
		}
	}
	else
	{	//否则，从开头开始算起
		if(MoveToFirstSector(p_dir) != 0)
		{
			DB_SendString("\nMoveToFirstSector error!\n");
			return 1;
		}
		offset = pos / fat32.bytes_per_sector; //本来应该加1的，不过最后一次要更新缓冲区
	}
	//先移动到指定的扇区
	while(offset--)	
	{
		if( MoveToNextSector(p_dir) == SECTOR_END )
		{	//如果遇到结束，重新初始化文件指针，再返回失败
			MoveToFirstSector(p_dir);
			return 1;
		}
	}
	//最后更新文件指针和缓冲区指针
	p_dir->ptr = pos;
	p_dir->buf.pos = pos % fat32.bytes_per_sector;
	
	//由于要UpdateFilebuffer，所以要准备
	PreFileOp(p_dir);
	
	if( fromwhere != 0 )	//判断是否要更新缓冲区
	{	//最后一次移动要更新缓冲区
		pos = MoveToNextSector(p_dir);
		if(pos != SECTOR_END)
		{	//如果不是结束
			if( UpdateFileBuffer( pos,  p_dir ) != UFB_SUCCEED)
			{	//如果更新缓冲区失败
				MoveToFirstSector(p_dir); 	//复位文件指针
				return 1;
			}
		}
		else
		{
			if(p_dir->buf.pos == 0)
			{	//到达目录尾了
				p_dir->buf.pos = fat32.bytes_per_sector;	//无效缓冲区
				return 0;
			}
			else
			{	//如果出错，复位文件指针
				MoveToFirstSector(p_dir); 	//复位文件指针
				return 1;
			} 
		}
	}
	return 0;
}

//说明：
//	子目录属性的文件结构体内部维护着指向目录项的指针，调用这个函数可以
//	移动指针到下一个合法的短目录项
//	（合法的长目录项只和文件名有关，所以只有在获取文件名的时候才会派上用场）
//参数：
//	p_dir	指向一个子目录属性的文件结构体，如果这个值为空，默认是当前目录
//返回：
//	成功返回0。只有返回0才可以读取目录项
//	到达目录尾返回 1（这个时候就不能读取目录项了）
//	错误返回其它值
u8 MoveToNextFile(File* p_dir)
{
	u8 temp;

	if(p_dir == NULL)
	{	//如果参数为空，那么就默认是当前目录
		p_dir = &cur_dir;
	}

	if(IsEndOfDir(p_dir))
	{	//如果到尾了，那么就返回
		return 1;
	}
	else if( p_dir->buf.pos + DIRITEM_LENGTH > fat32.bytes_per_sector)
	{	//检查缓冲区的合法性
		DB_SendString("dirseek error. file not found!\n");
		return 2;
	}
	temp = 	CheckDirItem(p_dir->buf.p + p_dir->buf.pos); 
	if(temp == DIRITEM_EMPTY)
	{	//检查看当前的是不是空，如果是，也返回
		return 1;
	}
	else if(temp == DIRITEM_ERROR)
	{
		return 2;
	}

	while(1)
	{
		if( dirseek(&cur_dir, DIRITEM_LENGTH, SEEK_CUR) != 0 )
		{	//移动文件指针和缓冲区指针
			DB_SendString("dirseek error. file not found!\n");
			return 2;
		}
		else if( IsEndOfDir(p_dir) )	//seek完要判断
		{	//如果到了目录尾了
			DB_SendString("reach the end of dir.\n");
			return 1;
		}
		//检查缓冲区的有效性
		else if( p_dir->buf.pos + DIRITEM_LENGTH > fat32.bytes_per_sector)
		{
			DB_SendString("dirseek error. file not found!\n");
			return 2;
		}

		temp = CheckDirItem(p_dir->buf.p + p_dir->buf.pos);

		if( temp == DIRITEM_SHORT)
		{	//如果找到了
			return 0;
		}
		else if( temp == DIRITEM_EMPTY)
		{	//如果是空，也视为目录尾
			return 1;
		}
		else if(temp == DIRITEM_ERROR)
		{	//如果出现错误
			DB_SendString("\nDIRITEM_ERROR!\n");
			return 2;
		}
	}
}

//说明：
//	子目录属性的文件结构体内部维护着指向目录项的指针，调用这个函数可以
//	移动指针到第一个合法的短目录项
//	（合法的长目录项只和文件名有关，所以只有在获取文件名的时候才会派上用场） 
//参数：
//	p_dir	指向一个子目录属性的文件结构体，如果这个值为空，默认是当前目录
//返回：
//	成功返回0。只有返回0才可以读取目录项
//	到达目录尾返回 1（说明这个目录里面没有内容）
//	错误返回其它值
u8 MoveToFirstFile(File* p_dir)
{
	u8 temp;
	if(p_dir == NULL)
	{	//如果参数为空，那么就默认是当前目录
		p_dir = &cur_dir;
	}
	if( MoveToFirstSector(p_dir) != 0)	//先复位指针
	{	//如果复位文件指针失败，也返回错误
		return 2;
	}

	if( dirseek(&cur_dir, 0, SEEK_SET) != 0 )
	{	//移动文件指针到文件开始
		DB_SendString("dirseek error. file not found!\n");
		return 2;
	}
	while(1)
	{	//检查目录项
		temp = CheckDirItem(p_dir->buf.p + p_dir->buf.pos);
		
		if( IsEndOfDir(p_dir) )	//seek完要判断
		{	//如果到了目录尾了
			DB_SendString("reach the end of dir.\n");
			return 1;
		}
//		//检查缓冲区的有效性
//		else if( p_dir->buf.pos + DIRITEM_LENGTH > fat32.bytes_per_sector)
//		{
//			DB_SendString("dirseek error. file not found!\n");
//			return 2;
//		}
		else if(temp == DIRITEM_SHORT)
		{	//如果找到了，就返回成功
			return 0;
		}
		else if( temp == DIRITEM_EMPTY)
		{	//遇到空目录项，说明后面没有内容了
			return 1;
		}
//		else if( temp == DIRITEM_ERROR)
//		{	//如果错误，说明文件系统可能有问题
//			DB_SendString("\nDIRITEM_ERROR!\n");
//			return 2;
//		}

		if( dirseek(&cur_dir, DIRITEM_LENGTH, SEEK_CUR) != 0 )
		{	//移动文件指针和缓冲区指针
			DB_SendString("dirseek error. file not found!\n");
			return 2;
		}
	}
}


//说明：
//	目录属性的文件内有个指向目录项的文件指针，调用这个函数可以得到目录项的短文件名
//参数：
//	p_dir	指向一个子目录属性的文件结构体，如果为空，默认为当前目录
//	buf		返回的文件名存放的位置，长度至少为12个字节
//返回：
//	成功返回0，否则返回其它值
//注意：
//	1.	这个函数只能紧随在以下几个函数后使用，否则结果是未定义的
//		MoveToFirstFile 
//		MoveToNextFile	
u8 GetShortFileNameX(File* p_dir, u8* buf)
{
	u8 i, j;	//, k;	//用作计数
	if(p_dir == NULL)
	{	//如果p_dir为空，默认是当前目录
		p_dir = &cur_dir;
	}
	//检查是不是目录属性
	if(!IsSubDir(p_dir))
	{
		return 1;
	}
	//检查目录项是否合法
	if(CheckDirItem(p_dir->buf.p + p_dir->buf.pos) != DIRITEM_SHORT)
	{
		return 1;
	}

	//获取短文件名
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
		if(buf[i] == 0x20)	//如果空格，跳出循环
		{
			break;
		}
	}
	buf[i] = 0;
	if( j == 8 )	//对于拓展名为空的情况下，去掉'.'
	{
		buf[i - 1] = 0;
	}

	return 0;
}


//说明：
//	为一个文件（子目录属性的也行）的尾簇分配一个新的簇
//参数：
//	p_file	指向一个要分配新簇的文件结构体
//			（对于文件属性）要求这个文件的MoveToNextSector返回SECTOR_END的情况下使用
//			当返回成功后，MoveToNextSector可以再使用，不会返回SECTOR_END了
//			（对于目录属性）要求IsEndOfDir得到非零的时候使用
//			返回成功后，可以原地fseek更新缓冲区，然后就不会IsEndOfDir了
//备注：
//	对于没有分配空间的文件，这里会分配首簇，这个时候需要fclose去完成写入目录项的工作
static
u8 AllocNewCluster(File* p_file)
{
	u16 sector_count;
	u16 index;
	u16 temp;
	u32 free;

	PreFileOp(NULL);	//因为这个函数里可能会读写扇区

	if( p_file->cluster_list[p_file->list_index] == FAT32_EOC )
	{	
		//先找到一个新的空闲簇去更新 fat32.cur_free_cluster
		if( fat32.cur_free_cluster == FAT32_EOC )
		{	//这表示已经没有空闲簇了
			return 1;
		}
		free = fat32.cur_free_cluster;	//保存原来的值

		//把free号簇分配给文件
		p_file->next_cluster = p_file->cluster_list[p_file->list_index] = free;
		if( p_file->list_index < CLU_LIST_ITEM - 1)	//如果簇列表还可以再写入
		{	//在簇列表中写入结束标志
			p_file->cluster_list[p_file->list_index + 1] = FAT32_EOC;
		}

		if(p_file->list_index != 0)
		{
			//对文件原来的最后一个簇对应的FAT表项写入free号簇
			temp = ( p_file->cluster_list[p_file->list_index - 1] * sizeof(u32) )
						 / fat32.bytes_per_sector;
			index = (  p_file->cluster_list[p_file->list_index - 1] * sizeof(u32) )
						 % fat32.bytes_per_sector;
			if( ReadSector( fat32.FAT1_address + temp, RWbuffer) != 0)
			{
				DB_SendString("\nread sector error!\n");
				return 1;	
			}
			PutDWordToLE(  RWbuffer + index, free);	//给表项写入free号簇
		}
		else
		{	
			p_file->cluster = free;			//给文件分配首簇
			temp = fat32.sectors_per_FAT;	//目的是让 sector_count != temp;
		}

		//计算free号簇的FAT表项的位置，给表项写入结束标志
		sector_count = ( free * sizeof(u32) ) / fat32.bytes_per_sector;
		index = ( free * sizeof(u32) ) % fat32.bytes_per_sector;

		if ( sector_count != temp)
		{	//如果两个表项不是在同一个扇区中
			//保存刚刚给文件原来的最后一个簇的FAT表项写入的内容，再更新缓冲区
			// p_file->list_index == 0 时前面没有读FAT，就别谈什么保存了
			if(p_file->list_index != 0)
			{	
				if( WriteSector( fat32.FAT1_address + temp, RWbuffer) != 0)
				{
					DB_SendString("\nread sector error!\n");
					return 1;	
				}
			}

			//更新缓冲区为free号簇表项所在的扇区
			if( ReadSector( fat32.FAT1_address + sector_count, RWbuffer) != 0)
			{
				DB_SendString("\nread sector error!\n");
				return 1;	
			}
		}
		PutDWordToLE(RWbuffer + index  ,FAT32_EOC );	//给free号簇表项写入结束标志
		if( WriteSector(fat32.FAT1_address + sector_count, RWbuffer) != 0)
		{	//保存写入的内容
			return 1;
		}

		//至此新的空闲簇已经分配给文件了，但是还需要对 fat32.cur_free_cluster 进行更新
		//寻找新的空闲簇

		index += sizeof(u32);	//从下一个FAT表项开始搜索新的空闲簇
		while(1)
		{
			while( index < fat32.bytes_per_sector )
			{	//在一个扇区内搜索
				if( GetDWord_S(RWbuffer, index) == 0)
				{	//如果遇到空闲簇
					//计算并保存空闲簇的簇号
					fat32.cur_free_cluster = 
						(sector_count * fat32.bytes_per_sector + index ) / sizeof(u32);
					goto GET_FREE_CLUSTER_COMPLETE;
				}
				index += sizeof(u32);	// 下一个FAT表项
			}
			index = 0;		//重新清零索引
			sector_count++;	//换到下一个扇区
			if( sector_count >= fat32.sectors_per_FAT )
			{	//判断是不是应该退出
				break;
			}
			//更新缓冲区中的内容，到下一个扇区搜索
			if( ReadSector( fat32.FAT1_address + sector_count, RWbuffer) != 0)
			{	//读FAT表
				DB_SendString("\nread sector error!\n");
				return 1;
			}	
		}
		GET_FREE_CLUSTER_COMPLETE:

		if( fat32.cur_free_cluster == free)
		{	//值没有改变，说明没有找到新的空闲簇
			fat32.cur_free_cluster = FAT32_EOC;
		}

		return 0;
	}
	else
	{	//对于list_index不是指到结尾的情况
		return 1;
	}	
}

//说明：
//	把内存中指定大小的数据块写入文件，对一个文件进行写入操作
//参数：
//	p_file	指向一个文件结构体
//	buffer	指向一个数据块，不支持buffer == NULL的情况
//	size	数据块的大小
//注意：
//	这里不支持buffer == NULL的直接扇区写情况
//备注：
//	写操作可能会改变文件的大小，由fclose去完成改变大小的操作
u16 fwrite(File* p_file,const u8* buffer,u16 size)
{
	u16 temp;
	u32 sector = SECTOR_END;

	if(p_file == NULL)
	{	//不可以是空
		return FW_ERROR;
	}
	else if((p_file->mode & FILE_WRITE) == 0)	//如果不是以写模式打开
	{
		DB_SendString("fwrite error! file is not open with write mode.\n");
		return FW_ERROR;
	}

	//可以把要写的部分分成三部分：
	//	第一部分 头部，一个大小小于扇区大小的内存块
	//	第二部分 中间部分，一个个扇区
	//			（PS：在51单片机应用上不可能出现这个了。。。不过还是有办法测试的）
	//	第三总分 尾部，也是一个大小小于扇区大小的内存块
	
	//为文件操作准备缓冲区
	PreFileOp(p_file);

	temp = 0;
	if( p_file->buf.pos < fat32.bytes_per_sector  )
	{	//对第一部分的处理，如果当前的buffer中有内容可以读
		//计算应该读的字节数保存到temp中

		//计算应该写入多少
		if( size < (fat32.bytes_per_sector - p_file->buf.pos))
		{
			temp = size;
		}
		else
		{
			temp = (fat32.bytes_per_sector - p_file->buf.pos);
		}

		//开始写入内容
		MemoryMove(p_file->buf.p + p_file->buf.pos, buffer ,temp);

		p_file->ptr += temp;		//更新文件指针
		p_file->buf.pos += temp;	//更新缓冲区下一要读写的位置
		p_file->buf.is_changed = 1;	//写入内容已被改变的标记

		//如果不用返回，为下一部分作准备
		size -= temp;
		buffer += temp;
	}

	while( size >= fat32.bytes_per_sector)
	{	//对第二部分进行处理，其实这一部分可以进行优化的，但是对驱动函数有新要求
		//而且在51单片机上不会有这种情况，所以暂时不做优化
		//直接读到用户buffer中
		sector = MoveToNextSector(p_file);
		if( sector == SECTOR_END )
		{	//如果需要分配新簇
			if( AllocNewCluster(p_file) != 0)
			{	//如果失败，就返回
				goto RETURN;
			}
			sector = MoveToNextSector(p_file);
		}
		if( WriteSector( sector, buffer) != 0 )
		{
			goto RETURN;
		}
		size -= fat32.bytes_per_sector;				//更新剩下的大小
		buffer += fat32.bytes_per_sector;			//更新用户buffer
		p_file->ptr += fat32.bytes_per_sector;		//更新文件指针
		temp += fat32.bytes_per_sector;				//temp用来记录实际读取的字节数
	}

	if( size > 0)
	{	//对第三部分进行处理
		sector = MoveToNextSector(p_file);
		if( sector == SECTOR_END )
		{	//如果需要分配新簇
			if( AllocNewCluster(p_file) != 0)
			{	//如果失败，就返回
				goto RETURN;
			}

			sector = MoveToNextSector(p_file);
		}	
		if(p_file->ptr < p_file->size)
		{	//如果不是对文件追加内容，而只是修改文件的内容，那么就有效缓冲区指针
			p_file->buf.pos = 0;	//这样update的时候才会更新缓冲区的内容
		}
		if( UpdateFileBuffer(sector, p_file) != UFB_SUCCEED)
		{	//如果更新缓存失败，返回错误
			DB_SendString("update ubffer error.\n");
			return FR_ERROR;
		}
		MemoryMove( p_file->buf.p, buffer , size);	//写到缓冲区
		p_file->ptr += size;						//更新文件指针
		temp += size;								//更新读取的大小
		p_file->buf.pos = size;						//更新缓冲区指针
		p_file->buf.is_changed = 1;					//写入缓冲区被改变标记
	}

	RETURN:
	if( p_file->ptr > p_file->size )
	{	//如果写入的内容超出大小，就修改文件的大小
		p_file->size = p_file->ptr;
	}
	return temp;	//返回读取到的字节数	
}

//说明：
//	到达一个被打开的文件所在的目录，并且让目录属性的文件结构体内的指针
//	指向一个文件所在的文件项
//参数：
//	p_file	指向一个被打开的文件
//	p_dir	目录属性，指向一个要更新位置的目录，如果为空，默认指向文件
//			系统内部的cur_dir
//返回：
//	成功返回0，否则返回其它值。
u8 GoToFileDir(File *p_file, File *p_dir)
{
	if(p_dir == NULL)
	{	// 如果为空，默认指向文件系统内部的cur_dir
		p_dir = &cur_dir;
	}
	if( !IsSubDir(p_dir))
	{
		return 1;
	}

	if((p_file->dir.cluster == fat32.root_cluster)
		 || p_file->dir.cluster == 0 ) //对于根目录下的文件，上级目录簇号写的可能会是0
	{	//如果文件是在根目录
		GoRootDir(p_dir);
	}
	else
	{	
		p_dir->cluster = p_file->dir.cluster;
		MoveToFirstSector(p_dir);				//复位文件指针
		//移动到开始，得到上级目录的信息，填充结构体的 内的dir结构体信息
		if( dirseek(p_dir, DIRITEM_LENGTH, SEEK_SET)!= 0)
		{	//移动到上级目录的目录项
			return 1;
		}
		if( CmpShortFileName_X("..", 2, p_dir->buf.p + p_dir->buf.pos) != 0)
		{	//如果这一项的目录项的名字不是".."，返回失败
			return 1;
		}
		//得到上级目录起始簇的高字节
		p_dir->dir.cluster = GetWordFromLE(p_dir->buf.p + p_dir->buf.pos + 0x14);
		p_dir->dir.cluster <<= 16;
		p_dir->dir.cluster += GetWordFromLE(p_dir->buf.p + p_dir->buf.pos + 0x1A);
		p_dir->dir.pos = 0;	//其实对于在目录属性的结构体中，这一项没有意义
	}
	return dirseek(p_dir, p_file->dir.pos, SEEK_SET);
}



//说明：
//	创建一个新的目录项，要求目录属性文件的指针已经在空目录项了
//参数：
//	name_x	a null-terminated string
//			要求是文件名（不大于8字节） + '.' + 拓展名（不大于3节）的格式
//			如果名字不合法，会返回失败
//	attribute
//			属性
//	p_dir	指定的目录，如果为NULL，默认为当前目录
//返回：
//	成功返回0，否则返回其它值
//备注：
//	暂时不支持写入创建时间，最后访问时间
static
u8 NewDirItem(const u8* name_x,u8 attribute, File* p_dir)
{
	u8 name_len;	//文件名部分长度
	u8 ext_len = 0;	//拓展名部分的长度

	while(*name_x == ' ')	//不要空格
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
	MemorySet(p_dir->buf.p + p_dir->buf.pos, 11,' ' );	//清零目录项中的内容
	MemorySet(p_dir->buf.p + p_dir->buf.pos + 11, DIRITEM_LENGTH - 11, 0);

	p_dir->buf.p[p_dir->buf.pos + 0x0b] = attribute;	//写入属性
	//attribute变量已经没用了，下面用来记数
	attribute = 0;
	while(name_len--)	//写入文件名到缓冲
	{
		p_dir->buf.p[ p_dir->buf.pos + attribute] = getupper(*name_x);
		attribute++;
		name_x++;
	}
	name_x++;	//跳过'.'字符
	attribute = 8;
	while(ext_len--)	//写入拓展名到缓冲
	{
		p_dir->buf.p[p_dir->buf.pos + attribute] = getupper(*name_x);
		attribute++;
		name_x++;	
	}
	p_dir->buf.is_changed = 1;		//置位被改变标记
	if( UpdateFileBuffer(p_dir->buf.sector, p_dir) )	//更新缓冲区
	{
		DB_SendString("\nfatal error! NewDirItem failed! UpdateFileBuffer error!");
		return 1;
	}
	return 0;
}

//说明：
//	用指定的值（一般为0）填充一个指定的簇
//参数：
//	cluster	指定的簇
//	val		指定的值
//返回：
//	成功返回0，错误返回其它值（读写存储器出错了）
u8 FillCluster(u32 cluster, u8 val)
{
	PreFileOp(NULL);	//准备缓冲区
	MemorySet(RWbuffer, fat32.bytes_per_sector, val);
	cluster = GetSectorAddr(cluster,0);	//得到对应的首扇区地址
	//val已经没用了，现在用来计数
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
//说明：
//	在指定的目录下创建一个文件，这个函数可以用来给外部调用
//参数：
//	name_x		文件名，包含拓展名
//	（注：现在只支持短文件名，所以文件名部分不要多于8字节，否则会失败）
//	attribute	文件属性
//u8 CreateFile(u8* name_x,u8 attribute, File* p_dir)
//{
//	
//}

//把p_fat32内部的cur_free_cluster更新到下一个空闲簇
NextFreeCluster(p_fat32)

//对于给目录新分配的簇，需要全部清零
*/
#endif
