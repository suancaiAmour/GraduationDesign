#ifndef IOSTREAM_H
#define IOSTREAM_H

#include "define.h"

#include "FIFO.h"
#include "utilities.h"

namespace nstd{

/**	@brief	输出流, T是输出流的缓冲区的基本数据类型，
*			而 SEND_T 是实现数据发送的类，需要实现静态函?Send(T t), 发送一个数据。
*/
// SEND_T 类提供直接发送数据到输出设备的函数 Send
template<class T>
class OStream
{
protected:
    virtual int SendChar(T ch) = 0;
    virtual int Flush(){ return 0; }
public:
	/**	@brief	输出流的构造函数 */
	OStream(){}
	void Init(){}
};

/**	@brief	文本输出流，以char作为缓冲区的基本数据类型。
*/
class TextOutStream: public OStream<char>
{
protected:
    virtual int SendChar(char ch);
public:
    TextOutStream(){}

	TextOutStream& operator <<(const char& ch)   {SendChar(ch); return *this;}
	TextOutStream& operator <<(unsigned char ch) {SendChar(ch); return *this;}
    
    /** @brief 输出字符串
      * @param str 字符串地址, 以 0 为结束符
      */
	TextOutStream& operator <<(const char* str);
    
     /** @brief 输出字符串
      * @param str 字符串地址, 以 0 为结束符
      */
	TextOutStream& operator <<(const unsigned char* str)	{return *this<<(char*)str;}
    
    /** @brief 输出指针的内容, 即地址 */
	TextOutStream& operator <<(const void* const p);
    
    
	TextOutStream& operator <<(u16 number)	{ return *this<<(u32)number; }
	TextOutStream& operator <<(u32 number);
	
	void operator ()(const char* str)	{*this<<str;}
	
	TextOutStream& operator <<(void (*manipulator)(TextOutStream& t)){
		(*manipulator)(*this);
		return *this;
	}
};

inline
void endl(TextOutStream& t){
	t<<"\r\n";	// 回车换行
	return;	
}

/*************************以下是基于串口的输出流的实现************************/
/**	@brief	基于串口的输出流的实现。
*/
extern TextOutStream output;

/*************************以上是基于串口的输出流的实现************************/

//	输入流
template<class T>
class IStream
{
protected:
    
	FIFO<T>* fifo;

	/**	@brief	从FIFO缓冲区中读字符，如果缓冲区是空的，那么就会一直等待
	*	@note	参数t 如果不使用引用类型的话，对于比较大的数据类型，效率就不好了，这样是为了保持通用性。
	*/
	finline void Read(T& t)	
	{
		while(fifo->IsEmpty());
		*fifo>>t;
	}
public:
    
	/**	@brief	输入流的构造函数，需要一个FIFO对象来作为输出流的缓冲区。
	*	该FIFO 对象需要由外部写入数据，文件输入流才能读到数据。
	*/
	IStream( FIFO<T>& q):fifo(&q)	{}

	IStream(){}
	
	void Init(FIFO<T>& q){fifo = &q;}

	finline IStream& operator >>(T& t)	{
		Read(t);
		return *this;
	}
};

//	文本输入流
class TextInStream:  public IStream<char>
{
protected:
	finline void Read(char& c)	{IStream<char>::Read(c);}
public:
    
    /** @brief 默认构造函数 */
    TextInStream(){}
        
	finline TextInStream( FIFO<char>& q):IStream<char>(q){}
    
	TextInStream& operator >>(char& c)	{
		Read(c); 
		return *this;
	}
	/**	@brief	读一个字符串
	*	@note	这个函数会吃掉输入流中前面的所有连续空格字符，以及下一个空格字符
	*/
	TextInStream& operator >>(char* str)
	{
		char temp = 0;
		do{
			Read(temp);	
		}while(IsSpaceChar(temp));
		*str = temp;
		str++;
		do{
			Read(*str);
		}while(!IsSpaceChar(*str++));
		str--;
		*str = 0;
		return *this;
	}
};

#ifdef SEND_CHAR_CLASS
/*************************以下是基于串口的输入流的实现************************/
/**	@brief	输入流的缓冲区。接收到的数据都会临时存储到缓冲区。
*/
extern FIFO<char> textInFIFO;
/**	@brief	基于串口的输出流的实现。
*/
extern TextInStream input;

/*************************以上是基于串口的输入流的实现************************/

void IOStream_Init();

#endif
}

#endif
