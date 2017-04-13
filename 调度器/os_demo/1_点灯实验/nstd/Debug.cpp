#ifdef __CC_ARM 
#include "sys.h"
#endif

#include "debug.h"
using namespace nstd;

#ifdef MYDEBUG 	//如果调试

/** @brief 为调试做的初始化工作 */
void Debug_Init() {
    #ifdef __CC_ARM
    using namespace periph;
    PinRename(pa8,led);
    /**< 利用 led 引脚的电平来判断是不是软件仿真
    如果是软件仿真 (led==0), 就不进行延时, 延时会
    比较消耗时间.
    如果不是软件仿真, 就进行延时, 以示系统启动, 
    且让 PC 端有足够的时间打开串口调试助手. 
    */
	if(led){ 
		led.Config(OUTPUT);
		led = 0;
		/*
		在硬件调试的情况下，刚上电时 PL2303 不稳定，
		所以延时 1.5s 启动
		*/
		Delay_ms(1000);
		Delay_ms(500);
		led = 1;
		led.Config(INPUT, FLOATING);
	}
    #endif
}

extern "C" void  abort();

#ifdef __CC_ARM 
extern "C"
void __aeabi_assert(const char *expr, const char *file, int line){
	output<<"assert failed: "<<expr
		<<". file: "<<file
		<<". line: "
		<<(u32)line<<"."<<endl;
	abort();
}

extern "C"
__attribute((weak))
 void abort(){
	output<<"abort()"<<endl;
	while(1);
}
#endif

/**	@brief	断言函数，对信息进行动态检查
*	@param	a 断言的内容
*	@param	file 调用这个函数时所在的源文件
*	@param	line 在源文件的第 x 行
*/
void nstd_AssertFailed (char* exp, char* file , int line) 
{
	output<<"assert failed: "<<exp
		<<". file: "<<file
		<<". line: "
		<<(u32)line<<"."<<endl;
	abort();					
}

#endif

#undef dout
struct NoDebugOutput dout;
