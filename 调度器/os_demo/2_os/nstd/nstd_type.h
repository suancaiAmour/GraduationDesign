#pragma once

#ifndef TYPE_H
#define TYPE_H

/** @file nstd_type.h
  * @brief 在这个文件里定义了一些基本的类型, 以及类型操纵的一些
  * 基本模板.
  */

#include <stddef.h>
#include <stdint.h>
#include "define.h"

/** @defgroup nstd
  * @{
  */
namespace nstd{

	/** @defgroup type
	  * @ingroup nstd
	  * @{
	  */

	typedef uint32_t  u32;  /**< 无符号 32 位整数 */
	typedef uint16_t u16;   /**< 无符号 16 位整数 */
	typedef uint8_t  u8;    /**< 无符号  8 位整数 */

	typedef int32_t  s32;   /**< 有符号 32 位整数 */
	typedef int16_t s16;    /**< 有符号 16 位整数 */
	typedef int8_t  s8;     /**< 有符号  8 位整数 */

	/**	@enum Result
	  * @brief 通用的函数调用结果
	  * @note 本库为这个枚举重载了 == 操作符，对于所有不等于 SUCCEEDED 的
	  * 值，例如 Result a = (Result)3 ，那么 a == FAILED 将会返回真，即使
	  *	它们在整数数值上是不相等的。即一切不成功都意味着失败。
	  */
	typedef enum {
			SUCCEEDED = 0	/**< 函数处理成功 */
			, FAILED = !SUCCEEDED	/**< 函数处理失败 */
	} Result;
	inline bool operator ==(Result a, Result b)
	{
		return
			((int)a ==(int)b)||
			(((int)a !=(int)SUCCEEDED)&&((int)b !=(int)SUCCEEDED));
	}

	/**	@enum Polarity
	  *	@brief 用来表示极性的枚举类
	  */
	typedef enum {
		NEGATIVE = 0	/**< 负极性 */
		, POSITIVE = !NEGATIVE /**< 正极性 */
	}Polarity;

	#if(HOST_ENDIAN == BIG_ENDIAN)	//大端字节序
		#pragma pack(push)	//保存对齐状态
		#pragma pack(1)

		typedef struct LittleEndianU16 leu16;
		typedef struct LittleEndianU32 leu32;
		/**	@brief 在内存中以小端字节序形式存储的 32位 无符号整数
		*/
		struct LittleEndianU32
		{
			u8 ll;	/**< 低16位低8位 */
			u8 lh;	/**< 低16位高8位 */
			u8 hl;	/**< 高16位低8位 */
			u8 hh;	/**< 高16位高8位 */
			finline LittleEndianU32(){}
			finline LittleEndianU32(leu16 a);
			finline LittleEndianU32(u32 a)		{*this = a;}
			finline operator u32()
			{
				return ( ((u32)ll) | (((u32)lh)<<8) |
					(((u32)hl)<<16)|(((u32)hh)<<24) );
			}
			finline LittleEndianU32 & operator =(u32 a)	{
				ll = (u8)a; lh = (u8)(a>>8);
				hl = (u8)(a>>16);
				hh = (u8)(a>>24);
				return *this;
			}
		};
		/**	@brief 在内存中以小端字节序形式存储的 16位 无符号整数
		*/
		struct LittleEndianU16
		{
			u8 l;	/**< 低 8 位 */
			u8 h;	/**< 高 8 位 */
			finline LittleEndianU16(){}
			finline LittleEndianU16(leu32 a)	{*this = (u16)((u32)a);}
			finline LittleEndianU16(u16 a)		{*this = a;}
			finline operator u16()	{ return (((u16)l) | (((u16)h)<<8));}
			finline LittleEndianU16& operator = (u16 a)
					{ l=(u8)a; h=(u8)(a>>8); return *this;}
		};

		finline
		LittleEndianU32::LittleEndianU32(leu16 a) {*this = (u32)((u16)a);}

		typedef u32 beu32;
		typedef u32 beu32;

		#pragma pack(pop)	//恢复对齐状态

	#else
		#pragma pack(push)	//保存对齐状态
		#pragma pack(1)

		typedef u32 leu32;
		typedef u16 leu16;

