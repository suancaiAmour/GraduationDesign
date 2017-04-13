#ifndef IOSTREAM_H
#define IOSTREAM_H

#include "define.h"

#include "FIFO.h"
#include "utilities.h"

namespace nstd{

/**	@brief	�����, T��������Ļ������Ļ����������ͣ�
*			�� SEND_T ��ʵ�����ݷ��͵��࣬��Ҫʵ�־�̬��?Send(T t), ����һ�����ݡ�
*/
// SEND_T ���ṩֱ�ӷ������ݵ�����豸�ĺ��� Send
template<class T>
class OStream
{
protected:
    virtual int SendChar(T ch) = 0;
    virtual int Flush(){ return 0; }
public:
	/**	@brief	������Ĺ��캯�� */
	OStream(){}
	void Init(){}
};

/**	@brief	�ı����������char��Ϊ�������Ļ����������͡�
*/
class TextOutStream: public OStream<char>
{
protected:
    virtual int SendChar(char ch);
public:
    TextOutStream(){}

	TextOutStream& operator <<(const char& ch)   {SendChar(ch); return *this;}
	TextOutStream& operator <<(unsigned char ch) {SendChar(ch); return *this;}
    
    /** @brief ����ַ���
      * @param str �ַ�����ַ, �� 0 Ϊ������
      */
	TextOutStream& operator <<(const char* str);
    
     /** @brief ����ַ���
      * @param str �ַ�����ַ, �� 0 Ϊ������
      */
	TextOutStream& operator <<(const unsigned char* str)	{return *this<<(char*)str;}
    
    /** @brief ���ָ�������, ����ַ */
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
	t<<"\r\n";	// �س�����
	return;	
}

/*************************�����ǻ��ڴ��ڵ��������ʵ��************************/
/**	@brief	���ڴ��ڵ��������ʵ�֡�
*/
extern TextOutStream output;

/*************************�����ǻ��ڴ��ڵ��������ʵ��************************/

//	������
template<class T>
class IStream
{
protected:
    
	FIFO<T>* fifo;

	/**	@brief	��FIFO�������ж��ַ�������������ǿյģ���ô�ͻ�һֱ�ȴ�
	*	@note	����t �����ʹ���������͵Ļ������ڱȽϴ���������ͣ�Ч�ʾͲ����ˣ�������Ϊ�˱���ͨ���ԡ�
	*/
	finline void Read(T& t)	
	{
		while(fifo->IsEmpty());
		*fifo>>t;
	}
public:
    
	/**	@brief	�������Ĺ��캯������Ҫһ��FIFO��������Ϊ������Ļ�������
	*	��FIFO ������Ҫ���ⲿд�����ݣ��ļ����������ܶ������ݡ�
	*/
	IStream( FIFO<T>& q):fifo(&q)	{}

	IStream(){}
	
	void Init(FIFO<T>& q){fifo = &q;}

	finline IStream& operator >>(T& t)	{
		Read(t);
		return *this;
	}
};

//	�ı�������
class TextInStream:  public IStream<char>
{
protected:
	finline void Read(char& c)	{IStream<char>::Read(c);}
public:
    
    /** @brief Ĭ�Ϲ��캯�� */
    TextInStream(){}
        
	finline TextInStream( FIFO<char>& q):IStream<char>(q){}
    
	TextInStream& operator >>(char& c)	{
		Read(c); 
		return *this;
	}
	/**	@brief	��һ���ַ���
	*	@note	���������Ե���������ǰ������������ո��ַ����Լ���һ���ո��ַ�
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
/*************************�����ǻ��ڴ��ڵ���������ʵ��************************/
/**	@brief	�������Ļ����������յ������ݶ�����ʱ�洢����������
*/
extern FIFO<char> textInFIFO;
/**	@brief	���ڴ��ڵ��������ʵ�֡�
*/
extern TextInStream input;

/*************************�����ǻ��ڴ��ڵ���������ʵ��************************/

void IOStream_Init();

#endif
}

#endif
