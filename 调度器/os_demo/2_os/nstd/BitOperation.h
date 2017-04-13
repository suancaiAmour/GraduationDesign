#ifndef __cplusplus		/* 在C语言的环境下不能使用这个头文件 */
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
	  * @brief 和位操作相关的函数，类和宏.
	  * @ingroup nstd
	  * @{
	  */

	/** @brief 用于保证宏的结果为编译期常数。
	  * @tparam N 一个任意 u32 编译期常数，否则会产生编译期错误
	  */
	template <u32 N>
    struct ct_const_u32{ static const u32 value =  N; };

	/** @def Bit(x)
	  * @brief 得到一个指定位被置位的 32 位无符号数，如:
	  * @code
	  * Bit(4) == 0x10;
	  *	unsigned int a = Bit(5); // a = 1<<5 or a = 0x20
	  * @endcode
	  */
    #define BIT(x)	(((u32)0x01)<<x)
    #define Bit		BIT
	/* 如果没有(u32)的强制转换，Bit(31)会有警告。*/


	/**	@struct u8IsSingleBit<u8 v>
	  * @brief 可用这个模板来判断一个u8型常量是不是只有一个 bit 的值为 1。
	  *	@param v 一个 u8 型常量
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
			/**< 如果参数 v 有多个位为 1 或 参数 v 为 0，那么 result 为
				0，否则 result 为 1。*/
			, value = result /**< value = result */
		};
        #undef ExtractBit
	};

	/**	@struct u16IsSingleBit<u16 v>
	  * @brief 可用这个模板来判断一个u16型常量是不是只有一个 bit 的值为1。
	  *		参考 @ref u8IsSingleBit.
	  */
	template<u16 v>
	struct u16IsSingleBit{
		enum{
			result = ( u8IsSingleBit<(u8)v>::result +
					u8IsSingleBit<(u8)(v>>8)>::result)==1?1:0
			/**< 参考 @ref u8IsSingleBit */

			, value = result /**< value = result */
		};
	};
	/**	@struct u32IsSingleBit<u32 v>
	  * @brief 可用这个模板来判断一个u32型常量是不是只有一个 bit 的值为1。
	  *		参考 @ref u8IsSingleBit.
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
	  *	@brief 得到一个指定位域被置位的值。如 BitFromTo(3, 0) == 0x0f。
	  * @param h 位域的最高位的号数(从0开始数)
	  * @param l 位域的最低位的号数(从0开始数)
	  */
	#define BitFromTo(h, l)  \
				(ct_const_u32<(h > l)?	\
					((BIT(h) - BIT(l) + BIT(h))):  \
					(BIT(l) - BIT(h) + BIT(l))>::value)
	#define BitF2		BitFromTo
	#define BitFT		BitFromTo

	/**	@brief 得到一个指定位域为指定值的编译期常数, 例如:
	  * @code
	  * field<0xf0, 0xa>::value 0xa0;
	  * @endcode
	  */
	template<u32 field_mask, u32 val>
	struct field{enum{value = (((field_mask<<1)|field_mask) ^ (field_mask<<1))*val};};

     /** @brief 这个结构体是为了保证 binary 宏能够被正确使用而写的。
	   *	binary 宏要求参数不得超过 8 位，且必须是按照二进行格式书写，
	   *	如果参数不满足没有按照规定填写，这个结构体内部的STATIC_ASSERT
	   *	宏就可以产生一个编译时错误。
       */
     template <u8 N, bool assertion>
	 struct binary_debugger{
         static const u8 value =  N;
         STATIC_ASSERT(assertion);
    };

	/** @define binary(b)
      *	@brief 实现写8位二进制数的宏。如 binary(01011010) == 0x5a。
      *	@param b: 以二进制格式写的长度，不可以超过一个字节(8位)
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
	  * @brief 实现写16位二进制的宏。如 BWord(011,0) == 0x30。
	  *	@param h 高8位
	  * @param l 低8位
	  */
 	#define BWord(h,l)		( (((u16)(BByte(h)))<<8) + ((u16)BByte(l)))


	/** @brief 得到一个变量 bits 里的连续的几个位的最低位。例如：
	  *	如果 u16 a = BByte(00111000);那么 GetLSB(a) 会返回BByte(00001000)；
	  * 如果 u16 a = BByte(11000111);那么 GetLSB(a) 会返回BByte(01000001)。
	  */
	template<class B>
	finline B GetLSB(B bits) {	return (B)(((bits<<1)|bits) ^ (bits<<1));	}

    // 弃用
	//说明:		setb
	//	得到一个变量某几位位被置位后的值，这个函数不会改变变量本身的值
	//注意:
	//	1. 如果flag是寄存器，必须是可读的
	template<class Flag_Type, class W>
	finline Flag_Type setb(Flag_Type flag, W whichBit)
	{
		return ((flag) | ((Flag_Type)whichBit));
	}


    // 弃用
	/* 说明:		clearb
	  *	清零位，flag变量中which_bit标明的位被清0
	  * 注意:
	  *     1. flag必须是可以正常读写的
      */
	template<class Flag_Type, class W>
	finline Flag_Type clearb(Flag_Type flag, W whichBit)
	{
		return ((flag) & (~((Flag_Type)whichBit)));
	}

    // 弃用
	//说明:
	//	检查flag中的某些是否被置位，如果那些位都为0，则返回0
	//	否则返回其它值
	//注意:
	//	1. flag必须是可以读的
	template<class Flag_Type, class W>
	finline Flag_Type checkb(Flag_Type flag, W whichBit)
	{
		return 	(Flag_Type)( ((Flag_Type)flag) & ((Flag_Type)whichBit) );
	}

	// 这个函数已经被弃用了
	//	这个函数用来得到一个变量的某个或某几个位被赋值后的值
	//参数:
	//	flag	要赋值的变量
	//	which_bit
	//			标明flag的哪几个位要被赋值
	//	val		表示要赋给flag的那几个位的值
	//注意:
	//	1. 对于宏，其参数不要用++i;之类的表达式！也不要直接使用调用函数得到的返回值!
	//	2. 如果type是寄存器，那么它必须是可读的
	//示例:
	//	u8 a = 0xf0;
	//	assignb( a, BIT4 | BIT0, BIT0);
	//	执行完后，a == 0xe1;
	//备注：
	//	1. 效率上, 如果参数（除flag外）都是常量, 编译器会自动优化, 如果参数是变量, 就算手动写代码也是这个样子的
	template<class Flag_Type, class W, class V>
	finline Flag_Type assignb(Flag_Type flag, W whichBit, V val)
	{return ((((Flag_Type)whichBit)&((Flag_Type)val)) | ((flag)&(~((Flag_Type)whichBit))));}

    // 函数已被弃用
	/*	@brie	AssignField
	*	得到变量的一个位段被赋值后的值，这个函数本身不会改变变量的值
	*	@param
	*		var: 变量
	*		LSB: 该位段的最低位
	*		len: 位段的长度，最大值为8
	*		val: 位段的值
	*	@note	如果var是一个寄存器，那么它必须是正常可读的
	*/
	template<class Flag_Type, class LSB,class L, class V>
	finline Flag_Type AssignField(Flag_Type flag, LSB lsb, L len, V val)
	{	return assignb(flag
					  , ((1<<(len)) - 1)*((Flag_Type)lsb)
					  ,(( (Flag_Type)val)&((Flag_Type)((1<<(len)) - 1)))*((Flag_Type)lsb));	}

    // 函数已经被弃用
	/*	@brief	得到一个变量的一个 bit field 被设置为指定的值后的变量值，
     *         这个函数本身不会改变变量的值
     *         得到变量的连续几个位被赋值后的值。
     *   @note   这个函数不会改变传入的变量的值
	 *	@param
	 *		flag: 一个变量
	 *		bits: 连续的几个位
	 *		val : bits表示的几个位要被赋的值
 	 *	@note	val 不需要预先移位，这个函数和assignb的使用不同。
	 *	@example
	 *			u16 a = 0;	a = assignbs(a, BitFromTo(11, 4), 0x5a);	//那么执行完后，a = 0x05a0;
	 */
	template<class Flag_Type, class BITS, class V>
	tinline Flag_Type AssignField(Flag_Type flag, BITS bits, V val)
	{	return assignb(flag, bits, (((Flag_Type)val)*((Flag_Type)( GetLSB(bits) ))));	}
	#define	assignf		AssignField			// assign bit field

	/**	@brief	读一个变量的一个 bit field 的值 */
	template<class Flag_Type, class BITS>
	tinline Flag_Type ReadField(Flag_Type flag, BITS bits)
	{ return (Flag_Type)((flag & bits)/(Flag_Type)GetLSB(bits));	}
	#define readf		ReadField


	/**	@brief	得到一个常量对应的 bit number. 例如:
	  *	@code
	  * bit_number<1>::value == 0, bit_number<Bit(7)> == 7
	  * @endcode
	  */
	template<u32 N>
	struct bit_number{
        /* 参数只能有一位被置位 */
        STATIC_ASSERT(u32IsSingleBit<N>::result);
		static const u8 value = bit_number<N/2>::value+1;
	};
	template<>	/* 特化 */
	struct bit_number<1>{
		static const u8 value = 0;
	};

	/** @brief 位置1函数，将一个变量的指定的位置1。
	  *	@param p 指向要操作的变量。该参数任意类型的整数。
	  *	@param b 指定要置1的位。该参数任意类型的整数。
	  */
    template<class Flag_Type, class bits> tinline
    Flag_Type& set_bits(Flag_Type* p, bits b)
	{
		return ((*p) |= ((Flag_Type)b));
	}
	/** @brief 位清零函数，将一个变量的指定的位清零。
	  *	@param p 指向要操作的变量。该参数任意类型的整数。
	  *	@param b 指定要清零的位。该参数任意类型的整数。
	  */
	template<class Flag_Type, class Bits_Type> tinline
	Flag_Type& clear_bits(Flag_Type* const p, Bits_Type const bits)
	{
		return ((*p) &= (~((Flag_Type)bits)));
	}
	/** @brief 位取反函数，将一个变量的指定的位取反。
	  *	@param p 指向要操作的变量。该参数任意类型的整数。
	  *	@param b 指定要取反的位。该参数任意类型的整数。
	  */
    template<class Flag_Type, class Bits_Type> tinline
	Flag_Type& toggle_bits(Flag_Type* const p, Bits_Type bits){
		return ((*p) ^= ((Flag_Type)bits));
    }

	/** @brief 位域插入函数。给一个变量的指定位域插入指定的数值。
	  * @param <field> u32型常量，用于指定一个位域。
	  *		@note field 不可以同时指定多个位域。
	  *	@param var 要插入数据的变量的地址（使用指针可以明确这个函数会对参数
	  *	进行修改。
	  *		@note var 指向的无符号整数类型的大小不可以大于4个字节。
	  *	@param insert_val 要插入的数值，例如 int a = 0x55;
	  *	insert_field<0xf0>(&a,0xa); 执行完后 a==0xa5 。
	  */
    template<u32 field, typename T> tinline
	T& field_insert(T* var, u32 insert_val){
        enum{ lsb
                = (((field<<1)|field) ^ (field<<1)),
              only_one_field = u32IsSingleBit<lsb>::result
		};
        /* 不支持大于 32 位的变量，不支持参数为多个 field，
           如果在这里产生了编译期错误，说明在使用
           CBitField 的时候使用了非法的参数，参数
           的大小不可以超过 4 个字节，而且参数
           中只能指定一个 field。
        */
        STATIC_ASSERT((sizeof(T)<=4)&&only_one_field);
        /* 设置的值不能太大 */
        assert(insert_val*lsb <= field);
		return *var=(((*var)&(~field)) |
            ((insert_val)*lsb));
	}

	/** @brief 位域提取函数。读一个变量的指定位域的值。
	  * @param <field> u32型常量，用于指定一个位域。
	  *		@note field 不可以同时指定多个位域。
	  *	@param var 任意类型的无符号整数变量。
	  *		@note var 所属的类型不能大于4个字节。
	  * @return 读到的指定位域的数值。如 field_extract<0xf>(0x78)==0x7。
	  */
    template<u32 field, typename T> tinline
    u32 field_extract(const T& var){
        enum{ lsb
                = (((field<<1)|field)^(field<<1)),
              only_one_field = u32IsSingleBit<lsb>::result
        };
        /* 不支持大于 32 位的变量，不支持参数为多个 field，
           如果在这里产生了编译期错误，说明在使用
           CBitField 的时候使用了非法的参数，参数
           的大小不可以超过 4 个字节，而且参数
           中只能指定一个 field。
        */
        STATIC_ASSERT((sizeof(T)<=4)&&only_one_field);
        return ((var&field)>>bit_number<lsb>::value);
    }

    /****************************************************
	function: BitField<u32 field>(variable)
	parameter:
		field: 指定位域
		variable: 指定变量
	description: 这是一个专门为位域的读写而封装的函数
	example:
		对一个变量 variable 的 D4 - D7 位进行赋值为
		value （位域写）：
			BitField<BitFromTo(7,4), variable> = value;
			若执行前 variable == 0xf0f0 且 value == 0x5
			则执行后 variable == 0xf050
		将 variable 的 D4 - D7 位的值读到变量 temp 中
		（位域读）：
			temp = BitField<BitFromTo(7,4),variable);
			若执行前 variable == 0xf070
			则执行后 temp == 0x7
	remark: 对于写操作，采用的是 读-修定-写 的方式，所以
		在多线程的程序中应该考虑代码的线程安全性。
	****************************************************/
	template<u32 field, typename T>
	class CBitField{

        enum{ least_significant_bit
                = (((field<<1)|field) ^ (field<<1)),
            only_one_field = u32IsSingleBit<least_significant_bit>::result
		};
        /* 不支持大于 32 位的变量，不支持参数为多个 field，
           如果在这里产生了编译期错误，说明在使用
           CBitField 的时候使用了非法的参数，参数
           的大小不可以超过 4 个字节，而且参数
           中只能指定一个 field。
        */
        STATIC_ASSERT((sizeof(T)<=4)&&only_one_field);
		T* const t;
		public:
		finline CBitField(T* init):t(init){}
		/* 为使用方式简洁，不返回任何内容 */
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
