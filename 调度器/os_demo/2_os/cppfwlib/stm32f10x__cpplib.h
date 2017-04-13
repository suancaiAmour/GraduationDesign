#pragma once

#ifndef REG_TYPEDEF_H
#define REG_TYPEDEF_H

#include "stm32f10x__COMMON.h"

#include "stm32f10x__rcc.h"

#include "stm32f10x__GPIO.h"

//#include "stm32f10x__TIM.h"

#include "stm32f10x__AFIO.h"

#include "stm32f10x__USART.h"

#include "stm32f10x__EXTI.h"

#include "stm32f10x__IWDG.h"

#include "stm32f10x__WWDG.h"

#include "stm32f10x__FLASH.h"

// core
#include "stm32f10x__NVIC.h"
#include "stm32f10x__SCB.h"
#include "stm32f10x__Systick.h"

#ifndef PERIPH_CPP
    #undef BIT_PLUS_OPERATOR_SUPPORT
    #undef PRegBit
#endif

#endif
