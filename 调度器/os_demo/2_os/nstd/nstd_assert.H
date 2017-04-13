#ifndef NSTD_ASSERT_H
#define NSTD_ASSERT_H



/**	@def DYNAMIC_ASSERT(expr)
  * @brief: 利用这个宏可以进行运行时断言. 在很多时候为了简化开发和
  * 方便调试常常会进行很多的假设, 如下面这函数的定义
  * @code
  *	int Div(int a, int b) { return a/b;}
  * @endcode
  *	显然逻辑上 b 是不可能为 0 的, 但是, 如果用户使用不当, 调用了 
  * Div(1,0), 那么在执行时就会导致计算机出错，甚至程序跑飞，而且有时
  * 候这一类运行时的逻辑错误是不容易检查出来的，所以可以把函数改为
  * @code
  *	int Div(int a, int b) 
  *		{DYNAMIC_ASSERT(b); return a/b;}
  * @endcode
  *	这样，当用户调用 Div(1,0) 执行时，就会打印出出错的信息，方便了调试.
  *	当然了，在运行时检查是占用 CPU 时间的，所以，当程序调试完毕的时候, 
  * 可以在包含这个头文件之前宏定义一个 NO_DEBUG, 这样, 所有的
  * DYNAMIC_ASSERT(x) 语句都会被除去，不再占用 CPU 时间。
  */

#ifndef NDEBUG
	void nstd_AssertFailed(char* exp, char* file , int line);
	/* 定义自己的 assert */
	#undef	assert
	#define assert(exp)	{if(!(exp))nstd_AssertFailed(#exp, __FILE__, __LINE__);}
	
	#undef	DYNAMIC_ASSERT
	#define DYNAMIC_ASSERT(exp)		\
			{if(!(exp))nstd_AssertFailed(#exp, __FILE__, __LINE__);}
#else
	/* 定义为空，直接忽略 */
	#undef assert
	#define assert(x)

	#undef DYNAMIC_ASSERT
	#define DYNAMIC_ASSERT(x)
#endif
	
	#include "nstd_static_assert.h"

#endif 
