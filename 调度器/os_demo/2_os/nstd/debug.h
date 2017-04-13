#ifndef __cplusplus
#define DEBUG_H
#endif
#ifndef DEBUG_H
#define DEBUG_H

/**	@brief ���ͷ�ļ����Ը����Գ���������㣬
	1. ����Ҫ����ĳ�� cpp �ļ��еĴ���ʱ�����԰������ͷ�ļ���
	����ʹ�õ���غ������Բο�����ļ��е� #ifdef MYDEBUG ... #else ... #endif ���������ݡ�
	2. �ڸ��� cpp �ļ�������ʱ�������ڰ������ͷ�ļ�֮ǰ���� N0_DEBUG ��
	3. ������ cpp �ļ���������ʱ�����������ͷ�ļ��ж��� N0_DEBUG ��
*/

// NDEBUG ���������C��׼����������
#ifdef NDEBUG
#define NO_DEBUG
#endif

#ifndef N0_DEBUG
#define MYDEBUG
#endif

#include "iostream.h"

#include "nstd_assert.H"

/** @brief �ڲ����Ե�ʱ�򣬰� dout ����� NoDebugOutput ���������е� dout ����ɿ���� */
struct NoDebugOutput{
	template<typename T>
	tinline NoDebugOutput& operator <<(const T& ) {return *this;}	/* ʲôҲ���� */
};

/**	@brief ��������� MYDEBUG �꣬˵���������ڵ���ģʽ�±��룬
	�Ϳ��Է�����������ﶨ��ĺ������������Ϣ��
	@note �� #ifdef �е�ÿһ���궨�壬�� #else �б���Ҫ��һ����Ӧ�ĺ궨�壬
	#else �еĺ궨�����ڷǵ���ģʽ��ʹ�õ�
*/
#ifdef MYDEBUG
	#define DB_Delay_ms(x)		(Delay_ms(x))
	#define DB_SendASC(x)
	#define DB_SendString		nstd::output
	#define DB_SendHex(x)		
	#define DB_SendBlock(x,y)	
	#define DB_SendDec(x)		(nstd::output<<x<<nstd::endl)
	#define DB_SendUnicode(x)	
	void Debug_Init();
	
	/**	@brief debug output */
	#define dout	nstd::output
	
#else
	/* ֱ�Ӷ���Ϊ�ռ��� */
	#define DB_UartInit()
	#define DB_SendASC(x)
	#define DB_SendString(x)
	#define DB_SendHex(x)
	#define DB_SendBlock(x,y)
	#define DB_SendDec(x)
	#define DB_SendUnicode(x)
	#define Debug_Init()
	
	extern NoDebugOutput dout;
#endif

#endif
