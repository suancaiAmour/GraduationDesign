#ifndef __cplusplus
#define IWDG_TYPEDEF_H
#endif
#ifndef IWDG_TYPEDEF_H
#define IWDG_TYPEDEF_H

namespace periph{
	namespace iwdg_reg{
		
		typedef enum				// �������Ź��Ĵ�����Կ��
		{
			  START = 0xcccc			// �������Ź�
			, WRITE_PR_RLR = 0X5555		// �޸�PR, RLR�Ĵ���
			, COUNTER_RELOAD = 0xaaaa	// ι���õ�KEY
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
			/**	@brief �����ڲ����Ź���һ���������޷�ֹͣ
			*/
			finline void Start()		{ KR = START; }

			/**	@brief  ι��
			*/
			finline void Feed()			{ KR = COUNTER_RELOAD; }

			/**	@brief	�����ڲ��������Ź�
			*	@param	reload_val	���Ź�������������ֵ����ת��0 - 0xfff֮��
			*	@param	div ��LSIʱ��(40kHz)��Ƶ���õ����Ź�ʱ�ӣ�div����ȡ���� 4, 8, 16, 32, 64, 128, 256
			*/
			void Config(u16 reload_val, u16 div = 256)
			{
				u8 pr = 0;
				//Feed();	// ��ֹ�ڿ��Ź�����ʱ���õ�ʱ��ǡ�ÿ��Ź���λ
				reload_val &= 0xfff;
				div%=256;
				div>>=2;
				while(div = div>>1, div!=0)
					pr++;
				
				while( checkb(SR, PVU) );	// �ȴ�ֱ�������޸�PR��ֵ
				KR = WRITE_PR_RLR;	// д��Կ��
				PR = pr;
				
				while( checkb(SR, RVU) );	// �ȴ�ֱ�������޸�RLR��ֵ
				KR = WRITE_PR_RLR;	// д��Կ��
				RLR = reload_val;
				Feed();				// ����ι�����ÿ��Ź����µ�����ֵ����
			}
		};
	}

}

#define iwdg	(*(periph::iwdg_reg::IWDG_Class*)IWDG)

#endif
