#ifndef FAT_FS_H
#define FAT_FS_H

#include "sys.h"
namespace nstd{

	//说明：
	//	对文件结构体的定义，外部只需要用到文件结构体的指针，所以不需要关心FileInfo结构体的具体定义
	//	因些下面这个warning 可以无视，结构体FileInfo已经在文件系统内部定义了
	//	..\main\main.c(58): warning C230: 'File': uses undefined struct/union tag 'FileInfo'
	//	使用xdata可以节省内存占用
	typedef struct FileInfo xdata File;
	typedef File*		PFile;

	/**	@brief 读写缓存，fread读到的内容就在这里 */
	#define BUFF_SIZE 512
	extern unsigned char xdata RWbuffer[BUFF_SIZE];

	u8 FileSysInit();

	//打开文件模式定义
	#define FILE_READ			Bit(0)		//00000001B
	#define FILE_WRITE 			Bit(1)		//00000010B
	#define FILE_CREATE			Bit(2)		//00000100B	以写打开时，如果文件不存在，则创建文件
	//#define FILE_ALWAYS_CREATE	((u8)BIT(3))	//总是创建，暂时不支持这个
	PFile fopen(const u8* name_x,u8 mode);

	//文件分隔符
	#define FILE_SEPARATOR	('\\')

	#define FR_ERROR	0xffff		//文件读错误
	u16 fread(PFile p_file,u8* buffer,u16 size);	//默认读到read buffer中，即RWbuffer
	#define FW_ERROR	0xffff		//文件写错误
	u16 fwrite(PFile p_file,const u8* buffer,u16 size);

	#define SEEK_SET	0		//文件开头
	#define SEEK_CUR	1		//当前位置
	#define SEEK_END	2		//文件尾
	u8 fseek(PFile p_file, signed long int offset, u8 fromwhere);

	u8 fclose(PFile p_file);

	u8 ChangeDir(u8* path, u8 len, PFile p_dir);

	u8 MoveToFirstFile(PFile p_dir);
	u8 MoveToNextFile(PFile p_dir);
	u8 GetShortFileNameX(PFile p_dir, u8* buf);

}

/*
在外部可以对子目录属性和非子目录属性作区分，以便用户使用

可以利用不透明指针来封闭File结构体。
*/
#endif			

