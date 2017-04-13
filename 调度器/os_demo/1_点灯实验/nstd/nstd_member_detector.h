#ifndef NSTD_MEMBER_DETECTOR_H
#define NSTD_MEMBER_DETECTOR_H

////////////////////////////////////////////////////////////////////////
// Original author: Alexandre Courpron
// from: http://www.codeproject.com/Articles/19444/Interface-Detection
// Modified by: Pony279
// 修改内容:
// 1. 增加 enum 的类型名字 DetectResult
// 2. 把 DetectMember_##Identifier 的结果改名为 value, 并从 enum 改为
// static const DetectResult, 转为强类型会好一些
// 3. 去掉了 CREATE_FUNCTION_DETECTOR 和 CREATE_DATA_DETECTOR 宏定义里
// 的命名空间包围
// 4. 引入 nstd 库, 所以修改了命名空间
// 5. 去掉了 CREATE_FUNCTION_DETECTOR 和 CREATE_DATA_DETECTOR
// 宏最后面的分号 ';'
// Thanks to Alexandre Courpron!
// ...
// 6. 由于 FUNCTION_DETECTOR 和 DATA_DETECTOR 都不能检测基类的成员
// 弃用了, 改用 MEMBER_DETECTOR , 参考自
// http://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Member_Detector
// 并做了些修改, 增加了非 classtype 识别以免产生错误
//
////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Copyright 2007 Alexandre Courpron
//
// Permission to use, copy, modify, redistribute and sell this software,
// provided that this copyright notice appears on all copies of the software.
///////////////////////////////////////////////////////////////////////////////

#include "nstd_type.h"
#include "nstd_static_assert.h"

/** @addtogroup nstd
  * @{
  */
namespace nstd{

	/** @addtogroup utilities
		* @{
		*/

    namespace Detector {
        typedef char NotFound;
        struct StaticFunction { NotFound x [2]; };
        struct NonStaticFunction { NotFound x [3]; };
        struct StaticData { NotFound x [4]; };
        struct NonStaticData { NotFound x [5]; };

		/**	@brief 类成员检测器的检测结果.
		  * 可参考 @ref DETECT_FUNCTION 和 @ref DETECT_DATA 宏.
		  */
        enum DetectResult{
			  NOT_FOUND = 0 /**< 没有找到指定的成员 */
			, STATIC_FUNCTION /**< 找到对应的静态函数成员 */
				= sizeof( StaticFunction ) - sizeof( NotFound )
			, NON_STATIC_FUNCTION /**< 找到了对应的非静态函数成员 */
				= sizeof( NonStaticFunction ) - sizeof( NotFound )
			, STATIC_DATA /**< 找到了对应的静态数据成员 */
				= sizeof( StaticData ) - sizeof( NotFound )
			, NON_STATIC_DATA /**< 找到了对应的非静态数据成员 */
				= sizeof( NonStaticData ) - sizeof( NotFound ) };




/** @def CREATE_FUNCTION_DETECTOR(Identifier)
  * @brief 创建<类/结构体函数成员检测器>.
  * 详细内容参考 @ref DETECT_FUNCTION 宏
  * @param Identifier 类/结构体的函数成员的标识符
  */
#define CREATE_FUNCTION_DETECTOR(Identifier) \
    template < class T, \
               class NonStaticType, \
               class NonStaticConstType, \
               class StaticType > \
    class DetectMember_##Identifier { \
        template < NonStaticType > \
        struct TestNonStaticNonConst ; \
        \
        template < NonStaticConstType > \
        struct TestNonStaticConst ; \
        \
        template < StaticType > \
        struct TestStatic ; \
        \
        template <class U > \
        static nstd::Detector::NonStaticFunction Test( TestNonStaticNonConst<&U::Identifier>*, ... ); \
        \
        template <class U > \
        static nstd::Detector::NonStaticFunction Test( TestNonStaticConst<&U::Identifier>*, int ); \
        \
        template <class U> \
        static nstd::Detector::StaticFunction Test( TestStatic<&U::Identifier>*, int ); \
        \
        template <class U> \
        static nstd::Detector::NotFound Test( ... ); \
    public : \
        static const nstd::Detector::DetectResult value = \
            (nstd::Detector::DetectResult)(sizeof( Test<T>( 0, 0 ) ) - sizeof( nstd::Detector::NotFound ));  \
    }

/** @def DETECT_FUNCTION(Class,ReturnType,Identifier,Params)
  * @brief 得到<类/结构体函数成员检测器>对指定成员的存在性的检测结果.
  * @note 在使用之前, 必须使用宏 CREATE_FUNCTION_DETECTOR 来创建一个类/结构
  * 体成员检测器.
  * @param Class 检查的类型
  *	@param ReturnType 函数成员的返回类型
  * @param Identifier 函数成员的标识符
  * @param Params 函数成员的参数列表, 如 (void)
  *		@note 需要写上括号 "()"
  * @return 返回值为 @ref DetectResult 类型的编译期常量. 这个宏只有有
  * NOT_FOUND, STATIC_FUNCTION, NON_STATIC_FUNCTION 三种返回值.
  * @note 这个检测器不可用于检测基类成员
  * @example nstd_detector_example.cpp
  */
#define DETECT_FUNCTION(Class,ReturnType,Identifier,Params) \
    DetectMember_##Identifier< Class,\
                                         ReturnType (Class::*)Params,\
                                         ReturnType (Class::*)Params const,\
                                         ReturnType (*)Params \
                                       >::value

/** @def CREATE_DATA_DETECTOR(Identifier)
  * @brief 创建一个<类/结构体数据成员检测器>.
  * 详细内容参考 @ref DETECT_DATA 宏
  * @param Identifier 类/结构体的数据成员的标识符
  */
#define CREATE_DATA_DETECTOR(Identifier) \
    template < class T, \
               class NonStaticType, \
               class StaticType > \
    class DetectMember_##Identifier { \
        template < NonStaticType > \
        struct TestNonStatic ; \
        \
        template < StaticType > \
        struct TestStatic ; \
        \
        template <class U > \
        static nstd::Detector::NonStaticData Test( TestNonStatic<&U::Identifier>* ); \
        \
        template <class U> \
        static nstd::Detector::StaticData Test( TestStatic<&U::Identifier>* ); \
        \
        template <class U> \
        static nstd::Detector::NotFound Test( ... ); \
    public : \
        static const nstd::Detector::DetectResult value = (nstd::Detector::DetectResult)(sizeof( Test<T>( 0 ) ) - sizeof( nstd::Detector::NotFound ));  \
    }

