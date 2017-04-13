#ifndef __cplusplus
#define NSTD_STATIC_ASSERT_H
#endif
#ifndef NSTD_STATIC_ASSERT_H
#define NSTD_STATIC_ASSERT_H

/*
	为 STATIC_ASSERT 宏定义的类，STATIC_ASSERT 利用的是对未定义的
	结构体进行 sizeof 时编译器会报告错误的特点。
	如：
		struct A;
		template<int x> struct test;
		typedef test< sizeof(A)> B;	// 错误！A 是未定义的类型。
	*/
namespace nstd{
    template <bool x> struct STATIC_ASSERTION_FAILURE;
    template <> struct STATIC_ASSERTION_FAILURE<true>{};
    template<int x> struct static_assert_test{};
}


/******************************************************
*	macro: STATIC_ASSERT
*	description: 利用这个宏可以进行编译期断言，不占
*		用运行时间。
*	example:
*		下面的例子可能用来确保 int 类型至少为 4 
*		个字节的大小才能编译成功。
*		STATIC_ASSERT(sizeof(int)>=4);
******************************************************/
	#define STATIC_ASSERT(B)    \
	typedef nstd::static_assert_test<sizeof(nstd::STATIC_ASSERTION_FAILURE<B>)> \
        static_assert_typedef_##__LINE__

#endif
