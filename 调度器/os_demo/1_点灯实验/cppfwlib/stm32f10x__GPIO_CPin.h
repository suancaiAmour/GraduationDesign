#ifndef GPIO_CPIN_H
#define GPIO_CPIN_H

/**	@brief	������һЩ�꣬ʹ��λ��������ʵ�ֵ���IO���ŵĶ�д�Ĺ��ܡ�
*/
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr & 0xFFFFF)<<5)+(bitnum<<2)) 

#define MEM_ADDR(addr)  *((volatile unsigned long *)(addr)) 

#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

namespace periph{

	/**	@brief	GPIO_CPin, Ϊÿ�����Ŷ���һ���� */
	template< uint32_t port_base, GPIO_Pin pin>
	class GPIO_CPin {
		typedef uint32_t mem_addr;
		typedef volatile unsigned long * vmem_ptr;
		
		enum{
			  ODR_OFFSET = offsetof(GPIO_TypeDef, ODR)	/*!< ODR ƫ�� */
			, IDR_OFFSET = offsetof(GPIO_TypeDef, IDR)	/*!< IDR ƫ�� */
		
			, odr_bit = BITBAND(port_base+ODR_OFFSET, bit_number<pin>::value)	/*!< ��Ӧ�� ODR λ�ĵ�ַ */
			, idr_bit = BITBAND(port_base+IDR_OFFSET, bit_number<pin>::value)	/*!< ��Ӧ�� IDR λ�ĵ�ַ */
		};
		static finline GPIO_Class* GetPort()		{return (GPIO_Class*)(port_base);}
		
	public:	/*-------------------- public method decleration begin --------------------*/
		
		enum{
			  PORT_BASE = port_base
			, PIN = pin
		};
	
		static void finline Config(const GPIO_Input input, const InputStyle style = PULL_UP)
						{GetPort()->Config(pin, input, style);}
		static void finline Config(const GPIO_Output output, const OutStyle style = OPEN_DRAIN
						,const GPIO_OutSpeed speed = SPEED_10M )
						{GetPort()->Config(pin, output, style, speed);}
		
		/*	@brief	�õ����ŵ�������ݼĴ���(ODR)��Ӧ��λ��λ���еĵ�ַ��������Ϊ�����ʱ��
					�������ַ������д 1������������ߵ�ƽ��д 0������������͵�ƽ��
		*/
		finline volatile unsigned long* GetODR_BitAddr() const { return (vmem_ptr)odr_bit;}
		/*	@brief	�õ����ŵ��������ݼĴ���(IDR)��Ӧ��λ��λ���еĵ�ַ��������Ϊ�����ʱ��
					������� 1��˵�����ŵĵ�ǰ״̬Ϊ�ߵ�ƽ��������� 0��˵�����ŵĵ�ǰ״̬Ϊ�͵�ƽ
		*/
		finline const volatile unsigned long* GetIDR_BitAddr() const { return (vmem_ptr)idr_bit;}
						
		/**	@brief	������Ϊ(�����ù���������)���ģʽ��ʱ������ = ������������д����
			@example pa8 = 1; // �� pa8 д 1��ʹ pa8 ����͵�ƽ
		*/
		finline void operator =(const volatile u32 val) {*GetODR_BitAddr() = val; } /*!< �������κ�ֵ����ֹʹ�÷�����ģ�� */
		/**	@brief	����������
		*	@expmple u32 a = pa8; // �� pa8 �����ݲ����浽���� a �У��������Ϊ�ߵ�ƽ���� a==1������ a==0
		*/
		finline operator u32() const {return *GetIDR_BitAddr();}
		
		/**	@brief ���ַ�ʽ��λ���Ĳ����ķ�ʽ����һЩ */	
		finline void Set()	{((GPIO_TypeDef*)(port_base))->BSRR=pin;}
		finline void Reset()	{((GPIO_TypeDef*)(port_base))->BRR=pin;}
		/*-------------------- public method decleration end --------------------*/
	};
		
	extern GPIO_CPin<GPIOA_BASE, Pin0>	pa0;
	extern GPIO_CPin<GPIOA_BASE, Pin1>	pa1;
	extern GPIO_CPin<GPIOA_BASE, Pin2>	pa2;
	extern GPIO_CPin<GPIOA_BASE, Pin3>	pa3;
	extern GPIO_CPin<GPIOA_BASE, Pin4>	pa4;
	extern GPIO_CPin<GPIOA_BASE, Pin5>	pa5;
	extern GPIO_CPin<GPIOA_BASE, Pin6>	pa6;
	extern GPIO_CPin<GPIOA_BASE, Pin7>	pa7;
	extern GPIO_CPin<GPIOA_BASE, Pin8>	pa8;
	extern GPIO_CPin<GPIOA_BASE, Pin9>	pa9;
	extern GPIO_CPin<GPIOA_BASE, Pin10>	pa10;
	extern GPIO_CPin<GPIOA_BASE, Pin11>	pa11;
	extern GPIO_CPin<GPIOA_BASE, Pin12>	pa12;
	extern GPIO_CPin<GPIOA_BASE, Pin13>	pa13;
	extern GPIO_CPin<GPIOA_BASE, Pin14>	pa14;
	extern GPIO_CPin<GPIOA_BASE, Pin15>	pa15;