/** @def DETECT_DATA(Class,Type,Identifier)
  * @brief 得到<类/结构体数据成员检测器>对指定成员的存在性的检测结果.
  * @note 在使用之前, 必须使用宏 CREATE_DATA_DETECTOR 来创建一个类/结构
  * 体成员检测器.
  * @param Class 检查的类型
  *	@param Type 数据成员的返回类型
  * @param Identifier 数据成员的标识符
  * @return 返回值为 @ref DetectResult 类型的编译期常量. 这个宏只有
  * NOT_FOUND, STATIC_DATA, NON_STATIC_DATA 三种返回值.
  * @note 这个检测器不可用于检测基类成员
  * @example nstd_detector_example.cpp
  */
#define DETECT_DATA(Class,Type,Identifier) \
    DetectMember_##Identifier< Class,\
                                         Type (Class::*),\
                                         Type (*)\
                                       >::value


/** @def CREATE_MEMBER_DETECTOR(X)
  * @brief 用于创建类成员检测器, 用于检测是否有指定名字的成员
  * 其工作原理是产生名字冲突, 再利用 SFINAE 原则让编译器去选择
  * 匹配的结果.
  */
#define CREATE_MEMBER_DETECTOR(X)                                                   \
template<typename T##X> class Detect_##X {                                             \
    struct Fallback { int X; };                                                     \
    struct Derived : T##X, Fallback { };                                               \
                                                                                    \
    template<typename U, U> struct Check;                                           \
                                                                                    \
    typedef char ArrayOfOne[1];                                                     \
    typedef char ArrayOfTwo[2];                                                     \
                                                                                    \
    template<typename U> static ArrayOfOne & func(Check<int Fallback::*, &U::X> *); \
    template<typename U> static ArrayOfTwo & func(...);                             \
  public:                                                                           \
    typedef Detect_##X type;                                                        \
    enum { value = sizeof(func<Derived>(0)) == 2 };                                 \
}

#undef CREATE_MEMBER_DETECTOR
/** 这个 member detector 对于 int 等内建的类型也能工作 */
#define CREATE_MEMBER_DETECTOR(X)					\
template<typename T##X> class Detect_##X {			\
    struct Fallback { int X; };          			\
    template <typename TT##X, int > 				\
    struct DerivedT:public TT##X, Fallback{}; 		\
    template<typename TT##X>						\
    struct DerivedT<TT##X,0>: Fallback {};			\
													\
    typedef DerivedT<T##X, nstd::is_class_type<T##X>::value> Derived;	\
																		\
    template<typename U, U> struct Check;								\
    typedef char ArrayOfOne[1];											\
    typedef char ArrayOfTwo[2];											\
																		\
    template<typename U> static ArrayOfOne &							\
        func(Check<int Fallback::*, &U::X> *);							\
    template<typename U> static ArrayOfTwo & func(...);	\
  public:												\
    enum { value = sizeof(func<Derived>(0)) == 2 };		\
}

// from c++11 standard:
// 10.2 member name lookup
// Name lookup takes place before access control
// 所以 CREATE_MEMBER_DETECTOR 是可靠的


#define DETECT_MEMBER(TYPE,NAME) Detect_##NAME<TYPE>::value

} /* namespace Detector */

// group ulitilies
/**
	* @}
	*/

} /* namespace nstd */

// group nstd
/**
  * @}
  */

/** @brief 为了 member detector 能够正常工作写的编译时检测代码 */
namespace DetectorTest{

    struct Base{int a;};
    struct T: public Base{};

    struct TestExist{
        CREATE_MEMBER_DETECTOR(a);
        STATIC_ASSERT(DETECT_MEMBER(DetectorTest::T,a));
    };
    struct TestNotExist{
        CREATE_MEMBER_DETECTOR(b);
        STATIC_ASSERT(DETECT_MEMBER(DetectorTest::T,b)==0);
    };
    struct NonClassTest{
        CREATE_MEMBER_DETECTOR(a);
        STATIC_ASSERT(DETECT_MEMBER(int,a)==0);
    };
}
#endif
