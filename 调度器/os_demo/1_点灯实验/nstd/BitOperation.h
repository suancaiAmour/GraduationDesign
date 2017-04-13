#ifndef __cplusplus		/* ��C���ԵĻ����²���ʹ�����ͷ�ļ� */
#define BITOPERATION_H
#endif
#ifndef BITOPERATION_H
#define BITOPERATION_H

#include "nstd_type.h"
#include "define.h"
#include "nstd_assert.H"

/** @addtogroup nstd
  * @{
  */

namespace nstd{
	/** @addtogroup bit_operations
	  * @brief ��λ������صĺ�������ͺ�.
	  * @ingroup nstd
	  * @{
	  */

	/** @brief ���ڱ�֤��Ľ��Ϊ�����ڳ�����
	  * @tparam N һ������ u32 �����ڳ������������������ڴ���
	  */
	template <u32 N>
    struct ct_const_u32{ static const u32 value =  N; };

	/** @def Bit(x)
	  * @brief �õ�һ��ָ��λ����λ�� 32 λ�޷���������:
	  * @code
	  * Bit(4) == 0x10;
	  *	unsigned int a = Bit(5); // a = 1<<5 or a = 0x20
	  * @endcode
	  */
    #define BIT(x)	(((u32)0x01)<<x)
    #define Bit		BIT
	/* ���û��(u32)��ǿ��ת����Bit(31)���о��档*/


	/**	@struct u8IsSingleBit<u8 v>
	  * @brief �������ģ�����ж�һ��u8�ͳ����ǲ���ֻ��һ�� bit ��ֵΪ 1��
	  *	@param v һ�� u8 �ͳ���
	  */
	template<u8 v>
	struct u8IsSingleBit{
        #undef ExtractBit
        #define ExtractBit(n) ((v>>n)&0x01)
		enum{
			result = ((ExtractBit(0)+ExtractBit(1)+
				ExtractBit(2)+ExtractBit(3)+ExtractBit(4)+
				ExtractBit(5)+ExtractBit(6)+ExtractBit(7))
				==1)?1:0
			/**< ������� v �ж��λΪ 1 �� ���� v Ϊ 0����ô result Ϊ
				0������ result Ϊ 1��*/
			, value = result /**< value = result */
		};
        #undef ExtractBit
	};

	/**	@struct u16IsSingleBit<u16 v>
	  * @brief �������ģ�����ж�һ��u16�ͳ����ǲ���ֻ��һ�� bit ��ֵΪ1��
	  *		�ο� @ref u8IsSingleBit.
	  */
	template<u16 v>
	struct u16IsSingleBit{
		enum{
			result = ( u8IsSingleBit<(u8)v>::result +
					u8IsSingleBit<(u8)(v>>8)>::result)==1?1:0
			/**< �ο� @ref u8IsSingleBit */

			, value = result /**< value = result */
		};
	};
	/**	@struct u32IsSingleBit<u32 v>
	  * @brief �������ģ�����ж�һ��u32�ͳ����ǲ���ֻ��һ�� bit ��ֵΪ1��
	  *		�ο� @ref u8IsSingleBit.
	  */
	template<u32 v>
	struct u32IsSingleBit{
		enum{
			result = (u16IsSingleBit<(u16)v>::result +
					u16IsSingleBit< (u16)(v>>16)>::result)
					== 1?1:0
			, value = result /**< value = result */
		};
	};

	/** @def BitFromTo(h,l)
	  *	@brief �õ�һ��ָ��λ����λ��ֵ���� BitFromTo(3, 0) == 0x0f��
	  * @param h λ������λ�ĺ���(��0��ʼ��)
	  * @param l λ������λ�ĺ���(��0��ʼ��)
	  */
	#define BitFromTo(h, l)  \
				(ct_const_u32<(h > l)?	\
					((BIT(h) - BIT(l) + BIT(h))):  \
					(BIT(l) - BIT(h) + BIT(l))>::value)
	#define BitF2		BitFromTo
	#define BitFT		BitFromTo

	/**	@brief �õ�һ��ָ��λ��Ϊָ��ֵ�ı����ڳ���, ����:
	  * @code
	  * field<0xf0, 0xa>::value 0xa0;
	  * @endcode
	  */
	template<u32 field_mask, u32 val>
	struct field{enum{value = (((field_mask<<1)|field_mask) ^ (field_mask<<1))*val};};

