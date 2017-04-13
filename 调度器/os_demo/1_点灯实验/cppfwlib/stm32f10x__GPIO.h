#ifndef GPIO_TYPEDEF_H
#define GPIO_TYPEDEF_H

#include "stm32f10x__COMMON.h"

namespace periph{
	/**	@brief	引脚
	*/
	enum GPIO_Pin{	
	  Pin0 = Bit(0)
	, Pin1 = Bit(1)
	, Pin2 = Bit(2)
	, Pin3 = Bit(3)
	, Pin4 = Bit(4)
	, Pin5 = Bit(5)
	, Pin6 = Bit(6)
	, Pin7 = Bit(7)
	, Pin8 = Bit(8)
	, Pin9 = Bit(9)
	, Pin10 = Bit(10)
	, Pin11 = Bit(11)
	, Pin12 = Bit(12)
	, Pin13 = Bit(13)
	, Pin14 = Bit(14)
	, Pin15 = Bit(15)
	, PinAll= BitFromTo(15, 0)		/*!< 全选 */
	};
	
	/**	@brief	INPUT 用作 GPIO_Class::Config 的参数，表示将IO口配置为输入模式。*/
	enum GPIO_Input		{	
		  INPUT		= 0	};
	/**	@brief	输入的方式 */
	typedef enum GPIO_InputStyle{			
		  ANALOG	= BByte(0000)			/*!< 模拟输入 */
		, FLOATING	= BByte(0100)			/*!< 浮空输入 */
		// 对于上拉下拉，低 4 位 bit 3:0 是根据手册上的值来设置的，
		// 而 bit 4 为 1 表示上拉，为 0 表示下拉，由软件识别再进行相应的处理。
		, PULL_UP	= BByte(11000)			/*!< 有上拉电阻 */
		, PULL_DOWN	= BByte(01000)			/*!< 有下拉电阻 */
	}InputStyle;
	
	
	/**	@brief	输出的类型 */
	enum GPIO_Output	{			//输出，	
		OUTPUT		= 0,				/*!< 普通的输出模式 */
		AF_OUTPUT	= BByte(1000)		/*!< Alternate Function output */
	};
	/**	@brief	输出方式 */
	typedef enum GPIO_OutStyle	{				
		PUSH_PULL	= BByte(0000),	/*!< 推挽输出 */
		OPEN_DRAIN	= BByte(0100)	/*!< 开漏输出 */
	}OutStyle;
	/**	@brief	输出的速度 */
	enum GPIO_OutSpeed	{				
		SPEED_10M	= BByte(0001),
		SPEED_2M	= BByte(0010),
		SPEED_50M	= 	BByte(0011)
	};

	namespace gpio_reg{
		
		class GPIO_Class: protected GPIO_TypeDef
		{
		protected:
			GPIO_Class(){};	//不允许外部建立对象!

          
            static const u16 pin_field_map[16];
            
            /**	@brief	配置IO口的模式，这个函数使用比较复杂，不建议用户使用。
				如果有特殊需求，则可能需要使用这个函数来实现。用来设置IO口的模式。 */
            void SetMode(const u16 pins, const u8 mode){
                u32 field;
                if(pins&BitFromTo(7,0)){    // 低 8 位引脚
                    field = pin_field_map[pins&0xf]
                                | (pin_field_map[(pins>>4)&0xf]<<16);
                    CRL = (CRL&(~(field*0xf))) 
                            | (field*(mode&0xf));
                }
                if(pins&BitFromTo(15,8)){    // 高 8 位引脚
                    field = pin_field_map[(pins>>8)&0xf]
                                | (pin_field_map[(pins>>12)&0xf]<<16);
                    CRH = (CRH&(~(field*0xf))) 
                            | (field*(mode&0xf));
                }
            }
            
			/**	@brief	配置IO口的模式，这个函数使用比较复杂，不建议用户使用。
				如果有特殊需求，则可能需要使用这个函数来实现。用来设置IO口的模式。 */
// 			finline void SetMode(const u16 pins,const u8 mode)
// 			{
// 				if(checkb(pins, BitFromTo(7, 0))) {	//低8位
// 					(this->CRL = AssignField(this->CRL, ExpandU8Bit(pins,4) , 4, mode));
// 				}
// 				if(checkb(pins, BitFromTo(15, 8))) {	//高8位
// 					 this->CRH = AssignField( this->CRH, ExpandU8Bit(((pins)>>8),4) , 4, mode);
// 				}
// 			};
            
		public:
            
