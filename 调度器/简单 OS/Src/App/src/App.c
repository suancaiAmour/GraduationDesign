#include    "stm32f10x.h"
#include    "stm32f10x_rcc.h"
#include    "stm32f10x_gpio.h"

#include "switch.h"
#include <stdio.h>

u32 TaskStack[2][40];


void Task0(void);
void Task1(void);


int main(void)
{
	SwitchTaskInt(Task0, &TaskStack[0][39]);
	SwitchTaskInt(Task1, &TaskStack[1][39]);
  SwitchStart();
	while(1);
}


void Task0(void)
{
   u8 i;
   for(;;)
   {
	 i = i;
	 //SwitchDelay(200);

   }
}
void Task1(void)
{
   u8 i;
   for(;;)
   {
	 //SwitchDelay(300);
	 i = i;
   }
}


