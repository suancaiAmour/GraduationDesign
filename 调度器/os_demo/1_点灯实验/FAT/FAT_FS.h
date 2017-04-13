#ifndef FAT_FS_H
#define FAT_FS_H

#include "sys.h"
namespace nstd{

	//˵����
	//	���ļ��ṹ��Ķ��壬�ⲿֻ��Ҫ�õ��ļ��ṹ���ָ�룬���Բ���Ҫ����FileInfo�ṹ��ľ��嶨��
	//	��Щ�������warning �������ӣ��ṹ��FileInfo�Ѿ����ļ�ϵͳ�ڲ�������
	//	..\main\main.c(58): warning C230: 'File': uses undefined struct/union tag 'FileInfo'
	//	ʹ��xdata���Խ�ʡ�ڴ�ռ��
	typedef struct FileInfo xdata File;
	typedef File*		PFile;

	/**	@brief ��д���棬fread���������ݾ������� */
	#define BUFF_SIZE 512
	extern unsigned char xdata RWbuffer[BUFF_SIZE];

	u8 FileSysInit();

	//���ļ�ģʽ����
	#define FILE_READ			Bit(0)		//00000001B
	#define FILE_WRITE 			Bit(1)		//00000010B
	#define FILE_CREATE			Bit(2)		//00000100B	��д��ʱ������ļ������ڣ��򴴽��ļ�
	//#define FILE_ALWAYS_CREATE	((u8)BIT(3))	//���Ǵ�������ʱ��֧�����
	PFile fopen(const u8* name_x,u8 mode);

	//�ļ��ָ���
	#define FILE_SEPARATOR	('\\')

	#define FR_ERROR	0xffff		//�ļ�������
	u16 fread(PFile p_file,u8* buffer,u16 size);	//Ĭ�϶���read buffer�У���RWbuffer
	#define FW_ERROR	0xffff		//�ļ�д����
	u16 fwrite(PFile p_file,const u8* buffer,u16 size);

	#define SEEK_SET	0		//�ļ���ͷ
	#define SEEK_CUR	1		//��ǰλ��
	#define SEEK_END	2		//�ļ�β
	u8 fseek(PFile p_file, signed long int offset, u8 fromwhere);

	u8 fclose(PFile p_file);

	u8 ChangeDir(u8* path, u8 len, PFile p_dir);

	u8 MoveToFirstFile(PFile p_dir);
	u8 MoveToNextFile(PFile p_dir);
	u8 GetShortFileNameX(PFile p_dir, u8* buf);

}

/*
���ⲿ���Զ���Ŀ¼���Ժͷ���Ŀ¼���������֣��Ա��û�ʹ��

�������ò�͸��ָ�������File�ṹ�塣
*/
#endif			

