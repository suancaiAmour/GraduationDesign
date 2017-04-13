#ifndef GPIO_TYPEDEF_H
#define GPIO_TYPEDEF_H

#include "stm32f10x__COMMON.h"

namespace periph{
	/**	@brief	����
	*/
	enum GPIO_Pin{	
	  Pin0 = Bit(0)
	, Pin1 = Bit(1)
	, Pin2 = Bit(2)
	, Pin3 = Bit(3)
	, Pin4 = Bit(4)
	, Pin5 = Bit(5)
	, Pin6 = Bit(6)
	, Pin7 = Bit(7)
	, Pin8 = Bit(8)
	, Pin9 = Bit(9)
	, Pin10 = Bit(10)
	, Pin11 = Bit(11)
	, Pin12 = Bit(12)
	, Pin13 = Bit(13)
	, Pin14 = Bit(14)
	, Pin15 = Bit(15)
	, PinAll= BitFromTo(15, 0)		/*!< ȫѡ */
	};
	
	/**	@brief	INPUT ���� GPIO_Class::Config �Ĳ�������ʾ��IO������Ϊ����ģʽ��*/
	enum GPIO_Input		{	
		  INPUT		= 0	};
	/**	@brief	����ķ�ʽ */
	typedef enum GPIO_InputStyle{			
		  ANALOG	= BByte(0000)			/*!< ģ������ */
		, FLOATING	= BByte(0100)			/*!< �������� */
		// ���������������� 4 λ bit 3:0 �Ǹ����ֲ��ϵ�ֵ�����õģ�
		// �� bit 4 Ϊ 1 ��ʾ������Ϊ 0 ��ʾ�����������ʶ���ٽ�����Ӧ�Ĵ���
		, PULL_UP	= BByte(11000)			/*!< ���������� */
		, PULL_DOWN	= BByte(01000)			/*!< ���������� */
	}InputStyle;
	
	
	/**	@brief	��������� */
	enum GPIO_Output	{			//�����	
		OUTPUT		= 0,				/*!< ��ͨ�����ģʽ */
		AF_OUTPUT	= BByte(1000)		/*!< Alternate Function output */
	};
	/**	@brief	�����ʽ */
	typedef enum GPIO_OutStyle	{				
		PUSH_PULL	= BByte(0000),	/*!< ������� */
		OPEN_DRAIN	= BByte(0100)	/*!< ��©��� */
	}OutStyle;
	/**	@brief	������ٶ� */
	enum GPIO_OutSpeed	{				
		SPEED_10M	= BByte(0001),
		SPEED_2M	= BByte(0010),
		SPEED_50M	= 	BByte(0011)
	};

	namespace gpio_reg{
		
		class GPIO_Class: protected GPIO_TypeDef
		{
		protected:
			GPIO_Class(){};	//�������ⲿ��������!

          
            static const u16 pin_field_map[16];
            
            /**	@brief	����IO�ڵ�ģʽ���������ʹ�ñȽϸ��ӣ��������û�ʹ�á�
				��������������������Ҫʹ�����������ʵ�֡���������IO�ڵ�ģʽ�� */
            void SetMode(const u16 pins, const u8 mode){
                u32 field;
                if(pins&BitFromTo(7,0)){    // �� 8 λ����
                    field = pin_field_map[pins&0xf]
                                | (pin_field_map[(pins>>4)&0xf]<<16);
                    CRL = (CRL&(~(field*0xf))) 
                            | (field*(mode&0xf));
                }
                if(pins&BitFromTo(15,8)){    // �� 8 λ����
                    field = pin_field_map[(pins>>8)&0xf]
                                | (pin_field_map[(pins>>12)&0xf]<<16);
                    CRH = (CRH&(~(field*0xf))) 
                            | (field*(mode&0xf));
                }
            }
            
			/**	@brief	����IO�ڵ�ģʽ���������ʹ�ñȽϸ��ӣ��������û�ʹ�á�
				��������������������Ҫʹ�����������ʵ�֡���������IO�ڵ�ģʽ�� */
// 			finline void SetMode(const u16 pins,const u8 mode)
// 			{
// 				if(checkb(pins, BitFromTo(7, 0))) {	//��8λ
// 					(this->CRL = AssignField(this->CRL, ExpandU8Bit(pins,4) , 4, mode));
// 				}
// 				if(checkb(pins, BitFromTo(15, 8))) {	//��8λ
// 					 this->CRH = AssignField( this->CRH, ExpandU8Bit(((pins)>>8),4) , 4, mode);
// 				}
// 			};
            
		public:
            
            /** @brief �Ѷ˿ڵ�������������Ϊָ�������빤��ģʽ
              */
            finline void Config(const GPIO_Input input,const InputStyle style = PULL_UP){
                u32 temp = 0x11111111*(input|style);
                CRL = temp;
                CRH = temp;
                if(style == PULL_DOWN)
					BRR = 0xffff;
				else if(style == PULL_UP)
					BSRR = 0xffff;
            }
            finline 
            void Config(const GPIO_Output output
                        ,const OutStyle style = OPEN_DRAIN
                        ,const GPIO_OutSpeed speed = SPEED_10M )
			{
                u32 temp = 0x11111111*(output | style | speed);
                CRL = temp;
                CRH = temp;
            }
        
