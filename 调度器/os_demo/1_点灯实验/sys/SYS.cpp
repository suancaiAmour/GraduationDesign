#include "sys.h"
#include "usart.h"
#include "debug.h"

//	<<< Use Configuration Wizard in Context Menu >>> 
//	<h>系统时钟配置
//		<e>使用外部时钟作为PLL输入时钟
#define USE_HSE_AS_PLL_INPUT		1
//		</e>
//		<o>PLL倍频设置(2-16)
#define PLL_MULTIPLIER			9
//	</h>
//	<<< end of configuration section >>>


//系统时钟初始化
//使用外部8M晶振,PLL到72M频率		    
//正点原子@SCUT
//2009/12/09 
//V1.3
//把NVIC KO了,没有使用任何库文件!
//中断初始化 RTC,USART已经测试,其他待测试!
static u8  fac_us;//us延时倍乘数

static u16 fac_ms;//ms延时倍乘数

/*
初始化延迟函数
*/
void delay_init(u8 SYSCLK){
	SysTick->CTRL&=0xfffffffb;//选择内部时钟 HCLK/8
	fac_us=SYSCLK/8;		    
	fac_ms=(u16)fac_us*1000;
}

//延时Nms
//注意Nms的范围
//Nms<=0xffffff*8/SYSCLK
//对72M条件下,Nms<=1864 
void delay_ms(u16 nms)
{		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;//时间加载
	SysTick->VAL =0x00;           //清空计数器
	SysTick->CTRL=0x01 ;          //开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL=0x00;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器	  	    
}

//	延时us								   
void delay_us(u32 Nus)
{
	u32 temp;	    	 
	SysTick->LOAD=Nus*fac_us; //时间加载	  		 
	SysTick->VAL=0x00;        //清空计数器
	SysTick->CTRL=0x01 ;      //开始倒数 	 
	do{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL=0x00;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器	 
}

System sys;

void System::Init()
{
	using namespace periph;
	
	scb.SetVectorTable();
	
	rcc.DeInit();	/* 复位RCC */
	
	/* 配置系统时钟 */
	SysclkConfig c;
	c.sysclkSource = PLL_AS_SYSCLK;
	c.PLL_Source = HSE_DIV_1;
	c.PLL_Mul = PLL_MULTIPLIER;
	sysclk = rcc.Config(c);
	
	scb.SetPriorityGrouping(GRP4_SUB4); /* 4个组优先级，4个子优先级 */
	
    USART1_Init(115200, sysclk);	/* 使能了串口，才能使用输入输出流 */
	
	/* JTAG 和 SWD 使能会占用某些引脚 */
	rcc.ClockCmd(&afio, ENABLE);
	afio.DebugConfig(SWD_DIS_JTAG_DIS);
	
	nstd::IOStream_Init();
	
	Delay_Init(sysclk/1000000);				/* 延时初始化 */
	
	Debug_Init();
	
	output<<endl
		<<"********************* Pony279@SCUT *********************"<<endl
		<<"NOTE: THIS PROJECT DISABLE JTAG AND SWD BY DEFAULT"<<endl
		<<"Compiled at: "<<__TIME__<<" "<<__DATE__<<endl
		<<__PRETTY_FUNCTION__<<" completed."<<endl<<endl;
}

/* 啥都不用做了 */
ARMAPI void SystemInit (void){}

/* 由于不能使用 no_init，而 sys 对象又必须在所有对象之前初始化
	_clock_init 这里是个初始化的好地方
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
