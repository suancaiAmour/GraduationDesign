#ifndef __MAIN_H
#define __MAIN_H	

extern"C"
{
#include "../h/led.h"
#include "../h/delay.h"
#include "../h/sys.h"
#include "../h/key.h"
#include "../h/usart.h"
#include "../h/stm32f10x.h"
#include "../h/stm32f10x_gpio.h"
#include "../h/core_cm3.h"

	void Task0(void);
	void Task1(void);
}
#include "../h/init.h"
#include "../h/timer.h"


#endif
