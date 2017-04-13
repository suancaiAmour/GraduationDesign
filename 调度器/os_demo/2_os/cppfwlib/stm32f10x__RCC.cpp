#define  PERIPH_CPP
#include "stm32f10x__rcc.h"
#include "stm32f10x__flash.H"

#include "debug.h"

namespace periph{
	
namespace rcc_reg{
		
/**	@brief 得到几个片上时钟的频率。
  *	@param pClocksFreq: 指向一个用来存放时钟频率的结构体。
  */
void RCC_Class::GetClocksFreq(RCC_ClocksFreq* pClocksFreq){
    RCC_ClocksFreq& clocksFreq = *pClocksFreq;
    u32 tempCFGR = CFGR;
    
    /*
    calculate SYSCLK, assign it to clocksFreq.SYSCLK
    */
    switch((SysclkSourceSelect)readf(tempCFGR,CFGR_SWS)){
        case HSI_AS_SYSCLK:{
            clocksFreq.SYSCLK = HSI_FREQ;
        }break;
        case HSE_AS_SYSCLK:{
            clocksFreq.SYSCLK = HSE_FREQ;
        }break;
        case PLL_AS_SYSCLK:{					
            u32 pll_mul = readf(tempCFGR, CFGR_PLLMUL);
            if(pll_mul==BByte(1111))
                pll_mul = 16;
            else
                pll_mul += 2;
            if(tempCFGR&CFGR_PLLSRC){
                 /* HSE as PLL input clock */
                clocksFreq.SYSCLK = pll_mul*
                    /*
                    check whether HSE is 
                    devided by 2 for PLL input
                    */
                    ((tempCFGR&CFGR_PLLXTPRE)?
                        HSE_FREQ/2:HSE_FREQ);
            }
            else{		/* HSI/2 as PLL input clock */
                clocksFreq.SYSCLK = pll_mul * (HSI/2);
            }
        }break;
        default:{
            /*
            normally this code can never be excuted
            */
            assert(0);
        }break;
    }
    
    /*
    calculate HCLK, assign it to clocksFreq.HCLK
    */
    u8 hpre = readf(tempCFGR, CFGR_HPRE);
    if(hpre&BByte(1000)){
        clocksFreq.HCLK = 
            clocksFreq.SYSCLK >> 
                ((hpre&BByte(0111))+1);
    }
    else{ /* sysclk not divided for hclk */
        clocksFreq.HCLK = clocksFreq.SYSCLK;
    }
    
    /*
    calculate PCLK1, assign it to clocksFreq.PCLK1
    */
    u8 ppre1 = readf(tempCFGR, CFGR_PPRE1);
    if(ppre1&BByte(100)){
        clocksFreq.PCLK1 =
            clocksFreq.HCLK >>
                ((ppre1&BByte(011))+1);
    }
    else{ /* hclk not divided for pre1 */
        clocksFreq.PCLK1 = clocksFreq.HCLK;
    }
    
    /*
    calculate PCLK2, assign it to clocksFreq.PCLK2
    */
    u8 ppre2 = readf(tempCFGR, CFGR_PPRE2);
    if(ppre2&BByte(100)){
        clocksFreq.PCLK2 =
            clocksFreq.HCLK >>
                ((ppre2&BByte(011))+1);
    }
    else{ /* hclk not divided for pre1 */
        clocksFreq.PCLK2 = clocksFreq.HCLK;
    }
    
    /*
    calculate ADCCLK, assign it to clocksFreq.ADCCLK
    */
    u8 adcpre = readf(tempCFGR, CFGR_ADCPRE);
    clocksFreq.ADCCLK = 
        clocksFreq.PCLK2 /
            ((adcpre+1)*2);
}
		
void 
RCC_Class::ClockCmd
    (const void* const peri, FunctionalState newState)
{
    u32 bit_mask=0;
    switch((addr_uint)peri){
        #define PeriphCase(r,p) \
            case (addr_uint)p: bit_mask = r##_##p##EN; break;
        using namespace rcc_reg;
        /* APB2 */
        PeriphCase(APB2ENR,TIM11)
        PeriphCase(APB2ENR,TIM10)
        PeriphCase(APB2ENR,TIM9)
        PeriphCase(APB2ENR,ADC3)
        PeriphCase(APB2ENR,USART1)
        PeriphCase(APB2ENR,TIM8)
        PeriphCase(APB2ENR,SPI1)
        PeriphCase(APB2ENR,TIM1)
        PeriphCase(APB2ENR,ADC2)
        PeriphCase(APB2ENR,ADC1)
        PeriphCase(APB2ENR,GPIOG)
        PeriphCase(APB2ENR,GPIOF)
        PeriphCase(APB2ENR,GPIOE)
        PeriphCase(APB2ENR,GPIOD)
        PeriphCase(APB2ENR,GPIOC)
        PeriphCase(APB2ENR,GPIOB)
        PeriphCase(APB2ENR,GPIOA)
        PeriphCase(APB2ENR,AFIO)
    }
    if(bit_mask){
        *bit_band_ptr(APB2ENR, bit_mask) = newState;
        return;
    }
    // 如果不是 APB2
    switch((addr_uint)peri){
        /* APB1 */
        PeriphCase(APB1ENR,DAC)
        PeriphCase(APB1ENR,PWR)
        PeriphCase(APB1ENR,BKP)
        PeriphCase(APB1ENR,CAN1)
        PeriphCase(APB1ENR,I2C2)
        PeriphCase(APB1ENR,I2C1)
        PeriphCase(APB1ENR,UART5)
        PeriphCase(APB1ENR,UART4)
        PeriphCase(APB1ENR,USART3)
        PeriphCase(APB1ENR,USART2)
        PeriphCase(APB1ENR,SPI3)
        PeriphCase(APB1ENR,SPI2)
        PeriphCase(APB1ENR,WWDG)
        PeriphCase(APB1ENR,TIM14)
        PeriphCase(APB1ENR,TIM13)
        PeriphCase(APB1ENR,TIM12)
        PeriphCase(APB1ENR,TIM7)
        PeriphCase(APB1ENR,TIM6)
        PeriphCase(APB1ENR,TIM5)
        PeriphCase(APB1ENR,TIM4)
        PeriphCase(APB1ENR,TIM3)
        PeriphCase(APB1ENR,TIM2)
        #undef PeriphCase	 
    }
    /* 如果在这里 assert_failed, 说明参数指向的外设是非法的 */
    assert(bit_mask);
    *bit_band_ptr(APB1ENR, bit_mask) = newState;
    return;
}

void RCC_Class::SetAHB_Prescaler(u8 div)
{
    u8 temp = 0;
    if(div != 1){
        temp = BByte(1000);
        div >>= 2;
        while(div){
            temp++;
            div >>= 1;
        }
    }
    CFGR = assignf(CFGR, HPRE, temp);
}

/**	@brief 设置 APB2 时钟预分频器
*	@param div 设置分频系数，参数可以是1, 2, 4, 8, 16
*/
void RCC_Class::SetAPB2_Prescaler(u8 div)
{
    u8 temp = 0;
    if(div != 1)
    {
        temp = BByte(100);
        div >>= 2;
        while(div)
        {
            temp++;
            div >>= 1;
        }
    }
    CFGR = assignf(CFGR, PPRE2, temp);
}

/**	@brief	设置 APB1 时钟预分频器的分频系数
  *	@param	div 设置分频系数，参数可以是1, 2, 4, 8, 16
  */
void RCC_Class::SetAPB1_Prescaler(u8 div)
{
    u8 temp = 0;
    if(div != 1)
    {
        temp = BByte(100);
        div >>= 2;
        while(div)
        {
            temp++;
            div >>= 1;
        }
    }
    CFGR = assignf(CFGR, PPRE1, temp);
}

/**	@brief	得到 APB1 时钟预分频系数 */
u8 RCC_Class::GetAPB1_Prescaler()
{
    u8 pre1 = readf(CFGR, PPRE1);
    u8 ret = (pre1&BByte(100) != 0)? 2 :1;
    pre1 &= BByte(011);
    while(pre1)
    {
        ret <<= 1;	// ret *= 2;
        pre1--;
    }
    return ret;
}

/** @brief 默认初始化 */
void RCC_Class::DeInit()
{
    APB1RSTR = 0x00000000;			 
    APB2RSTR = 0x00000000; 
    APB2ENR = 0x00000000; //外设时钟关闭.			   
    APB1ENR = 0x00000000;
    
    AHBENR = PERIPH_FLITF | PERIPH_SRAM;	// flash时钟,闪存时钟使能.DMA时钟关闭	
    
    CIR = 0x00000000;						// 关闭所有中断			
    
    ClockCmd(HSI, ENABLE);				// 使能内部高速时钟HSION	
    while(!IsHSI_Ready());					// 等待 HSI 就绪
    
    SetClockOutput(NO_CLOCK);				// 没有时钟输出
    SysclkSwitch(HSI_AS_SYSCLK);			// 切换 HSI 为系统时钟
    SetADC_Prescaler(2);
    SetAPB1_Prescaler(1);
    SetAPB2_Prescaler(1);
    SetAHB_Prescaler(1);
    SetUSB_Prescaler(PLL_DIV_1D5);
    while( HSI_AS_SYSCLK != GetSysclkSwitchStatus());
    
    ClockCmd(CLOCK_SECURITY_SYSTEM, DISABLE);
    ClockCmd(PLL, DISABLE);
    ClockCmd(HSE, DISABLE);
    ClockCmd(HSE_BYPASS, DISABLE);
    SetPLL_Mul(2);
}

/**	@brief 配置系统时钟的结构体。填好结构体 SysclkConfigStruct c 后，
  *	  调用 rcc.Config(c) 就可以对系统时钟进行配置
  *	@note 函数内部会根据选择的系统时钟，自动配置是否开启外部时
  *	  钟，是否开启PLL。
  *	@note 函数内部会自动根据系统时钟的频率来设定 flash 延时。
  *	@note 函数配置得到的系统时钟会使现有的 APB1 时钟大于36M，函
  *	  数内部会自动提高 APB1 的预分频系数。
  *	@note 函数只会自动使能需要使能时钟，其它配置会尽量保持原状。
  *	  (说尽量是因为APB1的预分频系数也可能被修改)
  *	@param sysclkSelection 选择的系统时钟源
  *	  @arg HSI_AS_SYSCLK: 如果使用这个参数，那么函数后面的参数会
  *	    被自动忽略，并且设置HSI为系统时钟。
  *	  @arg HSE_AS_SYSCLK: 如果使用这个参数，那么函数后面的参数会
  *	    被自动忽略，并且设置HSE为系统时钟。
  *	  @note 如果外部时钟 HSE 不稳定，程序可能会陷入死循环
  *	  @arg PLL_AS_SYSCLK: 设置 PLL 为系统时钟，只有使用这个参数时
  *	    ，后面的参数 pll_s, pll_mul的配置才效。这个时候函数会自动
  *     配置和开启系统的 PLL 。
  *	@param PLL_Source: PLL 时钟源的选择，参考 PLL_Source 的定义。
  *	@param PLL_Mul: PLL 倍频数，参数可以是 2 - 16。
  *	  @note 这个函数配置的系统时钟超过72M，否则，会自动切换为
  *	    HSI 作为系统时钟
  *	@return	返回配置后的系统时钟频率
  */
u32 RCC_Class::Config(const SysclkConfigStruct& c){
    
    u32 fclk = HSI_FREQ;						// 系统时钟频率
    AHBENR = PERIPH_FLITF | PERIPH_SRAM;	// flash时钟,闪存时钟使能.DMA时钟关闭
    
    u32 temp;
    temp = CIR;								// 保存原来的中断使能
    
    CIR = 0x00000000;						// 关闭所有RCC中断，防止中断妨碍函数工作
    
    u32 hsi_flag = CR & HSION;				// 记录下HSI的状态
    
    ClockCmd(HSI, ENABLE);
    while(!IsHSI_Ready());
    
    SysclkSwitch(HSI_AS_SYSCLK);			// 先切换 HSI 为系统时钟
    flash.SetLatency(HSI_FREQ);				// 设置flash的延时
    while( HSI_AS_SYSCLK != GetSysclkSwitchStatus());
    
    switch(c.sysclkSource) {
        case PLL_AS_SYSCLK: {
            // 计算 sysclk
            if(c.PLL_Source == HSE_DIV_1)
                fclk = HSE_FREQ*c.PLL_Mul;
            else if(c.PLL_Source == HSE_DIV_2)
                fclk = (HSE_FREQ/2)*c.PLL_Mul;
            else	// HSI_DIV_2
                fclk = (HSI_FREQ/2)*c.PLL_Mul;
            
            /* 如果要设置的频率超过最大限制，不进行配置。*/
            if(fclk > SYSCLK_MAX){
                fclk = HSI_FREQ;
                break;
            }
            
            ClockCmd(PLL, DISABLE);		/* 为了能够配置PLL */
            /*
            如果 PLL 时钟源是外部时钟，那么先开外部时钟
            */
            if(c.PLL_Source != HSI_DIV_2)
            {
                ClockCmd(HSE_BYPASS, DISABLE);
                ClockCmd(HSE, ENABLE);
                while(!IsHSE_Ready());		// 等待HSE
            }

            u8 apb1_prescaler = GetAPB1_Prescaler();
            if( fclk / apb1_prescaler > APB1_MAX) {		/* 防止 APB1 时钟过大 */
                while(apb1_prescaler<=16) {
                    apb1_prescaler <<= 1;
                    if( fclk / apb1_prescaler <= APB1_MAX)
                        break;
                }
                SetAPB1_Prescaler(apb1_prescaler);		// 重新设置
            }
            
            // 配置好 PLL 后以PLL作为系统时钟
            PLL_Config(c.PLL_Source, c.PLL_Mul);
            ClockCmd(PLL, ENABLE);
            while(!IsPLL_Ready());
            flash.SetLatency(fclk);		// 设置 FLASH 延时
            SysclkSwitch(PLL_AS_SYSCLK);
            while(PLL_AS_SYSCLK != GetSysclkSwitchStatus());
            
            if(hsi_flag == 0)
                ClockCmd(HSI, DISABLE);		// 恢复原来的 HSI 状态
        }break;
        case HSE_AS_SYSCLK:
        {
            ClockCmd(HSE_BYPASS, DISABLE);
            ClockCmd(HSE, ENABLE);
            while(!IsHSE_Ready());
            
            flash.SetLatency(HSE_FREQ);		// 设置 flash 的延时，具体原因不太懂。
            
            SysclkSwitch(HSE_AS_SYSCLK);
            while(HSE_AS_SYSCLK != GetSysclkSwitchStatus());
            
            if(hsi_flag == 0)
                ClockCmd(HSI, DISABLE);	// 恢复原来的 HSI 状态
        }break;
        case HSI_AS_SYSCLK:	break;	//已经配置了
        default:	break;
    }
    
    CIR = temp;		// 恢复
    return fclk; 
}

} /* namespace rcc_reg */

} /* namespace periph */
