#include "stm32f10x__gpio.h"

namespace periph{

	namespace gpio_reg{
		
		const u16 GPIO_Class::pin_field_map[16] = {
                0x0000, 0x0001, 0x0010, 0x0011,
                0x0100, 0x0101, 0x0110, 0x0111,
                0x1000, 0x1001, 0x1010, 0x1011,
                0x1100, 0x1101, 0x1110, 0x1111
            };
		
	}

}
