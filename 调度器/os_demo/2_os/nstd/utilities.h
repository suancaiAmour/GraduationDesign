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

    /** @brief �������letter��һ��Сд��ĸ��ʹ������������Եõ���Ӧ�Ĵ����ĸ
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

    /**	@brief	�õ��ַ�������
    */
    u16 StringLength(const u8* str);

    /**	@brief	�����ַ���
    */
    u8 StringCopy(u8* destination, u8* source);

    /**	@brief	���ַ�����СдתΪ��д
    */
    u8 ToUpper(u8* str);

    u16 FindChar(u8 c,const u8* str, u16 from);

    char CmpLetter(const u8 letter1,const u8 letter2);
    char CmpASCArray(const u8  * buffer1,const u8  * buffer2,u16 len, u8 case_sensitive);


    u8 Find(const u8 *str, u8 len, u8 to_find);

    u8* MemorySet(u8 *buf, u16 len, u8 val);

    void MemoryMove(u8* destination,const u8* source, u16 size);

    /** @brief �ж�һ���ַ��Ƿ��� white-space character */
    inline bool IsSpaceChar(const char c)		{ return ((c>='\t')&&(c<='\r'))||(c == ' '); }

    char NumToASCII(u8 num);


    /**	@brief �ѳ���Ϊ len �������Ԫ�����ó�ָ����ֵ
        * @param buf ָ��Ҫ�������ַ���
        * @param len �ַ����ĳ���, ע�ⲻ��ָ�ַ���ռ�õĿռ�Ĵ�С, �ַ���ռ
        * �õĿռ�Ĵ�СΪ len*sizeof(buf[0]).
        */
    template<class TYPE>
    inline void array_set(TYPE* buf, size_t len, const TYPE& val)	{
        while(len--)		*(buf++) = val;
        return ;
    }
        
    /**	@brief	�ѳ���Ϊ len ��һ����������ݸ��Ƶ�����һ������
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
