#pragma once
#ifndef DEFINE_H
#define DEFINE_H

/**	@brief ���ͷ�ļ��ﶨ����һЩ�û��ؼ��֡� */

// 51��ֲ���ĳ���xdata����Ϊ��
#define xdata
#define idata

#include <stddef.h>

/** @def finline
  * ���庯��Ϊǿ������.
  * ��ͨ�� inline �ؼ���ֻ�ǽ��������������
  * ������ʱ������������ӽ������������Ч�ʵ��£�
  * ʹ��ǿ���������Խ��������� 
  */
#if defined ( __CC_ARM   )
    #define finline		__forceinline
#else
    #define finline		inline
#endif

/** @def tinline
  * inline template function. ��Щ��������֧�ָ� template ���� inline,
  * ���ʱ�����Ҫ���¶��� tinline Ϊ��.  
  */
#define tinline		finline

/** @def pure_function
  * @brief ��������һ�������Ǵ�����, ��û���κθ�����, һ��������Ӧ
  * Ψһ�ķ���ֵ.
  */
#define pure_function	__pure

/* cpp�ļ��е��жϺ����Ķ�����Ҫ�������������
�ж�ʱ�Ż������жϺ��� */
#define ARMAPI	extern "C"

/** @def CAPI
  * @brief ��������һ�������ǿ��Ը� C ���Ե��õ�.
  */
#define CAPI    extern "C"

#define BIG_ENDIAN 1        /**< ��� */

#define LITTLE_ENDIAN 0     /**< С�� */


#ifdef __BIG_ENDIAN
    // ����ֽ���
    #define HOST_ENDIAN		BIG_ENDIAN
#else
    // С���ֽ���
    #define HOST_ENDIAN		LITTLE_ENDIAN
#endif

#endif
