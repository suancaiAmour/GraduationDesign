#ifndef SCB_TYPE_H
#define SCB_TYPE_H

namespace core{

	enum PriorityGroupingStyle
	{
		  GRP16_SUB1	= BByte(011)		// 16 priority groups, 1(none) sub priority 
		, GRP8_SUB2		= BByte(100)		// 8 priority groups, 2 sub priority (none)
		, GRP4_SUB4		= BByte(101)		// 4 priority groups, 4 sub priority (none)
		, GRP2_SUB8		= BByte(110)		// 2 priority groups, 8 sub priority (none)
		, GRP1_SUB16	= BByte(111)		// 1 (none priority groups, 16 sub priority (none)
	};
	
	enum VectorTableBase{
		  VECTOR_TABLE_FLASH	= ((uint32_t)0x08000000)
		, VECTOR_TABLE_RAM		=  ((uint32_t)0x20000000)
	};

	namespace scb_reg
	{
		enum AIRCR_KEY{
			WRITE_AIRCR = 0x05fa
		};
		// ע��ÿ��д���AIRCR�Ĵ�����Ҫ��VECTKEYSTAT��bit field��д��0x05fa
		enum SCB_AIRCR{
			  VECTKEYSTAT	= BitFromTo(31,16)	// Register key
			, ENDIANESS		= Bit(15)			//
			, PRIGROUP		= BitFromTo(10,8)	// Interrupt priority grouping field 
			, SYSRESETREQ	= Bit(2)			// System reset request
			, VECTCLRACTIVE = Bit(1)			// Reserved for Debug use. 
			, VECTRESET		= Bit(0)			// Reserved for Debug use. 
		};

		class SCB_Class: protected SCB_Type
		{
			public:
			/**	@brief	�õ���ǰ�ķ��鷽ʽ
			  */
			finline PriorityGroupingStyle 
					GetPriorityGrouping()	
					{return (PriorityGroupingStyle)ReadField(AIRCR, PRIGROUP);}
			
			/**	@brief	���÷��鷽ʽ
			  */
			void SetPriorityGrouping(PriorityGroupingStyle s)	
					{AIRCR = assignf(assignf(AIRCR,PRIGROUP,s),VECTKEYSTAT,WRITE_AIRCR) ;}
			
			/**	@brief	�����λϵͳ
			  */
			void SoftReset()	{ AIRCR = assignf( setb(AIRCR, SYSRESETREQ), VECTKEYSTAT,WRITE_AIRCR) ;}
			
			/**	@brief	����������
			  */
			void SetVectorTable(VectorTableBase vtb = VECTOR_TABLE_FLASH, u32 offset = 0)	{VTOR = clearb(vtb|offset, BitFromTo(8,0)|BitFromTo(31,30));}
		};
	}

}
#define scb		(*(core::scb_reg::SCB_Class*)SCB)

#endif
