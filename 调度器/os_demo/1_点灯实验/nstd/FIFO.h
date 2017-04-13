#ifndef __cplusplus
#define FIFO_H
#endif
#ifndef FIFO_H
#define FIFO_H
#include "nstd_type.h"

namespace nstd{

//���㣺
//	����������ˣ������κ�д��Ĳ������ᱻ����
//	�������Ϊ�գ������κζ�ȡ�Ĳ������ᱻ���ԣ�����û��֪ͨ�û�
//�Ľ�������
//	��������һ������Ĵ��������Ժ���ָ�����ʽ��Ϊ��Ա�����������������ʱ�򣬵������������
//	FIFO����
template<class T>
class FIFO
{
protected:
	volatile T* buffer;
	volatile size_t rIndex;
	volatile size_t wIndex;
	size_t maxSize;
	volatile size_t curSize;
public:
	FIFO(){}
	void Init(T* buf, unsigned int max){
		buffer=buf, rIndex=0, wIndex=0, maxSize=max, curSize=0;
	}
	// ���е�constructor������һ�����죬��С�Ͳ����ٸı���
	FIFO(T* buf,unsigned int max):buffer(buf), rIndex(0), wIndex(0), maxSize(max), curSize(0){}
	
	size_t finline GetCurSize() const	{return curSize;}
	size_t finline GetMaxSize()	const {return maxSize;}
	finline bool IsFull()	const {return curSize == maxSize;}
	finline bool IsEmpty()	const {return curSize == 0;}

	//	����������ˣ������κ�д��Ĳ������ᱻ����
	finline FIFO& operator << (T c)
	{
		if(!IsFull())
		{
			curSize++;
			buffer[wIndex++] = c;
			wIndex %= maxSize;
		}
		return *this;
	}
	//	������п��ˣ������κζ��Ĳ������ᱻ����
	finline FIFO& operator >> (T& c)
	{
		if(!IsEmpty())
		{
			curSize--;
			c = buffer[rIndex++];
			rIndex %= maxSize; 
		}
		return *this;
	}
	void finline Clean()		{rIndex = wIndex = curSize = 0;}
};

}

#endif
