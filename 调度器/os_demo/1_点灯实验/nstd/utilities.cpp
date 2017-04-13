#include "utilities.h"
#include "Debug.h"
#include "nstd_assert.h"
namespace nstd{

    /** @addtogroup utilities
        * @{
        */

    /**	@brief	从内存中得到一个DWord数据，数据在内存中是小端字节序的
    */
    u32 GetDWordFromLE(u8* buffer)	//get dword from little endian
    {
        u32 temp;
        temp = (u32)(buffer[3]);
        temp = (temp << 8);	
        temp += buffer[2];
        temp = (temp << 8);
        temp += buffer[1];
        temp = (temp << 8 );
        temp += buffer[0];
        return temp;
    }

    /**	@brief	写一个DWord型的数据到内存中，以小端字节序的格式
    *	@param	dat: 要写入的数据
    *	@param	buf: 指定写到的地址
    */
    void PutDWordToLE(u8* buf, u32 dat)
    {
        buf[0] = (u8)dat;
        dat >>= 8;
        buf[1] = (u8)dat;
        dat >>= 8;
        buf[2] = (u8)dat;
        dat >>= 8;
        buf[3] = (u8)dat;
        return ;
    }

    /**	@brief	从buffer得到一个word型的数据，数据在buffer里是小端字节序的
    */
    u16 GetWordFromLE(u8* buf)
    {
        u16 temp;
        temp = (u16)(buf[1]);
        temp <<= 8;
        temp += buf[0];
        return temp;
    }

    /**	@brief	写一个word数据到指定的内存中，在内存中以小端字节序的格式存储
    */
    void PutWordToLE(u8* buf, u16 dat)
    {
        buf[0] = (u8)dat;
        dat >>= 8;
        buf[1] = (u8)dat;
        return ;
    }

    /**	@brief	判断两个null-terminated-string是否相等
    *	@param	str1, str2: 两个null-terminated-string
    *	@return	相等返回 true，不相等返回 false
    */
    bool StringIsEqual(u8* str1, u8* str2)
    {
        while(1)
        {	
            if( (*str1 == 0) && (*str2 == 0))
                return true;
            else if(*(str1++) != *(str2++))
                return false;
        }
    }


    /** @brief 复制字符串,将源字符串复制到目标字符串，一直到（包括）NULL字符。
      *	@param p_source 源字符串，是一个null-terminated-string
      *	@param p_destination 目标字符串，null-terminated-string
      * @return 0
     */
    u8 StringCopy(u8* p_destination, u8* p_source)
    {
        while(*p_source)
        {
            *p_destination = *p_source;
            p_source++;
            p_destination++;
        }
        *p_destination = 0;	//加一个结束标志
        return 0;
    }


    /** @brief 将字符串从小写转为大写
      *	@param str null-terminated-string
      * @return 返回0
      */
    u8 ToUpper(u8* str)
    {
        while(*str)
        {
            if( (*str >= 'a') && (*str <='z'))	//如果小写
            {
                *str = *str - 'a' + 'A';
            }
            str++;
        }
        return 0;
    }

    /** @brief 得到字符串里第一个出现要找的字符的位置
      *	@param c 要找的字符
      *	@param str 指定字符串，从该字符串中寻找字符
      *	@param from 开始找的位置
      *	@return 字符在字符串中的位置，如果没找到，则返回的是字符串的长度，
      *	即str[return_val] == 0;
      */
    u16 FindChar(u8 c,const u8* str, u16 from)
    {
        while((*(str + from) != 0) && (*(str + from) != c) )
        {
            from++;
        }
        return from;
    }