	extern GPIO_CPin<GPIOB_BASE, Pin0>	pb0;
	extern GPIO_CPin<GPIOB_BASE, Pin1>	pb1;
	extern GPIO_CPin<GPIOB_BASE, Pin2>	pb2;
	extern GPIO_CPin<GPIOB_BASE, Pin3>	pb3;
	extern GPIO_CPin<GPIOB_BASE, Pin4>	pb4;
	extern GPIO_CPin<GPIOB_BASE, Pin5>	pb5;
	extern GPIO_CPin<GPIOB_BASE, Pin6>	pb6;
	extern GPIO_CPin<GPIOB_BASE, Pin7>	pb7;
	extern GPIO_CPin<GPIOB_BASE, Pin8>	pb8;
	extern GPIO_CPin<GPIOB_BASE, Pin9>	pb9;
	extern GPIO_CPin<GPIOB_BASE, Pin10>	pb10;
	extern GPIO_CPin<GPIOB_BASE, Pin11>	pb11;
	extern GPIO_CPin<GPIOB_BASE, Pin12>	pb12;
	extern GPIO_CPin<GPIOB_BASE, Pin13>	pb13;
	extern GPIO_CPin<GPIOB_BASE, Pin14>	pb14;
	extern GPIO_CPin<GPIOB_BASE, Pin15>	pb15;

	extern GPIO_CPin<GPIOC_BASE, Pin0>	pc0;
	extern GPIO_CPin<GPIOC_BASE, Pin1>	pc1;
	extern GPIO_CPin<GPIOC_BASE, Pin2>	pc2;
	extern GPIO_CPin<GPIOC_BASE, Pin3>	pc3;
	extern GPIO_CPin<GPIOC_BASE, Pin4>	pc4;
	extern GPIO_CPin<GPIOC_BASE, Pin5>	pc5;
	extern GPIO_CPin<GPIOC_BASE, Pin6>	pc6;
	extern GPIO_CPin<GPIOC_BASE, Pin7>	pc7;
	extern GPIO_CPin<GPIOC_BASE, Pin8>	pc8;
	extern GPIO_CPin<GPIOC_BASE, Pin9>	pc9;
	extern GPIO_CPin<GPIOC_BASE, Pin10>	pc10;
	extern GPIO_CPin<GPIOC_BASE, Pin11>	pc11;
	extern GPIO_CPin<GPIOC_BASE, Pin12>	pc12;
	extern GPIO_CPin<GPIOC_BASE, Pin13>	pc13;
	extern GPIO_CPin<GPIOC_BASE, Pin14>	pc14;
	extern GPIO_CPin<GPIOC_BASE, Pin15>	pc15;
	
