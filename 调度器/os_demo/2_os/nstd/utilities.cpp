#include "utilities.h"
#include "Debug.h"
#include "nstd_assert.h"
namespace nstd{

    /** @addtogroup utilities
        * @{
        */

    /**	@brief	���ڴ��еõ�һ��DWord���ݣ��������ڴ�����С���ֽ����
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

    /**	@brief	дһ��DWord�͵����ݵ��ڴ��У���С���ֽ���ĸ�ʽ
    *	@param	dat: Ҫд�������
    *	@param	buf: ָ��д���ĵ�ַ
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

    /**	@brief	��buffer�õ�һ��word�͵����ݣ�������buffer����С���ֽ����
    */
    u16 GetWordFromLE(u8* buf)
    {
        u16 temp;
        temp = (u16)(buf[1]);
        temp <<= 8;
        temp += buf[0];
        return temp;
    }

    /**	@brief	дһ��word���ݵ�ָ�����ڴ��У����ڴ�����С���ֽ���ĸ�ʽ�洢
    */
    void PutWordToLE(u8* buf, u16 dat)
    {
        buf[0] = (u8)dat;
        dat >>= 8;
        buf[1] = (u8)dat;
        return ;
    }

    /**	@brief	�ж�����null-terminated-string�Ƿ����
    *	@param	str1, str2: ����null-terminated-string
    *	@return	��ȷ��� true������ȷ��� false
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


    /** @brief �����ַ���,��Դ�ַ������Ƶ�Ŀ���ַ�����һֱ����������NULL�ַ���
      *	@param p_source Դ�ַ�������һ��null-terminated-string
      *	@param p_destination Ŀ���ַ�����null-terminated-string
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
        *p_destination = 0;	//��һ��������־
        return 0;
    }


    /** @brief ���ַ�����СдתΪ��д
      *	@param str null-terminated-string
      * @return ����0
      */
    u8 ToUpper(u8* str)
    {
        while(*str)
        {
            if( (*str >= 'a') && (*str <='z'))	//���Сд
            {
                *str = *str - 'a' + 'A';
            }
            str++;
        }
        return 0;
    }

    /** @brief �õ��ַ������һ������Ҫ�ҵ��ַ���λ��
      *	@param c Ҫ�ҵ��ַ�
      *	@param str ָ���ַ������Ӹ��ַ�����Ѱ���ַ�
      *	@param from ��ʼ�ҵ�λ��
      *	@return �ַ����ַ����е�λ�ã����û�ҵ����򷵻ص����ַ����ĳ��ȣ�
      *	��str[return_val] == 0;
      */
    u16 FindChar(u8 c,const u8* str, u16 from)
    {
        while((*(str + from) != 0) && (*(str + from) != c) )
        {
            from++;
        }
        return from;
    }


    /** @brief �Ƚ�������ĸ�Ĵ�С����Сд�޹�
      * @param letter1 ��ĸ1
      * @param letter2 ��ĸ2
      * return �������ֵ����0���� letter1 > letter2, �� CmpLetter('b', 'a')
      *	�������ֵ����0���� letter1 = letter2, �� CmpLetter('a', 'a')
      *	�������ֵС��0���� letter1 < letter2, �� CmpLetter('a', 'b')
      */
    char CmpLetter(u8 letter1, u8 letter2)
    {
        if( (letter1 >= 'a') && (letter1 <= 'z'))	
        {	//�����Сд��תΪ��д
            letter1 -= ('a' - 'A');
        }
        if( (letter2 >= 'a') && (letter2 <= 'z'))	
        {	//�����Сд��תΪ��д
            letter2 -=('a' - 'A');
        }
        //��תΪ��д����бȽϣ����ؽ��
        return ((char)letter1 - (char)letter2);
    }


    /** @brief �Ƚ�����ָ�����ȵ�ASCII������Ĵ�С
      * @param buffer1	��һ��ASCII������
      * @param buffer2	��һ��ASCII������
      * @param len ����Ĵ�С
      *	@param case_sensitive ָ���Ƿ��Сд���С���0Ϊ��Сд���У�0Ϊ��Сд�޹�
      * @return �������ֵΪ 0, ������ASCII����������ȣ��������������ǲ���ȵ�
      * ���� ABCD(buffer1) > ABAE(buffer2) ���ú󷵻�ֵ����0��
      * �������buffer1��buffer2����������ô����ֵС��0
      */
    char CmpASCArray(const u8 * buffer1,const u8 * buffer2,u16 len, u8 case_sensitive)
    {
        if(case_sensitive != 0)
        {	//����Ǵ�Сд����
            while(len--)
            {
                if(*buffer1 != *buffer2)
                {	//�������ȣ����رȽϽ��
                    return ((*(char*)buffer1) - (*(char*)buffer2));
                }
                //�����ȣ������Ƚ�
                buffer1++;				buffer2++;
            }
            //�������whileѭ����˵������array���
            return 0;
        }else
        {	//�����Сд�޹�
            while(len--)
            {
                if(CmpLetter(*buffer1, *buffer2) != 0)
                {	//�������ȣ����رȽϽ��
                    return CmpLetter(*buffer1, *buffer2);
                }
                //�����ȣ������Ƚ�
                buffer1++;
                buffer2++;
            }
            //�������whileѭ����˵������array���
            return 0;
        }
    }

    /** @brief �õ�һ���ַ����ĳ���
      *	@param str a null-terminated string��
      * @retrun �ַ����ĳ���
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

    /** @brief ��ָ���ĳ��ȵ��ַ�������ָ�����ַ�
      * @param str �ַ���(������null terminated)
      * @param len �ַ����ĳ���
      * @param to_find Ҫ���ҵ��ַ�
      * @return ����ɹ������������ţ����ʧ�ܣ����غ������� len��
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


    /** @brief ��bufָ��ĳ���Ϊlen���ڴ����ĵڸ��ֽڳ�ʼ��Ϊval
      * @param buf ָ��һ���ڴ���
      *	@param val ��ʼ����ֵ
      *	@param len buff�ĳ���
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

    /** @brief ��sourceָ��Ĵ�СΪlen���ڴ���ƶ���destinationָ����ڴ��
      * @param destination Ŀ��λ��
      * @param source Դλ��
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

    /** @brief �õ�һ�����ֶ�Ӧ�� ASCII ��
      * @param num һ�� 0x0-0xf ������
      * @return ��Ӧ�� ASCII ��, ������ NumToASCII(0) ��õ� '0'
      */
    char NumToASCII(u8 num){
        return num_to_ascii[num&0x0f]; /* ��� */
    }

    // group utilities
    /**
      * @}
      */

}