    /** @brief 比较两个字母的大小，大小写无关
      * @param letter1 字母1
      * @param letter2 字母2
      * return 如果返回值大于0，则 letter1 > letter2, 如 CmpLetter('b', 'a')
      *	如果返回值等于0，则 letter1 = letter2, 如 CmpLetter('a', 'a')
      *	如果返回值小于0，则 letter1 < letter2, 如 CmpLetter('a', 'b')
      */
    char CmpLetter(u8 letter1, u8 letter2)
    {
        if( (letter1 >= 'a') && (letter1 <= 'z'))	
        {	//如果是小写，转为大写
            letter1 -= ('a' - 'A');
        }
        if( (letter2 >= 'a') && (letter2 <= 'z'))	
        {	//如果是小写，转为大写
            letter2 -=('a' - 'A');
        }
        //都转为大写后进行比较，返回结果
        return ((char)letter1 - (char)letter2);
    }


    /** @brief 比较两块指定长度的ASCII码数组的大小
      * @param buffer1	第一个ASCII码数组
      * @param buffer2	另一个ASCII码数组
      * @param len 数组的大小
      *	@param case_sensitive 指定是否大小写敏感。非0为大小写敏感，0为大小写无关
      * @return 如果返回值为 0, 则两个ASCII码数组是相等，否则两个数组是不相等的
      * 例如 ABCD(buffer1) > ABAE(buffer2) 调用后返回值大于0，
      * 如果参数buffer1和buffer2反过来，那么返回值小于0
      */
    char CmpASCArray(const u8 * buffer1,const u8 * buffer2,u16 len, u8 case_sensitive)
    {
        if(case_sensitive != 0)
        {	//如果是大小写敏感
            while(len--)
            {
                if(*buffer1 != *buffer2)
                {	//如果不相等，返回比较结果
                    return ((*(char*)buffer1) - (*(char*)buffer2));
                }
                //如果相等，继续比较
                buffer1++;				buffer2++;
            }
            //如果跳出while循环，说明两个array相等
            return 0;
        }else
        {	//如果大小写无关
            while(len--)
            {
                if(CmpLetter(*buffer1, *buffer2) != 0)
                {	//如果不相等，返回比较结果
                    return CmpLetter(*buffer1, *buffer2);
                }
                //如果相等，继续比较
                buffer1++;
                buffer2++;
            }
            //如果跳出while循环，说明两个array相等
            return 0;
        }
    }

    /** @brief 得到一个字符串的长度
      *	@param str a null-terminated string。
      * @retrun 字符串的长度
      */
    u16 StringLength(const u8* str)
    {
        const u8* end = str;
        while(*end)
        {
            end++;
        }
        return (u16)(end - str);
    }

    /** @brief 给指定的长度的字符串查找指定的字符
      * @param str 字符串(不必是null terminated)
      * @param len 字符串的长度
      * @param to_find 要查找的字符
      * @return 如果成功，返回索引号，如果失败，返回函数参数 len。
      */
    u8 Find(const u8 *str, u8 len, u8 to_find)
    {
        u8 count = 0;
        while(1)
        {
            if(count >= len)
                return len;
            else if(str[count] == to_find)
                return count;
            count++;
        }
    }


    /** @brief 把buf指向的长度为len的内存区的第个字节初始化为val
      * @param buf 指向一个内存区
      *	@param val 初始化的值
      *	@param len buff的长度
      * @return buf + len
      */
    u8* MemorySet(u8 *buf,u16 len, u8 val)
    {
        while(len)
        {
            *buf = val;
            buf++;
            len--;
        }
        return buf;
    }

    /** @brief 把source指向的大小为len的内存块移动到destination指向的内存块
      * @param destination 目标位置
      * @param source 源位置
      */
    void MemoryMove(u8* destination,const u8* source, u16 size)
    {
        while(size--)
        {
            *(destination) = *(source);
            destination++;
            source++;
        }
        return;
    }


    static const char num_to_ascii[] = {
          '0', '1', '2', '3', '4', '5', '6', '7'
        , '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
    };

    /** @brief 得到一个数字对应的 ASCII 码
      * @param num 一个 0x0-0xf 的数字
      * @return 对应的 ASCII 码, 例如输 NumToASCII(0) 会得到 '0'
      */
    char NumToASCII(u8 num){
        return num_to_ascii[num&0x0f]; /* 查表法 */
    }

    // group utilities
    /**
      * @}
      */

}

