#ifndef UTILITIES_H
#define UTILITIES_H

#include "nstd_type.h"

/** @addtogroup nstd
  * @{
  */

namespace nstd{

    /** @addtogroup utilities
        * @ingroup nstd
        * @{
        */

    /** @brief 如果变量letter是一个小写字母，使用这个函数可以得到相应的大家字母
      */
    finline u8 getupper(u8 letter)
    {	
        return 	((((letter) >= 'a') && ((letter) <= 'z')) 
                    ? ((letter) - 'a' + 'A') : (letter) );	
    }

    #define GetDWord_S(buf, offset)			GetDWordFromLE(((u8*)buf) + offset)
    u32 GetDWordFromLE(u8* buffer);
    void PutDWordToLE(u8* buf, u32 dat);

    #define GetWord_S(buf, offset)			GetWordFromLE(((u8*)buf) + offset)
    u16 GetWordFromLE(u8* buf);
    void PutWordToLE(u8* buf, u16 dat);


    bool StringIsEqual(u8* str1, u8* str2);

    /**	@brief	得到字符串长度
    */
    u16 StringLength(const u8* str);

    /**	@brief	复制字符串
    */
    u8 StringCopy(u8* destination, u8* source);

    /**	@brief	将字符串从小写转为大写
    */
    u8 ToUpper(u8* str);

    u16 FindChar(u8 c,const u8* str, u16 from);

    char CmpLetter(const u8 letter1,const u8 letter2);
    char CmpASCArray(const u8  * buffer1,const u8  * buffer2,u16 len, u8 case_sensitive);


    u8 Find(const u8 *str, u8 len, u8 to_find);

    u8* MemorySet(u8 *buf, u16 len, u8 val);

    void MemoryMove(u8* destination,const u8* source, u16 size);

    /** @brief 判断一个字符是否是 white-space character */
    inline bool IsSpaceChar(const char c)		{ return ((c>='\t')&&(c<='\r'))||(c == ' '); }

    char NumToASCII(u8 num);


    /**	@brief 把长度为 len 的数组的元素设置成指定的值
        * @param buf 指向要操作的字符串
        * @param len 字符串的长度, 注意不是指字符串占用的空间的大小, 字符串占
        * 用的空间的大小为 len*sizeof(buf[0]).
        */
    template<class TYPE>
    inline void array_set(TYPE* buf, size_t len, const TYPE& val)	{
        while(len--)		*(buf++) = val;
        return ;
    }
        
    /**	@brief	把长度为 len 的一个数组的内容复制到另外一个数组
    */
    template <typename TYPE>
    inline void array_copy(TYPE* destination, TYPE* source, size_t len) {
        while(len--)		*(destination++) = *(source++);
        return ;

}

// group ulitilies
/**
	* @}
	*/
    
    
    namespace c_string{

    }

} /* namespace nstd */

// group nstd
/**
  * @}
  */

#endif
