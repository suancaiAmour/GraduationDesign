#ifdef __CC_ARM 
#include "sys.h"
#endif

#include "debug.h"
using namespace nstd;

#ifdef MYDEBUG 	//�������

/** @brief Ϊ�������ĳ�ʼ������ */
void Debug_Init() {
    #ifdef __CC_ARM
    using namespace periph;
    PinRename(pa8,led);
    /**< ���� led ���ŵĵ�ƽ���ж��ǲ����������
    ������������ (led==0), �Ͳ�������ʱ, ��ʱ��
    �Ƚ�����ʱ��.
    ��������������, �ͽ�����ʱ, ��ʾϵͳ����, 
    ���� PC �����㹻��ʱ��򿪴��ڵ�������. 
    */
	if(led){ 
		led.Config(OUTPUT);
		led = 0;
		/*
		��Ӳ�����Ե�����£����ϵ�ʱ PL2303 ���ȶ���
		������ʱ 1.5s ����
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

/**	@brief	���Ժ���������Ϣ���ж�̬���
*	@param	a ���Ե�����
*	@param	file �����������ʱ���ڵ�Դ�ļ�
*	@param	line ��Դ�ļ��ĵ� x ��
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
