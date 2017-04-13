#ifndef NSTD_MEMBER_DETECTOR_H
#define NSTD_MEMBER_DETECTOR_H

////////////////////////////////////////////////////////////////////////
// Original author: Alexandre Courpron
// from: http://www.codeproject.com/Articles/19444/Interface-Detection
// Modified by: Pony279
// �޸�����:
// 1. ���� enum ���������� DetectResult
// 2. �� DetectMember_##Identifier �Ľ������Ϊ value, ���� enum ��Ϊ
// static const DetectResult, תΪǿ���ͻ��һЩ
// 3. ȥ���� CREATE_FUNCTION_DETECTOR �� CREATE_DATA_DETECTOR �궨����
// �������ռ��Χ
// 4. ���� nstd ��, �����޸��������ռ�
// 5. ȥ���� CREATE_FUNCTION_DETECTOR �� CREATE_DATA_DETECTOR
// �������ķֺ� ';'
// Thanks to Alexandre Courpron!
// ...
// 6. ���� FUNCTION_DETECTOR �� DATA_DETECTOR �����ܼ�����ĳ�Ա
// ������, ���� MEMBER_DETECTOR , �ο���
// http://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Member_Detector
// ������Щ�޸�, �����˷� classtype ʶ�������������
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

		/**	@brief ���Ա������ļ����.
		  * �ɲο� @ref DETECT_FUNCTION �� @ref DETECT_DATA ��.
		  */
        enum DetectResult{
			  NOT_FOUND = 0 /**< û���ҵ�ָ���ĳ�Ա */
			, STATIC_FUNCTION /**< �ҵ���Ӧ�ľ�̬������Ա */
				= sizeof( StaticFunction ) - sizeof( NotFound )
			, NON_STATIC_FUNCTION /**< �ҵ��˶�Ӧ�ķǾ�̬������Ա */
				= sizeof( NonStaticFunction ) - sizeof( NotFound )
			, STATIC_DATA /**< �ҵ��˶�Ӧ�ľ�̬���ݳ�Ա */
				= sizeof( StaticData ) - sizeof( NotFound )
			, NON_STATIC_DATA /**< �ҵ��˶�Ӧ�ķǾ�̬���ݳ�Ա */
				= sizeof( NonStaticData ) - sizeof( NotFound ) };




/** @def CREATE_FUNCTION_DETECTOR(Identifier)
  * @brief ����<��/�ṹ�庯����Ա�����>.
  * ��ϸ���ݲο� @ref DETECT_FUNCTION ��
  * @param Identifier ��/�ṹ��ĺ�����Ա�ı�ʶ��
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
  * @brief �õ�<��/�ṹ�庯����Ա�����>��ָ����Ա�Ĵ����Եļ����.
  * @note ��ʹ��֮ǰ, ����ʹ�ú� CREATE_FUNCTION_DETECTOR ������һ����/�ṹ
  * ���Ա�����.
  * @param Class ��������
  *	@param ReturnType ������Ա�ķ�������
  * @param Identifier ������Ա�ı�ʶ��
  * @param Params ������Ա�Ĳ����б�, �� (void)
  *		@note ��Ҫд������ "()"
  * @return ����ֵΪ @ref DetectResult ���͵ı����ڳ���. �����ֻ����
  * NOT_FOUND, STATIC_FUNCTION, NON_STATIC_FUNCTION ���ַ���ֵ.
  * @note ���������������ڼ������Ա
  * @example nstd_detector_example.cpp
  */
#define DETECT_FUNCTION(Class,ReturnType,Identifier,Params) \
    DetectMember_##Identifier< Class,\
                                         ReturnType (Class::*)Params,\
                                         ReturnType (Class::*)Params const,\
                                         ReturnType (*)Params \
                                       >::value

/** @def CREATE_DATA_DETECTOR(Identifier)
  * @brief ����һ��<��/�ṹ�����ݳ�Ա�����>.
  * ��ϸ���ݲο� @ref DETECT_DATA ��
  * @param Identifier ��/�ṹ������ݳ�Ա�ı�ʶ��
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
  * @brief �õ�<��/�ṹ�����ݳ�Ա�����>��ָ����Ա�Ĵ����Եļ����.
  * @note ��ʹ��֮ǰ, ����ʹ�ú� CREATE_DATA_DETECTOR ������һ����/�ṹ
  * ���Ա�����.
  * @param Class ��������
  *	@param Type ���ݳ�Ա�ķ�������
  * @param Identifier ���ݳ�Ա�ı�ʶ��
  * @return ����ֵΪ @ref DetectResult ���͵ı����ڳ���. �����ֻ��
  * NOT_FOUND, STATIC_DATA, NON_STATIC_DATA ���ַ���ֵ.
  * @note ���������������ڼ������Ա
  * @example nstd_detector_example.cpp
  */
#define DETECT_DATA(Class,Type,Identifier) \
    DetectMember_##Identifier< Class,\
                                         Type (Class::*),\
                                         Type (*)\
                                       >::value


/** @def CREATE_MEMBER_DETECTOR(X)
  * @brief ���ڴ������Ա�����, ���ڼ���Ƿ���ָ�����ֵĳ�Ա
  * �乤��ԭ���ǲ������ֳ�ͻ, ������ SFINAE ԭ���ñ�����ȥѡ��
  * ƥ��Ľ��.
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
/** ��� member detector ���� int ���ڽ�������Ҳ�ܹ��� */
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
// ���� CREATE_MEMBER_DETECTOR �ǿɿ���


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

/** @brief Ϊ�� member detector �ܹ���������д�ı���ʱ������ */
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
