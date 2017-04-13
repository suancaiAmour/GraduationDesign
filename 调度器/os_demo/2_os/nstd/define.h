#pragma once
#ifndef DEFINE_H
#define DEFINE_H

/**	@brief 这个头文件里定义了一些用户关键字。 */

// 51移植来的程序，xdata定义为空
#define xdata
#define idata

#include <stddef.h>

/** @def finline
  * 定义函数为强制内联.
  * 普通的 inline 关键字只是建议编译器内联，
  * 但是有时候编译器会无视建议的内联导致效率低下，
  * 使用强制内联可以解决这个问题 
  */
#if defined ( __CC_ARM   )
    #define finline		__forceinline
#else
    #define finline		inline
#endif

/** @def tinline
  * inline template function. 有些编译器不支持给 template 声明 inline,
  * 这个时候就需要重新定义 tinline 为空.  
  */
#define tinline		finline

/** @def pure_function
  * @brief 用来声明一个函数是纯函数, 即没有任何负作用, 一个参数对应
  * 唯一的返回值.
  */
#define pure_function	__pure

/* cpp文件中的中断函数的定义需要加上这个声明，
中断时才会进入该中断函数 */
#define ARMAPI	extern "C"

/** @def CAPI
  * @brief 用于声明一个函数是可以给 C 语言调用的.
  */
#define CAPI    extern "C"

#define BIG_ENDIAN 1        /**< 大端 */

#define LITTLE_ENDIAN 0     /**< 小端 */


#ifdef __BIG_ENDIAN
    // 大端字节序
    #define HOST_ENDIAN		BIG_ENDIAN
#else
    // 小端字节序
    #define HOST_ENDIAN		LITTLE_ENDIAN
#endif

#endif