	extern GPIO_CPin<GPIOD_BASE, Pin0>	pd0;
	extern GPIO_CPin<GPIOD_BASE, Pin1>	pd1;
	extern GPIO_CPin<GPIOD_BASE, Pin2>	pd2;
	extern GPIO_CPin<GPIOD_BASE, Pin3>	pd3;
	extern GPIO_CPin<GPIOD_BASE, Pin4>	pd4;
	extern GPIO_CPin<GPIOD_BASE, Pin5>	pd5;
	extern GPIO_CPin<GPIOD_BASE, Pin6>	pd6;
	extern GPIO_CPin<GPIOD_BASE, Pin7>	pd7;
	extern GPIO_CPin<GPIOD_BASE, Pin8>	pd8;
	extern GPIO_CPin<GPIOD_BASE, Pin9>	pd9;
	extern GPIO_CPin<GPIOD_BASE, Pin10>	pd10;
	extern GPIO_CPin<GPIOD_BASE, Pin11>	pd11;
	extern GPIO_CPin<GPIOD_BASE, Pin12>	pd12;
	extern GPIO_CPin<GPIOD_BASE, Pin13>	pd13;
	extern GPIO_CPin<GPIOD_BASE, Pin14>	pd14;
	extern GPIO_CPin<GPIOD_BASE, Pin15>	pd15;

#define pa0_template_param_helper		GPIOA_BASE, Pin0
#define pa1_template_param_helper		GPIOA_BASE, Pin1
#define pa2_template_param_helper		GPIOA_BASE, Pin2
#define pa3_template_param_helper		GPIOA_BASE, Pin3
#define pa4_template_param_helper		GPIOA_BASE, Pin4
#define pa5_template_param_helper		GPIOA_BASE, Pin5
#define pa6_template_param_helper		GPIOA_BASE, Pin6
#define pa7_template_param_helper		GPIOA_BASE, Pin7
#define pa8_template_param_helper		GPIOA_BASE, Pin8
#define pa9_template_param_helper		GPIOA_BASE, Pin9
#define pa10_template_param_helper		GPIOA_BASE, Pin10
#define pa11_template_param_helper		GPIOA_BASE, Pin11
#define pa12_template_param_helper		GPIOA_BASE, Pin12
#define pa13_template_param_helper		GPIOA_BASE, Pin13
#define pa14_template_param_helper		GPIOA_BASE, Pin14
#define pa15_template_param_helper		GPIOA_BASE, Pin15

#define pb0_template_param_helper		GPIOB_BASE, Pin0
#define pb1_template_param_helper		GPIOB_BASE, Pin1
#define pb2_template_param_helper		GPIOB_BASE, Pin2
#define pb3_template_param_helper		GPIOB_BASE, Pin3
#define pb4_template_param_helper		GPIOB_BASE, Pin4
#define pb5_template_param_helper		GPIOB_BASE, Pin5
#define pb6_template_param_helper		GPIOB_BASE, Pin6
#define pb7_template_param_helper		GPIOB_BASE, Pin7
#define pb8_template_param_helper		GPIOB_BASE, Pin8
#define pb9_template_param_helper		GPIOB_BASE, Pin9
#define pb10_template_param_helper		GPIOB_BASE, Pin10
#define pb11_template_param_helper		GPIOB_BASE, Pin11
#define pb12_template_param_helper		GPIOB_BASE, Pin12
#define pb13_template_param_helper		GPIOB_BASE, Pin13
#define pb14_template_param_helper		GPIOB_BASE, Pin14
#define pb15_template_param_helper		GPIOB_BASE, Pin15

#define pc0_template_param_helper		GPIOC_BASE, Pin0
#define pc1_template_param_helper		GPIOC_BASE, Pin1
#define pc2_template_param_helper		GPIOC_BASE, Pin2
#define pc3_template_param_helper		GPIOC_BASE, Pin3
#define pc4_template_param_helper		GPIOC_BASE, Pin4
#define pc5_template_param_helper		GPIOC_BASE, Pin5
#define pc6_template_param_helper		GPIOC_BASE, Pin6
#define pc7_template_param_helper		GPIOC_BASE, Pin7
#define pc8_template_param_helper		GPIOC_BASE, Pin8
#define pc9_template_param_helper		GPIOC_BASE, Pin9
#define pc10_template_param_helper		GPIOC_BASE, Pin10
#define pc11_template_param_helper		GPIOC_BASE, Pin11
#define pc12_template_param_helper		GPIOC_BASE, Pin12
#define pc13_template_param_helper		GPIOC_BASE, Pin13
#define pc14_template_param_helper		GPIOC_BASE, Pin14
#define pc15_template_param_helper		GPIOC_BASE, Pin15

#define pd0_template_param_helper		GPIOD_BASE, Pin0
#define pd1_template_param_helper		GPIOD_BASE, Pin1
#define pd2_template_param_helper		GPIOD_BASE, Pin2
#define pd3_template_param_helper		GPIOD_BASE, Pin3
#define pd4_template_param_helper		GPIOD_BASE, Pin4
#define pd5_template_param_helper		GPIOD_BASE, Pin5
#define pd6_template_param_helper		GPIOD_BASE, Pin6
#define pd7_template_param_helper		GPIOD_BASE, Pin7
#define pd8_template_param_helper		GPIOD_BASE, Pin8
#define pd9_template_param_helper		GPIOD_BASE, Pin9
#define pd10_template_param_helper		GPIOD_BASE, Pin10
#define pd11_template_param_helper		GPIOD_BASE, Pin11
#define pd12_template_param_helper		GPIOD_BASE, Pin12
#define pd13_template_param_helper		GPIOD_BASE, Pin13
#define pd14_template_param_helper		GPIOD_BASE, Pin14
#define pd15_template_param_helper		GPIOD_BASE, Pin15

#define pe0_template_param_helper		GPIOE_BASE, Pin0
#define pe1_template_param_helper		GPIOE_BASE, Pin1
#define pe2_template_param_helper		GPIOE_BASE, Pin2
#define pe3_template_param_helper		GPIOE_BASE, Pin3
#define pe4_template_param_helper		GPIOE_BASE, Pin4
#define pe5_template_param_helper		GPIOE_BASE, Pin5
#define pe6_template_param_helper		GPIOE_BASE, Pin6
#define pe7_template_param_helper		GPIOE_BASE, Pin7
#define pe8_template_param_helper		GPIOE_BASE, Pin8
#define pe9_template_param_helper		GPIOE_BASE, Pin9
#define pe10_template_param_helper		GPIOE_BASE, Pin10
#define pe11_template_param_helper		GPIOE_BASE, Pin11
#define pe12_template_param_helper		GPIOE_BASE, Pin12
#define pe13_template_param_helper		GPIOE_BASE, Pin13
#define pe14_template_param_helper		GPIOE_BASE, Pin14
#define pe15_template_param_helper		GPIOE_BASE, Pin15

/**	@brief ��������һ������ 
  *	@example 
  * PinRename(pa8, led0); // �Ӵ� led0 �ʹ��� pa8
  *	led = 0;	// �൱�� pa8 = 0;
  */
#define PinRename(p,name)		static GPIO_CPin<p##_template_param_helper> name

}

#endif
