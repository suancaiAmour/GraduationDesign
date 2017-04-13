#ifndef __cplusplus		/* 在C语言的环境下不能使用这个头文件 */
#define STACK_H
#endif
#ifndef STACK_H
#define STACK_H

#include <stddef.h>


namespace nstd{
	
	/**	@brief	栈类，T为基本数据类型
	*/
// 	template
// 	<typename T>
// 	class Stack
// 	{
// 		protected:
// 		T*	stack;					/*!< 用作栈的一段内存 */
// 		size_t max;					/*!< 栈的大小，可以存放T类型数据的最大数目 */
// 		size_t sp;					/*!< stack pointer，对栈元素的索引 */
// 		public:
// 		/**	@brief 构造函数，stackMem 用作栈的一段内存
// 		*/
// 		Stack( T* stackMem,size_t maxElementsNum):
// 			  stack(stackMem)
// 			, max(maxElementsNum)
// 			, sp(0){}
// 		/**	@brief	从栈中弹出一个元素
// 		*/
// 		finline void Pop(T& t)		{if(sp!=0)	t=stack[--sp];}
// 		
// 		/**	@brief	压入一个元素到栈中
// 		*/
// 		finline void Push(T& t)		{if(sp<max)	stack[sp++]=t;}
// 	};
}

#endif