            /** @brief 把端口的所有引脚配置为指定的输入工作模式
              */
            finline void Config(const GPIO_Input input,const InputStyle style = PULL_UP){
                u32 temp = 0x11111111*(input|style);
                CRL = temp;
                CRH = temp;
                if(style == PULL_DOWN)
					BRR = 0xffff;
				else if(style == PULL_UP)
					BSRR = 0xffff;
            }
            finline 
            void Config(const GPIO_Output output
                        ,const OutStyle style = OPEN_DRAIN
                        ,const GPIO_OutSpeed speed = SPEED_10M )
			{
                u32 temp = 0x11111111*(output | style | speed);
                CRL = temp;
                CRH = temp;
            }
        
			/**	@brief	配置IO口的输入模式
			*	@param	pins: 指定的引脚，从 @ref GPIO_Pin 枚举类的成员，允许使用 Pinxx + Pinyy + ... 的形式作为函数的参数。
			*		@note	pins 这个参数必须是编译期的常量，否则编译器无法进行自动优化，导致生成比较多的代码。
			*	@param	input: 表示把引脚配置成输入，参考 @ref GPIO_Input 的定义，在这里只能为 INPUT。
			*	@param	style: 输入的方式，参考 @ref InputStyle 的定义。
			*	@example gpioa.Config(Pin0+Pin3, INPUT, PULL_DOWN);	// 把 PA0 和 PA3 设置为下拉输入
			*/
            inline
            void Config(const u16 pins
                            ,const GPIO_Input  input
                            ,const InputStyle style = PULL_UP)  {	
				SetMode( pins, input | style );
				/*!< 设置上/下拉电阻放在设置完模式后执行，
					因为：	1. 用户不可能在这个函数在执行的时候去读管脚的值
							2. 如果之前为推挽输出，随便设置可能会损坏器件。*/
				if(style == PULL_DOWN)		/*!< 如果是需要设置为有 上拉/下拉电阻 输入的 */
					BRR = pins;
				else if(style == PULL_UP)
					BSRR = pins;
			}

			
			/** @brief	配置IO口的输出模式
			  * @param	pins: 指定的引脚，从 @ref GPIO_Pin 枚举类的成员，允许使用 Pinxx + Pinyy + ... 的形式作为函数的参数。
			  *     @note   pins 这个参数必须是编译期的常量，否则编译器无法进行自动优化，导致生成比较多的代码。
			  * @param	output: 参考 @ref GPIO_Output 的定义，可以是 OUTPUT (普通输出) 或 AF_OUTPUT (复用功能输出)
			  * @param	style: 输出方式的配置，参考 @ref OutStyle 的定义。
			  * @param	speed: 指定IO口的速度，参考 @ref OutSpeed。
              * @example gpiob.Config(Pin0+Pin1+Pin2, OUTPUT, PUSH_PULL, SPEED_50M);	// PA0 PA1 PA2 推挽输出，50M 的速度。
			  */
			finline
            void Config(const u16 pins
                        ,const GPIO_Output output
                        ,const OutStyle style = OPEN_DRAIN
                        ,const GPIO_OutSpeed speed = SPEED_10M )
            {
                SetMode( pins, output | style | speed);
            }
			
			/** @brief	向端口写数据 */
			finline void Write(u16 dat)	{ ODR = (dat); }
			
			/**	@brief	向端口写数据，推荐这种用法。
			 *  @example
             *      gpioa = 0x55aa;	// 把 0x55aa 写到 GPIOA 上。
             */
			finline void operator = (u16 dat)	{ODR = dat;}
			
            /**	@brief	用来设置几个连续的引脚的输出值，注意这几个引脚都应该设置为输出模式！
             *	@param	lsp: 最低位引脚号
             *	@param	width: 位宽
             *	@param	val: 值
             */
			finline void WritePins(const GPIO_Pin& lsp,const u16& width,const u16& val)
					{	this->BSRR =	(((u16)val * (lsp)) & ((((lsp)) <<width)-(lsp)))   
									| (((u32)( ( ( (val * (lsp))^ ( (((lsp)) <<width) - (lsp))))
										&(((((lsp))<<width) - (lsp)))))<<16);	}
			
			/**	@brief	得到指定端口当前设置输出的内容 */
			finline u32 ReadOutputData()				{return this->ODR;}
		
			/**	@brief	读IO端口 */
			finline u32 Read(void)						{return IDR;}
			
			/**	@brief 读IO端口，推荐这种用法。
			  * @example  u32 a; a = gpioa;	// 把 GPIOA 端口的数据读到变量 a 中。
			  */
			finline operator u32()	{return IDR;}
			
			void finline Set(u16 pins)	{ BSRR = pins; }
	
		};
		
	}	// namespace gpio_reg
}

using periph::gpio_reg::GPIO_Class;

#include "stm32f10x__GPIO_CPin.h"

#define gpioa				(*((GPIO_Class*)GPIOA))
#define gpiob				(*((GPIO_Class*)GPIOB))
#define gpioc				(*((GPIO_Class*)GPIOC))
#define gpiod				(*((GPIO_Class*)GPIOD))

#endif
