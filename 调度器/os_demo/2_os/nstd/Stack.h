#ifndef __cplusplus		/* ��C���ԵĻ����²���ʹ�����ͷ�ļ� */
#define STACK_H
#endif
#ifndef STACK_H
#define STACK_H

#include <stddef.h>


namespace nstd{
	
	/**	@brief	ջ�࣬TΪ������������
	*/
// 	template
// 	<typename T>
// 	class Stack
// 	{
// 		protected:
// 		T*	stack;					/*!< ����ջ��һ���ڴ� */
// 		size_t max;					/*!< ջ�Ĵ�С�����Դ��T�������ݵ������Ŀ */
// 		size_t sp;					/*!< stack pointer����ջԪ�ص����� */
// 		public:
// 		/**	@brief ���캯����stackMem ����ջ��һ���ڴ�
// 		*/
// 		Stack( T* stackMem,size_t maxElementsNum):
// 			  stack(stackMem)
// 			, max(maxElementsNum)
// 			, sp(0){}
// 		/**	@brief	��ջ�е���һ��Ԫ��
// 		*/
// 		finline void Pop(T& t)		{if(sp!=0)	t=stack[--sp];}
// 		
// 		/**	@brief	ѹ��һ��Ԫ�ص�ջ��
// 		*/
// 		finline void Push(T& t)		{if(sp<max)	stack[sp++]=t;}
// 	};
}

#endif
