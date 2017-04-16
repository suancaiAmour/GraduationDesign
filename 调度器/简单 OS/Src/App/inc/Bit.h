/********************************************************************************
  * @File   : Bit.h
  * @Author : worldsing
  * @Version: V0.1
  * @Date   : 2013/05/05
  * @Brief  :
  *******************************************************************************
  * @Attention:
  */
#ifndef __BIT_H__
#define __BII_H__

//���Բ���ǰ����port��д��
#define RevBit(port, bitn)           (port ^=  (1<<(bitn)))                                 //��תport��bitnλ
#define SetBit(port, bitn)           (port |=  (1<<(bitn)))                                 //port��bitnλ = 1
#define ClrBit(port, bitn)           (port &= ~(1<<(bitn)))                                 //port��bitnλ = 0
#define GetBit(port, bitn)           (port &   (1<<(bitn)))                                 //��port��bitnλֵ
#define OutBit(port, bitn, value)    ((value) ? (SetBit(port, bitn)) : (ClrBit(port, bitn)))//port��bitnλ = value

#endif

//end of file


