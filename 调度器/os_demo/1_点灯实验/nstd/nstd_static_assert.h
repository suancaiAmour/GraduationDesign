#ifndef __cplusplus
#define NSTD_STATIC_ASSERT_H
#endif
#ifndef NSTD_STATIC_ASSERT_H
#define NSTD_STATIC_ASSERT_H

/*
	Ϊ STATIC_ASSERT �궨����࣬STATIC_ASSERT ���õ��Ƕ�δ�����
	�ṹ����� sizeof ʱ�������ᱨ�������ص㡣
	�磺
		struct A;
		template<int x> struct test;
		typedef test< sizeof(A)> B;	// ����A ��δ��������͡�
	*/
namespace nstd{
    template <bool x> struct STATIC_ASSERTION_FAILURE;
    template <> struct STATIC_ASSERTION_FAILURE<true>{};
    template<int x> struct static_assert_test{};
}


/******************************************************
*	macro: STATIC_ASSERT
*	description: �����������Խ��б����ڶ��ԣ���ռ
*		������ʱ�䡣
*	example:
*		��������ӿ�������ȷ�� int ��������Ϊ 4 
*		���ֽڵĴ�С���ܱ���ɹ���
*		STATIC_ASSERT(sizeof(int)>=4);
******************************************************/
	#define STATIC_ASSERT(B)    \
	typedef nstd::static_assert_test<sizeof(nstd::STATIC_ASSERTION_FAILURE<B>)> \
        static_assert_typedef_##__LINE__

#endif
