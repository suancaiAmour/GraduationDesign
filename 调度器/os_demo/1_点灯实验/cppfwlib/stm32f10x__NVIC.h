#ifndef NVIC_TYPE_H
#define NVIC_TYPE_H

namespace core
{
	class NVIC_Class: protected NVIC_Type{
	public:
		/**	@brief ��������ж�
		  *	@param	n: �жϺš��ο� @ref IRQn_Type �ĳ�Ա��
		  */
		finline void SoftTrig(IRQn_Type n)				{ STIR = n; }
		
		/**	@brief	NVIC����ж����á�
		  *	@param	n : �жϺš��ο� @ref IRQn_Type �ĳ�Ա��
		  *	@param	e : ʹ�ܻ�ʧ��
		  *	@param	groupPri: �����ȼ���Ĭ����������ȼ���
		  *	@param	subPri: �����ȼ���Ĭ����������ȼ���
          * @note ���������ȼ��������ȼ�, �ο� SCB �� SetPriorityGrouping
          * ����. 
		  */
		void Config(IRQn_Type n, FunctionalState e, u8 groupPri = 0xff, u8 subPri = 0xff);
	};

}using namespace core;

#define nvic	(* ((NVIC_Class*)NVIC))

#endif
