#include "stm32f10x__cpplib.h"
#include "stm32f10x__GPIO.h"
#include "stm32f10x__GPIO_CPin.h"

namespace periph{

		/**	@brief	定义 pa0, pa1, ..., pb0, pb1, ..., pd15 对象。
					事实上这些对象没有任何数据成员，定义仅仅是为了防止在使用的时候编译器报错说找不到符号。
		*/
		GPIO_CPin<GPIOA_BASE, Pin0>	pa0;
		GPIO_CPin<GPIOA_BASE, Pin1>	pa1;
		GPIO_CPin<GPIOA_BASE, Pin2>	pa2;
		GPIO_CPin<GPIOA_BASE, Pin3>	pa3;
		GPIO_CPin<GPIOA_BASE, Pin4>	pa4;
		GPIO_CPin<GPIOA_BASE, Pin5>	pa5;
		GPIO_CPin<GPIOA_BASE, Pin6>	pa6;
		GPIO_CPin<GPIOA_BASE, Pin7>	pa7;
		GPIO_CPin<GPIOA_BASE, Pin8>	pa8;
		GPIO_CPin<GPIOA_BASE, Pin9>	pa9;
		GPIO_CPin<GPIOA_BASE, Pin10>pa10;
		GPIO_CPin<GPIOA_BASE, Pin11>pa11;
		GPIO_CPin<GPIOA_BASE, Pin12>pa12;
		GPIO_CPin<GPIOA_BASE, Pin13>pa13;
		GPIO_CPin<GPIOA_BASE, Pin14>pa14;
		GPIO_CPin<GPIOA_BASE, Pin15>pa15;

		GPIO_CPin<GPIOB_BASE, Pin0>	pb0;
		GPIO_CPin<GPIOB_BASE, Pin1>	pb1;
		GPIO_CPin<GPIOB_BASE, Pin2>	pb2;
		GPIO_CPin<GPIOB_BASE, Pin3>	pb3;
		GPIO_CPin<GPIOB_BASE, Pin4>	pb4;
		GPIO_CPin<GPIOB_BASE, Pin5>	pb5;
		GPIO_CPin<GPIOB_BASE, Pin6>	pb6;
		GPIO_CPin<GPIOB_BASE, Pin7>	pb7;
		GPIO_CPin<GPIOB_BASE, Pin8>	pb8;
		GPIO_CPin<GPIOB_BASE, Pin9>	pb9;
		GPIO_CPin<GPIOB_BASE, Pin10>pb10;
		GPIO_CPin<GPIOB_BASE, Pin11>pb11;
		GPIO_CPin<GPIOB_BASE, Pin12>pb12;
		GPIO_CPin<GPIOB_BASE, Pin13>pb13;
		GPIO_CPin<GPIOB_BASE, Pin14>pb14;
		GPIO_CPin<GPIOB_BASE, Pin15>pb15;

		GPIO_CPin<GPIOC_BASE, Pin0>	pc0;
		GPIO_CPin<GPIOC_BASE, Pin1>	pc1;
		GPIO_CPin<GPIOC_BASE, Pin2>	pc2;
		GPIO_CPin<GPIOC_BASE, Pin3>	pc3;
		GPIO_CPin<GPIOC_BASE, Pin4>	pc4;
		GPIO_CPin<GPIOC_BASE, Pin5>	pc5;
		GPIO_CPin<GPIOC_BASE, Pin6>	pc6;
		GPIO_CPin<GPIOC_BASE, Pin7>	pc7;
		GPIO_CPin<GPIOC_BASE, Pin8>	pc8;
		GPIO_CPin<GPIOC_BASE, Pin9>	pc9;
		GPIO_CPin<GPIOC_BASE, Pin10>pc10;
		GPIO_CPin<GPIOC_BASE, Pin11>pc11;
		GPIO_CPin<GPIOC_BASE, Pin12>pc12;
		GPIO_CPin<GPIOC_BASE, Pin13>pc13;
		GPIO_CPin<GPIOC_BASE, Pin14>pc14;
		GPIO_CPin<GPIOC_BASE, Pin15>pc15;
		
		GPIO_CPin<GPIOD_BASE, Pin0>	pd0;
		GPIO_CPin<GPIOD_BASE, Pin1>	pd1;
		GPIO_CPin<GPIOD_BASE, Pin2>	pd2;
		GPIO_CPin<GPIOD_BASE, Pin3>	pd3;
		GPIO_CPin<GPIOD_BASE, Pin4>	pd4;
		GPIO_CPin<GPIOD_BASE, Pin5>	pd5;
		GPIO_CPin<GPIOD_BASE, Pin6>	pd6;
		GPIO_CPin<GPIOD_BASE, Pin7>	pd7;
		GPIO_CPin<GPIOD_BASE, Pin8>	pd8;
		GPIO_CPin<GPIOD_BASE, Pin9>	pd9;
		GPIO_CPin<GPIOD_BASE, Pin10>pd10;
		GPIO_CPin<GPIOD_BASE, Pin11>pd11;
		GPIO_CPin<GPIOD_BASE, Pin12>pd12;
		GPIO_CPin<GPIOD_BASE, Pin13>pd13;
		GPIO_CPin<GPIOD_BASE, Pin14>pd14;
		GPIO_CPin<GPIOD_BASE, Pin15>pd15;
		
}	// namespace periph							   
