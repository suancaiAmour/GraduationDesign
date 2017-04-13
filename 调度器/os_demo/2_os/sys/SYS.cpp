#include "sys.h"
#include "usart.h"
#include "debug.h"

//	<<< Use Configuration Wizard in Context Menu >>> 
//	<h>ϵͳʱ������
//		<e>ʹ���ⲿʱ����ΪPLL����ʱ��
#define USE_HSE_AS_PLL_INPUT		1
//		</e>
//		<o>PLL��Ƶ����(2-16)
#define PLL_MULTIPLIER			9
//	</h>
//	<<< end of configuration section >>>


//ϵͳʱ�ӳ�ʼ��
//ʹ���ⲿ8M����,PLL��72MƵ��		    
//����ԭ��@SCUT
//2009/12/09 
//V1.3
//��NVIC KO��,û��ʹ���κο��ļ�!
//�жϳ�ʼ�� RTC,USART�Ѿ�����,����������!
static u8  fac_us;//us��ʱ������

static u16 fac_ms;//ms��ʱ������

/*
��ʼ���ӳٺ���
*/
void delay_init(u8 SYSCLK){
	SysTick->CTRL&=0xfffffffb;//ѡ���ڲ�ʱ�� HCLK/8
	fac_us=SYSCLK/8;		    
	fac_ms=(u16)fac_us*1000;
}

//��ʱNms
//ע��Nms�ķ�Χ
//Nms<=0xffffff*8/SYSCLK
//��72M������,Nms<=1864 
void delay_ms(u16 nms)
{		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;//ʱ�����
	SysTick->VAL =0x00;           //��ռ�����
	SysTick->CTRL=0x01 ;          //��ʼ����  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����	  	    
}

//	��ʱus								   
void delay_us(u32 Nus)
{
	u32 temp;	    	 
	SysTick->LOAD=Nus*fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL=0x01 ;      //��ʼ���� 	 
	do{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����	 
}

System sys;

void System::Init()
{
	using namespace periph;
	
	scb.SetVectorTable();
	
	rcc.DeInit();	/* ��λRCC */
	
	/* ����ϵͳʱ�� */
	SysclkConfig c;
	c.sysclkSource = PLL_AS_SYSCLK;
	c.PLL_Source = HSE_DIV_1;
	c.PLL_Mul = PLL_MULTIPLIER;
	sysclk = rcc.Config(c);
	
	scb.SetPriorityGrouping(GRP4_SUB4); /* 4�������ȼ���4�������ȼ� */
	
    USART1_Init(115200, sysclk);	/* ʹ���˴��ڣ�����ʹ����������� */
	
	/* JTAG �� SWD ʹ�ܻ�ռ��ĳЩ���� */
	rcc.ClockCmd(&afio, ENABLE);
	afio.DebugConfig(SWD_DIS_JTAG_DIS);
	
	nstd::IOStream_Init();
	
	Delay_Init(sysclk/1000000);				/* ��ʱ��ʼ�� */
	
	Debug_Init();
	
	output<<endl
		<<"********************* Pony279@SCUT *********************"<<endl
		<<"NOTE: THIS PROJECT DISABLE JTAG AND SWD BY DEFAULT"<<endl
		<<"Compiled at: "<<__TIME__<<" "<<__DATE__<<endl
		<<__PRETTY_FUNCTION__<<" completed."<<endl<<endl;
}

/* ɶ���������� */
ARMAPI void SystemInit (void){}

/* ���ڲ���ʹ�� no_init���� sys �����ֱ��������ж���֮ǰ��ʼ��
	_clock_init �����Ǹ���ʼ���ĺõط�
	*/
ARMAPI void _clock_init(){
	sys.Init();
	return ;
}
	
/**
* @brief  This function handles NMI exception.
* @param  None
* @retval None
*/
ARMAPI void NMI_Handler(void){
	dout<<endl
			<<"NMI_Handler"<<endl;
	while(1);
}

ARMAPI  
void HardFault_Handler(){
	dout<<endl
			<<"HardFault_Handler"<<endl;
	while(1);
}

// MPU fault
ARMAPI  
void MemManage_Handler(){
	dout<<endl
			<<"MemManage_Handler"<<endl;
	while(1);
}
 ARMAPI  
void BusFault_Handler(){
	dout<<endl
			<<"BusFault_Handler"<<endl;
	while(1);
}
 ARMAPI  
void UsageFault_Handler(){
	dout<<endl
		<<"UsageFault_Handler"<<endl;
	while(1);
}
