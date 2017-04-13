#define  PERIPH_CPP
#include "stm32f10x__rcc.h"
#include "stm32f10x__flash.H"

#include "debug.h"

namespace periph{
	
namespace rcc_reg{
		
/**	@brief �õ�����Ƭ��ʱ�ӵ�Ƶ�ʡ�
  *	@param pClocksFreq: ָ��һ���������ʱ��Ƶ�ʵĽṹ�塣
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
    // ������� APB2
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
    /* ��������� assert_failed, ˵������ָ��������ǷǷ��� */
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

/**	@brief ���� APB2 ʱ��Ԥ��Ƶ��
*	@param div ���÷�Ƶϵ��������������1, 2, 4, 8, 16
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

/**	@brief	���� APB1 ʱ��Ԥ��Ƶ���ķ�Ƶϵ��
  *	@param	div ���÷�Ƶϵ��������������1, 2, 4, 8, 16
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

/**	@brief	�õ� APB1 ʱ��Ԥ��Ƶϵ�� */
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

/** @brief Ĭ�ϳ�ʼ�� */
void RCC_Class::DeInit()
{
    APB1RSTR = 0x00000000;			 
    APB2RSTR = 0x00000000; 
    APB2ENR = 0x00000000; //����ʱ�ӹر�.			   
    APB1ENR = 0x00000000;
    
    AHBENR = PERIPH_FLITF | PERIPH_SRAM;	// flashʱ��,����ʱ��ʹ��.DMAʱ�ӹر�	
    
    CIR = 0x00000000;						// �ر������ж�			
    
    ClockCmd(HSI, ENABLE);				// ʹ���ڲ�����ʱ��HSION	
    while(!IsHSI_Ready());					// �ȴ� HSI ����
    
    SetClockOutput(NO_CLOCK);				// û��ʱ�����
    SysclkSwitch(HSI_AS_SYSCLK);			// �л� HSI Ϊϵͳʱ��
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

/**	@brief ����ϵͳʱ�ӵĽṹ�塣��ýṹ�� SysclkConfigStruct c ��
  *	  ���� rcc.Config(c) �Ϳ��Զ�ϵͳʱ�ӽ�������
  *	@note �����ڲ������ѡ���ϵͳʱ�ӣ��Զ������Ƿ����ⲿʱ
  *	  �ӣ��Ƿ���PLL��
  *	@note �����ڲ����Զ�����ϵͳʱ�ӵ�Ƶ�����趨 flash ��ʱ��
  *	@note �������õõ���ϵͳʱ�ӻ�ʹ���е� APB1 ʱ�Ӵ���36M����
  *	  ���ڲ����Զ���� APB1 ��Ԥ��Ƶϵ����
  *	@note ����ֻ���Զ�ʹ����Ҫʹ��ʱ�ӣ��������ûᾡ������ԭ״��
  *	  (˵��������ΪAPB1��Ԥ��Ƶϵ��Ҳ���ܱ��޸�)
  *	@param sysclkSelection ѡ���ϵͳʱ��Դ
  *	  @arg HSI_AS_SYSCLK: ���ʹ�������������ô��������Ĳ�����
  *	    ���Զ����ԣ���������HSIΪϵͳʱ�ӡ�
  *	  @arg HSE_AS_SYSCLK: ���ʹ�������������ô��������Ĳ�����
  *	    ���Զ����ԣ���������HSEΪϵͳʱ�ӡ�
  *	  @note ����ⲿʱ�� HSE ���ȶ���������ܻ�������ѭ��
  *	  @arg PLL_AS_SYSCLK: ���� PLL Ϊϵͳʱ�ӣ�ֻ��ʹ���������ʱ
  *	    ������Ĳ��� pll_s, pll_mul�����ò�Ч�����ʱ�������Զ�
  *     ���úͿ���ϵͳ�� PLL ��
  *	@param PLL_Source: PLL ʱ��Դ��ѡ�񣬲ο� PLL_Source �Ķ��塣
  *	@param PLL_Mul: PLL ��Ƶ�������������� 2 - 16��
  *	  @note ����������õ�ϵͳʱ�ӳ���72M�����򣬻��Զ��л�Ϊ
  *	    HSI ��Ϊϵͳʱ��
  *	@return	�������ú��ϵͳʱ��Ƶ��
  */
u32 RCC_Class::Config(const SysclkConfigStruct& c){
    
    u32 fclk = HSI_FREQ;						// ϵͳʱ��Ƶ��
    AHBENR = PERIPH_FLITF | PERIPH_SRAM;	// flashʱ��,����ʱ��ʹ��.DMAʱ�ӹر�
    
    u32 temp;
    temp = CIR;								// ����ԭ�����ж�ʹ��
    
    CIR = 0x00000000;						// �ر�����RCC�жϣ���ֹ�жϷ�����������
    
    u32 hsi_flag = CR & HSION;				// ��¼��HSI��״̬
    
    ClockCmd(HSI, ENABLE);
    while(!IsHSI_Ready());
    
    SysclkSwitch(HSI_AS_SYSCLK);			// ���л� HSI Ϊϵͳʱ��
    flash.SetLatency(HSI_FREQ);				// ����flash����ʱ
    while( HSI_AS_SYSCLK != GetSysclkSwitchStatus());
    
    switch(c.sysclkSource) {
        case PLL_AS_SYSCLK: {
            // ���� sysclk
            if(c.PLL_Source == HSE_DIV_1)
                fclk = HSE_FREQ*c.PLL_Mul;
            else if(c.PLL_Source == HSE_DIV_2)
                fclk = (HSE_FREQ/2)*c.PLL_Mul;
            else	// HSI_DIV_2
                fclk = (HSI_FREQ/2)*c.PLL_Mul;
            
            /* ���Ҫ���õ�Ƶ�ʳ���������ƣ����������á�*/
            if(fclk > SYSCLK_MAX){
                fclk = HSI_FREQ;
                break;
            }
            
            ClockCmd(PLL, DISABLE);		/* Ϊ���ܹ�����PLL */
            /*
            ��� PLL ʱ��Դ���ⲿʱ�ӣ���ô�ȿ��ⲿʱ��
            */
            if(c.PLL_Source != HSI_DIV_2)
            {
                ClockCmd(HSE_BYPASS, DISABLE);
                ClockCmd(HSE, ENABLE);
                while(!IsHSE_Ready());		// �ȴ�HSE
            }

            u8 apb1_prescaler = GetAPB1_Prescaler();
            if( fclk / apb1_prescaler > APB1_MAX) {		/* ��ֹ APB1 ʱ�ӹ��� */
                while(apb1_prescaler<=16) {
                    apb1_prescaler <<= 1;
                    if( fclk / apb1_prescaler <= APB1_MAX)
                        break;
                }
                SetAPB1_Prescaler(apb1_prescaler);		// ��������
            }
            
            // ���ú� PLL ����PLL��Ϊϵͳʱ��
            PLL_Config(c.PLL_Source, c.PLL_Mul);
            ClockCmd(PLL, ENABLE);
            while(!IsPLL_Ready());
            flash.SetLatency(fclk);		// ���� FLASH ��ʱ
            SysclkSwitch(PLL_AS_SYSCLK);
            while(PLL_AS_SYSCLK != GetSysclkSwitchStatus());
            
            if(hsi_flag == 0)
                ClockCmd(HSI, DISABLE);		// �ָ�ԭ���� HSI ״̬
        }break;
        case HSE_AS_SYSCLK:
        {
            ClockCmd(HSE_BYPASS, DISABLE);
            ClockCmd(HSE, ENABLE);
            while(!IsHSE_Ready());
            
            flash.SetLatency(HSE_FREQ);		// ���� flash ����ʱ������ԭ��̫����
            
            SysclkSwitch(HSE_AS_SYSCLK);
            while(HSE_AS_SYSCLK != GetSysclkSwitchStatus());
            
            if(hsi_flag == 0)
                ClockCmd(HSI, DISABLE);	// �ָ�ԭ���� HSI ״̬
        }break;
        case HSI_AS_SYSCLK:	break;	//�Ѿ�������
        default:	break;
    }
    
    CIR = temp;		// �ָ�
    return fclk; 
}

} /* namespace rcc_reg */

} /* namespace periph */