			/**	@brief	����IO�ڵ�����ģʽ
			*	@param	pins: ָ�������ţ��� @ref GPIO_Pin ö����ĳ�Ա������ʹ�� Pinxx + Pinyy + ... ����ʽ��Ϊ�����Ĳ�����
			*		@note	pins ������������Ǳ����ڵĳ���������������޷������Զ��Ż����������ɱȽ϶�Ĵ��롣
			*	@param	input: ��ʾ���������ó����룬�ο� @ref GPIO_Input �Ķ��壬������ֻ��Ϊ INPUT��
			*	@param	style: ����ķ�ʽ���ο� @ref InputStyle �Ķ��塣
			*	@example gpioa.Config(Pin0+Pin3, INPUT, PULL_DOWN);	// �� PA0 �� PA3 ����Ϊ��������
			*/
            inline
            void Config(const u16 pins
                            ,const GPIO_Input  input
                            ,const InputStyle style = PULL_UP)  {	
				SetMode( pins, input | style );
				/*!< ������/�����������������ģʽ��ִ�У�
					��Ϊ��	1. �û������������������ִ�е�ʱ��ȥ���ܽŵ�ֵ
							2. ���֮ǰΪ���������������ÿ��ܻ���������*/
				if(style == PULL_DOWN)		/*!< �������Ҫ����Ϊ�� ����/�������� ����� */
					BRR = pins;
				else if(style == PULL_UP)
					BSRR = pins;
			}

			
			/** @brief	����IO�ڵ����ģʽ
			  * @param	pins: ָ�������ţ��� @ref GPIO_Pin ö����ĳ�Ա������ʹ�� Pinxx + Pinyy + ... ����ʽ��Ϊ�����Ĳ�����
			  *     @note   pins ������������Ǳ����ڵĳ���������������޷������Զ��Ż����������ɱȽ϶�Ĵ��롣
			  * @param	output: �ο� @ref GPIO_Output �Ķ��壬������ OUTPUT (��ͨ���) �� AF_OUTPUT (���ù������)
			  * @param	style: �����ʽ�����ã��ο� @ref OutStyle �Ķ��塣
			  * @param	speed: ָ��IO�ڵ��ٶȣ��ο� @ref OutSpeed��
              * @example gpiob.Config(Pin0+Pin1+Pin2, OUTPUT, PUSH_PULL, SPEED_50M);	// PA0 PA1 PA2 ���������50M ���ٶȡ�
			  */
			finline
            void Config(const u16 pins
                        ,const GPIO_Output output
                        ,const OutStyle style = OPEN_DRAIN
                        ,const GPIO_OutSpeed speed = SPEED_10M )
            {
                SetMode( pins, output | style | speed);
            }
			
			/** @brief	��˿�д���� */
			finline void Write(u16 dat)	{ ODR = (dat); }
			
			/**	@brief	��˿�д���ݣ��Ƽ������÷���
			 *  @example
             *      gpioa = 0x55aa;	// �� 0x55aa д�� GPIOA �ϡ�
             */
			finline void operator = (u16 dat)	{ODR = dat;}
			
            /**	@brief	�������ü������������ŵ����ֵ��ע���⼸�����Ŷ�Ӧ������Ϊ���ģʽ��
             *	@param	lsp: ���λ���ź�
             *	@param	width: λ��
             *	@param	val: ֵ
             */
			finline void WritePins(const GPIO_Pin& lsp,const u16& width,const u16& val)
					{	this->BSRR =	(((u16)val * (lsp)) & ((((lsp)) <<width)-(lsp)))   
									| (((u32)( ( ( (val * (lsp))^ ( (((lsp)) <<width) - (lsp))))
										&(((((lsp))<<width) - (lsp)))))<<16);	}
			
			/**	@brief	�õ�ָ���˿ڵ�ǰ������������� */
			finline u32 ReadOutputData()				{return this->ODR;}
		
			/**	@brief	��IO�˿� */
			finline u32 Read(void)						{return IDR;}
			
			/**	@brief ��IO�˿ڣ��Ƽ������÷���
			  * @example  u32 a; a = gpioa;	// �� GPIOA �˿ڵ����ݶ������� a �С�
			  */
			finline operator u32()	{return IDR;}
			
			void finline Set(u16 pins)	{ BSRR = pins; }
	
		};
		
	}	// namespace gpio_reg
}

using periph::gpio_reg::GPIO_Class;

#include "stm32f10x__GPIO_CPin.h"

#define gpioa				(*((GPIO_Class*)GPIOA))
#define gpiob				(*((GPIO_Class*)GPIOB))
#define gpioc				(*((GPIO_Class*)GPIOC))
#define gpiod				(*((GPIO_Class*)GPIOD))

#endif