     /** @brief ����ṹ����Ϊ�˱�֤ binary ���ܹ�����ȷʹ�ö�д�ġ�
	   *	binary ��Ҫ��������ó��� 8 λ���ұ����ǰ��ն����и�ʽ��д��
	   *	�������������û�а��չ涨��д������ṹ���ڲ���STATIC_ASSERT
	   *	��Ϳ��Բ���һ������ʱ����
       */
     template <u8 N, bool assertion>
	 struct binary_debugger{
         static const u8 value =  N;
         STATIC_ASSERT(assertion);
    };

	/** @define binary(b)
      *	@brief ʵ��д8λ���������ĺꡣ�� binary(01011010) == 0x5a��
      *	@param b: �Զ����Ƹ�ʽд�ĳ��ȣ������Գ���һ���ֽ�(8λ)
      */
    #define binary(b)  (binary_debugger<(               \
				(((( 0x##b##L )			) & 1 )<<0)	|	\
				(((( 0x##b##L )	>>4		) & 1 )<<1)	|	\
				(((( 0x##b##L )	>>8		) & 1 )<<2)	|	\
				(((( 0x##b##L )	>>12	) & 1 )<<3)	|	\
				(((( 0x##b##L )	>>16	) & 1 )<<4)	|	\
				(((( 0x##b##L )	>>20	) & 1 )<<5)	|	\
				(((( 0x##b##L )	>>24	) & 1 )<<6)	|	\
				(((( 0x##b##L )	>>28	) & 1 )<<7)		\
				),( 0x##b <=0x11111111)&&(( 0x##b &0xeeeeeeee)==0)>::value)
    #define BByte(b) binary(b)

	/** @define BWord(h,l)
	  * @brief ʵ��д16λ�����Ƶĺꡣ�� BWord(011,0) == 0x30��
	  *	@param h ��8λ
	  * @param l ��8λ
	  */
 	#define BWord(h,l)		( (((u16)(BByte(h)))<<8) + ((u16)BByte(l)))


	/** @brief �õ�һ������ bits ��������ļ���λ�����λ�����磺
	  *	��� u16 a = BByte(00111000);��ô GetLSB(a) �᷵��BByte(00001000)��
	  * ��� u16 a = BByte(11000111);��ô GetLSB(a) �᷵��BByte(01000001)��
	  */
	template<class B>
	finline B GetLSB(B bits) {	return (B)(((bits<<1)|bits) ^ (bits<<1));	}

    // ����
	//˵��:		setb
	//	�õ�һ������ĳ��λλ����λ���ֵ�������������ı���������ֵ
	//ע��:
	//	1. ���flag�ǼĴ����������ǿɶ���
	template<class Flag_Type, class W>
	finline Flag_Type setb(Flag_Type flag, W whichBit)
	{
		return ((flag) | ((Flag_Type)whichBit));
	}


    // ����
	/* ˵��:		clearb
	  *	����λ��flag������which_bit������λ����0
	  * ע��:
	  *     1. flag�����ǿ���������д��
      */
	template<class Flag_Type, class W>
	finline Flag_Type clearb(Flag_Type flag, W whichBit)
	{
		return ((flag) & (~((Flag_Type)whichBit)));
	}

    // ����
	//˵��:
	//	���flag�е�ĳЩ�Ƿ���λ�������Щλ��Ϊ0���򷵻�0
	//	���򷵻�����ֵ
	//ע��:
	//	1. flag�����ǿ��Զ���
	template<class Flag_Type, class W>
	finline Flag_Type checkb(Flag_Type flag, W whichBit)
	{
		return 	(Flag_Type)( ((Flag_Type)flag) & ((Flag_Type)whichBit) );
	}

	// ��������Ѿ���������
	//	������������õ�һ��������ĳ����ĳ����λ����ֵ���ֵ
	//����:
	//	flag	Ҫ��ֵ�ı���
	//	which_bit
	//			����flag���ļ���λҪ����ֵ
	//	val		��ʾҪ����flag���Ǽ���λ��ֵ
	//ע��:
	//	1. ���ں꣬�������Ҫ��++i;֮��ı��ʽ��Ҳ��Ҫֱ��ʹ�õ��ú����õ��ķ���ֵ!
	//	2. ���type�ǼĴ�������ô�������ǿɶ���
	//ʾ��:
	//	u8 a = 0xf0;
	//	assignb( a, BIT4 | BIT0, BIT0);
	//	ִ�����a == 0xe1;
	//��ע��
	//	1. Ч����, �����������flag�⣩���ǳ���, ���������Զ��Ż�, ��������Ǳ���, �����ֶ�д����Ҳ��������ӵ�
	template<class Flag_Type, class W, class V>
	finline Flag_Type assignb(Flag_Type flag, W whichBit, V val)
	{return ((((Flag_Type)whichBit)&((Flag_Type)val)) | ((flag)&(~((Flag_Type)whichBit))));}

    // �����ѱ�����
	/*	@brie	AssignField
	*	�õ�������һ��λ�α���ֵ���ֵ���������������ı������ֵ
	*	@param
	*		var: ����
	*		LSB: ��λ�ε����λ
	*		len: λ�εĳ��ȣ����ֵΪ8
	*		val: λ�ε�ֵ
	*	@note	���var��һ���Ĵ�������ô�������������ɶ���
	*/
	template<class Flag_Type, class LSB,class L, class V>
	finline Flag_Type AssignField(Flag_Type flag, LSB lsb, L len, V val)
	{	return assignb(flag
					  , ((1<<(len)) - 1)*((Flag_Type)lsb)
					  ,(( (Flag_Type)val)&((Flag_Type)((1<<(len)) - 1)))*((Flag_Type)lsb));	}

    // �����Ѿ�������
	/*	@brief	�õ�һ��������һ�� bit field ������Ϊָ����ֵ��ı���ֵ��
     *         �������������ı������ֵ
     *         �õ���������������λ����ֵ���ֵ��
     *   @note   �����������ı䴫��ı�����ֵ
	 *	@param
	 *		flag: һ������
	 *		bits: �����ļ���λ
	 *		val : bits��ʾ�ļ���λҪ������ֵ
 	 *	@note	val ����ҪԤ����λ�����������assignb��ʹ�ò�ͬ��
	 *	@example
	 *			u16 a = 0;	a = assignbs(a, BitFromTo(11, 4), 0x5a);	//��ôִ�����a = 0x05a0;
	 */
	template<class Flag_Type, class BITS, class V>
	tinline Flag_Type AssignField(Flag_Type flag, BITS bits, V val)
	{	return assignb(flag, bits, (((Flag_Type)val)*((Flag_Type)( GetLSB(bits) ))));	}
	#define	assignf		AssignField			// assign bit field

	/**	@brief	��һ��������һ�� bit field ��ֵ */
	template<class Flag_Type, class BITS>
	tinline Flag_Type ReadField(Flag_Type flag, BITS bits)
	{ return (Flag_Type)((flag & bits)/(Flag_Type)GetLSB(bits));	}
	#define readf		ReadField


	/**	@brief	�õ�һ��������Ӧ�� bit number. ����:
	  *	@code
	  * bit_number<1>::value == 0, bit_number<Bit(7)> == 7
	  * @endcode
	  */
	template<u32 N>
	struct bit_number{
        /* ����ֻ����һλ����λ */
        STATIC_ASSERT(u32IsSingleBit<N>::result);
		static const u8 value = bit_number<N/2>::value+1;
	};
	template<>	/* �ػ� */
	struct bit_number<1>{
		static const u8 value = 0;
	};

	/** @brief λ��1��������һ��������ָ����λ��1��
	  *	@param p ָ��Ҫ�����ı������ò����������͵�������
	  *	@param b ָ��Ҫ��1��λ���ò����������͵�������
	  */
    template<class Flag_Type, class bits> tinline
    Flag_Type& set_bits(Flag_Type* p, bits b)
	{
		return ((*p) |= ((Flag_Type)b));
	}
	/** @brief λ���㺯������һ��������ָ����λ���㡣
	  *	@param p ָ��Ҫ�����ı������ò����������͵�������
	  *	@param b ָ��Ҫ�����λ���ò����������͵�������
	  */
	template<class Flag_Type, class Bits_Type> tinline
	Flag_Type& clear_bits(Flag_Type* const p, Bits_Type const bits)
	{
		return ((*p) &= (~((Flag_Type)bits)));
	}
	/** @brief λȡ����������һ��������ָ����λȡ����
	  *	@param p ָ��Ҫ�����ı������ò����������͵�������
	  *	@param b ָ��Ҫȡ����λ���ò����������͵�������
	  */
    template<class Flag_Type, class Bits_Type> tinline
	Flag_Type& toggle_bits(Flag_Type* const p, Bits_Type bits){
		return ((*p) ^= ((Flag_Type)bits));
    }

	/** @brief λ����뺯������һ��������ָ��λ�����ָ������ֵ��
	  * @param <field> u32�ͳ���������ָ��һ��λ��
	  *		@note field ������ͬʱָ�����λ��
	  *	@param var Ҫ�������ݵı����ĵ�ַ��ʹ��ָ�������ȷ���������Բ���
	  *	�����޸ġ�
	  *		@note var ָ����޷����������͵Ĵ�С�����Դ���4���ֽڡ�
	  *	@param insert_val Ҫ�������ֵ������ int a = 0x55;
	  *	insert_field<0xf0>(&a,0xa); ִ����� a==0xa5 ��
	  */
    template<u32 field, typename T> tinline
	T& field_insert(T* var, u32 insert_val){
        enum{ lsb
                = (((field<<1)|field) ^ (field<<1)),
              only_one_field = u32IsSingleBit<lsb>::result
		};
        /* ��֧�ִ��� 32 λ�ı�������֧�ֲ���Ϊ��� field��
           �������������˱����ڴ���˵����ʹ��
           CBitField ��ʱ��ʹ���˷Ƿ��Ĳ���������
           �Ĵ�С�����Գ��� 4 ���ֽڣ����Ҳ���
           ��ֻ��ָ��һ�� field��
        */
        STATIC_ASSERT((sizeof(T)<=4)&&only_one_field);
        /* ���õ�ֵ����̫�� */
        assert(insert_val*lsb <= field);
		return *var=(((*var)&(~field)) |
            ((insert_val)*lsb));
	}

	/** @brief λ����ȡ��������һ��������ָ��λ���ֵ��
	  * @param <field> u32�ͳ���������ָ��һ��λ��
	  *		@note field ������ͬʱָ�����λ��
	  *	@param var �������͵��޷�������������
	  *		@note var ���������Ͳ��ܴ���4���ֽڡ�
	  * @return ������ָ��λ�����ֵ���� field_extract<0xf>(0x78)==0x7��
	  */
    template<u32 field, typename T> tinline
    u32 field_extract(const T& var){
        enum{ lsb
                = (((field<<1)|field)^(field<<1)),
              only_one_field = u32IsSingleBit<lsb>::result
        };
        /* ��֧�ִ��� 32 λ�ı�������֧�ֲ���Ϊ��� field��
           �������������˱����ڴ���˵����ʹ��
           CBitField ��ʱ��ʹ���˷Ƿ��Ĳ���������
           �Ĵ�С�����Գ��� 4 ���ֽڣ����Ҳ���
           ��ֻ��ָ��һ�� field��
        */
        STATIC_ASSERT((sizeof(T)<=4)&&only_one_field);
        return ((var&field)>>bit_number<lsb>::value);
    }

    /****************************************************
	function: BitField<u32 field>(variable)
	parameter:
		field: ָ��λ��
		variable: ָ������
	description: ����һ��ר��Ϊλ��Ķ�д����װ�ĺ���
	example:
		��һ������ variable �� D4 - D7 λ���и�ֵΪ
		value ��λ��д����
			BitField<BitFromTo(7,4), variable> = value;
			��ִ��ǰ variable == 0xf0f0 �� value == 0x5
			��ִ�к� variable == 0xf050
		�� variable �� D4 - D7 λ��ֵ�������� temp ��
		��λ�������
			temp = BitField<BitFromTo(7,4),variable);
			��ִ��ǰ variable == 0xf070
			��ִ�к� temp == 0x7
	remark: ����д���������õ��� ��-�޶�-д �ķ�ʽ������
		�ڶ��̵߳ĳ�����Ӧ�ÿ��Ǵ�����̰߳�ȫ�ԡ�
	****************************************************/
	template<u32 field, typename T>
	class CBitField{

        enum{ least_significant_bit
                = (((field<<1)|field) ^ (field<<1)),
            only_one_field = u32IsSingleBit<least_significant_bit>::result
		};
        /* ��֧�ִ��� 32 λ�ı�������֧�ֲ���Ϊ��� field��
           �������������˱����ڴ���˵����ʹ��
           CBitField ��ʱ��ʹ���˷Ƿ��Ĳ���������
           �Ĵ�С�����Գ��� 4 ���ֽڣ����Ҳ���
           ��ֻ��ָ��һ�� field��
        */
        STATIC_ASSERT((sizeof(T)<=4)&&only_one_field);
		T* const t;
		public:
		finline CBitField(T* init):t(init){}
		/* Ϊʹ�÷�ʽ��࣬�������κ����� */
		finline void operator =(const u32& value){ t=field_insert<field>(t,value); }
		finline operator T() { return ReadField(t,field); }
	};
	template <u32 field, typename T>
	tinline CBitField<field,T> BitField(T* t){
		return CBitField<field,T>(t);
	}

	// group bit_operations
    /**
	  * @}
	  */

}	// namespace nstd

// group nstd
/**
  * @}
  */

#endif
