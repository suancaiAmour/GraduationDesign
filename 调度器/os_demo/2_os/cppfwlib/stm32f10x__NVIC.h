#ifndef NVIC_TYPE_H
#define NVIC_TYPE_H

namespace core
{
	class NVIC_Class: protected NVIC_Type{
	public:
		/**	@brief 软件触发中断
		  *	@param	n: 中断号。参考 @ref IRQn_Type 的成员。
		  */
		finline void SoftTrig(IRQn_Type n)				{ STIR = n; }
		
		/**	@brief	NVIC里的中断配置。
		  *	@param	n : 中断号。参考 @ref IRQn_Type 的成员。
		  *	@param	e : 使能或失能
		  *	@param	groupPri: 组优先级，默认是最低优先级。
		  *	@param	subPri: 子优先级，默认是最低优先级。
          * @note 关于组优先级和子优先级, 参考 SCB 的 SetPriorityGrouping
          * 函数. 
		  */
		void Config(IRQn_Type n, FunctionalState e, u8 groupPri = 0xff, u8 subPri = 0xff);
	};

}using namespace core;

#define nvic	(* ((NVIC_Class*)NVIC))

#endif
