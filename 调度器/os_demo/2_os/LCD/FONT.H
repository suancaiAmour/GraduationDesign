#ifndef __FONT_H
#define __FONT_H 

#include "nstd_type.h"

/**	@brief 尺寸结构体 */
struct Size{
	int width;
	int height;
};

/**	@brief 字体信息结构体，这是一个字体需要提供给GUI的接口
*	如果你有自定义的字体，你可以很方便的继承这个类，重写这里的虚函数
*	再通过 GUI 类的 SelectFont 函数选择你自定义的字体 */
class FontBase{
public:
    
    // enum{ ... };
    // virtual get_format()

	/**	@brief 得到一个文字的尺寸
	*	@param code: 文字的编码
	*	@param pr: 保存尺寸的结构体 */
	virtual void GetSize(u32 code, Size* pr)const = 0;
	
	/**	@brief 得到字模，取模的方式为：（假设字体的宽度不超过 16 ，记函数返回的值为 ret ）
	*	ret[0]:0 ret[0]:1 ... ret[0]:7 ret[1]:0 ret[1]:1 ...
	*	ret[2]:0 ret[2]:1 ... ret[2]:7 ret[3]:0 ret[3]:1 ...
	*	...
	*	即逐行式低位在前取模，有字的点为 1。
	*	@note 返回的缓冲区是临时的，只要再次以另一个 code 作为参数调用这个函数，
	*	那么之前得到的缓冲区就无效了
	*	@ret 返回字模的缓冲区，如果没有那个字符，就会返回空指针。
	*/
	virtual u8 const* GetDotMetrix(u32 code)const = 0;
	virtual ~FontBase();
};

// should change the interface, to make it safer for dynamic object
FontBase const* GetSystemFont();

#endif
