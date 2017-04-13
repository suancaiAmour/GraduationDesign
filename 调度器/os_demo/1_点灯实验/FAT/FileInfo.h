#ifndef FILE_H
#define FILE_H
namespace nstd{
	//说明：
	//	每个被打开的文件内部都有一个簇列表，
	//	簇列表项数，最低建议值为4，原则上越大越好，建议4-10就可以了
	#define CLU_LIST_ITEM	4

	//说明：
	//	保存文件信息的结构体，文件夹当作特殊的文件处理
	//注意：
	//	读文件的底层函数必须对有next前缀的成员做更新(外部使用的不这管）
	//	暂不支持长文件名
	struct FileInfo
	{

	#ifdef FAT_FS_C	
		//下面是文件属性有关的宏定义，为防止名字冲突，所以使用了#ifdef，如果外部要使用，请自行注释
		#define 	RW				0x00		//读写
		#define 	READ_ONLY		0x01		//只读
		#define		HIDEDDEN		0x02		//隐藏
		#define		SYSTEM			0X04		//系统
		#define		VOLLABLE		0x08		//卷标
		#define		SUB_DIR			0x10		//子目录
		#define		ARCHIVE			0x20		//归档
	#endif
		u8 attribute;	//文件属性，外部请不要更改此值
		
	/******************以下内容请外部不要使用和更改******************/
			
		u32 cluster;			//首簇号，如果值为0，那么这个文件的大小是0，还没有分配空间
		
		//这两个变量对应的扇区应该是文件缓冲区对应扇区的下一个扇区
		//下一个要读的扇区所在的簇的簇号
		u32 next_cluster;
		//下一要读的扇区所在的簇的簇内偏移扇区
		u8 next_sector_offset;	

		u32 ptr;		//文件指针

		u8 mode;		//打开方式

		u32 cluster_list[CLU_LIST_ITEM];	//簇列表
		//簇列表索引，cluster_list[list_index]一般与file.next_cluster保持一致，但遇到连续标记后就不是这样了
		u8	list_index;		
	/******************以上内容请外部不要使用和更改******************/
		u32 size;	//文件的大小。对于目录项，这个值无意义

		struct	//用来表示目录项的位置，对于目录项，这个是上级目录的位置
		{
			u32 cluster;	//所在目录的首扇区
			//目录项在目录中的位置（类似文件指针），
			//因为有些地方实现起来麻烦，所以对于目录属性，这一项无意义
			//以后可以想办法增加对目录属性的支持
			u16 pos;		
		}dir;

		//pbuffer	//可以用来指向一个buffer结构体

		struct
		{	//文件缓冲的结构体，大小是一个扇区的大小
			u32 sector;		//对应的扇区地址
			u8 is_changed;	//用来表示缓存的内容是否已被改变
			u8* p;			//指向缓冲区
			//指向buffer中下一个要读写的位置，类似文件指针，这里叫缓冲区指针好了
			//如果pos >= bytes_per_sector 指向的缓冲区的大小，那么就认为缓冲区指针是无效的
			//这里的无效的意思是说，缓冲区的内容对应的扇区和文件指针对应扇区的内容没有关系了
			//这里的sector始终应该保持缓冲区对应的扇区，不管缓冲区指针是有效还是无效
			u16 pos;		
		}buf;
	};
}
#endif
