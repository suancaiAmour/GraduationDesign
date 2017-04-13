#ifndef STM32F10X__COMMON_H
#define STM32F10X__COMMON_H

#include "stm32f10x.h"

#include "define.h"
#include "nstd_type.h"
#include "BitOperation.h"
#include "nstd_assert.h"

namespace core{
	using namespace nstd;
	typedef uint32_t addr_uint;	/*!< ����������ʾ��ַ������ */
	
	/**	@brief BitBandPtr: �õ�һ��������ĳһλ��λ���ж�Ӧ�ĵ�ַָ��
	*	@param[i] bit_mask: ģ������������ڳ�����������������һλ
	*		@note bit_mask ������ֻ����һ��λ�Ǳ���λ��
	*	@param[i] p: �����ĵ�ַָ��
	*	@example	// ʹ��GPIOA��������Pin8Ϊ�����
	*				volatile uint32_t* p = BitBandPtr<Bit(8)>(GPIOA->ODR));
	*				*p = 1;		// ODR��bit8λ��д1��pa8 ��� 1
	*	@note ���������һ���������ﾡ����ҪǶ�׵��ã���������������Ż�
	*	����(������ǿ�������ĺ���)���� struct XXX{
	*		fun1(){...BitBandPtr...}
	*		fun2()	{ ... fun1() ...}
	*	}
	*	��������£������������޷��� fun2 �е��õ� fun1 �����Ż�
	*/
	template<u32 bit_mask> finline
	volatile const uint32_t * BitBandPtr(volatile const uint32_t& p) {
		/*
		ģ����� bit_mask ������ֻ��һ��λΪ 1 ��
		�����������ʾ error:  #70: incomplete type is not allowed
		˵�����ģ���ʹ���д��󣬶���������Ĵ��������⡣
		����ϸ��������ʾ�Ǳ��뵽�ĸ�Դ�ļ�������ٵ��Ǹ�Դ�ļ�ȥ
		�ҳ���Ӧ�Ĵ���
		*/
		STATIC_ASSERT(u32IsSingleBit<bit_mask>::Result);
		const uint32_t addr = (uint32_t)&p;
		return (__I uint32_t *)((addr & 0xF0000000)+0x2000000+
				((addr & 0xFFFFF)<<5)+(nstd::bit_number<bit_mask>::value<<2));
	}
	template<u32 bit_mask> finline
	volatile uint32_t * BitBandPtr(volatile uint32_t& p) {
		/*
		ģ����� bit_mask ������ֻ��һ��λΪ 1 ��
		�����������ʾ error:  #70: incomplete type is not allowed
		˵�����ģ���ʹ���д��󣬶���������Ĵ��������⡣
		����ϸ��������ʾ�Ǳ��뵽�ĸ�Դ�ļ�������ٵ��Ǹ�Դ�ļ�ȥ
		�ҳ���Ӧ�Ĵ���
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
		ģ����� bit_mask ������ֻ��һ��λΪ 1 ��
		�����������ʾ error:  #70: incomplete type is not allowed
		˵�����ģ���ʹ���д��󣬶���������Ĵ��������⡣
		����ϸ��������ʾ�Ǳ��뵽�ĸ�Դ�ļ�������ٵ��Ǹ�Դ�ļ�ȥ
		�ҳ���Ӧ�Ĵ���
		*/
		STATIC_ASSERT(u32IsSingleBit<bit_mask>::Result);
		const uint32_t addr = (uint32_t)&p;
		return (__I uint32_t *)((addr & 0xF0000000)+0x2000000+
				((addr & 0xFFFFF)<<5)+(nstd::bit_number<bit_mask>::value<<2));
	}
	template<u32 bit_mask> finline
	volatile uint32_t * BitBandPtr(volatile uint16_t& p) {
		/*
		ģ����� bit_mask ������ֻ��һ��λΪ 1 ��
		�����������ʾ error:  #70: incomplete type is not allowed
		˵�����ģ���ʹ���д��󣬶���������Ĵ��������⡣
		����ϸ��������ʾ�Ǳ��뵽�ĸ�Դ�ļ�������ٵ��Ǹ�Դ�ļ�ȥ
		�ҳ���Ӧ�Ĵ���
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

/* Ϊ�˷��㿪������һ���Ȳ����з��ظ��������� */

/**	@brief ����Ϊ�Ĵ�����������У�Ϊ�˷���Ͱ�ȫ��ʹ�ã�
*	�Ĵ�����ص�λ���Զ���� enum{ REG_BIT } �ĸ�ʽ������
*	��һλд 1 ʱ���Ϳ���ֱ���� PRegBit(REG, BIT) = 1 �ķ�
*	ʽ�������ˡ�
*/
#define PRegBit(reg, bit)		BitBandPtr<reg##_##bit>(reg)

/** @brief	�� BIT_PLUS_OPERATOR_SUPPORT ��������ι���ö�����Ͷ�֧���������
*			��ӵĽ�����Ǹ�ö�����͵����ݣ�����൱��һ�����ϣ�ͬʱ������������
*	@example gpioa.Config(Pin1+Pin2, OUTPUT);	// �� pa1 �� pa2 ������Ϊ���
*/
#define BIT_OR_OPERATOR_SUPPORT(enum_type)	\
finline enum_type operator | (const enum_type a, const enum_type b) pure_function {return (enum_type)(((u32)a)|((u32)b));}



