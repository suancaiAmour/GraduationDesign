#ifndef __cplusplus
#define IWDG_TYPEDEF_H
#endif
#ifndef IWDG_TYPEDEF_H
#define IWDG_TYPEDEF_H

namespace periph{
	namespace iwdg_reg{
		
		typedef enum				// 操作看门狗寄存器的钥匙
		{
			  START = 0xcccc			// 启动看门狗
			, WRITE_PR_RLR = 0X5555		// 修改PR, RLR寄存器
			, COUNTER_RELOAD = 0xaaaa	// 喂狗用的KEY
		}IWDG_KR_KEY_TYPEDEF;

		enum IWDG_PR{
			
		};

		enum IWDG_SR{
			  RVU = Bit(1)
			, PVU = Bit(0)
		};
		

		class IWDG_Class: protected IWDG_TypeDef
		{
			public:
			/**	@brief 启动内部看门狗，一旦启动，无法停止
			*/
			finline void Start()		{ KR = START; }

			/**	@brief  喂狗
			*/
			finline void Feed()			{ KR = COUNTER_RELOAD; }

			/**	@brief	配置内部独立看门狗
			*	@param	reload_val	看门狗计数器的重载值，范转在0 - 0xfff之间
			*	@param	div 对LSI时钟(40kHz)分频，得到看门狗时钟，div可以取的有 4, 8, 16, 32, 64, 128, 256
			*/
			void Config(u16 reload_val, u16 div = 256)
			{
				u8 pr = 0;
				//Feed();	// 防止在看门狗运行时配置的时候，恰好看门狗复位
				reload_val &= 0xfff;
				div%=256;
				div>>=2;
				while(div = div>>1, div!=0)
					pr++;
				
				while( checkb(SR, PVU) );	// 等待直到允许修改PR的值
				KR = WRITE_PR_RLR;	// 写入钥匙
				PR = pr;
				
				while( checkb(SR, RVU) );	// 等待直到允许修改RLR的值
				KR = WRITE_PR_RLR;	// 写入钥匙
				RLR = reload_val;
				Feed();				// 重新喂狗，让看门狗以新的重载值运行
			}
		};
	}

}

#define iwdg	(*(periph::iwdg_reg::IWDG_Class*)IWDG)

#endif
