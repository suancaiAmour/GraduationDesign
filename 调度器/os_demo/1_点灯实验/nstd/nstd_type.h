#pragma once

#ifndef TYPE_H
#define TYPE_H

/** @file nstd_type.h
  * @brief ������ļ��ﶨ����һЩ����������, �Լ����Ͳ��ݵ�һЩ
  * ����ģ��.
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

	typedef uint32_t  u32;  /**< �޷��� 32 λ���� */
	typedef uint16_t u16;   /**< �޷��� 16 λ���� */
	typedef uint8_t  u8;    /**< �޷���  8 λ���� */

	typedef int32_t  s32;   /**< �з��� 32 λ���� */
	typedef int16_t s16;    /**< �з��� 16 λ���� */
	typedef int8_t  s8;     /**< �з���  8 λ���� */

	/**	@enum Result
	  * @brief ͨ�õĺ������ý��
	  * @note ����Ϊ���ö�������� == ���������������в����� SUCCEEDED ��
	  * ֵ������ Result a = (Result)3 ����ô a == FAILED ���᷵���棬��ʹ
	  *	������������ֵ���ǲ���ȵġ���һ�в��ɹ�����ζ��ʧ�ܡ�
	  */
	typedef enum {
			SUCCEEDED = 0	/**< ��������ɹ� */
			, FAILED = !SUCCEEDED	/**< ��������ʧ�� */
	} Result;
	inline bool operator ==(Result a, Result b)
	{
		return
			((int)a ==(int)b)||
			(((int)a !=(int)SUCCEEDED)&&((int)b !=(int)SUCCEEDED));
	}

	/**	@enum Polarity
	  *	@brief ������ʾ���Ե�ö����
	  */
	typedef enum {
		NEGATIVE = 0	/**< ������ */
		, POSITIVE = !NEGATIVE /**< ������ */
	}Polarity;

	#if(HOST_ENDIAN == BIG_ENDIAN)	//����ֽ���
		#pragma pack(push)	//�������״̬
		#pragma pack(1)

		typedef struct LittleEndianU16 leu16;
		typedef struct LittleEndianU32 leu32;
		/**	@brief ���ڴ�����С���ֽ�����ʽ�洢�� 32λ �޷�������
		*/
		struct LittleEndianU32
		{
			u8 ll;	/**< ��16λ��8λ */
			u8 lh;	/**< ��16λ��8λ */
			u8 hl;	/**< ��16λ��8λ */
			u8 hh;	/**< ��16λ��8λ */
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
		/**	@brief ���ڴ�����С���ֽ�����ʽ�洢�� 16λ �޷�������
		*/
		struct LittleEndianU16
		{
			u8 l;	/**< �� 8 λ */
			u8 h;	/**< �� 8 λ */
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

		#pragma pack(pop)	//�ָ�����״̬

	#else
		#pragma pack(push)	//�������״̬
		#pragma pack(1)

		typedef u32 leu32;
		typedef u16 leu16;

		typedef struct BigEndianU32 beu32;
		typedef struct BigEndianU16 beu16;
		/**	@brief ���ڴ����Դ���ֽ�����ʽ�洢�� 32λ �޷�������
		*/
		struct BigEndianU32
		{
			u8 hh;	// ��16λ��8λ
			u8 hl;	// ��16λ��8λ
			u8 lh;	// ��16λ��8λ
			u8 ll;	// ��16λ��8λ
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
		/**	@brief ���ڴ�����С���ֽ�����ʽ�洢�� 16λ �޷�������
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

		#pragma pack(pop)	//�ָ�����״̬

	#endif


	/** @brief ���ģ�������ж�һ�������ǲ��� class,
      * ���� struct ���� union. ����:
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
  * @brief ��������ѡ����, �� TYPE_SELECTOR_RESULT ����ʹ��. �������
  * ѡ�������ŵ������ڴ�������ѡ������ʱ����Ҫ������ʵ��������
  * ����, ������ʹ�� TYPE_SELECTOR_RESULT �õ������ʱ��, ֻʵ������
  * ������. �÷�����:
  * @code
  * TYPE_SELECTOR_CREATE(int, bool, sel); // sel Ϊ����ѡ����������
  * TYPE_SELECTOR_RESULT(sel, true) i; // �� int i
  * TYPE_SELECTOR_RESULT(sel, false) i; // �� bool j
  * @endcode
  * @param if_type ѡ�����Ϊ���ʱ��Ľ������.
  * @param else_type ѡ�����Ϊ�ٵ�ʱ��Ľ������.
  * @param selector ����ѡ����������.
  */
#define TYPE_SELECTOR_CREATE(if_type, else_type, selector) \
template <bool , typename selector_dummy_type=void>        \
struct selector{ typedef if_type type; };                  \
template <typename selector_dummy_type>                    \
struct selector<false,selector_dummy_type>{ typedef else_type type; }

/** @def TYPE_SELECTOR_RESULT(selector, b)
  * @brief ��ͨ�� TYPE_SELECTOR_CREATE ����������ѡ������ѡ������. ��
  * �����Ϣ��ο� @ref TYPE_SELECTOR_CREATE.
  * @param selector ����ѡ����������.
  * @param b ѡ�����. ע��������������Ǳ����ڳ���.
  */
#define TYPE_SELECTOR_RESULT(selector, b)  selector<b>::type

    /** @brief ���ͱ�, ��������ѡ��.
      * ����:
      * @code
      * type_list<char, bool, int>::at<0>::type i; //�� char i;
      * type_list<char, bool, int>::at<1>::type j; //�� bool j;
      * type_list<char, bool, int>::at<2>::type k; //�� int k;
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

        /* �����в�֧����ȫ�ػ�, ����ʹ��ƫ�ػ�, ������Ĭ�ϲ���,
          ������Ӧ����ȫ�ػ�. */
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
