#ifndef __cplusplus
#define DEBUG_H
#endif
#ifndef DEBUG_H
#define DEBUG_H

/**	@brief 这个头文件可以给调试程序带来方便，
	1. 在需要调试某个 cpp 文件中的代码时，可以包含这个头文件，
	调试使用的相关函数可以参看这个文件中的 #ifdef MYDEBUG ... #else ... #endif 代码块的内容。
	2. 在个别 cpp 文件不调试时，可以在包含这个头文件之前定义 N0_DEBUG 宏
	3. 在所有 cpp 文件都不调试时，可以在这个头文件中定义 N0_DEBUG 宏
*/

// NDEBUG 这个符号在C标准库中有描述
#ifdef NDEBUG
#define NO_DEBUG
#endif

#ifndef N0_DEBUG
#define MYDEBUG
#endif

#include "iostream.h"

#include "nstd_assert.H"

/** @brief 在不调试的时候，把 dout 定义成 NoDebugOutput 类型让所有的 dout 语句变成空语句 */
struct NoDebugOutput{
	template<typename T>
	tinline NoDebugOutput& operator <<(const T& ) {return *this;}	/* 什么也不做 */
};

/**	@brief 如果定义了 MYDEBUG 宏，说明程序是在调试模式下编译，
	就可以方便的利用这里定义的宏来输出调试信息。
	@note 在 #ifdef 中的每一个宏定义，在 #else 中必需要有一个对应的宏定义，
	#else 中的宏定义是在非调试模式下使用的
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
	/* 直接定义为空即可 */
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
