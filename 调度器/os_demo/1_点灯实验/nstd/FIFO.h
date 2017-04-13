#ifndef __cplusplus
#define FIFO_H
#endif
#ifndef FIFO_H
#define FIFO_H
#include "nstd_type.h"

namespace nstd{

//不足：
//	如果队列满了，后面任何写入的操作都会被忽略
//	如果队列为空，后面任何读取的操作都会被忽略，并且没有通知用户
//改进方案：
//	可以增加一个错误的处理函数，以函数指针的形式作为成员变量，当发生错误的时候，调用这个函数。
//	FIFO队列
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
	// 队列的constructor，对象一旦构造，大小就不能再改变了
	FIFO(T* buf,unsigned int max):buffer(buf), rIndex(0), wIndex(0), maxSize(max), curSize(0){}
	
	size_t finline GetCurSize() const	{return curSize;}
	size_t finline GetMaxSize()	const {return maxSize;}
	finline bool IsFull()	const {return curSize == maxSize;}
	finline bool IsEmpty()	const {return curSize == 0;}

	//	如果队列满了，后面任何写入的操作都会被忽略
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
	//	如果队列空了，后面任何读的操作都会被忽略
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