		typedef struct BigEndianU32 beu32;
		typedef struct BigEndianU16 beu16;
		/**	@brief 在内存中以大端字节序形式存储的 32位 无符号整数
		*/
		struct BigEndianU32
		{
			u8 hh;	// 高16位高8位
			u8 hl;	// 高16位低8位
			u8 lh;	// 低16位高8位
			u8 ll;	// 低16位低8位
			finline BigEndianU32(){}
			finline BigEndianU32(beu16 a);
			finline BigEndianU32(u32 a)		{*this = a;}
			finline operator u32()
			{
				return ( ((u32)ll)|(((u32)lh)<<8)|
						(((u32)hl)<<16)|(((u32)hh)<<24) );
			}
			finline
			BigEndianU32 & operator =(u32 a)
			{
				ll = (u8)a; lh = (u8)(a>>8);
				hl = (u8)(a>>16);
				hh = (u8)(a>>24);
				return *this;
			}
		};
		/**	@brief 在内存中以小端字节序形式存储的 16位 无符号整数
		*/
		struct BigEndianU16
		{
			u8 h;
			u8 l;
			finline BigEndianU16(){}
			finline BigEndianU16(beu32 a)		{*this = (u16)((u32)a);}
			finline BigEndianU16(u16 a)			{*this = a;}
			finline operator u16()	{ return (((u16)l) | (((u16)h)<<8));}
			finline BigEndianU16& operator = (u16 a)
					{ l=(u8)a; h=(u8)(a>>8); return *this;}
		};
		finline
		BigEndianU32::BigEndianU32(beu16 a) {*this = (u32)((u16)a);}

		#pragma pack(pop)	//恢复对齐状态

	#endif


	/** @brief 这个模板用来判断一个类型是不是 class,
      * 或者 struct 或者 union. 例如:
      * @code
      * is_class_type<char>::result == false
      * @endcode
	  */
	template<typename T>
	class is_class_type{
		private:
		typedef char SizeOne;
		typedef struct { char a[2]; } SizeTwo;
		template<typename C> static SizeOne test(int C::*);
		template<typename C> static SizeTwo test(...);
		public:
		enum { result = sizeof(test<T>(0)) == 1
                ,value = result};
	};

/** @def TYPE_SELECTOR_CREATE(if_type, else_type, selector)
  * @brief 创建类型选择器, 和 TYPE_SELECTOR_RESULT 配套使用. 这个类型
  * 选择器的优点在于在创建类型选择器的时候不需要真正的实例化两个
  * 类型, 而是在使用 TYPE_SELECTOR_RESULT 得到结果的时候, 只实例化结
  * 果类型. 用法如下:
  * @code
  * TYPE_SELECTOR_CREATE(int, bool, sel); // sel 为类型选择器的名字
  * TYPE_SELECTOR_RESULT(sel, true) i; // 即 int i
  * TYPE_SELECTOR_RESULT(sel, false) i; // 即 bool j
  * @endcode
  * @param if_type 选择参数为真的时候的结果类型.
  * @param else_type 选择参数为假的时候的结果类型.
  * @param selector 类型选择器的名字.
  */
#define TYPE_SELECTOR_CREATE(if_type, else_type, selector) \
template <bool , typename selector_dummy_type=void>        \
struct selector{ typedef if_type type; };                  \
template <typename selector_dummy_type>                    \
struct selector<false,selector_dummy_type>{ typedef else_type type; }

/** @def TYPE_SELECTOR_RESULT(selector, b)
  * @brief 用通过 TYPE_SELECTOR_CREATE 创建的类型选择器来选择类型. 更
  * 多的信息请参考 @ref TYPE_SELECTOR_CREATE.
  * @param selector 类型选择器的名字.
  * @param b 选择参数. 注意这个参数必须是编译期常量.
  */
#define TYPE_SELECTOR_RESULT(selector, b)  selector<b>::type

    /** @brief 类型表, 用于类型选择.
      * 例如:
      * @code
      * type_list<char, bool, int>::at<0>::type i; //即 char i;
      * type_list<char, bool, int>::at<1>::type j; //即 bool j;
      * type_list<char, bool, int>::at<2>::type k; //即 int k;
      * @endcode
      */
    template<typename T0, typename T1
            , typename T2 = void, typename T3 = void
            , typename T4 = void, typename T5 = void
            , typename T6=void, typename T7 = void>
    struct type_list{
        typedef T0 type0;
        typedef T1 type1;
        typedef T2 type2;
        typedef T3 type3;
        typedef T4 type4;
        typedef T5 type5;
        typedef T6 type6;
        typedef T7 type7;

        template <int i, typename T=void>
        struct at{ };

        /* 在类中不支持完全特化, 所以使用偏特化, 由于有默认参数,
          所以相应于完全特化. */
        template <typename T>
        struct at<0,T>{ typedef type0 type;};
        template <typename T>
        struct at<1,T>{ typedef type1 type;};
        template <typename T>
        struct at<2,T>{ typedef type2 type;};
        template <typename T>
        struct at<3,T>{ typedef type3 type;};
        template <typename T>
        struct at<4,T>{ typedef type4 type;};
        template <typename T>
        struct at<5,T>{ typedef type5 type;};
        template <typename T>
        struct at<6,T>{ typedef type6 type;};
        template <typename T>
        struct at<7,T>{ typedef type7 type;};
    }; /* template struct type_list */

	// group type
	/**
	  * @}
	  */

} /* namespace nstd */

// group nstd
/**
  * @}
  */

using nstd::u32;
using nstd::u16;
using nstd::u8;

using nstd::s32;
using nstd::s16;
using nstd::s8;


#endif
