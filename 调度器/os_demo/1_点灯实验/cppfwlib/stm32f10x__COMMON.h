#ifndef STM32F10X__COMMON_H
#define STM32F10X__COMMON_H

#include "stm32f10x.h"

#include "define.h"
#include "nstd_type.h"
#include "BitOperation.h"
#include "nstd_assert.h"

namespace core{
	using namespace nstd;
	typedef uint32_t addr_uint;	/*!< 可以用来表示地址的整数 */
	
	/**	@brief BitBandPtr: 得到一个变量的某一位在位带中对应的地址指针
	*	@param[i] bit_mask: 模板参数，编译期常量，用来声明是哪一位
	*		@note bit_mask 中有且只能有一个位是被置位的
	*	@param[i] p: 变量的地址指针
	*	@example	// 使能GPIOA并且配置Pin8为输出后
	*				volatile uint32_t* p = BitBandPtr<Bit(8)>(GPIOA->ODR));
	*				*p = 1;		// ODR的bit8位被写1，pa8 输出 1
	*	@note 这个函数在一个外设类里尽量不要嵌套调用，否则编译器可能优化
	*	不好(即便是强制内联的函数)。如 struct XXX{
	*		fun1(){...BitBandPtr...}
	*		fun2()	{ ... fun1() ...}
	*	}
	*	这种情况下，编译器可能无法对 fun2 中调用的 fun1 进行优化
	*/
	template<u32 bit_mask> finline
	volatile const uint32_t * BitBandPtr(volatile const uint32_t& p) {
		/*
		模板参数 bit_mask 必须是只有一个位为 1 的
		如果编译器提示 error:  #70: incomplete type is not allowed
		说明你对模板的使用有错误，而不是这里的代码有问题。
		请仔细看编译提示是编译到哪个源文件后出错，再到那个源文件去
		找出相应的错误。
		*/
		STATIC_ASSERT(u32IsSingleBit<bit_mask>::Result);
		const uint32_t addr = (uint32_t)&p;
		return (__I uint32_t *)((addr & 0xF0000000)+0x2000000+
				((addr & 0xFFFFF)<<5)+(nstd::bit_number<bit_mask>::value<<2));
	}
	template<u32 bit_mask> finline
	volatile uint32_t * BitBandPtr(volatile uint32_t& p) {
		/*
		模板参数 bit_mask 必须是只有一个位为 1 的
		如果编译器提示 error:  #70: incomplete type is not allowed
		说明你对模板的使用有错误，而不是这里的代码有问题。
		请仔细看编译提示是编译到哪个源文件后出错，再到那个源文件去
		找出相应的错误。
		*/
		STATIC_ASSERT(u32IsSingleBit<bit_mask>::result);
		const uint32_t addr = (uint32_t)&p;
		return (volatile uint32_t *)((addr & 0xF0000000)+0x2000000+
			((addr & 0xFFFFF)<<5)+(nstd::bit_number<bit_mask>::value<<2));
	}
    
    inline
	volatile uint32_t * bit_band_ptr(volatile uint32_t& p, u32 bit_mask ) {
		u32 bit_num=0;
        assert(bit_mask);
        for(;(bit_mask&0x01)==0;bit_mask>>=1,bit_num++);
        assert((bit_mask-1)==0);
		const uint32_t addr = (uint32_t)&p;
		return (volatile uint32_t *)((addr & 0xF0000000)+0x2000000+
			((addr & 0xFFFFF)<<5)+(bit_num<<2));
	}
    
	template<u32 bit_mask> finline
	volatile const uint32_t * BitBandPtr(volatile const uint16_t& p) {
		/*
		模板参数 bit_mask 必须是只有一个位为 1 的
		如果编译器提示 error:  #70: incomplete type is not allowed
		说明你对模板的使用有错误，而不是这里的代码有问题。
		请仔细看编译提示是编译到哪个源文件后出错，再到那个源文件去
		找出相应的错误。
		*/
		STATIC_ASSERT(u32IsSingleBit<bit_mask>::Result);
		const uint32_t addr = (uint32_t)&p;
		return (__I uint32_t *)((addr & 0xF0000000)+0x2000000+
				((addr & 0xFFFFF)<<5)+(nstd::bit_number<bit_mask>::value<<2));
	}
	template<u32 bit_mask> finline
	volatile uint32_t * BitBandPtr(volatile uint16_t& p) {
		/*
		模板参数 bit_mask 必须是只有一个位为 1 的
		如果编译器提示 error:  #70: incomplete type is not allowed
		说明你对模板的使用有错误，而不是这里的代码有问题。
		请仔细看编译提示是编译到哪个源文件后出错，再到那个源文件去
		找出相应的错误。
		*/
		STATIC_ASSERT(u32IsSingleBit<bit_mask>::result);
		const uint32_t addr = (uint32_t)&p;
		return (volatile uint32_t *)((addr & 0xF0000000)+0x2000000+
			((addr & 0xFFFFF)<<5)+(nstd::bit_number<bit_mask>::value<<2));
	}
    
    
}	/* namespace core */

namespace periph{
	using namespace nstd;
	using namespace core;
}

#endif

/* 为了方便开发，这一段先不进行防重复包含处理 */

/**	@brief 用在为寄存器定义的类中，为了方便和安全的使用，
*	寄存器相关的位可以定义成 enum{ REG_BIT } 的格式，在向
*	那一位写 1 时，就可以直接用 PRegBit(REG, BIT) = 1 的方
*	式来操作了。
*/
#define PRegBit(reg, bit)		BitBandPtr<reg##_##bit>(reg)

/** @brief	由 BIT_PLUS_OPERATOR_SUPPORT 这个宏修饰过的枚举类型都支持相加运算
*			相加的结果还是该枚举类型的数据，结果相当于一个集合，同时包含两个加数
*	@example gpioa.Config(Pin1+Pin2, OUTPUT);	// 把 pa1 和 pa2 都配置为输出
*/
#define BIT_OR_OPERATOR_SUPPORT(enum_type)	\
finline enum_type operator | (const enum_type a, const enum_type b) pure_function {return (enum_type)(((u32)a)|((u32)b));}



